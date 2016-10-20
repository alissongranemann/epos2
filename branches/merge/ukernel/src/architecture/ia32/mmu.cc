// EPOS IA32 MMU Mediator Implementation

#include <architecture/ia32/mmu.h>

__BEGIN_SYS

// Class attributes
MMU::List MMU::_free[colorful * COLORS + 1];
MMU::Page_Directory * MMU::_master;

void MMU::set_flags(unsigned long log_addr, unsigned long size, unsigned long flags)
{
    CPU::Reg32 current_page_directory = CPU::cr3();
    db<MMU>(TRC) << "In place: " << reinterpret_cast<void *>(current_page_directory) << endl;

    if (log_addr != MMU::align_page(log_addr)) {
        db<MMU>(ERR) << "Error: address must be page-aligned." << endl;
    }

    unsigned long last = MMU::align_page(log_addr) + MMU::pages(size) * sizeof(MMU::Page);

    db<MMU>(TRC) << "log_addr = " << reinterpret_cast<void *>(log_addr) << endl;
    db<MMU>(TRC) << "MMU::align_page(log_addr) = " << MMU::align_page(log_addr) << endl;
    db<MMU>(TRC) << "size in pages = " << MMU::pages(size) << endl;
    db<MMU>(TRC) << "last (not included) = " << reinterpret_cast<void *>(last) << endl;

    for (unsigned long addr = log_addr; addr < last; addr += 0x1000) {
        unsigned long page_directory_offset = (addr & 0xffc00000) >> 22;
        unsigned long page_table_offset = (addr & 0x003ff000) >> 12;

        unsigned long page_table_phy_addr = *reinterpret_cast<unsigned long *>((current_page_directory | 0x80000000) + page_directory_offset * 0x4);

        if (page_table_phy_addr & 0x00000001) { /* Page Table Present */
            page_table_phy_addr &= 0xfffff000; // Discarding flags
            unsigned long page_table_log_addr = page_table_phy_addr | 0x80000000;
            unsigned long * tmp = reinterpret_cast<unsigned long *>(page_table_log_addr + page_table_offset * 0x4);

            if (*tmp & 0x00000001) { /* Page Frame Present */
                *tmp = *tmp & 0xfffffffd; /* Keep all flags but R/W. Keep the address. */
                *tmp = *tmp | flags; /* Apply the new flags. */
            }
            else {
                db<MMU>(ERR) << "Error! Unmapped logical address = " << reinterpret_cast<void *>(addr) << endl;
            }
        }
        else {
            db<MMU>(ERR) << "Error! Unmapped logical address = " << reinterpret_cast<void *>(addr) << endl;
        }
    }
}

__END_SYS

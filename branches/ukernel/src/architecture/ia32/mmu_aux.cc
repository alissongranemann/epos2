#include <architecture/ia32/mmu_aux.h>
#include <architecture/ia32/mmu.h>

__BEGIN_SYS

unsigned long IA32_MMU_Aux::physical_address(unsigned long log_addr, unsigned long * out_page_frame_present)
{
    unsigned long page_frame_phy_addr = 0x0;
    *out_page_frame_present = 0;

    Reg32 current_page_directory = CPU::cr3();

    db<void>(TRC) << "current_page_directory = " << reinterpret_cast<void *>(current_page_directory) << endl;

    unsigned long page_directory_offset = (log_addr & 0xffc00000) >> 22;
    unsigned long page_table_offset = (log_addr & 0x003ff000) >> 12;

    unsigned long page_table_phy_addr = *reinterpret_cast<unsigned long *>((current_page_directory | 0x80000000) + page_directory_offset * 0x4);

    if (page_table_phy_addr & 0x00000001) { /* Page Table Present */
        page_table_phy_addr &= 0xfffff000; // Discarding flags
        unsigned long page_table_log_addr = page_table_phy_addr | 0x80000000;
        unsigned long aux = *reinterpret_cast<unsigned long *>(page_table_log_addr + page_table_offset * 0x4);

        if (aux & 0x00000001) { /* Page Frame Present */
            page_frame_phy_addr = aux & 0xfffff000; // Discarding flags
            page_frame_phy_addr |= log_addr & 0xfff; // Adding back page offset
            *out_page_frame_present = 1;
        }
    }

    db<void>(TRC) << "page_directory_offset = " << reinterpret_cast<void *>(page_directory_offset) << endl;
    db<void>(TRC) << "page_table_offset = " << reinterpret_cast<void *>(page_table_offset) << endl;
    db<void>(TRC) << "page_table_phy_addr = " << reinterpret_cast<void *>(page_table_phy_addr) << endl;
    db<void>(TRC) << "page_frame_phy_addr = " << reinterpret_cast<void *>(page_frame_phy_addr) << endl;

    return page_frame_phy_addr;
}


void IA32_MMU_Aux::dump_memory_mapping()
{
    unsigned long phy_addr = 0x0;
    unsigned long * page_frame_present = 0;

    for (unsigned long log_addr = 0x0; log_addr <= 0xffffefff; log_addr += 0x1000) {
        phy_addr = physical_address(log_addr, page_frame_present);

        if (*page_frame_present) {
            db<void>(WRN) << reinterpret_cast<void *>(log_addr) << ":\t" << reinterpret_cast<void *>(phy_addr) << endl;
        }
    }
}


void IA32_MMU_Aux::dump_memory_mapping(unsigned long log_addr, unsigned long size, bool dump)
{
    db<void>(TRC) << "dump_memory_mapping - v2" << endl;
    db<void>(WRN) << "In place: " << reinterpret_cast<void *>(CPU::cr3()) << endl;

    unsigned long phy_addr = 0x0;
    unsigned long page_frame_present = 0;

    unsigned long last = MMU::align_page(log_addr) + MMU::pages(size) * sizeof(MMU::Page);

    db<void>(TRC) << "log_addr = " << reinterpret_cast<void *>(log_addr) << endl;
    db<void>(TRC) << "MMU::align_page(log_addr) = " << MMU::align_page(log_addr) << endl;
    db<void>(TRC) << "size in pages = " << MMU::pages(size) << endl;
    db<void>(TRC) << "last = " << reinterpret_cast<void *>(last) << endl;

    for (unsigned long addr = log_addr; addr < last; addr += 0x1000) {
        phy_addr = physical_address(addr, &page_frame_present);

        if (page_frame_present) {
            if (dump) {
                db<void>(WRN) << reinterpret_cast<void *>(addr) << ":\t" << reinterpret_cast<void *>(phy_addr) << endl;
            }
        }
        else {
            db<void>(ERR) << "Error! Unmapped logical address = " << reinterpret_cast<void *>(addr) << endl;
        }
    }
}

void IA32_MMU_Aux::set_as_read_only(unsigned long log_addr, unsigned long size, bool user)
{
    if (user) {
        set_flags(log_addr, size, MMU::IA32_Flags::APP_CODE);
    }
    else {
        set_flags(log_addr, size, MMU::IA32_Flags::SYS_CODE);
    }
}

void IA32_MMU_Aux::set_flags(unsigned long log_addr, unsigned long size, unsigned long flags)
{
    Reg32 current_page_directory = CPU::cr3();
    db<void>(TRC) << "In place: " << reinterpret_cast<void *>(current_page_directory) << endl;

    if (log_addr != MMU::align_page(log_addr)) {
        db<void>(ERR) << "Error: address must be page-aligned." << endl;
    }

    unsigned long last = MMU::align_page(log_addr) + MMU::pages(size) * sizeof(MMU::Page);

    db<void>(TRC) << "log_addr = " << reinterpret_cast<void *>(log_addr) << endl;
    db<void>(TRC) << "MMU::align_page(log_addr) = " << MMU::align_page(log_addr) << endl;
    db<void>(TRC) << "size in pages = " << MMU::pages(size) << endl;
    db<void>(TRC) << "last (not included) = " << reinterpret_cast<void *>(last) << endl;

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
                db<void>(ERR) << "Error! Unmapped logical address = " << reinterpret_cast<void *>(addr) << endl;
            }
        }
        else {
            db<void>(ERR) << "Error! Unmapped logical address = " << reinterpret_cast<void *>(addr) << endl;
        }
    }
}

void IA32_MMU_Aux::check_mapping(const char * component_name, unsigned long log_addr, unsigned long expected_phy_addr)
{
    unsigned long page_frame_present = 0;

    unsigned long obtained_phy_addr = MMU_Aux::physical_address(log_addr, &page_frame_present);

    if (! page_frame_present) {
        db<void>(ERR) << "Warning! " << component_name << " not mapped at logical address: " << reinterpret_cast<void *>(log_addr) << endl;
    }

    if (expected_phy_addr != obtained_phy_addr) {
        db<void>(ERR) << "Error on " << component_name << " mapping!\n"
                        << "The address: " << reinterpret_cast<void *>(log_addr) << "\n"
                        << "is expected to map to: " << reinterpret_cast<void *>(expected_phy_addr) << "\n"
                        << "however, it mapped to: " << reinterpret_cast<void *>(obtained_phy_addr) << " instead."
                        << endl;
    }
}


void IA32_MMU_Aux::check_memory_mapping()
{
    check_mapping("Local APIC",             Memory_Map<PC>::APIC, 0xfee00000);
    check_mapping("I/O APIC",               Memory_Map<PC>::IO_APIC, 0xfec00000);
    check_mapping("VGA",                    Memory_Map<PC>::VGA, 0x000b8000);

    if (Traits<Build>::RUNNING_ON_QEMU) {
        check_mapping("PCI",                Memory_Map<PC>::PCI, 0xfe000000);
    }
    else { /* Assuming HP xw4600 Workstation (Intel Core 2 Quad Q9550) */
        check_mapping("PCI",                Memory_Map<PC>::PCI, 0xf0000000);
    }

    check_mapping("SYS",                    Memory_Map<PC>::SYS, 0x0ffff000);
    check_mapping("IDT",                    Memory_Map<PC>::IDT, 0x0ffff000);
    check_mapping("GDT",                    Memory_Map<PC>::GDT, 0x0fffe000);
    check_mapping("SYS_PT",                 Memory_Map<PC>::SYS_PT, 0x0fffd000);
    check_mapping("SYS_PD",                 Memory_Map<PC>::SYS_PD, 0x0fffc000);
    check_mapping("SYS_INFO",               Memory_Map<PC>::SYS_INFO, 0x0fffb000);

    check_mapping("TSSO",                   Memory_Map<PC>::TSS0, 0x0fffa000);
    check_mapping("TSS1",                   Memory_Map<PC>::TSS0 + 1 * 4096, 0x0fff9000);
    check_mapping("TSS2",                   Memory_Map<PC>::TSS0 + 2 * 4096, 0x0fff8000);
    check_mapping("TSS3",                   Memory_Map<PC>::TSS0 + 3 * 4096, 0x0fff7000);
    check_mapping("TSS4",                   Memory_Map<PC>::TSS0 + 4 * 4096, 0x0fff6000);
    check_mapping("TSS5",                   Memory_Map<PC>::TSS0 + 5 * 4096, 0x0fff5000);
    check_mapping("TSS6",                   Memory_Map<PC>::TSS0 + 6 * 4096, 0x0fff4000);
    check_mapping("TSS7",                   Memory_Map<PC>::TSS7, 0x0fff3000);

    check_mapping("SYS_CODE",               Memory_Map<PC>::SYS_CODE, 0x0ff73000);
    check_mapping("SYS_SHARED",             Memory_Map<PC>::SYS_SHARED, 0x0ff72000);
    check_mapping("SYS_DATA",               Memory_Map<PC>::SYS_DATA, 0x0fef2000);
    check_mapping("SYS_STACK",              Memory_Map<PC>::SYS_STACK, 0x0fed2000);
    check_mapping("SYS_HEAP",               Memory_Map<PC>::SYS_HEAP, 0x0005c000);

    // Task Master
    if (CPU::cr3() == 0x0fffc000) {
        for (unsigned long log_addr = 0x00000000; log_addr <= 0x0ffff000 ; log_addr += 0x1000) {
            check_mapping("Low logical", log_addr, log_addr); // Identity-mapped memory
        }
    }

    for (unsigned long addr = 0x00000000; addr <= 0x0ffff000 ; addr += 0x1000) {
        check_mapping("Med logical", 0x80000000 | addr, addr);
    }
}

__END_SYS

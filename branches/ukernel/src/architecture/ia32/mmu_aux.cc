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
    unsigned long * page_frame_present = 0;

    unsigned long last = MMU::align_page(log_addr) + MMU::pages(size) * sizeof(MMU::Page);

    db<void>(TRC) << "log_addr = " << reinterpret_cast<void *>(log_addr) << endl;
    db<void>(TRC) << "MMU::align_page(log_addr) = " << MMU::align_page(log_addr) << endl;
    db<void>(TRC) << "size in pages = " << MMU::pages(size) << endl;
    db<void>(TRC) << "last = " << reinterpret_cast<void *>(last) << endl;

    for (unsigned long addr = log_addr; addr < last; addr += 0x1000) {
        phy_addr = physical_address(addr, page_frame_present);

        if (*page_frame_present) {
            if (dump) {
                db<void>(WRN) << reinterpret_cast<void *>(addr) << ":\t" << reinterpret_cast<void *>(phy_addr) << endl;
            }
        }
        else {
            db<void>(ERR) << "Error! Unmapped logical address = " << reinterpret_cast<void *>(addr) << endl;
        }
    }
}


void IA32_MMU_Aux::check_mapping(const char * component_name, unsigned long log_addr, unsigned long expected_phy_addr)
{
    unsigned long * page_frame_present = 0;

    unsigned long obtained_phy_addr = MMU_Aux::physical_address(log_addr, page_frame_present);

    if (! (*page_frame_present)) {
        db<void>(WRN) << "Warning! " << component_name << " not mapped at logical address: " << reinterpret_cast<void *>(log_addr) << endl;
    }

    if (expected_phy_addr != obtained_phy_addr) {
        db<void>(WRN) << "Error on " << component_name << " mapping!\n"
                        << "The address: " << reinterpret_cast<void *>(log_addr) << "\n"
                        << "is expected to map to: " << reinterpret_cast<void *>(expected_phy_addr) << "\n"
                        << "however, it mapped to: " << reinterpret_cast<void *>(obtained_phy_addr) << " instead."
                        << endl;
    }
}


void IA32_MMU_Aux::check_memory_mapping()
{
    check_mapping("Local APIC",             0xf0000000, 0xfee00000);
    check_mapping("VGA",                    0xf0001000, 0x000b8000);

    if (Traits<Build>::RUNNING_ON_QEMU) {
        check_mapping("PCI",                    0xf0011000, 0xfe000000);
    }
    else { /* Assuming HP xw4600 Workstation (Intel Core 2 Quad Q9550) */
        check_mapping("PCI",                    0xf0011000, 0xf0000000);
    }

    check_mapping("SYS",                    0xff400000, 0x0ffff000);
    check_mapping("IDT",                    0xff400000, 0x0ffff000);
    check_mapping("GDT",                    0xff401000, 0x0fffe000);
    check_mapping("SYS_PT",                 0xff402000, 0x0fffd000);
    check_mapping("SYS_PD",                 0xff403000, 0x0fffc000);
    check_mapping("SYS_INFO",               0xff404000, 0x0fffb000);

    check_mapping("TSSO",                   0xff405000, 0x0fffa000);
    check_mapping("TSS1",                   0xff406000, 0x0fff9000);
    check_mapping("TSS2",                   0xff407000, 0x0fff8000);
    check_mapping("TSS3",                   0xff408000, 0x0fff7000);
    check_mapping("TSS4",                   0xff409000, 0x0fff6000);
    check_mapping("TSS5",                   0xff40a000, 0x0fff5000);
    check_mapping("TSS6",                   0xff40b000, 0x0fff4000);
    check_mapping("TSS7",                   0xff40c000, 0x0fff3000);

    check_mapping("SYS_CODE",               0xff700000, 0x0ff73000);
    check_mapping("SYS_SHARED",             0xff740000, 0x0ff72000);
    check_mapping("SYS_DATA",               0xff741000, 0x0fef2000);
    check_mapping("SYS_STACK",              0xff7c1000, 0x0fed2000);
    check_mapping("SYS_HEAP",               0xff801000, 0x0005c000);

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

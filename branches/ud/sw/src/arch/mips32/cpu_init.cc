// EPOS MIPS32 CPU Mediator Initialization

#include <cpu.h>
#include <mmu.h>
#include <system.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void MIPS32::init()
{
    db<Init, MIPS32>(TRC) << "MIPS32::init()" << endl;

    // This is a temporary place for this information, since it was machine
    // dependable and should not be included in the arch_init
    System_Info<Machine> * si = System::info();

    // Boot map
    si->bm.mem_base = Memory_Map<Machine>::MEM_BASE;    // Memory base address
    si->bm.mem_top = Memory_Map<Machine>::MEM_TOP;      // Memory top address
    si->bm.io_mem_base = 0x20000000;                    // I/O Memory base address
    si->bm.io_mem_top = 0x20010000;                     // I/O Memory top address

    // Physical memory
    si->pmm.mem_base = Memory_Map<Machine>::MEM_BASE;
    si->pmm.mem_top = Memory_Map<Machine>::MEM_TOP;
    si->pmm.io_mem_base = 0x20000000;
    si->pmm.io_mem_top = 0x20010000;
    si->pmm.int_vec = Memory_Map<Machine>::INT_VEC;
    si->pmm.sys_code = Memory_Map<Machine>::SYS_CODE;
    si->pmm.sys_data = Memory_Map<Machine>::SYS_DATA;
    si->pmm.sys_stack = Memory_Map<Machine>::SYS_STACK;
    si->pmm.free_base = 0x1000D000; //0x10020000; //////////(unsigned)&__bss_end;
    //si->pmm.free_top = Memory_Map<Machine>::MEM_TOP - Traits<Machine>::SYSTEM_STACK_SIZE;
    si->pmm.free_top = Memory_Map<Machine>::MEM_TOP;
    si->bm.extras_offset = -1;//?????
    si->lm.has_sys = 0;

    // Initialize the MMU
    if(Traits<MIPS32_MMU>::enabled)
        MIPS32_MMU::init();
    else
        db<Init, MIPS32>(WRN) << "MMU is disabled!" << endl;
}

__END_SYS

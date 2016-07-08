// EPOS Task Abstraction Initialization

#include <utility/elf.h>
#include <mmu.h>
#include <system.h>
#include <task.h>

__BEGIN_SYS

void Task::init()
{
    db<Init, Task>(TRC) << "Task::init()" << endl;

    System_Info<Machine> * si = System::info();

    _master = new (SYSTEM) Task (new (SYSTEM) Address_Space(MMU::current()),
                                 new (SYSTEM) Segment(Log_Addr(si->lm.app_code), si->lm.app_code_size),
                                 new (SYSTEM) Segment(Log_Addr(si->lm.app_data), si->lm.app_data_size),
                                 CPU::Log_Addr(Memory_Map<Machine>::APP_CODE),
                                 CPU::Log_Addr(Memory_Map<Machine>::APP_DATA));

    if(si->bm.extras_offset != -1)
        db<Init>(INF) << "Task:: additional tasks at "  << reinterpret_cast<void *>(si->lm.app_extra) << ":" << si->lm.app_extra_size << endl;

//    MMU::PT_Entry * sys_pd = reinterpret_cast<MMU::PT_Entry *>((void *)si->pmm.sys_pd);
//
//    // Attach memory starting at MEM_BASE
//    for(unsigned int i = MMU::directory(MMU::align_directory(si->pmm.free1_base));
//        i < MMU::directory(MMU::align_directory(si->pmm.free1_top));
//        i++)
//        sys_pd[i] = 0;
//    for(unsigned int i = MMU::directory(MMU::align_directory(si->pmm.free2_base));
//        i < MMU::directory(MMU::align_directory(si->pmm.free2_top));
//        i++)
//        sys_pd[i] = 0;
//    for(unsigned int i = MMU::directory(MMU::align_directory(si->pmm.free3_base));
//        i < MMU::directory(MMU::align_directory(si->pmm.free3_top));
//        i++)
//        sys_pd[i] = 0;
}

__END_SYS

// EPOS ARMv7 MMU Mediator Initialization

#include <mmu.h>
#include <system.h>

//extern "C" unsigned __bss_end__;
//extern "C" unsigned MMUTable;

__BEGIN_SYS

void ARMv7_MMU::init()
{
    db<Init, ARMv7_MMU>(TRC) << "ARMv7_MMU::init()\n";

    /* L1 PT: 4096 entries 4 bytes long.
     * L2 PT: 256 entries 4 bytes long. There is one L2 PT for each entry in L1 (4096).
     * Therefore, MMU page tables size = 4*4096 + 4096*4*256 = 0x404000
     * However, the current implementation maps the 512mb:1023mb into 0:511mb, saving 512*1024 bytes.
     * */

    ARMv7_MMU::build_PTs(); //Build page tables.
    ARMv7_MMU::enable_caches();

    /*unsigned int* p = (unsigned int*)0x1ffffffc;
    unsigned int* q = (unsigned int*)(0x1ffffffc + 0x20000000);
    *p = 0xabcd;
    kout << "*p=" << *p << endl;
    kout << "*q=" << *q << endl;*/

    // TODO: This is causing a prefetch abort
    //db<Init, ARMv7_MMU>(TRC)<<"Enabling MMU...";
    //ARMv7_MMU::enable();
    //db<Init, ARMv7_MMU>(TRC)<<"done.\n";

    /*kout << "*p=" << *p << endl;
    kout << "*q=" << *q << endl;*/

    // let our stack breath!
    //const unsigned int limit =
        //Memory_Map<Machine>::TOP - Traits<Machine>::STACK_SIZE;
    const unsigned int limit = 0x1ffffffc - Traits<Machine>::STACK_SIZE;
    //Getting the memory position right after the end of the MMU Table
    CPU::Phy_Addr base = 0x104000+0x404000-0x80000;
    if(base % 0x4000 != 0)
        base = base + (4 - base % 4);

    db<Init, ARMv7_MMU>(TRC) << "MMU base = " << base << " limit = " << limit << "\n";
    ARMv7_MMU::free(base, pages(limit - base));
    db<Init, ARMv7_MMU>(TRC) << "Memory freed!" << endl;
}

void ARMv7_MMU::disable_domains()
{
    unsigned int x=0xffffffff; //all ones = manager (no domain check).
    ASM("mcr p15,0,%0,c3,c0,0 \n"
            ::"r"(x):);
}

void ARMv7_MMU::disable_access_flags()
{
    unsigned int old, flag = 1<<29;
    ASM("mrc p15,0,%0,c1,c0,0 \n"
         "bic %1, %0, %1 \n"
         "mcr p15,0,%1,c1,c0,0 \n"
         :"=r"(old), "=r"(flag) : : );
}

void ARMv7_MMU::disable()
{
    unsigned int flag;
    ASM("mrc p15, 0, %0, c1, c0, 0 \n"
         "bic %0, %0, #1\n"
         "mcr p15, 0, %0, c1, c0, 0 \n":
         "=r"(flag) : : "cc");
}

void ARMv7_MMU::set_ttbr()
{
    ASM("mcr p15, 0, %0, c2, c0, 0 \n"
         "mcr p15, 0, %0, c2, c0, 1 \n"
         : : "r"(MMU_TABLE_ADDR) : "cc");
}

unsigned int ARMv7_MMU::check_ttbr()
{
    unsigned int ret;
    ASM("mrc p15,0,%0,c2,c0,2 \n"
            :"=r"(ret) : :);
    if((ret&0x7)==0) kout<<"Using TTBR0 only!\n";

    ASM("mrc p15, 0, %0, c2, c0, 0 \n"
         : "=r"(ret): :);
    return ret;
}

void ARMv7_MMU::enable()
{
    ARMv7_MMU::set_ttbr();
    ARMv7_MMU::disable_domains();
    ARMv7_MMU::disable_access_flags();
    unsigned int flag;

    ASM("mrc p15, 0, %0, c1, c0, 0 \n"
         :"=r"(flag) : :);
    flag=(flag|1);
    //flag=(flag|0xc03807);
    ASM(//"orr %0, %0, #0x1 \n"
         "dsb \n"
         "isb \n"
         "mcr p15, 0, %0, c1, c0, 0 \n"
         ::"r"(flag): "cc");

    ASM(
         "dsb \n"
         "and r0, r0, r0 \n"
         "and r0, r0, r0 \n"
         "dsb \n"
         "and r0, r0, r0 \n"
         "and r0, r0, r0 \n"
         "isb \n"
        );
}

void ARMv7_MMU::enable_caches()
{   //setting bits 2 (data cache) and 12 (I-cache).
    unsigned int flag;//, val=0x1004;
    ASM("mrc p15, 0, %0, c1, c0, 0 \n"
         :"=r"(flag) : : );
    flag=flag|0x1004;
    ASM(//"orr %0, %0, %1 \n"
         "mcr p15, 0, %0, c1, c0, 0 \n"

         "and r0, r0, r0\n"
         "and r0, r0, r0\n"
         "dsb\n"
         "and r0, r0, r0\n"
         "and r0, r0, r0\n"
         "isb\n"
         ::"r"(flag) : );
}

__END_SYS

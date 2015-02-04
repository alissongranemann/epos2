// EPOS ARMV7 CPU Mediator Initialization

#include <cpu.h>
#include <mmu.h>
#include <tsc.h>

__BEGIN_SYS

void ARMV7::init()
{
    db<Init, ARMV7>(TRC) << "CPU::init()" << endl;

    if(Traits<MMU>::enabled)
        MMU::init();
    if(Traits<TSC>::enabled)
        TSC::init();
}

__END_SYS

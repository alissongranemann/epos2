// EPOS Cortex Mediator Initialization

#include <machine/cortex/machine.h>

__BEGIN_SYS

void Cortex::init()
{
    db<Init, Cortex>(TRC) << "Cortex::init()" << endl;

    Cortex_Model::init();

    if(Traits<Cortex_IC>::enabled)
        Cortex_IC::init();
    if(Traits<Cortex_Timer>::enabled)
        Cortex_Timer::init();
#ifndef __mmod_zynq__
    if(Traits<Cortex_USB>::enabled)
        Cortex_USB::init();
    if(Traits<Cortex_IEEE802_15_4>::enabled)
        Cortex_IEEE802_15_4::init();
#endif
}

__END_SYS

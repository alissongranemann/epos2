// EPOS Cortex Mediator Initialization

#include <machine/cortex/machine.h>

__BEGIN_SYS

void Machine::init()
{
    db<Init, Machine>(TRC) << "Cortex::init()" << endl;

    Cortex_Model::init();

    if(Traits<IC>::enabled)
        IC::init();
    if(Traits<Timer>::enabled)
        Timer::init();
#ifndef __mmod_zynq__
    if(Traits<USB>::enabled)
        USB::init();
#ifndef __no_networking__
    if(Traits<NIC>::enabled)
        NIC::init();
#endif
#endif
}

__END_SYS

// EPOS Cortex Mediator Initialization

#include <machine/cortex/machine.h>
#include <smart_plug.h>

__BEGIN_SYS

void Machine::init()
{
    db<Init, Machine>(TRC) << "Machine::init()" << endl;

    Machine_Model::init();

    if(Traits<IC>::enabled)
        IC::init();
    if(Traits<Timer>::enabled)
        Timer::init();
    if(Traits<Smart_Plug>::enabled)
        Smart_Plug::init();
#ifdef __USB_H
    if(Traits<USB>::enabled)
        USB::init();
#endif
#ifdef __NIC_H
    if(Traits<NIC>::enabled)
        NIC::init();
#endif
}

__END_SYS

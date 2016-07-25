// EPOS Cortex-A Mediator Initialization

#include <machine/cortex_a/machine.h>

__BEGIN_SYS

void Cortex::init()
{
    db<Init, Cortex>(TRC) << "Cortex::init()" << endl;

    Cortex_Model::init();

    if(Traits<Cortex_IC>::enabled)
        Cortex_IC::init();
    if(Traits<Cortex_Timer>::enabled)
        Cortex_Timer::init();
}

__END_SYS

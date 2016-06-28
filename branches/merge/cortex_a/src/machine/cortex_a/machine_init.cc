// EPOS Cortex-A Mediator Initialization

#include <machine/cortex_a/machine.h>

__BEGIN_SYS

void Cortex_A::init()
{
    db<Init, Cortex_A>(TRC) << "Cortex_A::init()" << endl;

    Cortex_A_Model::init();

    if(Traits<Cortex_A_IC>::enabled)
        Cortex_A_IC::init();
    if(Traits<Cortex_A_Timer>::enabled)
        Cortex_A_Timer::init();
}

__END_SYS

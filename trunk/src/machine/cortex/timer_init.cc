// EPOS Cortex Timer Mediator Initialization

#include <timer.h>
#include <ic.h>

__BEGIN_SYS

void Cortex_Timer::init()
{
    db<Init, Timer>(TRC) << "Timer::init()" << endl;

    Engine::init(FREQUENCY);
    IC::int_vector(IC::INT_TIMER, int_handler);
    // TODO: Maybe the ifdef isn't necessary. Where is INT_TIMER enabled on
    // Cortex-M?
#ifdef __mmod_zynq__
    IC::enable(IC::INT_TIMER);
#endif
    Engine::enable();
}

__END_SYS

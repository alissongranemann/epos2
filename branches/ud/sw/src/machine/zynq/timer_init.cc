// EPOS Zynq Timer Mediator Initialization

#include <timer.h>

__BEGIN_SYS

void Zynq_Timer::init()
{
    db<Init, Timer>(TRC) << "Timer::init()" << endl;

    status(INTERRUPT_CLEAR);
    control(TIMER_AUTO_RELOAD | TIMER_IT_ENABLE);
    IC::int_vector(IC::TIMERINT0, &Zynq_Timer::int_handler);
    IC::enable(IC::TIMERINT0);
}

__END_SYS

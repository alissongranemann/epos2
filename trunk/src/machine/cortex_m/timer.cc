// EPOS Cortex_M Timer Mediator Implementation

#include <machine.h>
#include <ic.h>
#include <machine/cortex_m/timer.h>

__BEGIN_SYS

// Class attributes
//Cortex_M_Timer::Handler* Cortex_M_Timer::handlers[4];
Cortex_M_Timer * Cortex_M_Timer::_channels[CHANNELS];

// Class methods
void Cortex_M_Timer::int_handler(const Interrupt_Id & i)
{
    if(_channels[SCHEDULER] && (--_channels[SCHEDULER]->_current[Machine::cpu_id()] <= 0)) {
        _channels[SCHEDULER]->_current[Machine::cpu_id()] = _channels[SCHEDULER]->_initial;
        _channels[SCHEDULER]->_handler();
    }

    if((!Traits<System>::multicore || (Traits<System>::multicore && (Machine::cpu_id() == 0))) && _channels[ALARM]) {
        _channels[ALARM]->_current[0] = _channels[ALARM]->_initial;
        _channels[ALARM]->_handler();
    }

    if((!Traits<System>::multicore || (Traits<System>::multicore && (Machine::cpu_id() == 0))) && _channels[USER]) {
        if(_channels[USER]->_retrigger)
            _channels[USER]->_current[0] = _channels[USER]->_initial;
        _channels[USER]->_handler();
    }
}

__END_SYS

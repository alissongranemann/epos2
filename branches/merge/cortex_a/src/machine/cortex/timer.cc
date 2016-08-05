// EPOS Cortex Timer Mediator Implementation

#include <machine.h>
#include <ic.h>
#include <machine/cortex/timer.h>

__BEGIN_SYS

// Class attributes
//Cortex_Timer::Handler* Cortex_Timer::handlers[4];
Cortex_Timer * Cortex_Timer::_channels[CHANNELS];

// Class methods
void Cortex_Timer::int_handler(const Interrupt_Id & i)
{
    // When emulating Zynq on on, the bit the triggered the interrupt must be
    // cleared before re-enabling them to avoid calling the same interrupt in a
    // loop.
#ifdef __mmod_zynq__
    isr_clr();
    CPU::int_enable();
#endif

    if(_channels[SCHEDULER] && (--_channels[SCHEDULER]->_current[Machine::cpu_id()] <= 0)) {
        _channels[SCHEDULER]->_current[Machine::cpu_id()] = _channels[SCHEDULER]->_initial;
        _channels[SCHEDULER]->_handler(i);
    }

    if((!Traits<System>::multicore || (Traits<System>::multicore && (Machine::cpu_id() == 0))) && _channels[ALARM]) {
        _channels[ALARM]->_current[0] = _channels[ALARM]->_initial;
        _channels[ALARM]->_handler(i);
    }
}

__END_SYS

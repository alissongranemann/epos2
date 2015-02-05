// EPOS Zynq Timer Mediator Implementation

#include <machine/zynq/timer.h>
#include <machine.h>

__BEGIN_SYS

Zynq_Timer * Zynq_Timer::_channels[CHANNELS] = {0,0,0};

void Zynq_Timer::int_handler(const IC::Interrupt_Id & id)
{
    if((!Traits<System>::multicore || (Traits<System>::multicore && (Machine::cpu_id() == 0))) && _channels[ALARM])
        _channels[ALARM]->_handler(id);

    if(_channels[SCHEDULER] && (--_channels[SCHEDULER]->_current[Machine::cpu_id()] <= 0)) {
        _channels[SCHEDULER]->_current[Machine::cpu_id()] = _channels[SCHEDULER]->_initial;
        _channels[SCHEDULER]->_handler(id);
    }
}

__END_SYS

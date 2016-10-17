// EPOS Cortex-M MAC Timer Mediator Implementation

#include <timer.h>

#ifdef __nic_timer_h

__BEGIN_SYS

// Class attributes
volatile CPU::Reg32 NIC_Timer::_overflow_count;
volatile CPU::Reg32 NIC_Timer::_ints;
NIC_Timer::Time_Stamp NIC_Timer::_int_request_time;
NIC_Timer::Time_Stamp NIC_Timer::_offset;
NIC_Timer::Handler NIC_Timer::_handler;
bool NIC_Timer::_overflow_match;
bool NIC_Timer::_msb_match;

// Methods

__END_SYS

#endif

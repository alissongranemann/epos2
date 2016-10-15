// EPOS ARMv7 Time-Stamp Counter Mediator Implementation

#include <tsc.h>

__BEGIN_SYS

#if defined(__mmod_emote3__) || defined(__mod_lm3S811__)

volatile TSC::Time_Stamp TSC::_overflow = 0;

#endif

__END_SYS

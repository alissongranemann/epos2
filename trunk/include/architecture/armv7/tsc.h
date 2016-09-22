// EPOS ARMv7 Time-Stamp Counter Mediator Declarations

#ifndef __armv7_tsc_h
#define __armv7_tsc_h

#include <tsc.h>

__BEGIN_SYS

class TSC: private TSC_Common
{
    friend class CPU;

private:
    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

public:
    using TSC_Common::Hertz;
    using TSC_Common::Time_Stamp;

public:
    TSC() {}

    static Hertz frequency() { return CLOCK; }

    static Time_Stamp time_stamp();

private:
    static void init() {}
};

__END_SYS

#endif

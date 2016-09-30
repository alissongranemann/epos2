// EPOS ARMv7 Time-Stamp Counter Mediator Declarations

#ifndef __armv7_tsc_h
#define __armv7_tsc_h

#include <cpu.h>
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

    static Time_Stamp time_stamp() {
        // FIXME: The MRC instruction doesn't to work correctly if ts is a Time_Stamp, but types are now incopatble
        CPU::Reg32 ts;
        ASM("mrc p15, 0, %0, c9, c13, 0" : "=r"(ts));
        return static_cast<Time_Stamp>(ts);
    }

private:
    static void init();
};

__END_SYS

#endif

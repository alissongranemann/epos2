// EPOS ARMV7 Time-Stamp Counter Mediator Declarations

#ifndef __armv7_tsc_h
#define __armv7_tsc_h

#include <cpu.h>
#include <tsc.h>

__BEGIN_SYS

// ARMV7_TSC is implemented using Cortex A9's 32-bit cycle counter register
// which isn't present in all ARMv7 processors. See ARM Architecture Reference
// Manual for further information.
class ARMV7_TSC: public TSC_Common
{
    friend class ARMV7;

public:
    ARMV7_TSC() {}

    static Hertz frequency() { return ARMV7::clock(); }

    static Time_Stamp time_stamp() {
        // The MRC instruction doesn't to work correctly if ts is a Time_Stamp
        CPU::Reg32 ts;
        ASMV("mrc p15, 0, %0, c9, c13, 0" : "=r"(ts));
        return ((Time_Stamp)ts);
    }

private:
    static void init();
};

__END_SYS

#endif

// EPOS IA32 Time-Stamp Counter Mediator Declarations

#ifndef __ia32_tsc_h
#define __ia32_tsc_h

#include <cpu.h>
#include <tsc.h>

__BEGIN_SYS

class IA32_TSC: private TSC_Common
{
public:
    using TSC_Common::Hertz;
    using TSC_Common::Time_Stamp;

public:
    IA32_TSC() {}

    static Hertz frequency() { return CPU::clock(); }

    /*! According to
     * https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html (x86 family)
     * This implementation variant is suitable for i386 and also for x86-64.
     * */
    static Time_Stamp time_stamp() {
        Time_Stamp ts;
        unsigned int hi;
        unsigned int lo;
        ASM("rdtsc" : "=a" (lo), "=d" (hi));

        ts = ((Time_Stamp) hi << 32) | lo;

        return ts;
    }

    /*! According to
     * https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html (x86 family)
     * This implementation variant is suitable for i386 but not for x86-64.
     * */
    static Time_Stamp time_stamp_i386() {
        Time_Stamp ts;
        ASM("rdtsc" : "=A" (ts) : ); // must be volatile!
        return ts;
    }
};

__END_SYS

#endif

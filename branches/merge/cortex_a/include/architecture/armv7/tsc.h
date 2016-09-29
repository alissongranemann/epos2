// EPOS ARMv7 Time-Stamp Counter Mediator Declarations

#ifndef __armv7_tsc_h
#define __armv7_tsc_h

#include <cpu.h>
#include <tsc.h>

__BEGIN_SYS

#ifdef __mmod_zynq__

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
        // FIXME: The MRC instruction doesn't seem to work correctly if ts is a Time_Stamp, but types are now incompatible
        CPU::Reg32 ts;
        ASM("mrc p15, 0, %0, c9, c13, 0" : "=r"(ts));
        return static_cast<Time_Stamp>(ts);
    }

private:
    static void init();
};

#else

class TSC: private TSC_Common
{
    friend class CPU;

private:
    static const unsigned int CLOCK = Traits<CPU>::CLOCK;
    enum {
        TSC_BASE = Traits<Build>::MODEL == Traits<Build>::eMote3 ? 0x40033000 /*TIMER3_BASE*/ : 0x40031000 /*TIMER1_BASE*/
    };

    enum {
        GPTMTAR = 0x48,
    };

public:
    using TSC_Common::Hertz;
    using TSC_Common::Time_Stamp;

public:
    TSC() {}

    static Hertz frequency() { return CLOCK; }

    // Not supported by LM3S811 on QEMU (version 2.7.50)
    static Time_Stamp time_stamp() { return (_overflow << 32) + reg(GPTMTAR); }

private:
    static void init();

    static void int_handler(const unsigned int & int_id) { _overflow++; }

    static volatile CPU::Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile CPU::Reg32 *>(TSC_BASE)[o / sizeof(CPU::Reg32)]; }

    static volatile Time_Stamp _overflow;
};

#endif

__END_SYS

#endif

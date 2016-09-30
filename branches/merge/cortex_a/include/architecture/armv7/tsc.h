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
    typedef CPU::Reg32 Reg32;
    typedef CPU::Reg64 Reg64;

    static const unsigned int CLOCK = Traits<CPU>::CLOCK/2;

    // Base address for Global Timer
    enum {
        GLOBAL_TIMER_BASE = 0xF8F00200,
    };

    // Global Timer Registers offsets
    enum {              // Description
        GTCTRL  = 0x00, // Low Counter
        GTCTRH  = 0x04, // High Counter
        GTCLR   = 0x08, // Control
    };

public:
    using TSC_Common::Hertz;
    using TSC_Common::Time_Stamp;

public:
    TSC() {}

    static Hertz frequency() { return CLOCK; }

    static Time_Stamp time_stamp() { return static_cast<Time_Stamp>(read()); }

private:
    static void init();

    static volatile Reg32 & global_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GLOBAL_TIMER_BASE)[o / sizeof(Reg32)]; }

    static Reg64 read() {
        Reg32 high, low;

        do {
            high = global_timer(GTCTRH);
            low = global_timer(GTCTRL);
        } while(global_timer(GTCTRH) != high);

        return static_cast<Reg64>(high) << 32 | low;
    }

    static void set(const Reg64 & count) {
        // Disable counting before programming
        global_timer(GTCLR) = 0;

        global_timer(GTCTRL) = count & 0xffffffff;
        global_timer(GTCTRH) = count >> 32;

        // Re-enable counting
        global_timer(GTCLR) = 1;
    }
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

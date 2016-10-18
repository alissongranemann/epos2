// EPOS ARMv7 Time-Stamp Counter Mediator Declarations

#ifndef __armv7_tsc_h
#define __armv7_tsc_h

#include <cpu.h>
#include <tsc.h>

__BEGIN_SYS

class TSC: private TSC_Common
{
    friend class CPU;
    friend class IC;

private:
    static const unsigned int CLOCK = Traits<CPU>::CLOCK / (Traits<Build>::MODEL == Traits<Build>::Zynq ? 2 : 1);

    enum {
        TSC_BASE =
            Traits<Build>::MODEL == Traits<Build>::eMote3  ? 0x40033000 /*TIMER3_BASE*/ :
            Traits<Build>::MODEL == Traits<Build>::LM3S811 ? 0x40031000 /*TIMER1_BASE*/ :
            Traits<Build>::MODEL == Traits<Build>::Zynq ? 0xF8F00200 /*GLOBAL_TIMER_BASE*/ : 
            0
    };

    // Cortex-M3 GPTM registers offsets
    enum {              // Description
        GPTMTAR = 0x48, // Counter
    };

    // Zynq Global Timer Registers offsets
    enum {             // Description
        GTCTRL = 0x00, // Low Counter
        GTCTRH = 0x04, // High Counter
        GTCLR  = 0x08, // Control
        GTISR  = 0x0C  // Interrupt Status
    };

public:
    using TSC_Common::Hertz;
    using TSC_Common::Time_Stamp;

public:
    TSC() {}

    static Hertz frequency() { return CLOCK; }

#ifdef __mmod_zynq__
    static Time_Stamp time_stamp() {
        if(sizeof(Time_Stamp) == sizeof(CPU::Reg32))
            return reg(GTCTRL);

        Time_Stamp high;
        CPU::Reg32 low;

        do {
            high = reg(GTCTRH);
            low = reg(GTCTRL);
        } while(reg(GTCTRH) != high);

        return (high << 32) | low;
    }
#else
    // Not supported by LM3S811 on QEMU (version 2.7.50)
    static Time_Stamp time_stamp() { return (_overflow << 32) + reg(GPTMTAR); }
#endif

private:
    static void init();

    static volatile CPU::Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile CPU::Reg32 *>(TSC_BASE)[o / sizeof(CPU::Reg32)]; }

#ifndef __mmod_zynq__
    static void int_handler(const unsigned int & int_id) { _overflow++; }

    static volatile Time_Stamp _overflow;
#endif
};

__END_SYS

#endif
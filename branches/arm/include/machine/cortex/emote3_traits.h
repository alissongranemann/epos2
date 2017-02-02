// EPOS EPOSMoteIII (ARM Cortex-M3) MCU Metainfo and Configuration

#ifndef __machine_traits_h
#define __machine_traits_h

#include <system/config.h>

__BEGIN_SYS

class Machine_Common;
template<> struct Traits<Machine_Common>: public Traits<void>
{
    static const bool debugged = Traits<void>::debugged;
};

template<> struct Traits<Machine>: public Traits<Machine_Common>
{
    static const unsigned int CPUS = Traits<Build>::CPUS;

    // Physical Memory
    static const unsigned int MEM_BASE  = 0x20000004;
    static const unsigned int MEM_TOP   = 0x20007ff7; // 32 KB (MAX for 32-bit is 0x70000000 / 1792 MB)

    // Logical Memory Map
    static const unsigned int APP_LOW   = 0x20000004;
    static const unsigned int APP_CODE  = 0x00204000;
    static const unsigned int APP_DATA  = 0x20000004;
    static const unsigned int APP_HIGH  = 0x20007ff7;

    static const unsigned int PHY_MEM   = 0x20000004;
    static const unsigned int IO_BASE   = 0x40000000;
    static const unsigned int IO_TOP    = 0x440067ff;

    static const unsigned int SYS       = 0x00204000;
    static const unsigned int SYS_CODE  = 0x00204000; // Library mode only => APP + SYS
    static const unsigned int SYS_DATA  = 0x20000004; // Library mode only => APP + SYS

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE = 3 * 1024;
    static const unsigned int HEAP_SIZE = 3 * 1024;
    static const unsigned int MAX_THREADS = 7;
};

template<> struct Traits<IC>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Timer>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;

    // Meaningful values for the timer frequency range from 100 to
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template<> struct Traits<SPI>: public Traits<Machine_Common>
{
    static const unsigned int UNITS = 1;
};

template<> struct Traits<UART>: public Traits<Machine_Common>
{
    static const unsigned int UNITS = 2;

    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

    static const unsigned int DEF_UNIT = 0;
    static const unsigned int DEF_BAUD_RATE = 115200;
    static const unsigned int DEF_DATA_BITS = 8;
    static const unsigned int DEF_PARITY = 0; // none
    static const unsigned int DEF_STOP_BITS = 1;
};

template<> struct Traits<USB>: public Traits<Machine_Common>
{
    // Some observed objects are created before initializing the Display, which may use the USB.
    // Enabling debug may cause trouble in some Machines
    static const bool debugged = false;

    static const unsigned int UNITS = 1;
    static const bool blocking = false;
    static const bool enabled = Traits<Serial_Display>::enabled && (Traits<Serial_Display>::ENGINE == Traits<Serial_Display>::USB);
};

template<> struct Traits<Watchdog>: public Traits<Machine_Common>
{
    static const bool enabled = true;

    enum {
        MS_1_9,    // 1.9ms
        MS_15_625, // 15.625ms
        S_0_25,    // 0.25s
        S_1,       // 1s
    };
    static const int PERIOD = S_1;
};

template<> struct Traits<Scratchpad>: public Traits<Machine_Common>
{
    static const bool enabled = false;
};

template<> struct Traits<NIC>: public Traits<Machine_Common>
{
    static const bool enabled = (Traits<Build>::NODES > 1);
    static const bool promiscuous = false;

    typedef LIST<CC2538> NICS;
    static const unsigned int UNITS = NICS::Length;
};

template<> struct Traits<CC2538>: public Traits<NIC>
{
    static const unsigned int UNITS = NICS::Count<CC2538>::Result;
    static const unsigned int RECEIVE_BUFFERS = 16; // per unit
    static const bool gpio_debug = false;
};

__END_SYS

#endif

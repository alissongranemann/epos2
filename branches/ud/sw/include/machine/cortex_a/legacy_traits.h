#ifndef __cortex_a_traits_h
#define __cortex_a_traits_h

#include <system/config.h>

__BEGIN_SYS

class Cortex_A_Common;
template <> struct Traits<Cortex_A_Common>: public Traits<void>
{
    static const bool debugged = Traits<void>::debugged;
};

template <> struct Traits<Cortex_A>: public Traits<Cortex_A_Common>
{
    static const unsigned int CPUS = Traits<Build>::CPUS;

    // Physical Memory
    // Using only DDR memory for data, OCM doesn't support exclusive accesses
    // needed for atomic operations. The vector table must be placed at
    // 0x00000000.
    static const unsigned int MEM_BASE  = 0x00000000;
    static const unsigned int MEM_TOP   = 0x080FFFFF; // 512 MB

    // Logical Memory Map
    static const unsigned int APP_LOW   = 0x00000000;
    static const unsigned int APP_CODE  = 0x00000000;
    static const unsigned int APP_DATA  = 0x03100000; // 192 MB
    static const unsigned int APP_HIGH  = 0x06100000; // 384 MB

    // TODO: Use real values
    static const unsigned int PHY_MEM   = 0x00000000;
    static const unsigned int IO_BASE   = 0x00000000;
    static const unsigned int IO_TOP    = 0x00000000;

    static const unsigned int SYS       = 0x06100000;
    static const unsigned int SYS_CODE  = 0x06100000;
    static const unsigned int SYS_DATA  = 0x07100000;

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE = 16 * 1024;
    static const unsigned int HEAP_SIZE = 16 * 1024 * 1024;
    static const unsigned int MAX_THREADS = 16;
};

template <> struct Traits<Cortex_A_IC>: public Traits<Cortex_A_Common>
{
};

template <> struct Traits<Cortex_A_Timer>: public Traits<Cortex_A_Common>
{
    static const bool debugged = hysterically_debugged;

    // Meaningful values for the timer frequency range from 100 to 10000 Hz. The
    // choice must respect the scheduler time-slice, i. e., it must be higher
    // than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<Cortex_A_UART>: public Traits<Cortex_A_Common>
{
    static const unsigned int BAUD_RATE = 115200;
};

template <> struct Traits<Cortex_A_NIC>: public Traits<Cortex_A_Common>
{
    static const unsigned int UNITS = 0;
};

template<> struct Traits<Cortex_A_Scratchpad>: public Traits<Cortex_A_Common>
{
    static const bool enabled = false;
};

__END_SYS

#endif

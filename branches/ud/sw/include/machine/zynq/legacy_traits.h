#ifndef __zynq_traits_h
#define __zynq_traits_h

#include <system/config.h>

__BEGIN_SYS

class Zynq_Common;
template <> struct Traits<Zynq_Common>: public Traits<void>
{
    static const bool debugged = Traits<void>::debugged;
};

template <> struct Traits<Zynq>: public Traits<Zynq_Common>
{
    static const unsigned int CPUS = Traits<Build>::CPUS;

    // Physical Memory
    static const unsigned int MEM_BASE  = 0x00000000;
    static const unsigned int MEM_TOP   = 0x20000000;

    // Logical Memory Map
    static const unsigned int APP_LOW   = 0x82000000;
    static const unsigned int APP_CODE  = 0x00000000; // To place the vector table at 0x0.
    static const unsigned int APP_DATA  = 0x00100340;
    static const unsigned int APP_HIGH  = 0x83EFFFFF;

    static const unsigned int PHY_MEM   = 0x20000000;
    static const unsigned int IO_BASE   = 0x48000000;
    //static const unsigned int IO_TOP    = 0x00000000;

    static const unsigned int SYS       = 0x00100140;
    static const unsigned int SYS_CODE  = 0x00160140;
    static const unsigned int SYS_DATA  = 0x00170140;

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE = 1024 * 4096;
    static const unsigned int HEAP_SIZE = 16 * 1024 * 1024;
    static const unsigned int MAX_THREADS = 16;
};

template <> struct Traits<Zynq_IC>: public Traits<Zynq_Common>
{
    static const bool enabled = true;
};

template <> struct Traits<Zynq_Timer>: public Traits<Zynq_Common>
{
    static const bool enabled = true;
    static const bool prescale = true;

    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<Zynq_UART>: public Traits<Zynq_Common>
{
    static const unsigned int BAUD_RATE = 115200;
};

template <> struct Traits<Zynq_NIC>: public Traits<Zynq_Common>
{
    static const unsigned int UNITS = 0;
};

template<> struct Traits<Zynq_Scratchpad>: public Traits<Zynq_Common>
{
    static const bool enabled = false;
};

__END_SYS

#endif

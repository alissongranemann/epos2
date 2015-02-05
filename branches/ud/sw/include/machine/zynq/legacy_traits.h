#ifndef __zynq_traits_h
#define __zynq_traits_h

__BEGIN_SYS

#include <system/config.h>

class Zynq_Common;
template <> struct Traits<Zynq_Common>: public Traits<void>
{
    static const bool debugged = Traits<void>::debugged;
};

template <> struct Traits<Zynq>: public Traits<Zynq_Common>
{
	static const unsigned int MAX_CPUS = 2;

	static const unsigned int CPUS = 1;
	static const unsigned int CLOCK = 666667000;

    // Boot Image
	static const unsigned int BOOT_LENGTH_MIN   = 128;
	static const unsigned int BOOT_LENGTH_MAX   = 512;

    // Physical Memory
	static const unsigned int MEM_BASE  = 0x00000000;
	static const unsigned int MEM_TOP   = 0x20000000;

    // Logical Memory Map
	static const unsigned int APP_CODE  = 0x00000000; // To place the vector table at 0x0.
	static const unsigned int APP_DATA  = 0x00100340;

	static const unsigned int SYS       = 0x00100140; // After undef stack, but before APP_DATA (sys_info uses 260 bytes)
	static const unsigned int SYS_HEAP  = 0x00508000; // SYS_HEAP comes right after APP_DATA, which is no bigger than 1 KB.

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE = 1024 * 4096;
    static const unsigned int HEAP_SIZE = 16 * 1024 * 1024;
    static const unsigned int MAX_THREADS = 16;

	static const unsigned int enabled = true;
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

template<> struct Traits<Zynq_Scratchpad>: public Traits<Zynq_Common>
{
    static const bool enabled = false;
};

__END_SYS

#endif


#ifndef __epossoc_traits_h
#define __epossoc_traits_h

__BEGIN_SYS

class EPOSSOC_Common;
template <> struct Traits<EPOSSOC_Common>: public Traits<void>
{
    static const bool debugged = Traits<void>::debugged;;
};

template <> struct Traits<EPOSSOC>: public Traits<EPOSSOC_Common>
{
    static const unsigned int MAX_CPUS = 1;

    // Boot Image
    // Necessary to run eposmkbi but unused in this machine
    static const unsigned int BOOT_LENGTH_MIN   = 128;
    static const unsigned int BOOT_LENGTH_MAX   = 512;
    static const unsigned int BOOT_IMAGE_ADDR   = 0x00008000;

    // Physical Memory
    static const unsigned int MEM_BASE  = 0x10000010;
    static const unsigned int MEM_TOP   = 0x10010000;

    // Logical Memory Map
    static const unsigned int APP_LOW   = 0x10000010;
    static const unsigned int APP_CODE  = 0x10000010;
    static const unsigned int APP_DATA  = 0x10007100;
    static const unsigned int APP_HIGH  = 0x10008000;

    static const unsigned int PHY_MEM   = 0x10000004;
    static const unsigned int IO_BASE   = 0x20000000;
    static const unsigned int IO_TOP    = 0x20010000;

    static const unsigned int SYS       = APP_HIGH;
    static const unsigned int SYS_CODE  = SYS + 0x00000200;
    static const unsigned int SYS_DATA  = SYS + 0x00002000;

    // TODO: This is a workaround used in cpu_init.cc, should be fixed
    static const unsigned int SYSTEM_STACK_SIZE = 1 * 512;

    static const unsigned int CLOCK = 50000000;

    static const unsigned int LEDS_ADDRESS = 0x80000400;
};

template <> struct Traits<EPOSSOC_NOC>: public Traits<EPOSSOC_Common>
{
    static const unsigned int BASE_ADDRESS = 0x80001000;
    static const unsigned int NOC_DATA_BUS_SIZE = 56;
    static const unsigned int PROC_NOC_BUS_RATE = 2;
    static const unsigned int CONCURRENCY_LIMIT = 4;
};

template <> struct Traits<EPOSSOC_IC>: public Traits<EPOSSOC_Common>
{
    static const unsigned int BASE_ADDRESS = 0x80000C00;
    static const unsigned int EDGE_ENABLE = ~0; // All interrupts configured as edge enabled
    static const unsigned int POLARITY = ~0; // All interrupts configured as raising edge
    static const unsigned int NUMBER_OF_INTERRUPTS = 32;
    static const unsigned int CPU_JMP_ADDRESS = 0x0000003C;
    static const bool ATTEND_ONLY_ONE = true; // Attend only one pending interrupt at each CPU Interrupt.
};

template <> struct Traits<EPOSSOC_UART>: public Traits<EPOSSOC_Common>
{
    static const unsigned int CLOCK = Traits<EPOSSOC>::CLOCK;
    static const unsigned int BASE_ADDRESS = 0x80000000;
    static const unsigned int BAUDRATE = 57600;
};

template <> struct Traits<EPOSSOC_Timer>: public Traits<EPOSSOC_Common>
{
    static const unsigned int CLOCK = Traits<EPOSSOC>::CLOCK;
    static const unsigned int FREQUENCY = 10000; // 10 kHz -> 100us
    static const unsigned int BASE_ADDRESS = 0x80000800;
};

template <> struct Traits<EPOSSOC_NIC>: public Traits<EPOSSOC_Common>
{
    typedef Implementation::LIST<> NICS;
    static const bool enabled = false;
};

template <> struct Traits<EPOSSOC_Component_Controller>: public Traits<EPOSSOC_Common>
{
    static const unsigned int BASE_ADDRESS = 0x80001400;
};

template <> struct Traits<EPOSSOC_PCAP>: public Traits<EPOSSOC_Common>
{
    static const unsigned int BASE_ADDRESS = 0xF8007000;
};

__END_SYS

#endif

// EPOS EPOSMoteIII (Cortex-M3) MCU Metainfo and Configuration

#ifndef __emote3_traits_h
#define __emote3_traits_h

#include <system/config.h>

__BEGIN_SYS

class Cortex_M_Common;
template <> struct Traits<Cortex_M_Common>: public Traits<void>
{
    static const bool debugged = Traits<void>::debugged;
};

template <> struct Traits<Cortex_M>: public Traits<Cortex_M_Common>
{
    static const unsigned int CPUS = Traits<Build>::CPUS;

    // Physical Memory
    static const unsigned int MEM_BASE  = 0x20000004;
    static const unsigned int MEM_TOP   = 0x20007ff7; // (MAX for 32-bit is 0x70000000 / 1792 MB)

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

    // Bootloader Memory Map
    // Word in RAM reserved for the bootloader
    static const unsigned int BOOTLOADER_STATUS_ADDRESS = 0x20007ffc;

    // Physical Memory
    static const unsigned int BOOTLOADER_MEM_BASE  = 0x20000000;
    static const unsigned int BOOTLOADER_MEM_TOP   = 0x20007ff7; // (MAX for 32-bit is 0x70000000 / 1792 MB)

    // Logical Memory Map
    static const unsigned int BOOTLOADER_APP_LOW   = 0x20000000;
    static const unsigned int BOOTLOADER_APP_CODE  = 0x00200000;
    static const unsigned int BOOTLOADER_APP_DATA  = 0x20000000;
    static const unsigned int BOOTLOADER_APP_HIGH  = 0x20007ff7;

    static const unsigned int BOOTLOADER_PHY_MEM   = 0x20000000;

    static const unsigned int BOOTLOADER_SYS       = 0x00200000;
    static const unsigned int BOOTLOADER_SYS_CODE  = 0x00200000; // Library mode only => APP + SYS
    static const unsigned int BOOTLOADER_SYS_DATA  = 0x20000000; // Library mode only => APP + SYS

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE = 3 * 1024;
    static const unsigned int HEAP_SIZE = 3 * 1024;
    static const unsigned int MAX_THREADS = 7;
};

template <> struct Traits<Cortex_M_IC>: public Traits<Cortex_M_Common>
{
    static const bool hysterically_debugged = false;
    static const bool reboot_on_hard_fault = true;
};

template <> struct Traits<Cortex_M_Timer>: public Traits<Cortex_M_Common>
{
    static const bool debugged = hysterically_debugged;

    // Meaningful values for the timer frequency range from 100 to
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<Cortex_M_UART>: public Traits<Cortex_M_Common>
{
    static const unsigned int UNITS = 2;

    static const unsigned int CLOCK = Traits<ARMv7>::CLOCK;

    static const unsigned int DEF_BAUD_RATE = 115200;
    static const unsigned int DEF_DATA_BITS = 8;
    static const unsigned int DEF_PARITY = 0; // none
    static const unsigned int DEF_STOP_BITS = 1;
};

template<> struct Traits<Cortex_M_Display>: public Traits<Cortex_M_Common>
{
    enum Engine {null, uart, usb};
    static const Engine ENGINE = usb;

    static const int COLUMNS = 80;
    static const int LINES = 25;
    static const int TAB_SIZE = 8;
};

template<> struct Traits<Cortex_M_USB_Serial_Display>: public Traits<Cortex_M_Display> { };

template <> struct Traits<Cortex_M_USB>: public Traits<Cortex_M_Common>
{
    static const bool enabled = Traits<Cortex_M_Display>::ENGINE == Traits<Cortex_M_Display>::Engine::usb;
    static const bool blocking = false;

    static const bool debugged = false;
};

template <> struct Traits<Cortex_M_Radio>: public Traits<Cortex_M_Common>
{
    static const bool enabled = (Traits<Build>::NODES > 1);

    typedef LIST<eMote3_IEEE802_15_4> NICS;

    static const unsigned int UNITS = NICS::Length;
};

template <> struct Traits<eMote3_IEEE802_15_4>: public Traits<Cortex_M_Radio>
{
    static const unsigned int UNITS = NICS::Count<eMote3_IEEE802_15_4>::Result;
    static const unsigned int SEND_BUFFERS = 16;
    static const unsigned int RECEIVE_BUFFERS = 16;
    static const unsigned int DEFAULT_CHANNEL = 15; // From 11 to 26

    // There is no listen command on the radio interface yet,
    // so the only way to receive data is setting this flag
    static const bool auto_listen = true;

    static const bool CSMA_CA = true;
    static const unsigned int CSMA_CA_MIN_BACKOFF_EXPONENT = 3;
    static const unsigned int CSMA_CA_MAX_BACKOFF_EXPONENT = 5;
    static const unsigned int CSMA_CA_UNIT_BACKOFF_PERIOD = 320; // us
    static const unsigned int CSMA_CA_MAX_TRANSMISSION_TRIALS = 4;

    static const bool ACK = true;
    static const unsigned int RETRANSMISSIONS = 3;
    static const unsigned int ACK_TIMEOUT = 3 * 832; // us
};

template <> struct Traits<Cortex_M_Scratchpad>: public Traits<Cortex_M_Common>
{
    static const bool enabled = false;
};

__END_SYS

#endif
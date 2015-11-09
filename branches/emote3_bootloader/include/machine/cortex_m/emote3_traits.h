// EPOS EPOSMoteIII (Cortex-M4) MCU Metainfo and Configuration

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
    static const unsigned int MEM_BASE  = 0x20000000;
    static const unsigned int MEM_TOP   = 0x20007ffb; // (MAX for 32-bit is 0x70000000 / 1792 MB)

    // Logical Memory Map
    static const unsigned int APP_LOW   = 0x20000000;
    static const unsigned int APP_CODE  = 0x00200000;
    static const unsigned int APP_DATA  = 0x20000000;
    static const unsigned int APP_HIGH  = 0x20007ffb;

    static const unsigned int PHY_MEM   = 0x20000000;
    static const unsigned int IO_BASE   = 0x40000000;
    static const unsigned int IO_TOP    = 0x440067ff;

    static const unsigned int SYS       = 0x00200000;
    static const unsigned int SYS_CODE  = 0x00200000; // Library mode only => APP + SYS
    static const unsigned int SYS_DATA  = 0x20000000; // Library mode only => APP + SYS

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE = 4 * 1024;
    static const unsigned int HEAP_SIZE = 4 * 1024;
    static const unsigned int MAX_THREADS = 2;
};

template<> struct Traits<Cortex_M_Bootloader>: public Traits<void>
{
    static const bool enabled = true;

    enum {nic, usb};
    static const unsigned int ENGINE = nic;

    // NIC specifics
    static const unsigned int NIC_PROTOCOL = 0x1010;

    // USB specifics
    static const unsigned int USB_ALIVE_WAITING_LIMIT = 500000; // in microseconds

    // Set to 0 to try forever
    static const unsigned int HANDSHAKE_WAITING_LIMIT = 1000000; // in microseconds

    // Word in RAM reserved for the bootloader
    static const unsigned int BOOTLOADER_STATUS_ADDRESS = 0x20007ffb;
    enum STATUS 
    {
        FINISHED = 42,
    };

    // Flash memory reserved for the bootloader
    static const unsigned int LOW_ADDR = Traits<Cortex_M>::SYS_CODE;
    static const unsigned int TOP_ADDR = 0x00204000;

    // Loaded image bounds
    static const unsigned int IMAGE_LOW = TOP_ADDR;
    static const unsigned int IMAGE_TOP = 0x0027f800; 

    // Pointers to loaded EPOS' interrupt table
    const static unsigned int LOADED_EPOS_STACK_POINTER_ADDRESS = IMAGE_LOW;
    const static unsigned int LOADED_EPOS_ENTRY_POINT_ADDRESS   = IMAGE_LOW + 4;
    const static unsigned int LOADED_EPOS_INT_HANDLER_ADDRESS   = IMAGE_LOW + 8;


    const static unsigned int BUFFER_SIZE = 256;
};

template <> struct Traits<Cortex_M_IC>: public Traits<Cortex_M_Common>
{
    static const bool hysterically_debugged = false;
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

template <> struct Traits<Cortex_M_USB>: public Traits<Cortex_M_Common>
{
    static const bool enabled = (Traits<Cortex_M_Bootloader>::ENGINE == Traits<Cortex_M_Bootloader>::usb)
                                || (Traits<Serial_Display>::ENGINE == Traits<Serial_Display>::usb);
};

template <> struct Traits<Cortex_M_Radio>: public Traits<Cortex_M_Common>
{
    static const bool enabled = (Traits<Cortex_M_Bootloader>::ENGINE == Traits<Cortex_M_Bootloader>::nic)
                                || (Traits<Build>::NODES > 1);

    typedef LIST<CC2538> NICS;
    static const unsigned int UNITS = NICS::Length;
};

template <> struct Traits<CC2538>: public Traits<Cortex_M_Radio>
{
    // static const bool debugged = true;

    static const unsigned int UNITS = NICS::Count<CC2538>::Result;
    static const unsigned int SEND_BUFFERS = 1;
    static const unsigned int RECEIVE_BUFFERS = 1;

    // There is no listen command on the radio interface yet,
    // so the only way to receive data is setting this flag
    static const bool auto_listen = true;
};


template <> struct Traits<Cortex_M_Scratchpad>: public Traits<Cortex_M_Common>
{
    static const bool enabled = false;
};

__END_SYS

#endif

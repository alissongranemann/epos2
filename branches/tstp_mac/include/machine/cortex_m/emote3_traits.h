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
    static const unsigned int MEM_BASE  = 0x20000004;
    static const unsigned int MEM_TOP   = 0x20007ffb; // (MAX for 32-bit is 0x70000000 / 1792 MB)

    // Logical Memory Map
    static const unsigned int APP_LOW   = 0x20000004;
    static const unsigned int APP_CODE  = 0x00204000;
    static const unsigned int APP_DATA  = 0x20000004;
    static const unsigned int APP_HIGH  = 0x20007ffb;

    static const unsigned int PHY_MEM   = 0x20000004;
    static const unsigned int IO_BASE   = 0x40000000;
    static const unsigned int IO_TOP    = 0x440067ff;

    static const unsigned int SYS       = 0x00204000;
    static const unsigned int SYS_CODE  = 0x00204000; // Library mode only => APP + SYS
    static const unsigned int SYS_DATA  = 0x20000004; // Library mode only => APP + SYS

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE = 4 * 1024;
    static const unsigned int HEAP_SIZE = 4 * 1024;
    static const unsigned int MAX_THREADS = 2;
};

template <> struct Traits<Cortex_M_IC>: public Traits<Cortex_M_Common>
{
    static const bool hysterically_debugged = false;
    static const bool debugged = false;

    enum ACTION { NOTHING, REBOOT, HALT };
    static const ACTION ACTION_ON_HARD_FAULT = HALT;
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
    static const bool enabled = Traits<Serial_Display>::ENGINE == Traits<Serial_Display>::usb;
    static const bool blocking = false;
};

template <> struct Traits<TSTP_MAC>: public Traits<Cortex_M_Common>
{
    //static const bool debugged = true;

    static const unsigned int TX_SCHEDULE_SIZE = 4;
    typedef eMote3_User_Timer_2 Timer;

    // All times in microseconds

    // == Configurable parameters == 
    static const unsigned int PERIOD = 150000;
    static const unsigned int ADDRESS_X = 0;
    static const unsigned int ADDRESS_Y = 0;
    static const unsigned int ADDRESS_Z = 0;
    static const unsigned int RETRANSMISSION_DEADLINE = 5 * PERIOD;
    static const unsigned int DATA_ACK_TIMEOUT = RETRANSMISSION_DEADLINE;
    static const int ADDRESS_MATCH_RADIUS = 100;

    // == Network / machine characteristics ==
    static const unsigned int Tu = 192; // IEEE 802.15.4 TX Turnaround Time
    static const unsigned int G = 320; // Tu + 8 / symbol_rate
    //static const unsigned int Ts = 480; // Time to send a single microframe (including PHY headers)
    static const unsigned int Ts = 707 - Tu; // Time to send a single microframe (including PHY headers)
    static const unsigned int MICROFRAME_TIME = Ts;
    static const unsigned int MIN_Ti = 2*Tu; // Minimum time between consecutive microframes
    static const unsigned int RADIO_RADIUS = 10 * 100; //TODO
    static const unsigned int TX_UNTIL_PROCESS_DATA_DELAY = 6; //TODO
    static const unsigned int DATA_SKIP_TIME = 5000;//Tu + 2032;

    // == Calculated parameters ==
    static const unsigned int N_MICROFRAMES = ((PERIOD / (MIN_Ti + Ts)) > 256) ? 256 : (PERIOD / (MIN_Ti + Ts));
    static const unsigned int Ti = (PERIOD / N_MICROFRAMES) - Ts;
    static const unsigned int TIME_BETWEEN_MICROFRAMES = Ti;
    static const unsigned int DATA_LISTEN_MARGIN = TIME_BETWEEN_MICROFRAMES / 2; // Subtract this amount when calculating time until data transmission
    static const unsigned int RX_MF_TIMEOUT = 2*Ts + 2*TIME_BETWEEN_MICROFRAMES;
    static const unsigned int SLEEP_PERIOD = PERIOD - RX_MF_TIMEOUT;
    static const unsigned int DUTY_CYCLE = (RX_MF_TIMEOUT * 100000) / PERIOD; //ppm

    static const unsigned int RX_DATA_TIMEOUT = DATA_SKIP_TIME;
    static const unsigned int CCA_TIME = 2 * (MICROFRAME_TIME + TIME_BETWEEN_MICROFRAMES);
};

template <> struct Traits<Cortex_M_Radio>: public Traits<Cortex_M_Common>
{
//    static const bool enabled = true;

    typedef LIST<CC2538> NICS;
    static const unsigned int UNITS = NICS::Length;
};

template <> struct Traits<CC2538>: public Traits<Cortex_M_Radio>
{
    static const unsigned int UNITS = NICS::Count<CC2538>::Result;
    static const unsigned int SEND_BUFFERS = 8;
    static const unsigned int RECEIVE_BUFFERS = 4;
    static const unsigned int DEFAULT_CHANNEL = 15; // From 11 to 26

    static const bool auto_listen = false;

    static const bool CSMA_CA = false;
    static const unsigned int CSMA_CA_MIN_BACKOFF_EXPONENT = 3;
    static const unsigned int CSMA_CA_MAX_BACKOFF_EXPONENT = 5;
    static const unsigned int CSMA_CA_UNIT_BACKOFF_PERIOD = 320; // us
    static const unsigned int CSMA_CA_MAX_TRANSMISSION_TRIALS = 4;

    static const bool ACK = false;
    static const unsigned int RETRANSMISSIONS = 3;
    static const unsigned int ACK_TIMEOUT = 3 * 832; // us
};

template <> struct Traits<Cortex_M_Scratchpad>: public Traits<Cortex_M_Common>
{
    static const bool enabled = false;
};

template <> struct Traits<Cortex_M_SPI>: public Traits<Cortex_M_Common>
{
    static const bool enabled = true;
    static const unsigned int UNITS = 2;
};

__END_SYS

#endif

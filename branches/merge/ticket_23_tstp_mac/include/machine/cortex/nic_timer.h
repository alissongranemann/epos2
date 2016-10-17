// EPOS Cortex-M MAC Timer Mediator Declarations

#ifndef __nic_timer_h
#define __nic_timer_h

#include <timer.h>
#include <cpu.h>

__BEGIN_SYS

class NIC_Timer: public Timer_Common
{
    friend class CC2538;
    friend class IC;

private:
    const static unsigned long long CLOCK = 32 * 1000 * 1000; // 32MHz

    typedef CPU::Reg32 Reg32;

    // Bases
    enum {
        MACTIMER_BASE        = 0x40088800,
    };

public:
    typedef unsigned long long Time_Stamp;
    typedef RTC::Microsecond Microsecond;

    static Hertz frequency() { return CLOCK; }

    // MACTIMER register offsets
    enum {       //Offset   Description                               Type    Value after reset
        MTCSPCFG = 0x00, // MAC Timer event configuration              RW     0x0
        MTCTRL   = 0x04, // MAC Timer control register                 RW     0x2
        MTIRQM   = 0x08, // MAC Timer interrupt mask                   RW     0x0
        MTIRQF   = 0x0C, // MAC Timer interrupt flags                  RW     0x0
        MTMSEL   = 0x10, // MAC Timer multiplex select                 RW     0x0
        MTM0     = 0x14, // MAC Timer multiplexed register 0           RW     0x0
        MTM1     = 0x18, // MAC Timer multiplexed register 1           RW     0x0
        MTMOVF2  = 0x1C, // MAC Timer multiplexed overflow register 2  RW     0x0
        MTMOVF1  = 0x20, // MAC Timer multiplexed overflow register 1  RW     0x0
        MTMOVF0  = 0x24, // MAC Timer multiplexed overflow register 0  RW     0x0
    };

    // Useful bits in MTCTRL
    enum {                  //Offset   Description                                                             Type    Value after reset
        MTCTRL_LATCH_MODE = 1 << 3, // 0: Reading MTM0 with MTMSEL.MTMSEL = 000 latches the high               RW      0
        // byte of the timer, making it ready to be read from MTM1. Reading
        // MTMOVF0 with MTMSEL.MTMOVFSEL = 000 latches the two
        // most-significant bytes of the overflow counter, making it possible to
        // read these from MTMOVF1 and MTMOVF2.
        // 1: Reading MTM0 with MTMSEL.MTMSEL = 000 latches the high
        // byte of the timer and the entire overflow counter at once, making it
        // possible to read the values from MTM1, MTMOVF0, MTMOVF1, and MTMOVF2.
        MTCTRL_STATE      = 1 << 2, // State of MAC Timer                                                      RO      0
        // 0: Timer idle
        // 1: Timer running
        MTCTRL_SYNC       = 1 << 1, // 0: Starting and stopping of timer is immediate; that is, synchronous    RW      1
        // with clk_rf_32m.
        // 1: Starting and stopping of timer occurs at the first positive edge of
        // the 32-kHz clock. For more details regarding timer start and stop,
        // see Section 22.4.
        MTCTRL_RUN        = 1 << 0, // Write 1 to start timer, write 0 to stop timer. When read, it returns    RW      0
        // the last written value.
    };

    // Useful bits in MSEL
    enum {
        MSEL_MTMOVFSEL = 1 << 4, // See possible values below
        MSEL_MTMSEL    = 1 << 0, // See possible values below
    };
    enum {
        OVERFLOW_COUNTER  = 0x00,
        OVERFLOW_CAPTURE  = 0x01,
        OVERFLOW_PERIOD   = 0x02,
        OVERFLOW_COMPARE1 = 0x03,
        OVERFLOW_COMPARE2 = 0x04,
    };
    enum {
        TIMER_COUNTER  = 0x00,
        TIMER_CAPTURE  = 0x01,
        TIMER_PERIOD   = 0x02,
        TIMER_COMPARE1 = 0x03,
        TIMER_COMPARE2 = 0x04,
    };
    enum {
        INT_OVERFLOW_COMPARE2 = 1 << 5,
        INT_OVERFLOW_COMPARE1 = 1 << 4,
        INT_OVERFLOW_PER      = 1 << 3,
        INT_COMPARE2          = 1 << 2,
        INT_COMPARE1          = 1 << 1,
        INT_PER               = 1 << 0
    };


public:
    NIC_Timer() {}

    static Time_Stamp read() { return read((OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL)); }
    static Time_Stamp sfd() { return read(TIMER_CAPTURE * MSEL_MTMSEL); }
    static Time_Stamp now() { return read() + _offset; }

    static void set(const Time_Stamp & t) {
        mactimer(MTCTRL) &= ~MTCTRL_RUN; // Stop counting
        mactimer(MTMSEL) = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);

        mactimer(MTMOVF0) = t >> 16ll;
        mactimer(MTMOVF1) = t >> 24ll;
        mactimer(MTMOVF2) = t >> 32ll; // MOVF2 must be written last
        _overflow_count = t >> 40ll;

        mactimer(MTM0) = t; // M0 must be written first
        mactimer(MTM1) = t >> 8ll;

        mactimer(MTCTRL) |= MTCTRL_RUN; // Start counting
    }

    static void interrupt(const Time_Stamp & when, const Handler & h) {
        //mactimer(MTCTRL) &= ~MTCTRL_RUN; // Stop counting
        mactimer(MTIRQM) = 0; // Mask interrupts
        // Clear any pending compare interrupts
        mactimer(MTIRQF) = mactimer(MTIRQF) & INT_OVERFLOW_PER;
        _ints = _ints & INT_OVERFLOW_PER;
        mactimer(MTMSEL) = (OVERFLOW_COMPARE1 * MSEL_MTMOVFSEL) | (TIMER_COMPARE1 * MSEL_MTMSEL);
        mactimer(MTM0) = when;
        mactimer(MTM1) = when >> 8;
        mactimer(MTMOVF0) = when >> 16;
        mactimer(MTMOVF1) = when >> 24;
        mactimer(MTMOVF2) = when >> 32;


        _handler = h;
        _int_request_time = when;

        int_enable(INT_COMPARE1 | INT_OVERFLOW_PER);
        //mactimer(MTCTRL) |= MTCTRL_RUN; // Start counting
        /*
           _overflow_match = false;
           _msb_match = false;

           Time_Stamp now = read();
           assert(when > now);
           if((when >> 40ll) > (now >> 40ll))
           int_enable(INT_OVERFLOW_PER);
           else {
           _overflow_match = true;
           if((when >> 16ll) > (now >> 16ll))
           int_enable(INT_OVERFLOW_COMPARE1 | INT_OVERFLOW_PER);
           else {
           _msb_match = true;
        // This will also be executed if when <= now,
        // and interrupt will occur on the next turn of the 16 LSBs
        int_enable(INT_COMPARE1 | INT_OVERFLOW_PER);
        }
        }
        */
    }

    static void int_disable() { mactimer(MTIRQM) = INT_OVERFLOW_PER; }

    static Time_Stamp us2count(const Microsecond & us) { return static_cast<Time_Stamp>(us) * CLOCK / 1000000ll; }
    static Microsecond count2us(const Time_Stamp & ts) { return ts * 1000000ll / CLOCK; }

private:
    static void int_enable(const Reg32 & interrupt) { mactimer(MTIRQM) |= interrupt; }

    static Time_Stamp read(unsigned int sel) {
        mactimer(MTMSEL) = sel;
        Time_Stamp oc, ts;
        do {
            ts = (oc = _overflow_count) << 40ll;
            ts += mactimer(MTM0); // M0 must be read first
            ts += mactimer(MTM1) << 8;
            ts += static_cast<long long>(mactimer(MTMOVF0)) << 16ll;
            ts += static_cast<long long>(mactimer(MTMOVF1)) << 24ll;
            ts += static_cast<long long>(mactimer(MTMOVF2)) << 32ll;
        } while(_overflow_count != oc);
        return ts;
    }

    static void int_handler(const IC::Interrupt_Id & interrupt) {
        Reg32 ints = _ints;
        _ints &= ~ints;

        if(ints & INT_OVERFLOW_PER)
            _overflow_count++;

        if(_handler && (_int_request_time <= read())) {
            int_disable();
            Handler h = _handler;
            _handler = 0;
            h(interrupt);
        }

        /*
           if(ints & INT_OVERFLOW_PER) {
           _overflow_count++;
           if(_handler && !_overflow_match && ((_int_request_time >> 40ll) <= _overflow_count)) {
           _overflow_match = true;
           int_enable(INT_OVERFLOW_COMPARE1);
           }
           }
           if(_handler) {
           if((ints & INT_OVERFLOW_COMPARE1) && _overflow_match && !_msb_match) {
           _msb_match = true;
           mactimer(MTIRQM) = (INT_COMPARE1 | INT_OVERFLOW_PER);
           } else if((ints & INT_COMPARE1) && _msb_match) {
           int_disable();
           Handler h = _handler;
           _handler = 0;
           h(interrupt);
           }
           }
           */
    }

    static void eoi(const IC::Interrupt_Id & interrupt) {
        _ints |= mactimer(MTIRQF);
        mactimer(MTIRQF) = 0;
    }

    static void init();

private:
    static volatile Reg32 & mactimer(unsigned int offset) { return *(reinterpret_cast<volatile Reg32 *>(MACTIMER_BASE + offset)); }

    static Time_Stamp _offset;
    static Handler _handler;
    static volatile Reg32 _overflow_count;
    static volatile Reg32 _ints;
    static Time_Stamp _int_request_time;
    static bool _overflow_match;
    static bool _msb_match;
};

__END_SYS

#endif

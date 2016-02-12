// EPOS Cortex_M Timer Mediator Declarations

#ifndef __cortex_m_timer_h
#define __cortex_m_timer_h

#include <cpu.h>
#include <ic.h>
#include <rtc.h>
#include <timer.h>
#include <machine.h>
#include __MODEL_H

__BEGIN_SYS

class Cortex_M_Sys_Tick: public Cortex_M_Model
{
private:
    typedef TSC::Hertz Hertz;

public:
    typedef CPU::Reg32 Count;
    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

protected:
    Cortex_M_Sys_Tick() {}

public:
    static void enable() {
        scs(STCTRL) |= ENABLE;
    }
    static void disable() {
        scs(STCTRL) &= ~ENABLE;
    }

    static Hertz clock() { return CLOCK; }

    static void init(unsigned int f) {
        scs(STCTRL) = 0;
        scs(STCURRENT) = 0;
        scs(STRELOAD) = CLOCK / f;
        scs(STCTRL) = CLKSRC | INTEN;
    }
};


class Cortex_M_Timer: protected Timer_Common
{
    friend class Cortex_M;
    friend class Init_System;

protected:
    static const unsigned int CHANNELS = 3;
    static const unsigned int FREQUENCY = Traits<Cortex_M_Timer>::FREQUENCY;

    typedef Cortex_M_Sys_Tick Engine;
    typedef Engine::Count Count;
    typedef IC::Interrupt_Id Interrupt_Id;

public:
    using Timer_Common::Hertz;
    using Timer_Common::Tick;
    using Timer_Common::Handler;
    using Timer_Common::Channel;

    // Channels
    enum {
        SCHEDULER,
        ALARM,
        USER
    };

public:
    Cortex_M_Timer(const Hertz & frequency, const Handler & handler, const Channel & channel, bool retrigger = true):
        _channel(channel), _initial(FREQUENCY / frequency), _retrigger(retrigger), _handler(handler) {
        db<Timer>(TRC) << "Timer(f=" << frequency << ",h=" << reinterpret_cast<void*>(handler)
                       << ",ch=" << channel << ") => {count=" << _initial << "}" << endl;

        if(_initial && (unsigned(channel) < CHANNELS) && !_channels[channel])
            _channels[channel] = this;
        else
            db<Timer>(WRN) << "Timer not installed!"<< endl;

        for(unsigned int i = 0; i < Traits<Machine>::CPUS; i++)
            _current[i] = _initial;
    }

    ~Cortex_M_Timer() {
        db<Timer>(TRC) << "~Timer(f=" << frequency() << ",h=" << reinterpret_cast<void*>(_handler)
                       << ",ch=" << _channel << ") => {count=" << _initial << "}" << endl;

        _channels[_channel] = 0;
    }

    Hertz frequency() const { return (FREQUENCY / _initial); }
    void frequency(const Hertz & f) { _initial = FREQUENCY / f; reset(); }

    Tick read() { return _current[Machine::cpu_id()]; }

    int reset() {
        db<Timer>(TRC) << "Timer::reset() => {f=" << frequency()
                       << ",h=" << reinterpret_cast<void*>(_handler)
                       << ",count=" << _current[Machine::cpu_id()] << "}" << endl;

        int percentage = _current[Machine::cpu_id()] * 100 / _initial;
        _current[Machine::cpu_id()] = _initial;

        return percentage;
    }

    void handler(const Handler & handler) { _handler = handler; }

    static void enable() { Engine::enable(); }
    static void disable() { Engine::disable(); }

private:
    static Hertz count2freq(const Count & c) {
        return c ? Engine::clock() / c : 0;
    }

    static Count freq2count(const Hertz & f) {
        return f ? Engine::clock() / f : 0;
    }

    static void int_handler(const Interrupt_Id & i);

    static void init();

private:
    unsigned int _channel;
    Count _initial;
    bool _retrigger;
    volatile Count _current[Traits<Machine>::CPUS];
    Handler _handler;

    static Cortex_M_Timer * _channels[CHANNELS];
};


// Timer used by Thread::Scheduler
class Scheduler_Timer: public Cortex_M_Timer
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler & handler): Cortex_M_Timer(1000000 / quantum, handler, SCHEDULER) {}
};


// Timer used by Alarm
class Alarm_Timer: public Cortex_M_Timer
{
public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

public:
    Alarm_Timer(const Handler & handler): Cortex_M_Timer(FREQUENCY, handler, ALARM) {}
};

// TODO: replace with timers 0-3
// Timer available for users
class User_Timer: public Cortex_M_Timer
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    User_Timer(const Microsecond & quantum, const Handler & handler): Cortex_M_Timer(1000000 / quantum, handler, USER, true) {}
};

// CC2538's General Purpose Timer definitions
class CC2538_GPTIMER 
{
protected:
    enum Base 
    {
        GPTIMER0_BASE = 0x40030000,
        GPTIMER1_BASE = 0x40031000,
        GPTIMER2_BASE = 0x40032000,
        GPTIMER3_BASE = 0x40033000
    };

    enum Offset 
    {
        //Register Name  Offset  Type  Width  Reset Value
        CFG           =   0x00,  //RW    32    0x00000000
        TAMR          =   0x04,  //RW    32    0x00000000
        TBMR          =   0x08,  //RW    32    0x00000000
        CTL           =   0x0C,  //RW    32    0x00000000
        SYNC          =   0x10,  //RW    32    0x00000000
        IMR           =   0x18,  //RW    32    0x00000000
        RIS           =   0x1C,  //RO    32    0x00000000
        MIS           =   0x20,  //RO    32    0x00000000
        ICR           =   0x24,  //RW    32    0x00000000
        TAILR         =   0x28,  //RW    32    0xFFFFFFFF
        TBILR         =   0x2C,  //RW    32    0x0000FFFF
        TAMATCHR      =   0x30,  //RW    32    0xFFFFFFFF
        TBMATCHR      =   0x34,  //RW    32    0x0000FFFF
        TAPR          =   0x38,  //RW    32    0x00000000
        TBPR          =   0x3C,  //RW    32    0x00000000
        TAPMR         =   0x40,  //RW    32    0x00000000
        TBPMR         =   0x44,  //RW    32    0x00000000
        TAR           =   0x48,  //RO    32    0xFFFFFFFF
        TBR           =   0x4C,  //RO    32    0x0000FFFF
        TAV           =   0x50,  //RW    32    0xFFFFFFFF
        TBV           =   0x54,  //RW    32    0x0000FFFF
        TAPS          =   0x5C,  //RO    32    0x00000000
        TBPS          =   0x60,  //RO    32    0x00000000
        TAPV          =   0x64,  //RO    32    0x00000000
        TBPV          =   0x68,  //RO    32    0x00000000
        PP            =  0xFC0,  //RO    32    0x00000000
    };

    enum CTL 
    {
        TBPWML = 1 << 14, // GPTM Timer B PWM output level
                          // 0: Output is unaffected.
                          // 1: Output is inverted. RW 0
        TBOTE = 1 << 13, // GPTM Timer B output trigger enable
                         // 0: The ADC trigger of output Timer B is disabled.
                         // 1: The ADC trigger of output Timer B is enabled.
        TBEVENT = 1 << 10, // GPTM Timer B event mode
                           // 0x0: Positive edge
                           // 0x1: Negative edge
                           // 0x2: Reserved
                           // 0x3: Both edges RW 0x0
        TBSTALL = 1 << 9, // GPTM Timer B stall enable
                          // 0: Timer B continues counting while the processor is halted by the
                          // debugger.
                          // 1: Timer B freezes counting while the processor is halted by the
                          // debugger. RW 0
        TBEN = 1 << 8, // GPTM Timer B enable
                       // 0: Timer B is disabled.
                       // 1: Timer B is enabled and begins counting or the capture logic is
                       // enabled based on the GPTMCFG register. RW 0
        TAPWML = 1 << 6, // GPTM Timer A PWM output level
                         // 0: Output is unaffected.
                         // 1: Output is inverted. RW 0
        TAOTE = 1 << 5, // GPTM Timer A output trigger enable
                        // 0: The ADC trigger of output Timer A is disabled.
                        // 1: The ADC trigger of output Timer A is enabled. RW 0
        TAEVENT = 1 << 2, // GPTM Timer A event mode
                          // 0x0: Positive edge
                          // 0x1: Negative edge
                          // 0x2: Reserved
                          // 0x3: Both edges RW 0x0
        TASTALL = 1 << 1, // GPTM Timer A stall enable
                          // 0: Timer A continues counting while the processor is halted by the
                          // debugger.
                          // 1: Timer A freezes counting while the processor is halted by the
                          // debugger. RW 0
        TAEN = 1 << 0, // GPTM Timer A enable
                       // 0: Timer A is disabled.
                       // 1: Timer A is enabled and begins counting or the capture logic is
                       // enabled based on the GPTMCFG register.
    };

    enum TAMR 
    {
        TAPLO = 1 << 11, // Legacy PWM operation
                         // 0: Legacy operation
                         // 1: CCP is set to 1 on time-out. RW 0
        TAMRSU = 1 << 10, // Timer A match register update mode
                          // 0: Update GPTMAMATCHR and GPTMAPR if used on the next
                          // cycle.
                          // 1: Update GPTMAMATCHR and GPTMAPR if used on the next
                          // time-out. If the timer is disabled (TAEN is clear) when this bit is set,
                          // GPTMTAMATCHR and GPTMTAPR are updated when the timer is
                          // enabled. If the timer is stalled (TASTALL is set), GPTMTAMATCHR
                          // and GPTMTAPR are updated according to the configuration of this
                          // bit. RW 0
        TAPWMIE = 1 << 9, // GPTM Timer A PWM interrupt enable
                          // This bit enables interrupts in PWM mode on rising, falling, or both
                          // edges of the CCP output.
                          // 0: Interrupt is disabled.
                          // 1: Interrupt is enabled.
                          // This bit is valid only in PWM mode. RW 0
        TAILD = 1 << 8, // GPTM Timer A PWM interval load write
                        // 0: Update the GPTMTAR register with the value in the GPTMTAILR
                        // register on the next cycle. If the prescaler is used, update the
                        // GPTMTAPS register with the value in the GPTMTAPR register on
                        // the next cycle.
                        // 1: Update the GPTMTAR register with the value in the GPTMTAILR
                        // register on the next cycle. If the prescaler is used, update the
                        // GPTMTAPS register with the value in the GPTMTAPR register on
                        // the next time-out. RW 0
        TASNAPS = 1 << 7, // GPTM Timer A snap-shot mode
                          // 0: Snap-shot mode is disabled.
                          // 1: If Timer A is configured in periodic mode, the actual free-running
                          // value of Timer A is loaded at the time-out event into the GPTM
                          // Timer A (GPTMTAR) register. RW 0
        TAWOT = 1 << 6, // GPTM Timer A wait-on-trigger
                        // 0: Timer A begins counting as soon as it is enabled.
                        // 1: If Timer A is enabled (TAEN is set in the GPTMCTL register),
                        // Timer A does not begin counting until it receives a trigger from the
                        // Timer in the previous position in the daisy-chain. This bit must be
                        // clear for GP Timer module 0, Timer A. RW 0
        TAMIE = 1 << 5, // GPTM Timer A match interrupt enable
                        // 0: The match interrupt is disabled.
                        // 1: An interrupt is generated when the match value in the
                        // GPTMTAMATCHR register is reached in the one-shot and periodic
                        // modes. RW 0
        TACDIR = 1 << 4, // GPTM Timer A count direction
                         // 0: The timer counts down.
                         // 1: The timer counts up. When counting up, the timer starts from a
                         // value of 0x0. RW 0
        TAAMS = 1 << 3, // GPTM Timer A alternate mode
                        // 0: Capture mode is enabled.
                        // 1: PWM mode is enabled.
                        // Note: To enable PWM mode, the TACM bit must be cleared and the
                        // TAMR field must be configured to 0x2. RW 0
        TACMR = 1 << 2, // GPTM Timer A capture mode
                        // 0: Edge-count mode
                        // 1: Edge-time mode
        TAMR_TAMR = 1 << 0, // GPTM Timer A mode
                            // 0x0: Reserved
                            // 0x1: One-shot mode
                            // 0x2: Periodic mode
                            // 0x3: Capture mode
                            // The timer mode is based on the timer configuration defined by bits
                            // [2:0] in the GPTMCFG register.
    };

    enum IMR
    {
        TBMIM = 1 << 11, // GPTM Timer B match interrupt mask
        CBEIM = 1 << 10, // GPTM Timer B capture event interrupt mask
        CBMIM = 1 << 9, // GPTM Timer B capture match interrupt mask
        TBTOIM = 1 << 8, // GPTM Timer B time-out interrupt mask
        TAMIM = 1 << 4, // GPTM Timer A match interrupt mask
        CAEIM = 1 << 2, // GPTM Timer A capture event interrupt mask
        CAMIM = 1 << 1, // GPTM Timer A capture match interrupt mask
        TATOIM = 1 << 0, // GPTM Timer A time-out interrupt mask
    };
};

class eMote3_GPTM : private CC2538_GPTIMER, private Cortex_M_Model
{
public:
    const static unsigned int CLOCK = Traits<CPU>::CLOCK;

    static void delay(unsigned int time_microseconds, unsigned int timer = 3) {
        eMote3_GPTM g(timer, time_microseconds);
        g.enable();
        while(g.running());
    }

    typedef CPU::Reg32 Reg32;

    eMote3_GPTM(unsigned int which_timer, unsigned int time_microseconds = 1):
        _base(reinterpret_cast<volatile Reg32*>(GPTIMER0_BASE + 0x1000 * (which_timer < 4 ? which_timer : 0))), 
        _irq((which_timer < 3) ? (19 + 2*which_timer) : 35)
    {
        disable();
        Cortex_M_Model::config_GPTM(which_timer);
        reg(CFG) = 0; // 32-bit timer
        reg(TAMR) = 1; // One-shot
        set(time_microseconds);
    }
    ~eMote3_GPTM() {
        disable();
    }
    volatile Reg32 read() { 
        return reg(TAV); 
    }
    void set(unsigned int time_microseconds) {
        reg(TAILR) = time_microseconds * (CLOCK / 1000000);
    }
    void disable() {
        reg(CTL) &= ~TAEN; // Disable timer A
        IC::disable(_irq);
        reg(CC2538_GPTIMER::IMR) = 0; 
    }
    void enable() {
        clear_interrupt();
        reg(CTL) |= TAEN; // Enable timer A
    }

    void clear_interrupt() { reg(CC2538_GPTIMER::ICR) = -1; }

    typedef void (Handler)(const unsigned int & int_id);

    void handler(const Handler & handler) {
        IC::int_vector(IC::irq2int(_irq), handler);
        reg(CC2538_GPTIMER::Offset::IMR) = CC2538_GPTIMER::TATOIM; // Enable timeout interrupt
        IC::enable(_irq);
    }

    volatile bool running() {
        return !reg(RIS);
    }

protected:
    volatile Reg32 & reg(unsigned int o) { return _base[o / sizeof(Reg32)]; }

    volatile Reg32* _base;

    unsigned int _irq;
};


class eMote3_User_Timer_0 : public eMote3_GPTM 
{
public:
    static void delay(unsigned int time_microseconds) {
        eMote3_GPTM::delay(time_microseconds, 0);
    }

    eMote3_User_Timer_0(unsigned int time_microseconds = 1) : eMote3_GPTM(0, time_microseconds) {}
};
class eMote3_User_Timer_1 : public eMote3_GPTM 
{
public:
    static void delay(unsigned int time_microseconds) {
        eMote3_GPTM::delay(time_microseconds, 1);
    }

    eMote3_User_Timer_1(unsigned int time_microseconds = 1) : eMote3_GPTM(1, time_microseconds) {}
};
class eMote3_User_Timer_2 : public eMote3_GPTM 
{
public:
    static void delay(unsigned int time_microseconds) {
        eMote3_GPTM::delay(time_microseconds, 2);
    }

    eMote3_User_Timer_2(unsigned int time_microseconds = 1) : eMote3_GPTM(2, time_microseconds) {}
};
class eMote3_User_Timer_3 : public eMote3_GPTM 
{
public:
    static void delay(unsigned int time_microseconds) {
        eMote3_GPTM::delay(time_microseconds, 3);
    }

    eMote3_User_Timer_3(unsigned int time_microseconds = 1) : eMote3_GPTM(3, time_microseconds) {}
};

// Dedicated MAC Timer present in CC2538
class MAC_Timer
{
    typedef RTC::Microsecond Microsecond;

    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;

    const static unsigned int CLOCK = 32 * 1000 * 1000; // 32MHz

    public:
    static unsigned int frequency() { return CLOCK; }

    private:
    enum
    {
        MAC_TIMER_BASE = 0x40088800,
    };

    enum {     //Offset   Description                               Type    Value after reset
        CSPCFG = 0x00, // MAC Timer event configuration              RW     0x0
        CTRL   = 0x04, // MAC Timer control register                 RW     0x2
        IRQM   = 0x08, // MAC Timer interrupt mask                   RW     0x0
        IRQF   = 0x0C, // MAC Timer interrupt flags                  RW     0x0
        MSEL   = 0x10, // MAC Timer multiplex select                 RW     0x0
        M0     = 0x14, // MAC Timer multiplexed register 0           RW     0x0
        M1     = 0x18, // MAC Timer multiplexed register 1           RW     0x0
        MOVF2  = 0x1C, // MAC Timer multiplexed overflow register 2  RW     0x0
        MOVF1  = 0x20, // MAC Timer multiplexed overflow register 1  RW     0x0
        MOVF0  = 0x24, // MAC Timer multiplexed overflow register 0  RW     0x0
    };

    enum CTRL {           //Offset   Description                                                             Type    Value after reset
        CTRL_LATCH_MODE = 1 << 3, // 0: Reading MTM0 with MTMSEL.MTMSEL = 000 latches the high               RW      0
                                  // byte of the timer, making it ready to be read from MTM1. Reading
                                  // MTMOVF0 with MTMSEL.MTMOVFSEL = 000 latches the two
                                  // most-significant bytes of the overflow counter, making it possible to
                                  // read these from MTMOVF1 and MTMOVF2.
                                  // 1: Reading MTM0 with MTMSEL.MTMSEL = 000 latches the high
                                  // byte of the timer and the entire overflow counter at once, making it
                                  // possible to read the values from MTM1, MTMOVF0, MTMOVF1, and MTMOVF2.
        CTRL_STATE      = 1 << 2, // State of MAC Timer                                                      RO      0
                                  // 0: Timer idle
                                  // 1: Timer running
        CTRL_SYNC       = 1 << 1, // 0: Starting and stopping of timer is immediate; that is, synchronous    RW      1
                                  // with clk_rf_32m.
                                  // 1: Starting and stopping of timer occurs at the first positive edge of
                                  // the 32-kHz clock. For more details regarding timer start and stop,
                                  // see Section 22.4.
        CTRL_RUN        = 1 << 0, // Write 1 to start timer, write 0 to stop timer. When read, it returns    RW      0
                                  // the last written value.
    };
    enum MSEL {
        MSEL_MTMOVFSEL = 1 << 4, // See possible values below
        MSEL_MTMSEL    = 1 << 0, // See possible values below
    };
    enum MSEL_MTMOVFSEL {
        OVERFLOW_COUNTER  = 0x00,
        OVERFLOW_CAPTURE  = 0x01,
        OVERFLOW_PERIOD   = 0x02,
        OVERFLOW_COMPARE1 = 0x03,
        OVERFLOW_COMPARE2 = 0x04,
    };
    enum MSEL_MTMSEL {
        TIMER_COUNTER  = 0x00,
        TIMER_CAPTURE  = 0x01,
        TIMER_PERIOD   = 0x02,
        TIMER_COMPARE1 = 0x03,
        TIMER_COMPARE2 = 0x04,
    };

protected:
    static volatile Reg32 & reg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(MAC_TIMER_BASE + offset)); }

public:
    MAC_Timer() { config(); }

    struct Timestamp
    {
        Timestamp() : overflow_count(0), timer_count(0) {}
        Timestamp(Reg32 val) : overflow_count(val >> 16), timer_count(val) {}
        Timestamp(Reg32 of, Reg16 ti) : overflow_count(of), timer_count(ti) {}

        Reg32 overflow_count;
        Reg16 timer_count;

        operator Reg32() { return (overflow_count << 16) + timer_count; }
    };

    static Timestamp read()
    {
        Reg32 index = reg(MSEL);
        reg(MSEL) = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);

        Timestamp ret;

        ret.timer_count = reg(M0); // M0 must be read first
        ret.timer_count += reg(M1) << 8;

        ret.overflow_count = (reg(MOVF2) << 16) + (reg(MOVF1) << 8) + reg(MOVF0);

        reg(MSEL) = index;

        return ret;
    }

    static Timestamp us_to_ts(const Microsecond & us) { return us * (frequency() / 1000000); }
    static Microsecond ts_to_us(const Reg32 & ts) { return ts / (frequency() / 1000000); }

    static void set(const Microsecond & us) { set(us_to_ts(us)); }

    static void set(const Timestamp & t)
    {
        bool r = running();
        if(r) stop();
        Reg32 index = reg(MSEL);
        reg(MSEL) = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);

        reg(MOVF0) = t.overflow_count;
        reg(MOVF1) = t.overflow_count >> 8;
        reg(MOVF2) = t.overflow_count >> 16; // MOVF2 must be written last

        reg(M0) = t.timer_count; // M0 must be written first
        reg(M1) = t.timer_count >> 8;

        reg(MSEL) = index;
        if(r) start();
    }

    static void config()
    {
        disable();
        reg(CTRL) &= ~CTRL_SYNC; // We can't use the sync feature because we want to change
                                 // the count and overflow values when the timer is stopped
        reg(CTRL) |= CTRL_LATCH_MODE; // count and overflow will be latched at once
    }

    static void start() { reg(CTRL) |= CTRL_RUN; }
    static void stop()  { reg(CTRL) &= ~CTRL_RUN; }

    static bool running() { return reg(CTRL) & CTRL_STATE; }

    static void disable()
    {
        reg(IRQM) = 0; // Disable interrupts
        stop();
    }

    static void enable() { start(); }
};
__END_SYS

#endif


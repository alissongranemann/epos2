// EPOS Cortex Timer Mediator Declarations

#ifndef __cortex_timer_h
#define __cortex_timer_h

#include <cpu.h>
#include <ic.h>
#include <rtc.h>
#include <timer.h>
#include <machine.h>
#include __MODEL_H

__BEGIN_SYS

class Sys_Tick;
class GPTM;
class Private_Timer;
class Global_Timer;

#ifdef __mmod_zynq__

class Private_Timer: public Cortex_Model
{
private:
    typedef TSC::Hertz Hertz;

public:
    typedef CPU::Reg32 Count;
    static const unsigned int CLOCK = Traits<CPU>::CLOCK/2;

protected:
    Private_Timer() {}

public:
    static Hertz clock() { return CLOCK; }

    static void enable() { priv_timer(PTCLR) |= TIMER_ENABLE; }
    static void disable() { priv_timer(PTCLR) &= ~TIMER_ENABLE; }

    void power(const Power_Mode & mode);

    static void isr_clr() { priv_timer(PTISR) = INT_CLR; }

    static void init(unsigned int f) {
        priv_timer(PTCLR) = 0;
        isr_clr();
        priv_timer(PTLR) = CLOCK / f;
        priv_timer(PTCLR) = IRQ_EN | AUTO_RELOAD;
    }
};

class Global_Timer: public Cortex_Model
{
private:
    typedef CPU::Reg64 Count;
    typedef TSC::Hertz Hertz;

public:
    static const Hertz CLOCK = Traits<CPU>::CLOCK/2;

public:
    Global_Timer(int channel, const Count & count, bool interrupt = true, bool periodic = true);

    static Hertz clock() { return CLOCK; }

    Count read() {
        Reg32 high, low;

        do {
            high = global_timer(GTCTRH);
            low = global_timer(GTCTRL);
        } while(global_timer(GTCTRH) != high);

        return static_cast<Count>(high) << 32 | low;
    }

    static void enable();
    static void disable();

    void power(const Power_Mode & mode);

    void set(const Count & count) {
        // Disable counting before programming
        global_timer(GTCLR) = 0;

        global_timer(GTCTRL) = count & 0xffffffff;
        global_timer(GTCTRH) = count >> 32;

        // Reenable counting
        global_timer(GTCLR) = 1;
    }
};

#else

class Sys_Tick: public Cortex_Model
{
private:
    typedef TSC::Hertz Hertz;

public:
    typedef CPU::Reg32 Count;
    static const Hertz CLOCK = Traits<CPU>::CLOCK;

protected:
    Sys_Tick() {}

public:
    static Hertz clock() { return CLOCK; }

    static void enable() { scs(STCTRL) |= ENABLE; }
    static void disable() { scs(STCTRL) &= ~ENABLE; }

    static void isr_clr() {}

    static void init(unsigned int f) {
        scs(STCTRL) = 0;
        scs(STCURRENT) = 0;
        scs(STRELOAD) = CLOCK / f;
        scs(STCTRL) = CLKSRC | INTEN;
    }
};

class GPTM: public Cortex_Model
{
protected:
    const static unsigned int CLOCK = Traits<CPU>::CLOCK;

    typedef CPU::Reg32 Count;

protected:
    GPTM(int channel, const Count & count, bool interrupt = true, bool periodic = true)
    : _channel(channel), _base(reinterpret_cast<Reg32 *>(TIMER0_BASE + 0x1000 * channel)) {
        disable();
        power(FULL);
        reg(GPTMCFG) = 0; // 32-bit timer
        reg(GPTMTAMR) = periodic ? 2 : 1; // 2 -> Periodic, 1 -> One-shot
        reg(GPTMTAILR) = count;
        enable();
    }

public:
    ~GPTM() { disable(); power(OFF); }

    unsigned int clock() const { return CLOCK; }
//    bool running() { return !reg(GPTMRIS); }

    Count read() { return reg(GPTMTAR); }

    void enable() { reg(GPTMICR) = -1; reg(GPTMCTL) |= TAEN; } // TODO: Why are pending interrupts discharted?
    void disable() { reg(GPTMCTL) &= ~TAEN; }

    void power(const Power_Mode & mode) { Cortex_Model::timer_power(_channel, mode); }

private:
    volatile Reg32 & reg(unsigned int o) { return _base[o / sizeof(Reg32)]; }

private:
    int _channel;
    Reg32 * _base;
};

#endif

// Tick timer used by the system
class Cortex_Timer: private Timer_Common
{
    friend class Cortex;
    friend class Init_System;

protected:
    static const unsigned int CHANNELS = 2;
    static const unsigned int FREQUENCY = Traits<Cortex_Timer>::FREQUENCY;

    typedef IF<Traits<Build>::MODEL == Traits<Build>::Zynq, Private_Timer, Sys_Tick>::Result Engine;
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

protected:
    Cortex_Timer(const Hertz & frequency, const Handler & handler, const Channel & channel, bool retrigger = true)
    : _channel(channel), _initial(FREQUENCY / frequency), _retrigger(retrigger), _handler(handler) {
        db<Timer>(TRC) << "Timer(f=" << frequency << ",h=" << reinterpret_cast<void*>(handler) << ",ch=" << channel << ") => {count=" << _initial << "}" << endl;

        if(_initial && (channel < CHANNELS) && !_channels[channel])
            _channels[channel] = this;
        else
            db<Timer>(WRN) << "Timer not installed!"<< endl;

        for(unsigned int i = 0; i < Traits<Machine>::CPUS; i++)
            _current[i] = _initial;
    }

public:
    ~Cortex_Timer() {
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
    static Hertz count2freq(const Count & c) { return c ? Engine::clock() / c : 0; }
    static Count freq2count(const Hertz & f) { return f ? Engine::clock() / f : 0;}

    static void int_handler(const Interrupt_Id & i);

    static void isr_clr() { Engine::isr_clr(); }

    static void init();

private:
    unsigned int _channel;
    Count _initial;
    bool _retrigger;
    volatile Count _current[Traits<Machine>::CPUS];
    Handler _handler;

    static Cortex_Timer * _channels[CHANNELS];
};

// Timer used by Thread::Scheduler
class Scheduler_Timer: public Cortex_Timer
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler & handler): Cortex_Timer(1000000 / quantum, handler, SCHEDULER) {}
};

// Timer used by Alarm
class Alarm_Timer: public Cortex_Timer
{
public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

public:
    Alarm_Timer(const Handler & handler): Cortex_Timer(FREQUENCY, handler, ALARM) {}
};


// User timer
class User_Timer: private Timer_Common, private IF<Traits<Build>::MODEL == Traits<Build>::Zynq, Global_Timer, GPTM>::Result
{
private:
    typedef IF<Traits<Build>::MODEL == Traits<Build>::Zynq, Global_Timer, GPTM>::Result Engine;

public:
    using Timer_Common::Microsecond;
    using Timer_Common::Handler;
    using Timer_Common::Channel;

public:
    User_Timer(const Handler & handler, const Channel & channel, const Microsecond & time, bool periodic = false)
    : Engine(channel, us2count(time), handler ? true : false, periodic), _handler(handler) {}
    ~User_Timer() {}

    Microsecond read() { return count2us(Engine::read()); }

    using Engine::enable;
    using Engine::disable;
    using Engine::power;

private:
    static void int_handler(const IC::Interrupt_Id & i);

    static Reg32 us2count(const Microsecond & us) { return us * (CLOCK / 1000000); }
    static Microsecond count2us(Reg32 count) { return count / (CLOCK / 1000000); }

private:
    Handler _handler;
};

__END_SYS

#endif

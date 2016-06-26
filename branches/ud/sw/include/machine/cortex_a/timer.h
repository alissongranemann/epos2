// EPOS Cortex_A Timer Mediator Declarations

#ifndef __cortex_a_timer_h
#define __cortex_a_timer_h

#include <cpu.h>
#include <ic.h>
#include <timer.h>
#include <machine.h>
#include __MODEL_H

__BEGIN_SYS

class Cortex_A_Global_Timer: public Cortex_A_Model
{
private:
    typedef TSC::Hertz Hertz;
    typedef TSC::Time_Stamp Time_Stamp;
    typedef CPU::Reg32 Count;

public:
    static const Hertz CLOCK = Traits<CPU>::CLOCK/2;

public:
    void set(Time_Stamp time) {
        // Disable Global Timer
        global_timer(GTCLR) = 0x0;

        // Updating Global Timer Counter Register
        global_timer(GTCTRL) = (Count)time;
        global_timer(GTCTRH) = (Count)(time>>32);

        // Enable Global Timer
        global_timer(GTCLR) = 0x1;
    }

    Time_Stamp get() {
        volatile Count high, low;

        // Reading Global Timer Counter Register
        do {
            high = global_timer(GTCTRH);
            low = global_timer(GTCTRL);
        } while(global_timer(GTCTRH) != high);

        return (((Time_Stamp)high) << 32) | (Time_Stamp)low;
    }
};

class Cortex_A_Priv_Timer: public Cortex_A_Model
{
private:
    typedef TSC::Hertz Hertz;

public:
    typedef CPU::Reg32 Count;
    static const unsigned int CLOCK = Traits<CPU>::CLOCK/2;

protected:
    Cortex_A_Priv_Timer() {}

public:
    static void enable() { priv_timer(PTCLR) |= TIMER_ENABLE; }
    static void disable() { priv_timer(PTCLR) &= ~TIMER_ENABLE; }

    static Hertz clock() { return CLOCK; }

    static void isr_clr() { priv_timer(PTISR) = INT_CLR; }

    static void init(unsigned int f) {
        priv_timer(PTCLR) = 0;
        isr_clr();
        priv_timer(PTLR) = CLOCK / f;
        priv_timer(PTCLR) = IRQ_EN | AUTO_RELOAD;
    }
};

class Cortex_A_Timer: public Timer_Common
{
    friend class Cortex_A;
    friend class Init_System;

protected:
    static const unsigned int CHANNELS = 3;
    static const unsigned int FREQUENCY = Traits<Cortex_A_Timer>::FREQUENCY;

    typedef Cortex_A_Priv_Timer Engine;
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
    Cortex_A_Timer(const Hertz & frequency, const Handler & handler, const Channel & channel, bool retrigger = true)
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
    ~Cortex_A_Timer() {
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
    static Count freq2count(const Hertz & f) { return f ? Engine::clock() / f : 0; }

    static void int_handler(const Interrupt_Id & i);

    static void isr_clr() { Engine::isr_clr(); }

    static void init();

private:
    unsigned int _channel;
    Count _initial;
    bool _retrigger;
    volatile Count _current[Traits<Machine>::CPUS];
    Handler _handler;

    static Cortex_A_Timer * _channels[CHANNELS];
};


// Timer used by Thread::Scheduler
class Scheduler_Timer: public Cortex_A_Timer
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler & handler): Cortex_A_Timer(1000000 / quantum, handler, SCHEDULER) {}
};

// Timer used by Alarm
class Alarm_Timer: public Cortex_A_Timer
{
public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

public:
    Alarm_Timer(const Handler & handler): Cortex_A_Timer(FREQUENCY, handler, ALARM) {}
};


// TODO: Use a different timer
class User_Timer: public Cortex_A_Timer
{
public:
    using Timer_Common::Microsecond;

public:
    User_Timer(const Microsecond & quantum, const Handler & handler)
    : Cortex_A_Timer(1000000 / quantum, handler, USER, true) {}
};

__END_SYS

#endif

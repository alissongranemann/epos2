// EPOS Zynq Timer Mediator Declarations

#ifndef __zynq_timer_h
#define __zynq_timer_h

#include <cpu.h>
#include <ic.h>
#include <timer.h>
#include <machine.h>

__BEGIN_SYS

class Zynq_Global_Timer
{
private:
    typedef TSC::Hertz Hertz;
    typedef TSC::Time_Stamp Time_Stamp;
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Reg32 Count;

    enum {
        GLOBAL_TIMER_BASE = 0xF8F00200
    };

    // Global Timer Registers offsets
    enum {
        GTCTRL  = 0x00, // Low Counter
        GTCTRH  = 0x04, // High Counter
        GTCLR   = 0x08, // Control
        GTISR   = 0x0C  // Interrupt Status
    };

    static Log_Addr & global_timer(unsigned int o) { return reinterpret_cast<Log_Addr *>(GLOBAL_TIMER_BASE)[o / sizeof(Log_Addr)]; }

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

class Zynq_Priv_Timer
{
private:
    typedef TSC::Hertz Hertz;
    typedef CPU::Log_Addr Log_Addr;

    enum {
        PRIV_TIMER_BASE = 0XF8F00600
    };

    // Private Timer Registers offsets
    enum {
        PTLR    = 0x00, // Load
        PTCTR   = 0x04, // Counter
        PTCLR   = 0x08, // Control
        PTISR   = 0x0C  // Interrupt Status
    };
    enum PTCLR {
        EN          = 1 << 0,
        AUTO_RELOAD = 1 << 1,
        IRQ_EN      = 1 << 2
    };
    enum PTISR {
        INT_CLR = 1 << 0
    };

    static Log_Addr & priv_timer(unsigned int o) { return reinterpret_cast<Log_Addr *>(PRIV_TIMER_BASE)[o / sizeof(Log_Addr)]; }

public:
    typedef CPU::Reg32 Count;
    static const unsigned int CLOCK = Traits<CPU>::CLOCK/2;

protected:
    Zynq_Priv_Timer() {}

public:
    static void enable() { priv_timer(PTCLR) |= EN; }
    static void disable() { priv_timer(PTCLR) &= ~EN; }

    static Hertz clock() { return CLOCK; }

    static void isr_clr() { priv_timer(PTISR) = INT_CLR; }

    static void init(unsigned int f) {
        priv_timer(PTCLR) = 0;
        isr_clr();
        priv_timer(PTLR) = CLOCK / f;
        priv_timer(PTCLR) = IRQ_EN | AUTO_RELOAD;
    }
};

class Zynq_Timer: public Timer_Common
{
    friend class Zynq;
    friend class Init_System;

protected:
    static const unsigned int CHANNELS = 3;
    static const unsigned int FREQUENCY = Traits<Zynq_Timer>::FREQUENCY;

    typedef Zynq_Priv_Timer Engine;
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
    Zynq_Timer(const Hertz & frequency, const Handler & handler, const Channel & channel, bool retrigger = true):
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

    ~Zynq_Timer() {
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

    static void isr_clr() { Engine::isr_clr(); }

    static void int_handler(const Interrupt_Id & id);

    static void init();

private:
    Channel _channel;
    Count _initial;
    bool _retrigger;
    volatile Count _current[Traits<Machine>::CPUS];
    Handler _handler;

    static Zynq_Timer * _channels[CHANNELS];
};


// Timer used by Thread::Scheduler
class Scheduler_Timer: public Zynq_Timer
{
private:
    typedef unsigned long Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler & handler):
        Zynq_Timer(1000000 / quantum, handler, SCHEDULER) {}
};


// Timer used by Alarm
class Alarm_Timer: public Zynq_Timer
{
public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

public:
    Alarm_Timer(const Handler & handler): Zynq_Timer(FREQUENCY, handler, ALARM) {}
};


// TODO: Use a different timer
class User_Timer: public Zynq_Timer
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    User_Timer(const Microsecond & quantum, const Handler & handler): Zynq_Timer(1000000 / quantum, handler, USER, true) {}
};

__END_SYS

#endif

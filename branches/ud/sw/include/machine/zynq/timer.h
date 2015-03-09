// EPOS Zynq Timer Mediator Declarations

#ifndef __zynq_timer_h
#define __zynq_timer_h

#include <ic.h>
#include <timer.h>

__BEGIN_SYS

class Zynq_Timer: public Timer_Common
{
    friend class Zynq;
    friend class Init_System;

protected:
    static const unsigned int CHANNELS = 3;
    static const unsigned int FREQUENCY = Traits<Zynq_Timer>::FREQUENCY;

    typedef unsigned long Hertz;
    typedef CPU::Reg32 Count;
    typedef short Channel;
    typedef IC::Interrupt_Id Interrupt_Id;

public:
    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

    enum {
        TSC,
        ALARM,
        SCHEDULER
    };

public:
    Zynq_Timer(const Hertz & freq, const Handler & handler,
        const Channel & channel): _channel(channel), _handler(handler) {
        set_initial(freq);
        db<Timer>(TRC) << "Timer(f=" << freq << ",h=" << reinterpret_cast<void*>(handler)
                       << ",ch=" << channel << ") => {count=" << _initial << "}" << endl;

        // _initial will be 0 if f > CLOCK, which is reasonable.
        if(_initial && (unsigned(channel) < CHANNELS) && !_channels[channel]) {
            _channels[channel] = this;
            if (channel == ALARM)
                frequency(freq);
        } else
            db<Timer>(WRN) << "Timer not installed!"<< endl;

        for(unsigned int i = 0; i < Traits<Machine>::CPUS; i++)
            _current[i] = _initial;

        enable();
    }

    ~Zynq_Timer() {
        db<Timer>(TRC) << "~Timer(f=" << frequency() << ",h=" << reinterpret_cast<void*>(_handler)
                       << ",ch=" << _channel << ") => {count=" << _initial << "}" << endl;

        _channels[_channel] = 0;
    }

    Hertz frequency() { return (CLOCK/(2*prescale()))/load(); }
    static void frequency(const Hertz& f) { load((CLOCK/(2*prescale()))/f); }

    Tick read() { return value(); }

    void reset() { value(load()); }

    void handler(const Handler & handler) { _handler = handler; }

    void enable() { control(control() | TIMER_ENABLE); }
    void disable() { control(control() & ~(TIMER_ENABLE)); }

private:
    static const unsigned int PRIVATE_TIMER_BASE = 0XF8F00600;

    enum {
        PTLR    = 0x00, // Private Timer Load Register
        PTCTR   = 0x04, // Private Timer Counter Register
        PTCLR   = 0x08, // Private Timer Control Register
        PTISR   = 0x0C, // Private Timer Interrupt Status Register
        WLR     = 0x20, // Watchdog Load Register
        WCTR    = 0x24, // Watchdog Counter Register
        WCLR    = 0x28, // Watchdog Control Register
        WISR    = 0x2C, // Watchdog Interrupt Status Register
        WRSR    = 0x30, // Watchdog Reset Status Register
        WDR     = 0x34, // Watchdog Disable Register
    };

    enum {
        INTERRUPT_CLEAR         = 1,
        TIMER_ENABLE            = 1,
        TIMER_AUTO_RELOAD       = 2, // Periodic
        TIMER_IT_ENABLE         = 4, // IRQ enable
        TIMER_WD_MODE           = 8,
        TIMER_PRESCALE_SHIFT    = 8,
    };

    static const CPU::Reg8 IRQ = IC::INT_TIMER;

private:
    static void init();

    static void int_handler(const Interrupt_Id & id);

    static void value(CPU::Reg32 val) { CPU::out32(PRIVATE_TIMER_BASE + PTCTR, val); }
    static CPU::Reg32 value() { return CPU::in32(PRIVATE_TIMER_BASE + PTCTR); }

    static void load(CPU::Reg32 val) { CPU::out32(PRIVATE_TIMER_BASE + PTLR, val); }
    static CPU::Reg32 load() { return CPU::in32(PRIVATE_TIMER_BASE + PTLR); }

    static void control(CPU::Reg32 val) { CPU::out32(PRIVATE_TIMER_BASE + PTCLR, val); }
    static CPU::Reg32 control() { return CPU::in32(PRIVATE_TIMER_BASE + PTCLR); }

    static void status(CPU::Reg32 val) { CPU::out32(PRIVATE_TIMER_BASE + PTISR, val); }
    static CPU::Reg32 status() { return CPU::in32(PRIVATE_TIMER_BASE + PTISR); }

    void set_initial(const Hertz& f) {
        if(_channel==ALARM) {
            set_prescale(f);
            _initial = (CLOCK/(2*prescale())) / f;
        } else
            _initial = FREQUENCY / f;
    }

    static Hertz count2freq(const Count & c) { return c ? CLOCK / c : 0; }

    static Count freq2count(const Hertz & f) { return f ? CLOCK / f : 0; }

    static unsigned int prescale(const Hertz& f) {
        if(Traits<Zynq_Timer>::prescale)
            return ((CLOCK/2)/f + 1) > 0xff ? 0xff : ((CLOCK/2)/f + 1);
        return 1;
    }

    static unsigned int prescale() { return ((control() >> TIMER_PRESCALE_SHIFT) & 0xff) + 1; }

    static void set_prescale(const Hertz& f) {
        control(control() | ((prescale(f)-1) << TIMER_PRESCALE_SHIFT));
    }

private:
    Channel _channel;
    Count _initial;
    volatile Count _current[Traits<Machine>::CPUS];
    Handler _handler;

    static Zynq_Timer * _channels[CHANNELS];
};

class Alarm_Timer: public Zynq_Timer
{
public:
    Alarm_Timer(const Handler & handler): Zynq_Timer(Zynq_Timer::FREQUENCY,
        handler, ALARM) {}
};

class Scheduler_Timer: public Zynq_Timer
{
private:
    typedef unsigned long Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler & handler):
        Zynq_Timer(1000000 / quantum, handler, SCHEDULER) {}
};

class TSC_Timer: public Zynq_Timer
{
public:
    TSC_Timer(const Hertz & freq, const Handler & handler): Zynq_Timer(1,
        handler, TSC) {}
};

__END_SYS

#endif

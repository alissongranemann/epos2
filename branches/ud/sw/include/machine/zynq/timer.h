#ifndef __zynq_timer_h
#define __zynq_timer_h

#include <timer.h>
#include <ic.h>
#include <system/meta.h>

__BEGIN_SYS

class Zynq_Timer: public Timer_Common
{
protected:
    typedef unsigned long Hertz;
    typedef CPU::Reg32 Count;
    typedef short Channel;

    static const unsigned int FREQUENCY = Traits<Zynq_Timer>::FREQUENCY;
    Handler _handler;
    Channel _channel;
    static const unsigned int CHANNELS = 3;
    static Zynq_Timer * _channels[CHANNELS];
    volatile Count _current[Traits<Machine>::MAX_CPUS];
    Count _initial;

public:
    static const unsigned int CLOCK = Traits<Machine>::CLOCK;

    enum {
        TSC         = 0,
        ALARM       = 1,
        SCHEDULER   = 2,
    };

public:
    Zynq_Timer(const Hertz & freq, const Handler & handler, const Channel & channel):
        _handler(handler), _channel(channel)
    {
        set_initial(freq);

        // _initial will be 0 if f > CLOCK, which is reasonable.
        if(_initial && !_channels[channel]) {
            _channels[channel] = this;
            if (channel == ALARM)
                frequency(freq);
        } else {
            db<Timer>(ERR) << "Timer not installed!" << endl;
            db<Timer>(ERR) << "Freq: " << freq << endl;
            db<Timer>(ERR) << "_channel["<<channel<<"] = " << _channels[channel] << endl;
            db<Timer>(ERR) << "_initial: "<<_initial << endl;
        }


        for(unsigned int i = 0; i < Traits<Machine>::MAX_CPUS; i++)
            _current[i] = _initial;

        enable();
    }

    Hertz frequency() {
        return (CLOCK/(2*prescale())) / load();
    }

    static void init();

    static void int_handler(const IC::Interrupt_Id & id);

    void reset() { value(load()); }

    void enable() {
        db<IC>(TRC) << "Timer_"<<_channel<<"::enable()\n";
        control(control() | TIMER_ENABLE);
    }

    void disable() {
        db<IC>(TRC) << "Timer_"<<_channel<<"::disable()\n";
        control(control() & ~(TIMER_ENABLE));
    }

    Tick read() { return value(); }

    static void value(CPU::Reg32 val) {
        CPU::out32(PRIVATE_TIMER_BASE + PTCTR, val);
    }

    static CPU::Reg32 value() { return CPU::in32(PRIVATE_TIMER_BASE + PTCTR); }

    void handler(const Handler & handler) { _handler = handler; }

    static void frequency(const Hertz& f) {
        //set_prescale(f); //set_initial is already doing this job
        load( (CLOCK/(2*prescale())) / f );
    }

private:
    static const unsigned int PRIVATE_TIMER_BASE = 0xf8f00600;

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

    static const CPU::Reg8 IRQ = IC::TIMERINT0;

private:
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


// EPOS EPOSSOC Timer Mediator Declarations

#ifndef __epossoc_timer_h
#define __epossoc_timer_h

#include <cpu.h>
#include <ic.h>
#include <rtc.h>
#include <timer.h>

__BEGIN_SYS

class EPOSSOC_Timer:  public Timer_Common
{
protected:
    typedef CPU::Reg32 Count;
	typedef volatile CPU::Reg32 Timer_Reg;

    // Plasma Timer private imports, types and constants
    static const unsigned int CLOCK = Traits<EPOSSOC_Timer>::CLOCK;
	static const unsigned int FREQUENCY = Traits<EPOSSOC_Timer>::FREQUENCY;
    static const unsigned int CHANNELS = 3;

public:
  typedef int Channel;
  enum {
    SCHEDULER,
    ALARM,
	TEST
  };
 
public:
    EPOSSOC_Timer(const Hertz & frequency,
                 const Handler * handler,
                 const Channel & channel):
        _initial(FREQUENCY / frequency), _current(_initial), 
	    _channel(channel), _handler(handler)
    {
      db<Timer>(TRC) << "Timer(f=" << frequency
        << ",h=" << reinterpret_cast<void*>(handler)
        << ",ch=" << channel 
        << ") => {count=" << _initial << "}\n";

      if(_initial && !_channels[channel]) 
         _channels[channel] = this;
      else
         db<Timer>(ERR) << "Timer not installed!\n";
    }

    ~EPOSSOC_Timer() {}

    void frequency(const Hertz & f) {
		_initial = FREQUENCY / f;
        reset();
        db<EPOSSOC_Timer>(TRC) << "EPOSSOC_Timer::frequency(freq=" << frequency()
		                             << ",cnt=" << (void*)_initial << ")\n";
    }

	Hertz frequency() {  return FREQUENCY / _initial; }

    void enable() {}

    void disable() {}

    void reset() { _current = _initial; }

    static void int_handler(unsigned int interrupt);

    static void init();

    static void start(unsigned int cycles) {*_timer_reg = cycles;}

    static void stop() { *_timer_reg = 0;}

protected:
    // Plasma_Timer attributes
    Count _initial;
    volatile Count _current;
    unsigned int _channel;
    Handler * _handler;
	static Timer_Reg * _timer_reg;
    static EPOSSOC_Timer * _channels[CHANNELS];

};


// Timer used by Alarm
class Alarm_Timer: public EPOSSOC_Timer
{
  public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

  public:
    Alarm_Timer(const Handler * handler):
      EPOSSOC_Timer(FREQUENCY, handler, ALARM) {}
};


// Timer used by Thread::Scheduler
class Scheduler_Timer: public EPOSSOC_Timer
{
  private:
    typedef RTC::Microsecond Microsecond;

  public:
    Scheduler_Timer(const Microsecond & quantum, const Handler * handler): 
      EPOSSOC_Timer(1000000 / quantum, handler, SCHEDULER) {}
};


__END_SYS

#endif

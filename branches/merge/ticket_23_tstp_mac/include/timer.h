// EPOS Timer Mediator Common Package

#ifndef __timer_h
#define __timer_h

#include <tsc.h>
#include <rtc.h>
#include <ic.h>

__BEGIN_SYS

class Timer_Common
{
public:
    typedef TSC::Hertz Hertz;
    typedef int Tick;
    typedef RTC::Microsecond Microsecond;
    typedef IC::Interrupt_Handler Handler;

protected:
    Timer_Common() {}
};

__END_SYS

#ifdef __TIMER_H
#include __TIMER_H
#endif
#ifdef __NIC_TIMER_H
#include __NIC_TIMER_H
#else
__BEGIN_SYS
class NIC_Timer: private Timer_Common
{
public:
    typedef TSC::Time_Stamp Time_Stamp;
    static void set(const Time_Stamp & t) {}
    static Time_Stamp read() { return TSC::time_stamp(); }

    static Hertz frequency() { return TSC::frequency(); }
};
__END_SYS
#endif

#endif

// EPOS Chronometer Abstraction Declarations

#ifndef __chronometer_h
#define __chronometer_h

#include <tsc.h>
#include <rtc.h>
#include <alarm.h>

__BEGIN_SYS

class TSC_Chronometer
{
private:
    typedef TSC::Time_Stamp Time_Stamp;

public:
    typedef TSC::Hertz Hertz;
    typedef RTC::Second Second;
    typedef RTC::Microsecond Microsecond;
    typedef RTC::Nanosecond Nanosecond;

public:
    TSC_Chronometer() : _start(0), _stop(0) {}

    Hertz frequency() { return tsc.frequency(); }

    void reset() { _start = 0; _stop = 0; }
    void start() { if(_start == 0) _start = tsc.time_stamp(); }
    void lap() { if(_start != 0) _stop = tsc.time_stamp(); }
    void stop() { lap(); }

    Microsecond read() { return ticks() * 1000000 / frequency(); }

private:
    Time_Stamp ticks() {
        if(_start == 0)
            return 0;
        if(_stop == 0)
            return tsc.time_stamp() - _start;
        return _stop - _start;
    }

private:
    TSC tsc;
    Time_Stamp _start;
    Time_Stamp _stop;
};

class Alarm_Chronometer
{
private:
    typedef Alarm::Tick Time_Stamp;

public:
    typedef TSC::Hertz Hertz;
    typedef RTC::Second Second;
    typedef RTC::Microsecond Microsecond;
    typedef RTC::Nanosecond Nanosecond;

public:
    Alarm_Chronometer() : _start(0), _stop(0) {}

    Hertz frequency() { return Alarm::frequency(); }

    void reset() { _start = 0; _stop = 0; }
    void start() { if(_start == 0) _start = Alarm::_elapsed; }
    void lap() { if(_start != 0) _stop = Alarm::_elapsed; }
    void stop() { lap(); }

    Microsecond read() { return ticks() * 1000000 / frequency(); }

private:
    Time_Stamp ticks() {
        if(_start == 0)
            return 0;
        if(_stop == 0)
            return Alarm::_elapsed - _start;
        return _stop - _start;
    }

private:
    TSC tsc;
    Time_Stamp _start;
    Time_Stamp _stop;
};

class Chronometer: public IF<Traits<TSC>::enabled && !Traits<System>::multicore, TSC_Chronometer, Alarm_Chronometer>::Result {};


/*! Utilities for time-measurement */
class Chronometer_Aux
{
public:
    typedef TSC::Time_Stamp Time_Stamp;
    typedef Chronometer::Second Second;
    typedef Chronometer::Microsecond Microsecond;
    typedef Chronometer::Nanosecond Nanosecond;

public:

    /* Time_Stamp() is represented by a 64-bit number
     * 1000000000 is represented by a 32-bit number
     * The multiplication of both must not surpass a 64-bit number.
     * It is safe to assume that if ticks() <= 0xffffffff then the return of
     * the method is reliable.
     * Therefore we are able to measure (more than) 4 G cycles (ticks).
     * For a CPU running at 4 GHz we can measure up to 1 second.
     * */
    static Nanosecond elapsed_nano(Time_Stamp start, Time_Stamp stop)
    {
        check_ranges(start, stop, 0xffffffff);

        return (stop - start) * 1000000000 / TSC::frequency();
    }

    /* (2^43)*1000000 < 2^63 (2^63 is used by Framework)
     * For a CPU running at 4 GHz we can measure up to around 69 years.
     * */
    static Microsecond elapsed_micro(Time_Stamp start, Time_Stamp stop)
    {
        check_ranges(start, stop, 0x7a12000000000000LL);

        return (stop - start) * 1000000 / TSC::frequency();
    }

    static Second elapsed_sec(Time_Stamp start, Time_Stamp stop)
    {
        check_ranges(start, stop, 0x7a12000000000000LL);

        return (stop - start) / TSC::frequency();
    }

    static Nanosecond nano(Time_Stamp ticks)
    {
        return elapsed_nano(0, ticks);
    }

    static Microsecond micro(Time_Stamp ticks)
    {
        return elapsed_micro(0, ticks);
    }

    static Second sec(Time_Stamp ticks)
    {
        return elapsed_sec(0, ticks);
    }

private:
    static void check_ranges(Time_Stamp start, Time_Stamp stop, Time_Stamp max)
    {
        assert(stop >= start);
        assert((stop - start) <= max);

        if (! (stop >= start)) {
            db<void>(ERR) << "Check range error: stop < start there is no negative time!" << endl;
        }

        if (! ((stop - start) <= max)) {
            db<void>(ERR) << "Check range error: range to large!" << endl;
        }
    }

};

__END_SYS

#endif

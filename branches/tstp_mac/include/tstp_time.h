#ifndef __tstp_time_h
#define __tstp_time_h

#include <units.h>

__BEGIN_SYS

class PTP : public TSTP_Common
{
    friend class TSTP;

    typedef TSTP_Common::Time Time;

public:
    static void t0(const Time & t) { _t0 = t; }
    static void t1(const Time & t) { _t1 = t; }
    //static void t2(const Time & t = time_now()) { _t2 = t; }
    //static void t3(const Time & t = time_now()) { _t3 = t; }

    static void adjust();
    static void set();

private:
    static void init();

    static Time _t0, _t1;
};

__END_SYS

#endif

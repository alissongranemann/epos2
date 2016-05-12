#ifndef __tstp_time_h
#define __tstp_time_h

#include <timer.h>
#include <utility/math.h>

__BEGIN_SYS

// Passive Time Synchronization
class PTS : public TSTP_Common, private TSTPNIC::Observer
{
    friend class TSTP;
    friend class TSTPOE;

    typedef TSTP_Timer Timer;
    static const unsigned int TX_DELAY = TSTPNIC::TX_DELAY; 

    typedef TSTPNIC::Buffer Buffer;
    typedef TSTPNIC::Packet Packet;

    typedef TSTP_Timer::Time_Stamp Time_Stamp;
    static const IF<(static_cast<Time_Stamp>(-1) < static_cast<Time_Stamp>(0)), bool, void>::Result _time_stamp_sign_check; // TSTP_Timer::Time_Stamp must be signed!
    typedef TSTP_Common::Time_Stamp Short_Time_Stamp;
    typedef TSTP_Common::Microsecond Time;

    // Methods available to TSTP
    PTS() {
        _timer.start();
        TSTPNIC::attach(this, NIC_Observing_Condition::ALL);
    }
    ~PTS() {
        _timer.stop();
        TSTPNIC::detach(this, NIC_Observing_Condition::ALL);
    }

    static Time now() {
        auto t = _timer.now();
        auto t2 = ts_to_us(t);
        kout << t << " " << t2 << " hey" << endl;
        return t2;
        return ts_to_us(_timer.now()); 
    }

    static bool bootstrap() { return true; } //TODO


    // Methods available to TSTPNIC
    static void cancel_interrupt() { _timer.cancel_interrupt(); }

    static void interrupt(const Time & when) {
        auto tnow = _timer.now();
        Time_Stamp w = us_to_ts(when);
        if(when <= tnow) {
            _timer.interrupt(tnow + 1000, &int_handler); //TODO
        } else {
            _timer.interrupt(w, &int_handler);
        }
    }

    static void stamp_receive(Buffer * buf) { buf->sfd_time(_timer.sfd()); }

    static void stamp_send(Buffer * buf) { buf->frame()->data<Packet>()->last_hop_time(_timer.now()); }

    // Observer method
    void update(TSTPNIC::Observed * obs, NIC_Observing_Condition c, Buffer * buf) {
        Time_Stamp adjust = 0;
        auto packet = buf->frame()->data<Packet>();
        switch(packet->type()) {
        case INTEREST: {
            Short_Time_Stamp sfd = buf->sfd_time();
            Short_Time_Stamp to_set = packet->last_hop_time() + TX_DELAY;
            Time_Stamp diff1 = sfd - to_set;
            Time_Stamp diff2 = to_set - sfd;
            adjust = _UTIL::abs(diff1) < _UTIL::abs(diff2) ? diff1 : diff2;
            _timer.adjust(adjust);
            kout << "sfd = " << sfd << endl;
            kout << "to_set = " << to_set << endl;
            kout << "diff1 = " << diff1 << endl;
            kout << "diff2 = " << diff2 << endl;
            kout << "adjust = " << adjust << endl;
        } break;
        default: break;
        }
    }


    // Truly private methods and attributes (shouldn't be used by friends)
    static void int_handler(const unsigned int & interrupt) {} //TODO
    static Time ts_to_us(const Time_Stamp & ts) { return ts * 1000000ll / _timer.frequency(); }
    static Time_Stamp us_to_ts(const Time & us) { return us * _timer.frequency() / 1000000ll; }

    static Timer _timer; // TODO: several units?
};

__END_SYS

#endif

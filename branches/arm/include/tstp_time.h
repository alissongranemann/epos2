#ifndef __tstp_time_h
#define __tstp_time_h

__BEGIN_SYS

// Passive Time Synchronization
class PTS
{
    friend class TSTP;
    typedef Traits<TSTP>::Time_Config<> Config;
    typedef Config::Timer Timer;
    static const unsigned int TX_DELAY = Config::TX_DELAY;

    PTS(unsigned int unit) : _timer() {
        _timer.config();
        _timer.start();
        _pts = this; // TODO: several units if more than 1 NIC
    }

    TSTP * _tstp;

    bool bootstrap();

public:
    typedef long long Time;

    Time last_sfd() { return _timer.last_sfd(); }

private:
    static PTS * _pts; // TODO: several units if more than 1 NIC
    Time time_now() { return _timer.read(); }

    static void int_handler(const unsigned int & interrupt);

    void cancel_interrupt() {
        _timer.int_disable();
    }

    void interrupt(const Time & when) {
        //db<PTS>(TRC) << "PTS::interrupt(when=" << when << ")" << endl;
        //db<PTS>(TRC) << "now=" << time_now() << endl;
        _timer.interrupt(when, &int_handler);
    }

    void update_interest(char * header);
    void update_data(char * header) {
        if(not Traits<TSTP>::is_sink) {
            do_update_data(header); 
        }
    }

    void do_update_data(char * header);

    Timer _timer;
};

__END_SYS

#endif

#ifndef __tstp_time_h
#define __tstp_time_h

__BEGIN_SYS

class PTP 
{
    friend class TSTP;

    PTP(unsigned int unit);

    TSTP * _tstp;

    bool bootstrap();

public:
    typedef int Time;

//private: TODO:remove comment
    static PTP * _ptp; // TODO
    Time time_now();
    void interrupt(const Time & when);
    void handle(unsigned int interrupt);
    static void int_handler(const unsigned int & interrupt);
};

__END_SYS

#endif

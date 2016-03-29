#ifndef __tstp_time_h
#define __tstp_time_h

__BEGIN_SYS

class PTP 
{
    friend class TSTP;    

    PTP(unsigned int unit) { }

    TSTP * _tstp;

    bool bootstrap();

public:
    typedef int Time;

private:
    Time time_now() { return 0; } // TODO
};

__END_SYS

#endif

#ifndef __tstp_time_h
#define __tstp_time_h

__BEGIN_SYS

class PTP 
{
    friend class TSTP;    

    PTP(unsigned int unit) { }

    TSTP * _tstp;
};

__END_SYS

#endif

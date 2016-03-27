#ifndef __tstp_security_h
#define __tstp_security_h

__BEGIN_SYS

class TSTP_Security 
{
    friend class TSTP;    

    TSTP_Security(unsigned int unit) { }

    TSTP * _tstp;
};

__END_SYS

#endif

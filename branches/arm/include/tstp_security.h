#ifndef __tstp_security_h
#define __tstp_security_h

#include <tstp_time.h>
#include <tstp_router.h>

__BEGIN_SYS

class TSTP_Security 
{
    friend class TSTP;    

    TSTP_Security(unsigned int unit) { }

    TSTP * _tstp;

    bool bootstrap();

public:
    typedef unsigned char MAC[16];
};

__END_SYS

#endif

#ifndef __tstp_router_h
#define __tstp_router_h

__BEGIN_SYS

class Geo_Greedy_Router 
{
    friend class TSTP;    

    Geo_Greedy_Router(unsigned int unit) { }

    TSTP * _tstp;

public:
    class Address { };
};

__END_SYS

#endif

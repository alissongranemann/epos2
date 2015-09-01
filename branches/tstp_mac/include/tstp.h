// EPOS TSTP Mediator Common Package

#include <nic.h>

#ifndef __tstp_h
#define __tstp_h

#include <cpu.h>
#include <utility/list.h>
#include <utility/observer.h>

__BEGIN_SYS

template<typename Addr>
class TSTP
{
public:
    typedef Addr Address;
};

__END_SYS

#endif

#ifndef __traits_unified_h
#define __traits_unified_h

#include <system/config.h>

__BEGIN_SYS

template<typename T>
struct Traits
{
    static const bool hardware = false;
};

template<> struct Traits<Build>
{
#ifdef HIGH_LEVEL_SYNTHESIS
    static const bool hardware_domain = true;
#else
    static const bool hardware_domain = false;
#endif
};

template<> struct Traits<Adder>: public Traits<void>
{
    static const bool hardware = true;
};

__END_SYS

#endif

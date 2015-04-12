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
    const static bool hardware_domain = true;
#else
    const static bool hardware_domain = false;
#endif
};

template<> struct Traits<Adder>: public Traits<void>
{
    static const bool hardware = true;

    static const bool singleton = false;
    const static bool static_alloc = false;

    typedef void Alloc_Obj_Type;
    typedef void Alloc_Idx;

    const static unsigned int Alloc_Max = 0;
    const static unsigned int serdes_buffer = 8;
    static const unsigned int n_ids = 1;
};

__END_SYS

#endif

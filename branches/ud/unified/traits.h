#ifndef __traits_unified_h
#define __traits_unified_h

template<> struct Traits<Add>: public Traits<void>
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

#endif

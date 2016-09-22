// EPOS ARMv7 Architecture Metainfo
#ifndef __armv7_traits_h
#define __armv7_traits_h

#include <system/config.h>

__BEGIN_SYS

template<> struct Traits<CPU>: public Traits<void>
{
    enum {LITTLE, BIG};
    static const unsigned int ENDIANESS         = LITTLE;
    static const unsigned int WORD_SIZE         = 32;
    static const unsigned int CLOCK             = Traits<Build>::MODEL == Traits<Build>::LM3S811 ? 50000000 : Traits<Build>::MODEL == Traits<Build>::Zynq ? 666666687 : 32000000;
    static const bool unaligned_memory_access   = false;
};

template<> struct Traits<MMU>: public Traits<void>
{
    static const bool colorful = false;
    static const unsigned int COLORS = 1;
};

template<> struct Traits<TSC>: public Traits<void>
{
    static const bool enabled = (Traits<Build>::MODEL == Traits<Build>::Zynq);
//TODO: http://stackoverflow.com/questions/16236460/arm-cortex-a9-event-counters-return-0
};

__END_SYS

#endif

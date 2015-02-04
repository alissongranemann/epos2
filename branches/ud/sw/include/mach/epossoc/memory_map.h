// EPOS EPOSSOC Memory Map

#ifndef __epossoc_memory_map_h
#define __epossoc_memory_map_h

#include <system/memory_map.h>

__BEGIN_SYS

template <>
struct Memory_Map<EPOSSOC>
{
    // Physical Memory
    enum {
        MEM_BASE =	Traits<EPOSSOC>::MEM_BASE,
        MEM_TOP =	Traits<EPOSSOC>::MEM_TOP
    };

    // Logical Address Space
    enum {
        APP_LOW =   Traits<EPOSSOC>::APP_LOW,
        APP_CODE =  Traits<EPOSSOC>::APP_CODE,
        APP_DATA =  Traits<EPOSSOC>::APP_DATA,
        APP_HIGH =  Traits<EPOSSOC>::APP_HIGH,

        PHY_MEM =   Traits<EPOSSOC>::PHY_MEM,
        IO =        Traits<EPOSSOC>::IO_BASE,
        INT_VEC =   0x0000003C,

        SYS =       Traits<EPOSSOC>::SYS,
        SYS_INFO =  SYS + 0x00000000,
        SYS_CODE =  SYS + 0x00000200,
        SYS_DATA =  SYS + 0x00002000,
        SYS_STACK = SYS + 0x00005000,
        SYS_HEAP =  SYS + 0x00008000
    };
};

__END_SYS

#endif

// EPOS Cortex Memory Map

#ifndef __cortex_memory_map_h
#define __cortex_memory_map_h

#include <system/memory_map.h>

__BEGIN_SYS

template <>
struct Memory_Map<Cortex>
{
    // Physical Memory
    enum {
        MEM_BASE        = Traits<Cortex>::MEM_BASE,
        MEM_TOP         = Traits<Cortex>::MEM_TOP
    };

    // Logical Address Space
    enum {
        APP_LOW         = Traits<Cortex>::APP_LOW,
        APP_CODE        = Traits<Cortex>::APP_CODE,
        APP_DATA        = Traits<Cortex>::APP_DATA,
        APP_HIGH        = Traits<Cortex>::APP_HIGH,

        PHY_MEM         = Traits<Cortex>::PHY_MEM,
        IO              = Traits<Cortex>::IO_BASE,

        SYS             = Traits<Cortex>::SYS,
        SYS_INFO        = unsigned(-1),                 // Not used during boot. Dynamically built during initialization.
        SYS_CODE        = Traits<Cortex>::SYS_CODE,
        SYS_DATA        = Traits<Cortex>::SYS_DATA,
        SYS_HEAP        = SYS_DATA,                     // Not used (because multiheap can only be enabled with an MMU)
        SYS_STACK       = MEM_TOP + 1 - Traits<Cortex>::STACK_SIZE      // This stack is used before main(). The stack pointer is initialized at crt0.S
    };
};

/*
template <>
struct IO_Map<Cortex>
{
    enum {
        ITC_BASE                = 0x80020000,
        ITC_NIPEND              = ITC_BASE + 0x38,
    };
};
*/

__END_SYS

#endif

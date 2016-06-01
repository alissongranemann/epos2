// EPOS Cortex-A Memory Map

#ifndef __cortex_a_memory_map_h
#define __cortex_a_memory_map_h

#include <system/memory_map.h>

__BEGIN_SYS

template <>
struct Memory_Map<Cortex_A>
{
    // Physical Memory
    enum {
        MEM_BASE    = Traits<Cortex_A>::MEM_BASE,
        MEM_TOP     = Traits<Cortex_A>::MEM_TOP
    };

    // Logical Address Space
    enum {
        APP_LOW     = Traits<Cortex_A>::APP_LOW,
        APP_CODE    = Traits<Cortex_A>::APP_CODE,
        APP_DATA    = Traits<Cortex_A>::APP_DATA,
        APP_HIGH    = Traits<Cortex_A>::APP_HIGH,

        PHY_MEM     = Traits<Cortex_A>::PHY_MEM,
        IO          = Traits<Cortex_A>::IO_BASE,

        SYS         = Traits<Cortex_A>::SYS,
        // Not used during boot. Dynamically built during initialization.
        SYS_INFO    = unsigned(-1),
        SYS_CODE    = Traits<Cortex_A>::SYS_CODE,
        SYS_DATA    = Traits<Cortex_A>::SYS_DATA,
        SYS_HEAP    = SYS_DATA,
        // This stack is used before main(). The stack pointer is initialized at
        // crt0.S
        SYS_STACK   = MEM_TOP + 1 - Traits<Cortex_A>::STACK_SIZE
    };
};

__END_SYS

#endif


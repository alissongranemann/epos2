// EPOS Zynq Memory Map

#ifndef __memory_map_zynq_h
#define __memory_map_zynq_h

#include <system/memory_map.h>

__BEGIN_SYS

template <>
struct Memory_Map<Zynq>
{
    // Physical Memory
    enum {
        MEM_BASE    = Traits<Zynq>::MEM_BASE,
        MEM_TOP     = Traits<Zynq>::MEM_TOP
    };

    // Logical Address Space
    enum {
        APP_LOW     = Traits<Zynq>::APP_LOW,
        APP_CODE    = Traits<Zynq>::APP_CODE,
        APP_DATA    = Traits<Zynq>::APP_DATA,
        APP_HIGH    = Traits<Zynq>::APP_HIGH,

        PHY_MEM     = Traits<Zynq>::PHY_MEM,
        IO          = Traits<Zynq>::IO_BASE,

        SYS         = Traits<Zynq>::SYS,
        // Not used during boot. Dynamically built during initialization.
        SYS_INFO    = unsigned(-1),
        SYS_CODE    = Traits<Zynq>::SYS_CODE,
        SYS_DATA    = Traits<Zynq>::SYS_DATA,
        SYS_HEAP    = SYS_DATA,
        // This stack is used before main(). The stack pointer is initialized at
        // crt0.S
        SYS_STACK   = MEM_TOP + 1 - Traits<Zynq>::STACK_SIZE
    };
};

__END_SYS

#endif


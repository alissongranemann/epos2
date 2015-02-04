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
        MEM_BASE =  Traits<Zynq>::MEM_BASE,
        MEM_TOP =   Traits<Zynq>::MEM_TOP
    };

    // Logical Address Space
    enum {
        //BASE =      0x82000000,
        TOP =       0x1ffffffc,

        //APP_LO =    0x82000000,
        APP_CODE =  Traits<Zynq>::APP_CODE,
        APP_DATA =  Traits<Zynq>::APP_DATA,
        //APP_HI =    0x83EFFFFF,

        PHY_MEM =	0x20000000,//512mb
        //IO_MEM =    0x48000000,
        //APIC_MEM =  0x00000000,
        //SYS =       Traits<Zynq>::SYS,
		/*
        IDT =       SYS + 0x00010000,
        GDT =       SYS + 0x00020000,
        SYS_PT =    SYS + 0x00030000,
        SYS_PD =    SYS + 0x00040000,
		*/
        SYS_INFO =  Traits<Zynq>::SYS, //Used in machine_init.cc
		/*
        SYS_CODE =  SYS + 0x00060000,
        SYS_DATA =  SYS + 0x00070000,
        SYS_STACK = SYS + 0x00080000
		*/
		SYS_HEAP = Traits<Zynq>::SYS_HEAP,
    };
};

__END_SYS

#endif


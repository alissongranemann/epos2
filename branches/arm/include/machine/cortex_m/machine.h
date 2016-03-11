// EPOS Cortex_M Mediator Declarations

#ifndef __cortex_m_h
#define __cortex_m_h

#include <utility/list.h>
#include <cpu.h>
#include <mmu.h>
#include <tsc.h>
#include <machine.h>
#include <rtc.h>
#include <usb.h>
#include __MODEL_H
#include "info.h"
#include "memory_map.h"
#include "ic.h"

__BEGIN_SYS

class Cortex_M: private Machine_Common, private Cortex_M_Model
{
    friend class Init_System;

public:
    Cortex_M() {}

    static unsigned int cpu_id() { return 0; }
    static unsigned int n_cpus() { return 1; }

    static void panic();
    static void reboot() { 
        db<Machine>(WRN) << "Machine::reboot()" << endl;
        Reg32 val = scs(AIRCR) & (~((-1u / VECTKEY) * VECTKEY));
        val |= SYSRESREQ;
        val |= 0x05FA * VECTKEY;
        scs(AIRCR) =  val;
    }
    static void poweroff() { reboot(); }

    static void smp_barrier() {};
    static void smp_init(unsigned int) {};

private:
    static void init();
};

__END_SYS

#ifdef __TIMER_H
#include __TIMER_H
#endif
#ifdef __UART_H
#include __UART_H
#endif
#ifdef __NIC_H
#include __NIC_H
#endif

#endif

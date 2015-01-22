// EPOS Cortex_M Mediator Declarations

#ifndef __cortex_m_h
#define __cortex_m_h

#include <utility/list.h>
#include <cpu.h>
#include <mmu.h>
#include <tsc.h>
#include <machine.h>
#include <rtc.h>
#include __MODEL_H
#include "info.h"
#include "memory_map.h"
#include "ic.h"

__BEGIN_SYS

class Cortex_M: private Machine_Common, private Cortex_Model_Specifics
{
    friend class Init_System;

public:
    Cortex_M() {}

    static unsigned int cpu_id() { return 0; }
    static unsigned int n_cpus() { return 1; }

    static void panic();
    static void reboot() { 
        db<Cortex_M>(WRN) << "Machine::reboot()" << endl;
        scs(AIRCR) |=  SYSRESREQ ;
        for(;;); // TODO: the above is not working!
    }
    static void poweroff() { reboot(); }

    static void smp_barrier() {};
    static void smp_init(unsigned int) {};

private:
    static void init();
};

__END_SYS

#include "timer.h"
#include "rtc.h"
#include "uart.h"
#include "nic.h"

#endif

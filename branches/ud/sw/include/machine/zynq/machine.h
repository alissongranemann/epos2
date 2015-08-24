// EPOS Zynq Mediator Declarations

#ifndef __zynq_h
#define __zynq_h

#include <machine.h>
#include <cpu.h>
#include <mmu.h>
#include <tsc.h>

__BEGIN_SYS

class Zynq: public Machine_Common
{
private:
    friend class Init_System;

    typedef CPU_Common::Log_Addr Log_Addr;

    enum {
        SLCR_BASE = 0xF8000000
    };

    // SLCR Registers offsets
    enum {
        PSS_RST_CTRL = 0x200
    };

public:
    Zynq() {}

    static unsigned int n_cpus() { return Traits<Machine>::CPUS; }
    static unsigned int cpu_id() {
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5"
            : "=r"(id)
            : : );
        return id & 0x3;
    }

    static void panic() {}
    static void reboot() {
        db<Machine>(WRN) << "Machine::reboot()" << endl;
        // FIXME: The line bellow will mess with qemu but seems to be working on
        // real hardware, possibly a bug in qemu. Note that the asserting reset
        // will clear the RAM where the application is stored.
        //slcr(PSS_RST_CTRL) = 1;
        while(1);
    }

    static void poweroff() {}

    static void smp_barrier(unsigned int n_cpus = n_cpus()) {
        static volatile unsigned int ready[2];
        static volatile unsigned int i;

        if(smp) {
            int j = i;

            CPU::finc(ready[j]);

            if(cpu_id() == 0) {
                while(ready[j] < n_cpus); // wait for all CPUs to be ready
                i = !i;                   // toggle ready
                ready[j] = 0;             // signalizes waiting CPUs
            } else
                while(ready[j]);          // wait for CPU[0] signal
        }
    }
    static void smp_init(unsigned int n_cpus) {};

private:
    static void init();

    static Log_Addr & slcr(unsigned int o) { return reinterpret_cast<Log_Addr *>(SLCR_BASE)[o / sizeof(Log_Addr)]; }

private:
    static const bool smp = Traits<System>::multicore;
};

__END_SYS

#include "info.h"
#include "uart.h"
#include "rtc.h"
#include "timer.h"
#include "nic.h"
#include "gpio.h"

#endif

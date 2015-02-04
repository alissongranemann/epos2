// EPOS Zynq Mediator Declarations

#ifndef __zyqn_h
#define __zyqn_h

#include <machine.h>
#include <cpu.h>
#include <mmu.h>
#include <tsc.h>

__BEGIN_SYS

class Zynq: public Machine_Common
{
    friend class Init_System;

public:
    Zynq() {}

    static void panic() {
        db<Zynq>(ERR) << "PANIC!" << endl;
        CPU::int_disable();
        for(;;);
    }

    static void reboot() {
        db<Zynq>(TRC) << "Machine::reboot()" << endl;
        // qemu is messing with the console when we reset the board
        // this for() will avoid reseting by now
        //for(;;);
        CPU::out32(CM_CTRL, CPU::in32(CM_CTRL) | (1 << 3));
    }

    static void poweroff() {  for(;;); }

    static unsigned int n_cpus() { return Traits<Zynq>::CPUS; }

    static unsigned int cpu_id() {
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5"
            : "=r"(id)
            : : );
        return id & 0x3; //cpu ID varies from 0 to 3
    }

    static void smp_init(unsigned int n_cpus) {};

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
    };
private:
    static const unsigned long CM_CTRL = 0x1000000C;

private:
    static void init();

private:
    static const bool smp = Traits<System>::multicore;
};

__END_SYS

#include "flash.h"
#include "info.h"
#include "uart.h"
#include "rtc.h"
#include "timer.h"
#include "adc.h"
#include "nic.h"
#include "gpio.h"

#endif

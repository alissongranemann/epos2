// EPOS Cortex_A Mediator Declarations

#ifndef __cortex_a_h
#define __cortex_a_h

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

class Cortex_A: public Machine_Common, private Cortex_A_Model
{
    friend class Init_System;

private:
    static const bool smp = Traits<System>::multicore;

public:
    Cortex_A() {}

    static void delay(const RTC::Microsecond & time) {
        TSC::Time_Stamp end = TSC::time_stamp() + time * (TSC::frequency() / 1000000);
        while(end > TSC::time_stamp());
    }

    static void panic();
    static void reboot();
    static void poweroff() { reboot(); }

    static unsigned int n_cpus() { return smp ? _n_cpus : 1; }
    static unsigned int cpu_id() { return smp ? Cortex_A_Model::cpu_id() : 0; }

    static void smp_init(unsigned int n_cpus) {
        if(smp)
            _n_cpus = n_cpus;
    };
    static void smp_barrier(unsigned int n_cpus = n_cpus()) {
        static volatile unsigned int ready[2];
        static volatile unsigned int i;

        if(smp) {
            int j = i;

            CPU::finc(ready[j]);

            if(cpu_id() == 0) {
                while(ready[j] < n_cpus); // Wait for all CPUs to be ready
                i = !i;                   // Toggle ready
                ready[j] = 0;             // Signalizes waiting CPUs
            } else
                while(ready[j]);          // Wait for CPU[0] signal
        }
    }

private:
    static void init();

private:
    static volatile unsigned int _n_cpus;
};

__END_SYS

#ifdef __TIMER_H
#include __TIMER_H
#endif
#ifdef __RTC_H
#include __RTC_H
#endif
#ifdef __UART_H
#include __UART_H
#endif
#ifdef __DISPLAY_H
#include __DISPLAY_H
#endif
#ifdef __GPIO_H
#include __GPIO_H
#endif
#ifdef __NIC_H
#include __NIC_H
#endif

#endif

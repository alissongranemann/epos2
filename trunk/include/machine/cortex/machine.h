// EPOS Cortex Mediator Declarations

#ifndef __cortex_h
#define __cortex_h

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

class Cortex: private Machine_Common, private Cortex_Model
{
    friend class Init_System;

public:
    Cortex() {}

    static void delay(const RTC::Microsecond & time);
//    {
//        eMote3_GPTM g(3, time_microseconds);
//        g.enable();
//        while(g.running());
//    }

    static void panic();
    static void reboot();
    static void poweroff() { reboot(); }

    static unsigned int n_cpus() { return 1; }
    static unsigned int cpu_id() { return 0; }

    static void smp_barrier() {};
    static void smp_init(unsigned int) {};

private:
    static void init();
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
#ifdef __USB_H
#include __USB_H
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

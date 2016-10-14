// EPOS LM3S811 (Cortex-M3) MCU Mediators Configuration

#ifndef __config_h
#define __config_h

#include <system/meta.h>
#include __APPL_TRAITS_H

#define __CPU_H         __HEADER_ARCH(cpu)
#define __TSC_H         __HEADER_ARCH(tsc)
#define __MMU_H         __HEADER_ARCH(mmu)

#define __MACH_H        __HEADER_MACH(machine)
#define __MODEL_H       __HEADER_MACH(MMOD)
#define __IC_H          __HEADER_MACH(ic)
#define __TIMER_H       __HEADER_MACH(timer)
#define __RTC_H         __HEADER_MACH(rtc)
#define __UART_H        __HEADER_MACH(uart)
#define __USB_H         __HEADER_MACH(usb)
#define __DISPLAY_H     __HEADER_MACH(display)
#define __GPIO_H        __HEADER_MACH(gpio)
#define __NIC_H         __HEADER_MACH(nic)

__BEGIN_SYS

typedef ARMv7              CPU;
typedef ARMv7_MMU          MMU;
typedef ARMv7_TSC          TSC;

typedef Cortex_M             Machine;
typedef IC          IC;
typedef Timer       Timer;
typedef RTC         RTC;
typedef UART        UART;

typedef SWITCH<Traits<Display>::ENGINE, 
        CASE<Traits<Display>::Engine::null, Null_Display,
        CASE<Traits<Display>::Engine::uart, Serial_Display
        >>>::Result          Display;

typedef Radio       NIC;
typedef Scratchpad  Scratchpad;
typedef USB         USB;
typedef GPIO        GPIO;

__END_SYS

#endif

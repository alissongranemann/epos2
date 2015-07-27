// EPOS LM3S811 (Cortex-M3) MCU Mediators Configuration

#ifndef __cortex_m_config_h
#define __cortex_m_config_h

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

__BEGIN_SYS

typedef ARMv7              CPU;
typedef ARMv7_MMU          MMU;
typedef ARMv7_TSC          TSC;

typedef Cortex_M             Machine;
typedef Cortex_M_IC          IC;
typedef Cortex_M_Timer       Timer;
typedef Cortex_M_RTC         RTC;
typedef Cortex_M_UART        UART;
typedef IF<Traits<Serial_Display>::enabled, Serial_Display, Dummy>::Result Display;
typedef Cortex_M_Radio       NIC;
typedef Cortex_M_Scratchpad  Scratchpad;
typedef Cortex_M_USB         USB;

__END_SYS

#endif

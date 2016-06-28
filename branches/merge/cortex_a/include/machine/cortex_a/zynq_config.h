// EPOS Zynq-7000 (Cortex-A9) Mediators Configuration

#ifndef __machine_config_h
#define __machine_config_h

#include <system/meta.h>
#include __APPL_TRAITS_H

#define __CPU_H                     __HEADER_ARCH(cpu)
#define __TSC_H                     __HEADER_ARCH(tsc)
#define __MMU_H                     __HEADER_ARCH(mmu)

#define __MACH_H                    __HEADER_MACH(machine)
#define __MODEL_H                   __HEADER_MACH(MMOD)
#define __IC_H                      __HEADER_MACH(ic)
#define __TIMER_H                   __HEADER_MACH(timer)
#define __UART_H                    __HEADER_MACH(uart)
#define __NIC_H                     __HEADER_MACH(nic)
#define __RTC_H                     __HEADER_MACH(rtc)

__BEGIN_SYS

typedef ARMv7                           CPU;
typedef ARMv7_MMU                       MMU;
typedef ARMv7_TSC                       TSC;

typedef Cortex_A                        Machine;
typedef Cortex_A_IC                     IC;
typedef Cortex_A_UART                   UART;
typedef void                            USB;
typedef Cortex_A_Timer                  Timer;
typedef Serial_Display                  Display;
typedef Cortex_A_RTC                    RTC;
typedef Cortex_A_Scratchpad             Scratchpad;
typedef Cortex_A_Ethernet               NIC;

__END_SYS

#endif

// EPOS Zynq-7000 (Cortex-A9) Mediators Configuration

#ifndef __zynq_config_h
#define __zynq_config_h

#include <system/meta.h>
#include __APPL_TRAITS_H

#define __CPU_H                     __HEADER_ARCH(cpu)
#define __TSC_H                     __HEADER_ARCH(tsc)
#define __MMU_H                     __HEADER_ARCH(mmu)

#define __MACH_H                    __HEADER_MACH(machine)
#define __MODEL_H                   __HEADER_MACH(MMOD)
#define __GPIO_H                    __HEADER_MACH(gpio)
#define __IC_H                      __HEADER_MACH(ic)
#define __TIMER_H                   __HEADER_MACH(timer)
#define __UART_H                    __HEADER_MACH(uart)
//#define __NIC_H                     __HEADER_MACH(nic)
#define __RTC_H                     __HEADER_MACH(rtc)
#define __COMPONENT_CONTROLLER_H    __HEADER_MACH(component_controller)
#define __PCAP_H                    __HEADER_MACH(pcap)
#define __TRAFFIC_GEN_H             __HEADER_MACH(traffic_gen)
#define __AXI_PERF_MON_H            __HEADER_MACH(axi_perf_mon)

__BEGIN_SYS

typedef ARMv7                           CPU;
typedef ARMv7_MMU                       MMU;
typedef ARMv7_TSC                       TSC;

typedef Cortex_A                        Machine;
typedef Cortex_A_GPIO                   GPIO;
typedef Cortex_A_IC                     IC;
typedef Cortex_A_UART                   UART;
typedef Cortex_A_Timer                  Timer;
typedef Serial_Display                  Display;
typedef Cortex_A_NIC                    NIC;
typedef Cortex_A_RTC                    RTC;
typedef Cortex_A_Component_Controller   Component_Controller;
typedef Cortex_A_PCAP                   PCAP;
typedef Cortex_A_Traffic_Gen            Traffic_Gen;
typedef Cortex_A_AXI_Perf_Mon           AXI_Perf_Mon;
typedef Cortex_A_Scratchpad             Scratchpad;

__END_SYS

#endif

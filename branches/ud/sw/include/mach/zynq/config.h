// EPOS Zynq Mediators Configuration

#ifndef __zynq_config_h
#define __zynq_config_h

#include __APPLICATION_TRAITS_H

#define __CPU_H                     __HEADER_ARCH(cpu)
#define __TSC_H                     __HEADER_ARCH(tsc)
#define __MMU_H                     __HEADER_ARCH(mmu)

#define __MACH_H                    __HEADER_MACH(machine)
#define __GPIO_H                    __HEADER_MACH(gpio)
#define __IC_H                      __HEADER_MACH(ic)
#define __TIMER_H                   __HEADER_MACH(timer)
#define __UART_H                    __HEADER_MACH(uart)
#define __NIC_H                     __HEADER_MACH(nic)
#define __RTC_H                     __HEADER_MACH(rtc)
#define __COMPONENT_CONTROLLER_H    __HEADER_MACH(component_controller)
#define __PCAP_H                    __HEADER_MACH(pcap)

__BEGIN_SYS

typedef ARMV7                       CPU;
typedef ARMV7_MMU                   MMU;
typedef ARMV7_TSC                   TSC;

typedef Zynq                        Machine;
typedef Zynq_GPIO                   GPIO;
typedef Zynq_IC                     IC;
typedef Zynq_UART                   UART;
typedef Zynq_Timer                  Timer;
typedef Serial_Display              Display;
typedef Zynq_NIC                    NIC;
typedef Zynq_RTC                    RTC;
typedef Zynq_Component_Controller   Component_Controller;
typedef Zynq_PCAP                   PCAP;

// TODO: Fix this ugly workaround for ctti.h
typedef Dummy<1>                    PCI;
typedef Dummy<2>                    NOC;
typedef Dummy<3>                    EEPROM;

__END_SYS

#endif

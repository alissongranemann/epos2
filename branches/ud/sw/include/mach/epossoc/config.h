// EPOS EPOSSOC Mediators Configuration

#ifndef __epossoc_config_h
#define __epossoc_config_h

#include __APPLICATION_TRAITS_H

#define __CPU_H                     __HEADER_ARCH(cpu)
#define __TSC_H                     __HEADER_ARCH(tsc)
#define __MMU_H                     __HEADER_ARCH(mmu)

#define __MACH_H                    __HEADER_MACH(machine)
#define __NOC_H                     __HEADER_MACH(noc)
#define __IC_H                      __HEADER_MACH(ic)
#define __TIMER_H                   __HEADER_MACH(timer)
#define __UART_H                    __HEADER_MACH(uart)
#define __RTC_H                     __HEADER_MACH(rtc)
#define __NIC_H                     __HEADER_MACH(nic)
#define __COMPONENT_CONTROLLER_H    __HEADER_MACH(component_controller)
#define __PCAP_H                    __HEADER_MACH(pcap)

__BEGIN_SYS

typedef MIPS32                          CPU;
typedef MIPS32_MMU                      MMU;
typedef MIPS32_TSC                      TSC;
typedef Dummy<0>                        PMU;

typedef EPOSSOC                         Machine;
typedef Dummy<1>                        PCI;
typedef EPOSSOC_NOC                     NOC;
typedef Dummy<2>                        CAN;
typedef EPOSSOC_IC                      IC;
typedef EPOSSOC_Timer                   Timer;
typedef EPOSSOC_RTC                     RTC;
typedef Dummy<3>                        EEPROM;
typedef Dummy<4>                        Flash;
typedef EPOSSOC_UART                    UART;
typedef Dummy<5>                        USART;
typedef Dummy<6>                        SPI;
typedef Serial_Display                  Display;
typedef EPOSSOC_NIC                     NIC;
typedef EPOSSOC_Component_Controller    Component_Controller;
typedef Dummy<7>                        Radio;
typedef Dummy<8>                        GPIO_Pin;
typedef Dummy<9>                        ADC;
typedef Dummy<10>                       Battery;
typedef Dummy<11>                       Temperature_Sensor;
typedef Dummy<12>                       Photo_Sensor;
typedef Dummy<13>                       Accelerometer;
typedef EPOSSOC_PCAP                    PCAP;

__END_SYS

#endif

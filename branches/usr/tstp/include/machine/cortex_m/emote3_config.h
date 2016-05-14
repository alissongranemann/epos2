// EPOS EPOSMoteIII (Cortex-M4) Mediators Configuration

#ifndef __emote3_config_h
#define __emote3_config_h

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
#define __EEPROM_H      __HEADER_MACH(eeprom)
#define __UART_H        __HEADER_MACH(uart)
#define __DISPLAY_H     __HEADER_MACH(display)
#define __NIC_H         __HEADER_MACH(nic)
#define __USB_H         __HEADER_MACH(usb)
//#define __SCRATCHPAD_H  __HEADER_MACH(scratchpad)
//#define __SPI_H         __HEADER_MACH(spi)
//#define __I2C_H         __HEADER_MACH(i2c)
#define __GPIO_H        __HEADER_MACH(gpio)
#define __ADC_H         __HEADER_MACH(adc)
#define __FLASH_H       __HEADER_MACH(emote3_flash)

__BEGIN_SYS

typedef ARMv7              CPU;
typedef ARMv7_MMU          MMU;
typedef ARMv7_TSC          TSC;

typedef Cortex_M             Machine;
typedef Cortex_M_IC          IC;
typedef Cortex_M_Timer       Timer;
typedef Cortex_M_RTC         RTC;
typedef Cortex_M_EEPROM      EEPROM;
typedef Cortex_M_UART        UART;
typedef Cortex_M_USB         USB;

typedef SWITCH<Traits<Cortex_M_Display>::ENGINE, 
        CASE<Traits<Cortex_M_Display>::Engine::null, Null_Display,
        CASE<Traits<Cortex_M_Display>::Engine::uart, Serial_Display,
        CASE<Traits<Cortex_M_Display>::Engine::usb, Cortex_M_USB_Serial_Display
        >>>>::Result         Display;

typedef Cortex_M_Radio       NIC;
typedef Cortex_M_Scratchpad  Scratchpad;
typedef Cortex_M_GPIO        GPIO;
typedef Cortex_M_ADC         ADC;
typedef eMote3_Flash         Flash;

__END_SYS

#endif

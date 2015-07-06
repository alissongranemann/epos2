// EPOS EPOSMoteIII (Cortex-M4) MCU Mediator Declarations
#ifndef __emote3_h
#define __emote3_h

#include <cpu.h>
#include <tsc.h>
#include <utility/handler.h>

__BEGIN_SYS

class eMote3
{
public:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Reg32 Reg32;

public:
    // Base address for memory-mapped System Control Registers
    enum {
        UART0_BASE      = 0x4000c000,
        UART1_BASE      = 0x4000d000,
        SCR_BASE        = 0x400d2000,
        IOC_BASE        = 0x400d4000
    };

    // System Control Registers offsets
    enum {                              // Description                                          Type    Value after reset
        CLOCK_CTRL      = 0x00,
        CLOCK_STA       = 0x04,
        RCGCGPT         = 0x08,
        SCGCGPT         = 0x0C,
        DCGCGPT         = 0x10,
        SRGPT           = 0x14,
        RCGCSSI         = 0x18,
        SCGCSSI         = 0x1C,
        DCGCSSI         = 0x20,
        SRSSI           = 0x24,
        RCGCUART        = 0x28,
        SCGCUART        = 0x2C,
        DCGCUART        = 0x30,
        SRUART          = 0x34,
        RCGCI2C         = 0x38,
        SCGCI2C         = 0x3C,
        DCGCI2C         = 0x40,
        SRI2C           = 0x44,
        RCGCSEC         = 0x48,
        SCGCSEC         = 0x4C,
        DCGCSEC         = 0x50,
        SRSEC           = 0x54,
        PMCTL           = 0x58,
        SRCRC           = 0x5C,
        PWRDBG          = 0x74,
        CLD             = 0x80,
        IWE             = 0x94,
        I_MAP           = 0x98,
        RCGCRFC         = 0xA8,
        SCGCRFC         = 0xAC,
        DCGCRFC         = 0xB0,
        EMUOVR          = 0xB4
    };
    enum {
        UART0  = 1 << 0,
        UART1  = 1 << 1
    };
    enum RCGCRFC {
        RCGCRFC_RFC0  = 1 << 0,
    };
    enum I_MAP {
        I_MAP_ALTMAP = 1 << 0,
    };

    // Useful Bits in the Clock Control Register
    enum {             // Description                                      Type    Value after reset
        OSC32K_CALDIS = 1 << 25,  // Disable calibration 32-kHz RC oscillator.        rw      0
                                  // 0: Enable calibration
                                  // 1: Disable calibration
        OSC32K        = 1 << 24,  // 32-kHz clock oscillator selection                rw      1
                                  // 0: 32-kHz crystal oscillator
                                  // 1: 32-kHz RC oscillator
        AMP_DET       = 1 << 21,  // Amplitude detector of XOSC during power up       rw      0
                                  // 0: No action
                                  // 1: Delay qualification of XOSC until amplitude
                                  //    is greater than the threshold.
        OSC_PD        = 1 << 17,  // 0: Power up both oscillators
                                  // 1: Power down oscillator not selected by
                                  //    OSC bit (hardware-controlled when selected).
        OSC           = 1 << 16,  // System clock oscillator selection                rw      1
                                  // 0: 32-MHz crystal oscillator
                                  // 1: 16-MHz HF-RC oscillator
        IO_DIV        = 1 <<  8,  // I/O clock rate setting                           rw      1
                                  // Cannot be higher than OSC setting
                                  // 000: 32 MHz
                                  // 001: 16 MHz
                                  // 010: 8 MHz
                                  // 011: 4 MHz
                                  // 100: 2 MHz
                                  // 101: 1 MHz
                                  // 110: 0.5 MHz
                                  // 111: 0.25 MHz
        SYS_DIV       = 1 <<  0,  // System clock rate setting                        rw      1
                                  // Cannot be higher than OSC setting
                                  // 000: 32 MHz
                                  // 001: 16 MHz
                                  // 010: 8 MHz
                                  // 011: 4 MHz
                                  // 100: 2 MHz
                                  // 101: 1 MHz
                                  // 110: 0.5 MHz
                                  // 111: 0.25 MHz
    };
    // Useful Bits in the Clock Status Register
    enum CLOCK_STA {                 // Description                                      Type    Value after reset
        STA_SYNC_32K      = 1<<26,   // 32-kHz clock source synced to undivided          ro      0
                                     // system clock (16 or 32 MHz).
        STA_OSC32K_CALDIS = 1<<25,   // Disable calibration 32-kHz RC oscillator.        ro      0
        STA_OSC32K        = 1<<24,   // Current 32-kHz clock oscillator selected.        ro      1
                                     // 0: 32-kHz crystal oscillator
                                     // 1: 32-kHz RC oscillator
        STA_RST           = 1<<22,   // Returns last source of reset                     ro      0
                                     // 00: POR
                                     // 01: External reset
                                     // 10: WDT
                                     // 11: CLD or software reset
        STA_SOURCE_CHANGE = 1<<20,   // 0: System clock is not requested to change.      ro      0
                                     // 1: A change of system clock source has been
                                     //    initiated and is not finished. Same as when
                                     //    OSC bit in CLOCK_STA and CLOCK_CTRL register
                                     //    are not equal
        STA_XOSC_STB      = 1<<19,   // whether crystal oscillator (XOSC) is stable      ro      0
        STA_HSOSC_STB     = 1<<18,   // whether HSOSC is stable                          ro      0
        STA_OSC_PD        = 1<<17,   // 0: Both oscillators powered up and stable and    ro      0
                                     //    OSC_PD_CMD = 0.
                                     // 1: Oscillator not selected by CLOCK_CTRL. OSC
                                     //    bit is powered down.
        STA_OSC           = 1<<16,   // Current clock source selected                    ro      1
                                     // 0: 32-MHz crystal oscillator
                                     // 1: 16-MHz HF-RC oscillator
        STA_IO_DIV        = 1<<8,    // Returns current functional frequency for IO_CLK  ro      1
                                     // (may differ from setting in the CLOCK_CTRL register)
                                     // 000: 32 MHz
                                     // 001: 16 MHz
                                     // 010: 8 MHz
                                     // 011: 4 MHz
                                     // 100: 2 MHz
                                     // 101: 1 MHz
                                     // 110: 0.5 MHz
                                     // 111: 0.25 MHz
        STA_SYS_DIV       = 1<<0,    // Returns current functional frequency for         ro     1
                                     // system clock
                                     // (may differ from setting in the CLOCK_CTRL register)
                                     // 000: 32 MHz
                                     // 001: 16 MHz
                                     // 010: 8 MHz
                                     // 011: 4 MHz
                                     // 100: 2 MHz
                                     // 101: 1 MHz
                                     // 110: 0.5 MHz
                                     // 111: 0.25 MHz
    };

    enum { // IOC Registers offsets
        PA0_SEL        = 0x000, //RW 32 0x0000 0000
        PA1_SEL        = 0x004, //RW 32 0x0000 0000
        PA2_SEL        = 0x008, //RW 32 0x0000 0000
        PA3_SEL        = 0x00C, //RW 32 0x0000 0000
        PA4_SEL        = 0x010, //RW 32 0x0000 0000
        PA5_SEL        = 0x014, //RW 32 0x0000 0000
        PA6_SEL        = 0x018, //RW 32 0x0000 0000
        PA7_SEL        = 0x01C, //RW 32 0x0000 0000
        PB0_SEL        = 0x020, //RW 32 0x0000 0000
        PB1_SEL        = 0x024, //RW 32 0x0000 0000
        PB2_SEL        = 0x028, //RW 32 0x0000 0000
        PB3_SEL        = 0x02C, //RW 32 0x0000 0000
        PB4_SEL        = 0x030, //RW 32 0x0000 0000
        PB5_SEL        = 0x034,//RW 32 0x0000 0000
        PB6_SEL        = 0x038,//RW 32 0x0000 0000
        PB7_SEL        = 0x03C,//RW 32 0x0000 0000
        PC0_SEL        = 0x040,//RW 32 0x0000 0000
        PC1_SEL        = 0x044,//RW 32 0x0000 0000
        PC2_SEL        = 0x048, //RW 32 0x0000 0000
        PC3_SEL        = 0x04C, //RW 32 0x0000 0000
        PC4_SEL        = 0x050, //RW 32 0x0000 0000
        PC5_SEL        = 0x054, //RW 32 0x0000 0000
        PC6_SEL        = 0x058, //RW 32 0x0000 0000
        PC7_SEL        = 0x05C, //RW 32 0x0000 0000
        PD0_SEL        = 0x060, //RW 32 0x0000 0000
        PD1_SEL        = 0x064, //RW 32 0x0000 0000
        PD2_SEL        = 0x068, //RW 32 0x0000 0000
        PD3_SEL        = 0x06C, //RW 32 0x0000 0000
        PD4_SEL        = 0x070, //RW 32 0x0000 0000
        PD5_SEL        = 0x074, //RW 32 0x0000 0000
        PD6_SEL        = 0x078, //RW 32 0x0000 0000
        PD7_SEL        = 0x07C, //RW 32 0x0000 0000
        PA0_OVER       = 0x080, //RW 32 0x0000 0004
        PA1_OVER       = 0x084, //RW 32 0x0000 0004
        PA2_OVER       = 0x088, //RW 32 0x0000 0004
        PA3_OVER       = 0x08C, //RW 32 0x0000 0004
        PA4_OVER       = 0x090, //RW 32 0x0000 0004
        PA5_OVER       = 0x094, //RW 32 0x0000 0004
        PA6_OVER       = 0x098, //RW 32 0x0000 0004
        PA7_OVER       = 0x09C, //RW 32 0x0000 0004
        PB0_OVER       = 0x0A0, //RW 32 0x0000 0004
        PB1_OVER       = 0x0A4, //RW 32 0x0000 0004
        PB2_OVER       = 0x0A8, //RW 32 0x0000 0004
        PB3_OVER       = 0x0AC, //RW 32 0x0000 0004
        PB4_OVER       = 0x0B0, //RW 32 0x0000 0004
        PB5_OVER       = 0x0B4, //RW 32 0x0000 0004
        PB6_OVER       = 0x0B8, //RW 32 0x0000 0004
        PB7_OVER       = 0x0BC, //RW 32 0x0000 0004
        PC0_OVER       = 0x0C0, //RW 32 0x0000 0004
        PC1_OVER       = 0x0C4, //RW 32 0x0000 0004
        PC2_OVER       = 0x0C8, //RW 32 0x0000 0004
        PC3_OVER       = 0x0CC, //RW 32 0x0000 0004
        PC4_OVER       = 0x0D0, //RW 32 0x0000 0004
        PC5_OVER       = 0x0D4, //RW 32 0x0000 0004
        PC6_OVER       = 0x0D8, //RW 32 0x0000 0004
        PC7_OVER       = 0x0DC, //RW 32 0x0000 0004
        PD0_OVER       = 0x0E0, //RW 32 0x0000 0004
        PD1_OVER       = 0x0E4, //RW 32 0x0000 0004
        PD2_OVER       = 0x0E8, //RW 32 0x0000 0004
        PD3_OVER       = 0x0EC, //RW 32 0x0000 0004
        PD4_OVER       = 0x0F0, //RW 32 0x0000 0004
        PD5_OVER       = 0x0F4, //RW 32 0x0000 0004
        PD6_OVER       = 0x0F8, //RW 32 0x0000 0004
        PD7_OVER       = 0x0FC, //RW 32 0x0000 0004
        UARTRXD_UART0  = 0x100, //RW 32 0x0000 0000
        UARTCTS_UART1  = 0x104, //RW 32 0x0000 0000
        UARTRXD_UART1  = 0x108, //RW 32 0x0000 0000
        CLK_SSI_SSI0   = 0x10C, //RW 32 0x0000 0000
        SSIRXD_SSI0    = 0x110, //RW 32 0x0000 0000
        SSIFSSIN_SSI0  = 0x114, //RW 32 0x0000 0000
        CLK_SSIIN_SSI0 = 0x118, //RW 32 0x0000 0000
        CLK_SSI_SSI1   = 0x11C, //RW 32 0x0000 0000
        SSIRXD_SSI1    = 0x120, //RW 32 0x0000 0000
        SSIFSSIN_SSI1  = 0x124, //RW 32 0x0000 0000
        CLK_SSIIN_SSI1 = 0x128, //RW 32 0x0000 0000
        I2CMSSDA       = 0x12C, //RW 32 0x0000 0000
        I2CMSSCL       = 0x130, //RW 32 0x0000 0000
        GPT0OCP1       = 0x134, //RW 32 0x0000 0000
        GPT0OCP2       = 0x138, //RW 32 0x0000 0000
        GPT1OCP1       = 0x13C, //RW 32 0x0000 0000
        GPT1OCP2       = 0x140, //RW 32 0x0000 0000
        GPT2OCP1       = 0x144, //RW 32 0x0000 0000
        GPT2OCP2       = 0x148, //RW 32 0x0000 0000
        GPT3OCP1       = 0x14C, //RW 32 0x0000 0000
        GPT3OCP2       = 0x150  //RW 32 0x0000 0000
    };
    enum { // Peripheral Signal Select Values (Same for All IOC_Pxx_SEL Registers)
        UART0_TXD       =  0x00,
        UART1_RTS       =  0x01,
        UART1_TXD       =  0x02,
        SSI0_TXD        =  0x03,
        SSI0_CLK_OUT    =  0x04,
        SSI0_FSS_OUT    =  0x05,
        SSI0_TX_SER_OUT =  0x06,
        SSI1_TXD        =  0x07,
        SSI1_CLK_OUT    =  0x08,
        SSI1_FSS_OUT    =  0x09,
        SSI1_TX_SER_OUT =  0x0A,
        I2C_SDA         =  0x0B,
        I2C_SCL         =  0x0C,
        GPT0CP1         =  0x0D,
        GPT0CP2         =  0x0E,
        GPT1CP1         =  0x0F,
        GPT1CP2         =  0x10,
        GPT2CP1         =  0x11,
        GPT2CP2         =  0x12,
        GPT3CP1         =  0x13,
        GPT3CP2         =  0x14
    };
    enum { // Valid values for IOC_Pxx_OVER
        OE  = 0x8,// output enable
        PUE = 0x4,// pullup enable
        PDE = 0x2,// pulldown enable
        ANA = 0x1 // analog enable
    };


    // Base address for memory-mapped System Control Space
    enum {
        SCS_BASE        = 0xe000e000
    };

    // System Control Space offsets
    enum {                              // Description                                          Type    Value after reset
        MCR             = 0x000,        // Master Control Register                              -       0x00000000
        ICTR            = 0x004,        // Interrupt Controller Type Register                   RO      0x????????
        ACTLR           = 0x008,        // Auxiliary Control Register                           R/W     0x????????
        STCTRL          = 0x010,        // SysTick Control and Status Register                  R/W     0x00000000
        STRELOAD        = 0x014,        // SysTick Reload Value Register                        R/W     0x00000000
        STCURRENT       = 0x018,        // SysTick Current Value Register                       R/WC    0x00000000
        IRQ_ENABLE      = 0x100,        // Interrupt 0-32 Set Enable                            R/W     0x00000000
        IRQ_ENABLE1     = 0x104,        // Interrupt 0-64 Set Enable                            R/W     0x00000000
        IRQ_ENABLE2     = 0x108,        // Interrupt 0-96 Set Enable                            R/W     0x00000000
        IRQ_DISABLE     = 0x180,        // Interrupt 0-32 Clear Enable                          R/W     0x00000000
        IRQ_DISABLE1    = 0x184,        // Interrupt 0-32 Clear Enable                          R/W     0x00000000
        IRQ_DISABLE2    = 0x188,        // Interrupt 0-32 Clear Enable                          R/W     0x00000000
        IRQ_PEND        = 0x200,        // Interrupt 0-32 Set Pending                           R/W     0x00000000
        IRQ_PEND1       = 0x204,        // Interrupt 0-32 Set Pending                           R/W     0x00000000
        IRQ_PEND2       = 0x208,        // Interrupt 0-32 Set Pending                           R/W     0x00000000
        IRQ_UNPEND      = 0x280,        // Interrupt 0-32 Clear Pending                         R/W     0x00000000
        IRQ_UNPEND1     = 0x284,        // Interrupt 0-32 Clear Pending                         R/W     0x00000000
        IRQ_UNPEND2     = 0x288,        // Interrupt 0-32 Clear Pending                         R/W     0x00000000
        IRQ_ACTIVE      = 0x300,        // Interrupt 0-32 Active Bit                            R/W     0x00000000
        IRQ_ACTIVE1     = 0x304,        // Interrupt 0-32 Active Bit                            R/W     0x00000000
        IRQ_ACTIVE2     = 0x308,        // Interrupt 0-32 Active Bit                            R/W     0x00000000
        CPUID           = 0xd00,        // CPUID Base Register                                  RO      0x410fc231
        INTCTRL         = 0xd04,        // Interrupt Control and State Register                 R/W     0x00000000
        VTOR            = 0xd08,        // Vector Table Offset Register                         R/W     0x00000000
        AIRCR           = 0xd0c,        // Application Interrupt/Reset Control Register         R/W
        SCR             = 0xd10,        // System Control Register                              R/W     0x00000000
        CCR             = 0xd14,        // Configuration Control Register                       R/W     0x00000000
        SHPR1           = 0xd18,        // System Handlers 4-7 Priority                         R/W     0x00000000
        SHPR2           = 0xd1c,        // System Handlers 8-11 Priority                        R/W     0x00000000
        SHPR3           = 0xd20,        // System Handlers 12-15 Priority                       R/W     0x00000000
        SHCSR           = 0xd24,        // System Handler Control and State Register            R/W     0x00000000
        CFSR            = 0xd28,        // Configurable Fault Status Register                   R/W     0x00000000
        HFSR            = 0xd2c,        // Hard Fault Status Register
        SWTRIG          = 0xf00         // Software Trigger Interrupt Register                  WO      0x00000000
    };

    // Useful Bits in the ISysTick Control and Status Register
    enum STCTRL {                       // Description                                          Type    Value after reset
        ENABLE          = 1 <<  0,      // Enable / disable                                     rw      0
        INTEN           = 1 <<  1,      // Interrupt pending                                    rw      0
        CLKSRC          = 1 <<  2,      // Clock source (0 -> external, 1 -> core)              rw      0
        COUNT           = 1 << 16       // Count underflow                                      ro      0
    };

    // Useful Bits in the Interrupt Control and State Register
    enum INTCTRL {                      // Description                                          Type    Value after reset
        ICSR_ACTIVE     = 1 <<  0,      // Active exceptions (IPSR mirror; 0 -> thread mode)    ro
        ICSR_PENDING    = 1 << 12,      // Pending exceptions (0 -> none)                       ro
        ICSR_ISRPENDING = 1 << 22,      // Pending NVIC IRQ                                     ro
        ICSR_SYSPENDING = 1 << 25       // Clear pending SysTick                                wo
    };

    // Useful Bits in the Application Interrupt/Reset Control Register
    enum AIRCR {                        // Description                                          Type    Value after reset
        VECTRESET       = 1 << 0,       // Reserved for debug                                   wo      0
        VECTCLRACT      = 1 << 1,       // Reserved for debug                                   wo      0
        SYSRESREQ       = 1 << 2        // System Reset Request                                 wo      0
    };

    // Useful Bits in the Configuration Control Register
    enum CCR {                          // Description                                          Type    Value after reset
        BASETHR         = 1 <<  0,      // Thread state can be entered at any level of int.     rw      0
        MAINPEND        = 1 <<  1,      // SWTRIG can be written to in user mode                rw      0
        UNALIGNED       = 1 <<  3,      // Trap on unaligned memory access                      rw      0
        DIV0            = 1 <<  4,      // Trap on division by zero                             rw      0
        BFHFNMIGN       = 1 <<  8,      // Ignore Precise Data Access Faults for pri -1 and -2  rw      0
        STKALIGN        = 1 <<  9       // Align stack point on exception entry to 8 butes      rw      0
    };

    // Base address for memory-mapped GPIO Ports Registers
    enum {
        GPIOA_BASE      = 0x400D9000,   // GPIO Port A
        GPIOB_BASE      = 0x400DA000,   // GPIO Port B
        GPIOC_BASE      = 0x400DB000,   // GPIO Port C
        GPIOD_BASE      = 0x400DC000,   // GPIO Port D
    };

    // GPIO Ports Registers offsets
    enum {                              // Description                  Type    Value after reset
        DATA		= 0x000,	// Data  	                R/W	0x0000.0000
        DIR		= 0x400,	// Direction    	        R/W	0x0000.0000
        IS		= 0x404,	// Interrupt Sense      	R/W	0x0000.0000
        IBE		= 0x408,	// Interrupt Both Edges 	R/W	0x0000.0000
        IEV		= 0x40c,	// Interrupt Event 	        R/W	0x0000.0000
        IM		= 0x410,	// Interrupt Mask 	        R/W	0x0000.0000
        GRIS		= 0x414,	// Raw Interrupt Status 	RO	0x0000.0000
        //MIS		= 0x418,	// Masked Interrupt Status	RO	0x0000.0000
        ICR		= 0x41c,	// Interrupt Clear 	        W1C	0x0000.0000
        AFSEL		= 0x420,	// Alternate Function Select	R/W	-
        DR2R		= 0x500,	// 2-mA Drive Select	        R/W	0x0000.00ff
        DR4R		= 0x504,	// 4-mA Drive Select	        R/W	0x0000.0000
        DR8R		= 0x508,	// 8-mA Drive Select	        R/W	0x0000.0000
        ODR		= 0x50c,	// Open Drain Select	        R/W	0x0000.0000
        PUR		= 0x510,	// Pull-Up Select 	        R/W	0x0000.00ff
        PDR		= 0x514,	// Pull-Down Select 	        R/W	0x0000.0000
        SLR		= 0x518,	// Slew Rate Control Select	R/W	0x0000.0000
        DEN		= 0x51c,	// Digital Enable 	        R/W	0x0000.00ff
    /*
        PeriphID4	= 0xfd0,	// Peripheral Identification 4	RO	0x0000.0000
        PeriphID5	= 0xfd4,	// Peripheral Identification 5 	RO	0x0000.0000
        PeriphID6	= 0xfd8,	// Peripheral Identification 6	RO	0x0000.0000
        PeriphID7	= 0xfdc,	// Peripheral Identification 7	RO	0x0000.0000
        PeriphID0	= 0xfe0,	// Peripheral Identification 0	RO	0x0000.0061
        PeriphID1	= 0xfe4,	// Peripheral Identification 1	RO	0x0000.0000
        PeriphID2	= 0xfe8,	// Peripheral Identification 2	RO	0x0000.0018
        PeriphID3	= 0xfec,	// Peripheral Identification 3	RO	0x0000.0001
        PCellID0	= 0xff0,	// PrimeCell Identification 0	RO	0x0000.000d
        PCellID1	= 0xff4,	// PrimeCell Identification 1	RO	0x0000.00f0
        PCellID2	= 0xff8,	// PrimeCell Identification 2	RO	0x0000.0005
        PCellID3	= 0xffc		// PrimeCell Identification 3	RO	0x0000.00b1
        */
    };

    // Common pin codes for GPIO registers
    enum GPIO_PINS {
        PIN0     = 1 <<  0,
        PIN1     = 1 <<  1,
        PIN2     = 1 <<  2,
        PIN3     = 1 <<  3,
        PIN4     = 1 <<  4,
        PIN5     = 1 <<  5,
        PIN6     = 1 <<  6,
        PIN7     = 1 <<  7
    };

    // Useful Bits in the Digital Enable Register
    enum DEN {                        // Description                    Type    Value after reset
        DEN_DIGP0     = 1 <<  0,      // Pin 0 (1 -> Digital Enable)    r/w     1
        DEN_DIGP1     = 1 <<  1,      // Pin 1 (1 -> Digital Enable)    r/w     1
        DEN_DIGP2     = 1 <<  2,      // Pin 2 (1 -> Digital Enable)    r/w     1
        DEN_DIGP3     = 1 <<  3,      // Pin 3 (1 -> Digital Enable)    r/w     1
        DEN_DIGP4     = 1 <<  4,      // Pin 4 (1 -> Digital Enable)    r/w     1
        DEN_DIGP5     = 1 <<  5,      // Pin 5 (1 -> Digital Enable)    r/w     1
        DEN_DIGP6     = 1 <<  6,      // Pin 6 (1 -> Digital Enable)    r/w     1
        DEN_DIGP7     = 1 <<  7       // Pin 7 (1 -> Digital Enable)    r/w     1
    };

    // Enable clock to the RF CORE module
    static void radio_enable()
    {
        scr(RCGCRFC) |= RCGCRFC_RFC0;
    }
    // Disable clock to the RF CORE module
    static void radio_disable()
    {
        scr(RCGCRFC) &= ~RCGCRFC_RFC0;
    }
public:
    static void init();
protected:
    static void init_clock();
    static bool _init_clock_done;
protected:
    eMote3() {}

    void config_UART(volatile Log_Addr * base)
    {
        init_clock(); // Setup the clock first!
        if(base == reinterpret_cast<Log_Addr *>(UART0_BASE)) {
            //1. Enable the UART module using the SYS_CTRL_RCGCUART register.
            scr(RCGCUART) |= UART0; // Enable clock for UART0 while in Running mode

            //2. Set the GPIO pin configuration through the Pxx_SEL registers for the desired output
            ioc(PA1_SEL) = UART0_TXD;

            //3. To enable IO pads to drive outputs, the corresponding IPxx_OVER bits in IOC_Pxx_OVER register
            //   has to be configured to 0x8 (OE - Output Enable).
            ioc(PA1_OVER) = OE;
            ioc(PA0_OVER) = 0;

            //4. Connect the appropriate input signals to the UART module
            // The value is calculated as: (port << 3) + pin
            ioc(UARTRXD_UART0) = (0 << 3) + 0;

            //5. Set GPIO pins A1 and A0 to peripheral mode
            gpioa(AFSEL) |= (PIN0) + (PIN1);
        }
        // TODO: UART1 configuration
        /*
        else
        {
        }
        */
    }

    // Set D+ USB pull-up resistor, which is controlled by GPIO pin C2 in eMote3
    static void config_USB()
    {
        init_clock();
        const unsigned int pin_bit = 1 << 2;
        gpioc(AFSEL) &= ~pin_bit; // Set pin C2 as software-controlled
        gpioc(DIR) |= pin_bit; // Set pin C2 as output
        gpioc(pin_bit << 2) = 0xff; // Set pin C2 (high)
    }
    
    static void disable_USB()
    {
        const unsigned int pin_bit = 1 << 2;
        gpioc(pin_bit << 2) = 0; // Clear pin C2 (low)
    }

    static void config_GPTM(unsigned int which)
    {
        assert(which < 4);
        scr(RCGCGPT) |= 1 << which;
        switch(which)
        {
            case 0: ioc(PC5_SEL) = GPT0CP1; break; 
            case 1: ioc(PC5_SEL) = GPT1CP1; break;
            case 2: ioc(PC5_SEL) = GPT2CP1; break;
            case 3: ioc(PC5_SEL) = GPT3CP1; break;
        }
        ioc(PC5_OVER) = OE;
        gpioc(AFSEL) |= PIN5;
    }

public:
    static volatile Reg32 & ioc(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(IOC_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & scr(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SCR_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & scs(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SCS_BASE)[o / sizeof(Reg32)]; }

    static volatile Reg32 & gpioa(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOA_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & gpiob(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOB_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & gpioc(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOC_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & gpiod(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOD_BASE)[o / sizeof(Reg32)]; }
};

typedef eMote3 Cortex_M_Model;

__END_SYS

#include "emote3_tsc.h"
#include "emote3_gpio.h"

#include "emote3_usb.h"
__BEGIN_SYS
typedef eMote3_USB Cortex_M_Model_USB;
class Cortex_M_USB : private USB_Common, public Cortex_M_Model_USB{};
__END_SYS

#include "pl011.h"
__BEGIN_SYS
typedef PL011 Cortex_M_Model_UART;
__END_SYS
#include "emote3_rom.h"
#include "emote3_adc.h"
#include "emote3_flash.h"
#include "emote3_pwm.h"

#endif

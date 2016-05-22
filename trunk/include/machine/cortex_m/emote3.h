// EPOS EPOSMoteIII (Cortex-M3) MCU Mediator Declarations

#ifndef __emote3_h
#define __emote3_h

#include <cpu.h>
#include <tsc.h>

__BEGIN_SYS

class eMote3
{
protected:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:
    static const unsigned int IRQS = 64;
    static const unsigned int GPIO_PORTS = 4;
//    static const bool supports_gpio_power_up = true;


    // Base address for memory-mapped System Control Registers
    enum {
        SSI0_BASE       = 0x40008000,
        SSI1_BASE       = 0x40009000,
        UART0_BASE      = 0x4000c000,
        UART1_BASE      = 0x4000d000,
        I2C_MASTER_BASE = 0x40020000,
        I2C_SLAVE_BASE  = 0x40028000,
        SCR_BASE        = 0x400d2000,
        IOC_BASE        = 0x400d4000,
    };

    // I2C Master offsets
    enum {
        I2CM_SA         = 0x00,
        I2CM_CTRL       = 0x04,
        I2CM_STAT       = I2CM_CTRL,
        I2CM_DR         = 0X08,
        I2CM_TPR        = 0x0C,
        I2CM_IMR        = 0x10,
        I2CM_RIS        = 0x14,
        I2CM_MIS        = 0x18,
        I2CM_ICR        = 0x1C,
        I2CM_CR         = 0x20,
    };

    // I2C slave offsets
    enum {                              // Description
        I2CS_OAR        = 0x00,         // Own Address
        I2CS_STAT       = 0x04,         // Control and Status
        I2CS_CTRL       = I2CS_STAT,    // Control and Status
        I2CS_DR         = 0x08,         // Data
        I2CS_IMR        = 0x0C,         // Interrupt Mask
        I2CS_RIS        = 0x10,         // Raw Interrupt Status
        I2CS_MIS        = 0x14,         // Masked Interrupt Status
        I2CS_ICR        = 0x18,         // Interrupt Clear
    };

    // Useful bits in the I2CM_SA register
    enum {
        RS              = 0x01,
    };

    // Useful bits in the I2CM_CTRL register
    enum {
        ACK             = 1 << 3,
        STOP            = 1 << 2,
        START           = 1 << 1,
        RUN             = 1 << 0,
    };

    // Useful bits in the I2CM_STAT register
    enum {                              // Description (type)
        BUSBSY          = 1 << 6,       // Bus Busy (RO)
        IDLE            = 1 << 5,       // I2C Idle (RO)
        ARBLST          = 1 << 4,       // Arbitration Lost (RO)
        DATACK          = 1 << 3,       // Acknowledge Data (RO)
        ADRACK          = 1 << 2,       // Acknowledge Address (RO)
        ERROR           = 1 << 1,       // Error (RO)
        BUSY            = 1 << 0,       // I2C Busy (RO)
    };

    // Useful bits in the I2CM_IMR register
    enum {                              // Description (type)
        I2CM_IMR_IM     = 0x01,         // Interrupt Mask (RW)
    };

    // Useful bits in the I2CM_RIS register
    enum {                              // Description (type)
        I2CM_RIS_BIT    = 0x01,         // Raw Interrupt Status (RO)
    };

    // Useful bits in the I2CM_MIS register
    enum {                              // Description (type)
        I2CM_MIS_MIS    = 0x01,         // Masked Interrupt Status (RO)
    };

    // Useful bits in the I2CM_ICR register
    enum {                              // Description (type)
        I2CM_ICR_IC     = 0x01,         // Interrupt Clear (WO)
    };

    // Useful bits in the I2CM_CR register
    enum {                              // Description (type)
        SFE             = 1 << 5,       // I2C Slave Function Enable (RW)
        MFE             = 1 << 4,       // I2C Master Function Enable (RW)
        LPBK            = 1 << 0,       // I2C Loopback (RW)
    };

    // Useful bits in the I2CS_STAT register
    enum {                              // Description (type)
        FBR             = 1 << 2,       // First Byte Received (RO)
        TREQ            = 1 << 1,       // Transmit Request (RO)
        RREQ            = 1 << 0,       // Receive Request (RO)
    };

    // Useful bits in the I2CS_CTRL register
    enum {                              // Description (type)
        DA              = 0x01,         // Device Active (WO)
    };

    // Useful bits in the I2CS_IMR register
    enum {                              // Description (type)
        STOPIM          = 1 << 2,       // Stop Condition Interrupt Mask (RO)
        STARTIM         = 1 << 1,       // Start Condition Interrupt Mask (RO)
        DATAIM          = 1 << 0,       // Data Interrupt Mask (RW)
    };

    // Useful bits in the I2CS_RIS register
    enum {                              // Description (type)
        STOPRIS         = 1 << 2,       // Stop Condition Raw Interrupt Status (RO)
        STARTRIS        = 1 << 1,       // Start Condition Raw Interrupt Status (RO)
        DATARIS         = 1 << 0,       // Data Interrupt Status (RO)
    };

    // Useful bits in the I2CS_MIS register
    enum {                              // Description (type)
        STOPMIS         = 1 << 2,       // Stop Condition Masked Interrupt Status (RO)
        STARTMIS        = 1 << 1,       // Start Condition Masked Interrupt Status (RO)
        DATAMIS         = 1 << 0,       // Data Masked Interrupt Status (RO)
    };

    // Useful bits in the I2CS_ICR register
    enum {                              // Description (type)
        STOPIC          = 1 << 2,       // Stop Condition Interrupt Clear (WO)
        STARTIC         = 1 << 1,       // Start Condition Interrupt Clear (WO)
        DATAIC          = 1 << 0,       // Data Interrupt Clear (WO)
    };

    // Synchronous Serial Interface (SSI) offsets
    enum {
        SSI_CR0         = 0x000,
        SSI_CR1         = 0x004,
        SSI_DR          = 0x008,
        SSI_SR          = 0x00C,
        SSI_CPSR        = 0x010,
        SSI_IM          = 0x014,
        SSI_RIS         = 0x018,
        SSI_MIS         = 0x01C,
        SSI_ICR         = 0x020,
        SSI_DMACTL      = 0x024,
        SSI_CC          = 0xFC8,

    };

    // Useful bits in the SSI CR0 register
    enum {                          // Description                   Type    Value after reset
        CR0_SCR         = 1 << 8,   // serial clock rate              RW      0x00
        SPH             = 1 << 7,   // serial clock phase high        RW      0x0
                                    // applicable only to the Motorola SPI format
        SPO             = 1 << 6,   // serial clock phase low         RW      0x0
                                    // applicable only to the Motorola SPI format
        FRF             = 1 << 4,   // frame format selection         RW      0x0
                                    // 00: Motorola SPI format
                                    // 01: TI synchronous serial frame format
                                    // 10: National microwave frame format
                                    // 11: Reserved
        DSS             = 1 << 0,   // data size select              RW      0x0
                                    // 0000-0010: Reserved
                                    // 0011: 4-bit data
                                    // 0100: 5-bit data
                                    // 0101: 6-bit data
                                    // 0110: 7-bit data
                                    // 0111: 8-bit data
                                    // 1000: 9-bit data
                                    // 1001: 10-bit data
                                    // 1010: 11-bit data
                                    // 1011: 12-bit data
                                    // 1100: 13-bit data
                                    // 1101: 14-bit data
                                    // 1110: 15-bit data
                                    // 1111: 16-bit data
    };

    //Frame format
    enum SSI_Frame_Format {
        FORMAT_MOTO_0   = 0x00, // Moto fmt, polarity 0, phase 0
        FORMAT_MOTO_1   = 0x02, // Moto fmt, polarity 0, phase 1
        FORMAT_MOTO_2   = 0x01, // Moto fmt, polarity 1, phase 0
        FORMAT_MOTO_3   = 0x03, // Moto fmt, polarity 1, phase 1
        FORMAT_TI       = 0x10, // TI synchronous serial frame format
        FORMAT_NMW      = 0x20, // National microwave frame format
    };

    //SSI Operation Mode
    enum SSI_Mode {
        MASTER          = 0x00,
        SLAVE           = 0x01,
        SLAVE_OD        = 0x02,
    };


    // Useful bits in the SSI CR1 register
    enum {                          // Description                          Type    Value after reset
        SOD             = 1 << 3,   // slave mode output disable              RW      0x00
                                    // 0: SSI can drive SSITXD in slave output mode
                                    // 1: SSI must not drive the SSITXD output in slave mode
        MS              = 1 << 2,   // master and slave select                RW      0x0
                                    // 0: Device configured as a master (default)
                                    // 1: Device configured as a slave
        SSE             = 1 << 1,   // synchronous serial port enable         RW      0x0
                                    // 0: SSI operation is disabled.
                                    // 1: SSI operation is enabled.
        LBM             = 1 << 0,   // loop-back mode                        RW      0x0
                                    // 0: Normal serial port operation is enabled.
                                    // 1: The output of the transmit serial shifter is connected to the input
                                    // of the receive serial shift register internally.
    };

    // Useful bits in the SSI DR register
    enum {                          // Description                          Type    Value after reset
        SSI_DATA       = 1 << 15,   // receive/transmit data register        RW         0x0000
                                    // A read operation reads the receive FIFO. A write operation writes the transmit FIFO.
    };

    // Useful bits in the SSI SR register
    enum {                          // Description                          Type    Value after reset
        BSY             = 1 << 4,   // busy bit                               RO      0x0
        RFF             = 1 << 3,   // receive FIFO full                      RO      0x0
                                    // 0: Receive FIFO is not full.
                                    // 1: Receive FIFO is full.
        RNE             = 1 << 2,   // receive FIFO not empty                 RO      0x0
                                    // 0: Receive FIFO is empty.
                                    // 1: Receive FIFO is not empty.
        TNF             = 1 << 1,   // transmit FIFO not full                 RO      0x0
                                    // 0: Transmit FIFO is full.
                                    // 1: Transmit FIFO is not full.
        TFE             = 1 << 0,   // transmit FIFO empty
                                    // 0: Transmit FIFO is not empty.
                                    // 1: Transmit FIFO is empty
    };

    // Useful bits in the SSI CPSR register
    enum {                          // Description                          Type    Value after reset
        CPSDVSR         = 1 << 7,   // clock prescale divisor                  RW        0x00
                                    // This value must be an even number from 2 to 254, depending on
                                    // the frequency of SSICLK. The LSB always returns zero on reads.
    };

    // Useful bits in the SSI IM register
    enum {                          // Description                          Type    Value after reset
        TXIM            = 1 << 3,   // transmit FIFO interrupt mask                  RW        0x00
                                    // 0: TX FIFO half empty or condition interrupt is masked.
                                    // 1: TX FIFO half empty or less condition interrupt is not masked.
        RXIM            = 1 << 2,   // receive FIFO interrupt mask                   RW        0x00
                                    // 0: RX FIFO half empty or condition interrupt is masked
                                    // 1: RX FIFO half empty or less condition interrupt is not masked
        RTIM            = 1 << 1,   // receive time-out interrupt mask               RW        0x00
                                    // 0: RX FIFO time-out interrupt is masked
                                    // 1: RX FIFO time-out interrupt is not masked
        RORIM           = 1 << 0,   // receive overrun interrupt mask                RW        0x00
                                    // RX FIFO Overrun interrupt is masked
                                    // RX FIFO Overrun interrupt is not masked
    };

    // Useful bits in the SSI RIS register
    enum {                          // Description                              Type    Value after reset
        TXRIS            = 1 << 3,  // SSITXINTR raw state                              RO        0x01
                                    // Gives the raw interrupt state (before masking) of SSITXINTR
        RXRIS            = 1 << 2,  // SSIRXINTR raw state                              RO        0x00
                                    // Gives the raw interrupt state (before masking) of SSIRXINTR
        RTRIS            = 1 << 1,  // SSIRTINTR raw state                             RO        0x00
                                    // Gives the raw interrupt state (before masking) of SSIRTINTR
        RORIS            = 1 << 0,  // SSIRORINTR raw state                            RO        0x00
                                    // Gives the raw interrupt state (before masking) of SSIRORINTR
    };

    // Useful bits in the SSI MIS register
    enum {                          // Description                              Type    Value after reset
        TXMIS            = 1 << 3,  // SSITXINTR masked state                              RO        0x01
                                    // Gives the raw interrupt state (after masking) of SSITXINTR
        RXMIS            = 1 << 2,  // SSIRXINTR masked state                              RO        0x00
                                    // Gives the raw interrupt state (after masking) of SSIRXINTR
        RTMIS            = 1 << 1,  // SSIRTINTR masked state                             RO        0x00
                                    // Gives the raw interrupt state (after masking) of SSIRTINTR
        ROMIS            = 1 << 0,  // SSIRORINTR masked state                            RO        0x00
                                    // Gives the raw interrupt state (after masking) of SSIRORINTR
    };

    // Useful bits in the SSI ICR register
    enum {                          // Description                              Type    Value after reset
        TXDMAE            = 1 << 1, // Transmit DMA enable                             RW        0x00
                                    // 0: uDMA for the transmit FIFO is disabled.
                                    // 1: uDMA for the transmit FIFO is enabled.
        RXDMAE            = 1 << 0, // Receive DMA enable                              RW        0x00
                                    // 0: uDMA for the receive FIFO is disabled.
                                    // 1: uDMA for the receive FIFO is enabled.
    };

    // Useful bits in the SSI CC register
    enum {                          // Description                              Type    Value after reset
        CS                = 1 << 0, // baud and system clock source               RW        0x00
                                    /* bit0 (PIOSC):
                                    1: The SSI baud clock is determined by the IO DIV setting in the
                                    system controller.
                                    0: The SSI baud clock is determined by the SYS DIV setting in the
                                    system controller.
                                    bit1: Unused
                                    bit2: (DSEN) Only meaningful when the system is in deep sleep
                                    mode. This bit is a don't care when not in sleep mode.
                                    1: The SSI system clock is running on the same clock as the baud
                                    clock, as per PIOSC setting above.
                                    0: The SSI system clock is determined by the SYS DIV setting in
                                    the system controller.
                                    */
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
        ANA = 0x1,// analog enable
        DIS = 0x0 // override disable
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
        IRQ_ENABLE0     = 0x100,        // Interrupt  0-31 Set Enable                           R/W     0x00000000
        IRQ_ENABLE1     = 0x104,        // Interrupt 32-63 Set Enable                           R/W     0x00000000
        IRQ_ENABLE2     = 0x108,        // Interrupt 64-95 Set Enable                           R/W     0x00000000
        IRQ_DISABLE0    = 0x180,        // Interrupt  0-31 Clear Enable                         R/W     0x00000000
        IRQ_DISABLE1    = 0x184,        // Interrupt 32-63 Clear Enable                         R/W     0x00000000
        IRQ_DISABLE2    = 0x188,        // Interrupt 64-95 Clear Enable                         R/W     0x00000000
        IRQ_PEND0       = 0x200,        // Interrupt  0-31 Set Pending                          R/W     0x00000000
        IRQ_PEND1       = 0x204,        // Interrupt 32-63 Set Pending                          R/W     0x00000000
        IRQ_PEND2       = 0x208,        // Interrupt 64-95 Set Pending                          R/W     0x00000000
        IRQ_UNPEND0     = 0x280,        // Interrupt  0-31 Clear Pending                        R/W     0x00000000
        IRQ_UNPEND1     = 0x284,        // Interrupt 32-63 Clear Pending                        R/W     0x00000000
        IRQ_UNPEND2     = 0x288,        // Interrupt 64-95 Clear Pending                        R/W     0x00000000
        IRQ_ACTIVE0     = 0x300,        // Interrupt  0-31 Active Bit                           R/W     0x00000000
        IRQ_ACTIVE1     = 0x304,        // Interrupt 32-63 Active Bit                           R/W     0x00000000
        IRQ_ACTIVE2     = 0x308,        // Interrupt 64-95 Active Bit                           R/W     0x00000000
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
        SYSRESREQ       = 1 << 2,       // System Reset Request                                 wo      0
        VECTKEY         = 1 << 16,      // Register Key                                         rw      0xfa05
                                        // This field is used to guard against accidental 
                                        // writes to this register.  0x05FA must be written 
                                        // to this field in order to change the bits in this
                                        // register. On a read, 0xFA05 is returned.
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
        MIS		= 0x418,	// Masked Interrupt Status	RO	0x0000.0000
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
        P_EDGE_CTRL = 0x704, // Power-up Interrupt Edge Control R/W 0x0000.0000
        PI_IEN  = 0x710,	// Power-up Interrupt Enable       R/W	0x0000.0000
        IRQ_DETECT_ACK = 0x718, 
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

    enum POWER_MODE {
        ACTIVE = 0,
        SLEEP,
        POWER_MODE_0,
        POWER_MODE_1,
        POWER_MODE_2,
        POWER_MODE_3,
    };
    enum
    {
        SLEEPDEEP = 1 << 2,
    };
    enum WAKE_UP_EVENT {
        SLEEP_MODE_TIMER = 1 << 5,
        USB = 1 << 4,
        GPIO_D = 1 << 3,
        GPIO_C = 1 << 2,
        GPIO_B = 1 << 1,
        GPIO_A = 1 << 0,
    };

    // Change in power mode will only be effective when ASM("wfi") is called
    static void power_mode(POWER_MODE p)
    {
        if(p <= SLEEP) {
            scs(SCR) &= ~SLEEPDEEP;
        }
        else {
            scs(SCR) |= SLEEPDEEP;
            scr(PMCTL) = p - POWER_MODE_0;
        }
        //if(p != POWER_MODE::ACTIVE)
        //    ASM("wfi");
    }

    static void wake_up_on(WAKE_UP_EVENT e) {
        scs(IWE) = e;
    }

protected:
    static void init();
    static void init_clock();
    static bool _init_clock_done;
protected:
    eMote3() {}

    static void reboot() {
        Reg32 val = scs(AIRCR) & (~((-1u / VECTKEY) * VECTKEY));
        val |= SYSRESREQ;
        val |= 0x05fa * VECTKEY;
        scs(AIRCR) = val;
    }

    void uart_config(volatile Log_Addr * base)
    {
        init_clock(); // Setup the clock first!
        if(base == reinterpret_cast<Log_Addr *>(UART0_BASE)) {
            //1. Enable the UART module using the SYS_CTRL_RCGCUART register.
            scr(RCGCUART) |= UART0; // Enable clock for UART0 while in Running mode
            scr(SCGCUART) |= UART0; // Enable clock for UART0 while in Sleep mode

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
        else
        {
            scr(RCGCUART) |= UART1;
            scr(SCGCUART) |= UART1;

            /*ioc(PB3_SEL) = UART1_TXD;
            ioc(PB3_OVER) = OE;
            ioc(PB4_OVER) = 0;
            ioc(UARTRXD_UART1) = (1 << 3) + 4; //B4
            gpiob(AFSEL) |= (PIN3) + (PIN4);*/

           ioc(PD1_SEL) = UART1_TXD;
           ioc(PD1_OVER) = OE;
           ioc(PD0_OVER) = 0;
           ioc(UARTRXD_UART1) = (3 << 3) + 0;
           gpiod(AFSEL) |= (PIN0) + (PIN1);
        }
    }
    static void uart_enable() {};
    static void uart_disable() {};

    // Set D+ USB pull-up resistor, which is controlled by GPIO pin C2 in eMote3
    static void usb_config()
    {
        init_clock();
        const unsigned int pin_bit = 1 << 2;
        gpioc(AFSEL) &= ~pin_bit; // Set pin C2 as software-controlled
        gpioc(DIR) |= pin_bit; // Set pin C2 as output
        gpioc(pin_bit << 2) = 0xff; // Set pin C2 (high)
    }
    static void usb_enable() {};
    static void usb_disable()
    {
        const unsigned int pin_bit = 1 << 2;
        gpioc(pin_bit << 2) = 0; // Clear pin C2 (low)
    }

    void gpio_pull_up(int port, int pin) {
        auto over = PA0_OVER + 0x20*port + 0x4*pin;
        ioc(over) = PUE;
    }
    void gpio_pull_down(int port, int pin) {
        auto over = PA0_OVER + 0x20*port + 0x4*pin;
        ioc(over) = PDE;
    }

    // Enable clock to the RF CORE module
    static void radio_enable()
    {
        scr(RCGCRFC) |= RCGCRFC_RFC0;
        scr(SCGCRFC) |= RCGCRFC_RFC0;
    }
    // Disable clock to the RF CORE module
    static void radio_disable()
    {
        scr(RCGCRFC) &= ~RCGCRFC_RFC0;
        scr(SCGCRFC) &= ~RCGCRFC_RFC0;
    }

    static void config_GPTM(unsigned int which_timer)
    {
        assert(which_timer < 4);
        scr(RCGCGPT) |= 1 << which_timer;
        scr(SCGCGPT) |= 1 << which_timer;
    }

    static void config_PWM(unsigned int which_timer, char gpio_port, unsigned int gpio_pin)
    {
        config_GPTM(which_timer);

        if((gpio_port >= 'A') && (gpio_port <= 'D'))
            gpio_port += ('a'-'A');
        assert((gpio_port >= 'a') && (gpio_port <= 'd'));
        assert(gpio_pin <= 7);

        // Calculate the offset for the GPIO's IOC_Pxx_SEL
        auto n = gpio_port - 'a';
        auto sel = PA0_SEL + 0x20*n + 0x4*gpio_pin;

        switch(which_timer)
        {
            case 0: ioc(sel) = GPT0CP1; break;
            case 1: ioc(sel) = GPT1CP1; break;
            case 2: ioc(sel) = GPT2CP1; break;
            case 3: ioc(sel) = GPT3CP1; break;
        }

        auto over = sel + 0x80;
        ioc(over) = OE;

        auto pin_bit = 1 << gpio_pin;
        switch(gpio_port)
        {
            case 'a': gpioa(AFSEL) |= pin_bit; break;
            case 'b': gpiob(AFSEL) |= pin_bit; break;
            case 'c': gpioc(AFSEL) |= pin_bit; break;
            case 'd': gpiod(AFSEL) |= pin_bit; break;
        }
    }

    static void config_SSI(volatile Log_Addr * base, Reg32 clock, SSI_Frame_Format protocol, SSI_Mode mode, Reg32 bit_rate, Reg32 data_width) {
        Reg32 pre_div, max_bit_rate, value;

        //Configure GPIO Pins for SSI0 or SSI1
        if(base == reinterpret_cast<Log_Addr *>(SSI0_BASE)) {
            //Enable the SSI module using the SYS_CTRL_RCGCSSI register.
            scr(RCGCSSI) |= 0x1; // Enable clock for SSI0 while in Running mode
            scr(SCGCSSI) |= 0x1; // Enable clock for SSI0 while in Sleep mode

            //Set the GPIO pin configuration through the Pxx_SEL registers for the desired output
            if(mode == MASTER)
                ioc(PA2_SEL) = SSI0_CLK_OUT;
            else
                ioc(CLK_SSI_SSI0) = 0x02; //PA2

            ioc(PA3_SEL) = SSI0_FSS_OUT;
            ioc(PA5_SEL) = SSI0_TXD;
            ioc(SSIRXD_SSI0) = 0x04; //PA4
        } else {

            scr(RCGCSSI) |= 0x02; // Enable clock for SSI1 while in Running mode
            scr(SCGCSSI) |= 0x02; // Enable clock for SSI1 while in Sleep mode

            if(mode == MASTER)
                ioc(PA2_SEL) = SSI1_CLK_OUT;
            else
                ioc(CLK_SSI_SSI1) = 0x02; //PA2
            ioc(PA3_SEL) = SSI1_FSS_OUT;
            ioc(PA5_SEL) = SSI1_TXD;
            ioc(SSIRXD_SSI1) = 0x04; //PA4
        }

        // Set the mode
        value = (mode == SLAVE_OD) ? SOD : 0;
        value |= (mode == MASTER) ? 0 : MS;
        reinterpret_cast<volatile Reg32*>(base)[SSI_CR1 / sizeof(Reg32)] = value;

        //Set the Clock sources
        reinterpret_cast<volatile Reg32*>(base)[SSI_CC / sizeof(Reg32)] = 0x00;

        // Set the clock predivider
        max_bit_rate = clock / bit_rate;
        pre_div = 0;
        do
        {
            pre_div += 2;
            value = (max_bit_rate / pre_div) - 1;
        } while(value > 255);

        reinterpret_cast<volatile Reg32*>(base)[SSI_CPSR / sizeof(Reg32)] = pre_div;

        // Set protocol and clock rate
        // logical & with 0x30 (protocol & 0x30) because it contains all the 3 formats (00 | 01 | 10)
        int tmp1 = (((unsigned int) protocol) & 3) << 6;
        int tmp2 = ((unsigned int) protocol) & 0x30;
        value = (value << 8) | tmp1 | tmp2 | (data_width - 1);
        reinterpret_cast<volatile Reg32*>(base)[SSI_CR0 / sizeof(Reg32)] = value;

        //To enable IO pads to drive outputs, the corresponding IPxx_OVER bits in IOC_Pxx_OVER register
        //has to be configured to 0x8 (OE - Output Enable).
        if(mode == MASTER)
            ioc(PA2_OVER) = OE;
        ioc(PA3_OVER) = OE;
        ioc(PA5_OVER) = OE;

        //Set GPIO pins A2, A3, A4 and A5 to peripheral mode
        gpioa(AFSEL) |= (PIN2) + (PIN3) + (PIN4) + (PIN5);
    }

    static void config_I2C(volatile Log_Addr * base, char gpio_port_sda, unsigned int gpio_pin_sda, char gpio_port_scl, unsigned int gpio_pin_scl)
    {
        // Enable the I2C clock using the SYS_CTRL_RCGCI2C register
        scr(RCGCI2C) = 0x1; // When the CPU is in active (run) mode

        if((gpio_port_sda >= 'A') && (gpio_port_sda <= 'D'))
            gpio_port_sda += ('a'-'A');
        assert((gpio_port_sda >= 'a') && (gpio_port_sda <= 'd'));
        assert(gpio_pin_sda <= 7);

        if((gpio_port_scl >= 'A') && (gpio_port_scl <= 'D'))
            gpio_port_scl += ('a'-'A');
        assert((gpio_port_scl >= 'a') && (gpio_port_scl <= 'd'));
        assert(gpio_pin_scl <= 7);

        // Calculate the offset for the GPIO's IOC_Pxx_SEL
        auto n_sda = gpio_port_sda - 'a';
        auto n_scl = gpio_port_scl - 'a';
        auto sel_sda = PA0_SEL + 0x20*n_sda + 0x4*gpio_pin_sda;
        auto sel_scl = PA0_SEL + 0x20*n_scl + 0x4*gpio_pin_scl;

        auto over_sda = sel_sda + 0x80;
        auto over_scl = sel_scl + 0x80;

        unsigned int pin_bit_sda = 1 << gpio_pin_sda;
        switch(gpio_port_sda)
        {
            case 'a': gpioa(AFSEL) |= pin_bit_sda; gpioa(DIR) &= ~pin_bit_sda; break;
            case 'b': gpiob(AFSEL) |= pin_bit_sda; gpiob(DIR) &= ~pin_bit_sda; break;
            case 'c': gpioc(AFSEL) |= pin_bit_sda; gpioc(DIR) &= ~pin_bit_sda; break;
            case 'd': gpiod(AFSEL) |= pin_bit_sda; gpiod(DIR) &= ~pin_bit_sda; break;
        }

        unsigned int pin_bit_scl = 1 << gpio_pin_scl;
        switch(gpio_port_scl)
        {
            case 'a': gpioa(AFSEL) |= pin_bit_scl; gpioa(DIR) &= ~pin_bit_scl; break;
            case 'b': gpiob(AFSEL) |= pin_bit_scl; gpiob(DIR) &= ~pin_bit_scl; break;
            case 'c': gpioc(AFSEL) |= pin_bit_scl; gpioc(DIR) &= ~pin_bit_scl; break;
            case 'd': gpiod(AFSEL) |= pin_bit_scl; gpiod(DIR) &= ~pin_bit_scl; break;
        }

        ioc(over_sda) = DIS;
        ioc(over_scl) = DIS;

        ioc(sel_sda) = I2C_SDA;
        ioc(sel_scl) = I2C_SCL;

        ioc(I2CMSSDA) = (n_sda << 3) + gpio_pin_sda;
        ioc(I2CMSSCL) = (n_scl << 3) + gpio_pin_scl;
    }

public:
    static volatile Reg32 & ioc(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(IOC_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & scr(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SCR_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & scs(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SCS_BASE)[o / sizeof(Reg32)]; }

    static volatile Reg32 & gpio(unsigned int port, unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOA_BASE + (0x1000 * port))[o / sizeof(Reg32)]; }
    static volatile Reg32 & gpioa(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOA_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & gpiob(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOB_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & gpioc(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOC_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & gpiod(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GPIOD_BASE)[o / sizeof(Reg32)]; }
};

typedef eMote3 Cortex_M_Model;

__END_SYS

#endif

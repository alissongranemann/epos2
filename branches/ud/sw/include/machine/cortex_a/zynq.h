// EPOS Zynq-7000 (Cortex-A9) Mediator Declarations

#ifndef __zynq_h
#define __zynq_h

#include <cpu.h>

__BEGIN_SYS

class Zynq
{
protected:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:
    // Base address for memory-mapped System Control Registers
    enum {
        UART0_BASE          = 0xE0000000,
        UART1_BASE          = 0xE0001000,
        SLCR_BASE           = 0xF8000000,
        CPU_ITF_BASE        = 0xF8F00100,
        GLOBAL_TIMER_BASE   = 0xF8F00200,
        PRIV_TIMER_BASE     = 0XF8F00600,
        DIST_BASE           = 0xF8F01000
    };

    // SLCR Registers offsets
    enum {                              // Description
        FPGA0_CLK_CTRL  = 0x170,        // PL Clock 0 Output control
        PSS_RST_CTRL    = 0x200,        // PS Software Reset Control
        FPGA_RST_CTRL   = 0x240         // FPGA Software Reset Control
    };

    // Useful bits in FPGAN_CLK_CTRL
    enum {                              // Description                  Type    Value after reset
        DIVISOR0        = 1 << 8,       // First cascade divider        r/w     0x18
        DIVISOR1        = 1 << 20       // Second cascade divider       r/w     0x1
    };

    // CPU Interface Registers offsets
    enum {                              // Description
        ICCICR          = 0x000,        // CPU Interface Control
        ICCPMR          = 0x004,        // Interrupt Priority Mask
        ICCIAR          = 0x00C,        // Interrupt Ack
        ICCEOIR         = 0x010         // End Of Interrupt
    };

    // Useful bits in ICCICR
    enum {                              // Description                  Type    Value after reset
        ITF_EN_S        = 1 << 0,       // Enable secure signaling      r/w     0
        ITF_EN_NS       = 1 << 1,       // Enable non-secure signaling  r/w     0
        ACK_CTL         = 1 << 2        // Acknowledge control          r/w     0
    };

    // Global Timer Registers offsets
    enum {                              // Description
        GTCTRL          = 0x00,         // Low Counter
        GTCTRH          = 0x04,         // High Counter
        GTCLR           = 0x08,         // Control
        GTISR           = 0x0C          // Interrupt Status
    };

    // Private Timer Registers offsets
    enum {                              // Description
        PTLR            = 0x00,         // Load
        PTCTR           = 0x04,         // Counter
        PTCLR           = 0x08,         // Control
        PTISR           = 0x0C          // Interrupt Status
    };

    // Useful bits in PTCLR
    enum {                              // Description                  Type    Value after reset
        TIMER_ENABLE    = 1 << 0,       // Enable                       r/w     0
        AUTO_RELOAD     = 1 << 1,       // Auto reload                  r/w     0
        IRQ_EN          = 1 << 2        // Enable interrupt             r/w     0
    };

    // Useful bits in PTISR
    enum {                              // Description                  Type    Value after reset
        INT_CLR         = 1 << 0        // Interrupt clear bit          r/w     0
    };

    // Distributor Registers offsets
    enum {                              // Description
        ICDDCR          = 0x000,        // Distributor Control
        ICDISER0        = 0x100,        // Interrupt Set-Enable
        ICDISER1        = 0x104,        // Interrupt Set-Enable
        ICDISER2        = 0x108,        // Interrupt Set-Enable
        ICDICER0        = 0x180,        // Interrupt Clear-Enable
        ICDICER1        = 0x184,        // Interrupt Clear-Enable
        ICDICER2        = 0x188         // Interrupt Clear-Enable
    };

    // Useful bits in ICDDCR
    enum {                              // Description                  Type    Value after reset
        DIST_EN_S       = 1 << 0        // Enable secure interrupts     r/w     0
    };

protected:
    Zynq() {}

    static void reboot() {
        // This will mess with qemu but works on real hardware, possibly a bug
        // in qemu. Note that the asserting reset will clear the RAM where the
        // application is stored.
        slcr(PSS_RST_CTRL) = 1;
    }

    static unsigned int cpu_id() {
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5"
            : "=r"(id)
            : : );
        return id & 0x3;
    }

    // Returns the frequency set, -1 if frequency can't be set
    static int fpga0_clk_freq(unsigned int freq) {
        const unsigned int div_max = 63, tol = 20;
        unsigned int div0 = 0, div1 = 0,
                io_pll_clock = Traits<Cortex_A>::IO_PLL_CLOCK;
        Reg32 tmp;

        while(++div1 <= div_max) {
            div0 = 1;
            while(++div0 <= div_max)
                if((io_pll_clock/(div0*div1) < (freq + freq/tol)) &&
                        (io_pll_clock/(div0*div1) > (freq - freq/tol)))
                    goto set_clk_ctrl;
        }

        return -1;

        set_clk_ctrl:
        tmp = slcr(FPGA0_CLK_CTRL);
        tmp &= ~((DIVISOR0 * 0x3f) | (DIVISOR1 * 0x3f));
        slcr(FPGA0_CLK_CTRL) = tmp | (DIVISOR0 * div0) | (DIVISOR1 * div1);

        return io_pll_clock/(div0*div1);
    }

    // PL logic connecting to the PS must not be reset when active transactions
    // exist, since uncompleted transactions could be left pending in the PS
    static void fpga_reset(int n) {
        assert(n < 4);
        slcr(FPGA_RST_CTRL) |= 1 << n;
        // FPGA peripherals will reset on the rising/falling edge of their
        // clocks if reset is asserted. This "rough" 1 us delay will ensure all
        // peripherals operating on clocks higher than 1 MHz will be correctly
        // reseted.
        for(unsigned int i = 0; i < Traits<CPU>::CLOCK/1000000; i++)
            ASM("nop");
        slcr(FPGA_RST_CTRL) &= ~(1 << n);
    }

public:
    static volatile Reg32 & slcr(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SLCR_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & cpu_itf(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(CPU_ITF_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & global_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GLOBAL_TIMER_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & priv_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(PRIV_TIMER_BASE)[o / sizeof(Log_Addr)]; }
    static volatile Reg32 & dist(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(DIST_BASE)[o / sizeof(Reg32)]; }
};

typedef Zynq Cortex_A_Model;

class Zynq_UART: protected Zynq
{
private:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Reg32 Reg32;

    static const unsigned int UNITS = Traits<UART>::UNITS;
    static const unsigned int CLOCK = Traits<UART>::CLOCK;

private:
    // Register addresses relative to base
    enum {                                      // Description              Type    Value after reset
        CONTROL_REG0                = 0x00,     // Control                  r/w     0x00000128
        MODE_REG0                   = 0x04,     // Mode                     r/w     0x00000000
        INTRPT_EN_REG0              = 0x08,     // Interrupt enable         r/w     0x00000000
        INTRPT_DIS_REG0             = 0x0C,     // Interrupt disable        r/w     0x00000000
        BAUD_RATE_GEN_REG0          = 0x18,     // Baud rate generator      r/w     0x0000028B
        RCVR_FIFO_TRIGGER_LEVEL0    = 0x20,     // Receive trigger level    r/w     0x00000020
        CHANNEL_STS_REG0            = 0x2C,     // Channel status           ro      0x00000000
        TX_RX_FIFO0                 = 0x30,     // Transmit/receive FIFO    r/w     0x00000000
        BAUD_RATE_DIVIDER_REG0      = 0x34      // Baud rate divider        r/w     0x0000000F
    };

    // Useful bits in the CONTROL_REG0 register
    enum {                                      // Description              Type    Value after reset
        RXRES                       = 1 << 0,   // Reset Rx data path       r/w     0
        TXRES                       = 1 << 1,   // Reset Tx data path       r/w     0
        RXEN                        = 1 << 2,   // Receive enable           r/w     0
        TXEN                        = 1 << 4    // Transmit enable          r/w     0
    };

    // Useful bits in the MODE_REG0 register
    enum {                                      // Description              Type    Value after reset
        CHRL8                       = 0 << 1,   // Character Length 8 bits  r/w     0
        CHRL7                       = 2 << 1,   // Character Length 7 bits  r/w     0
        CHRL6                       = 3 << 1,   // Character Length 6 bits  r/w     0
        PAREVEN                     = 0 << 3,   // Even parity              r/w     0
        PARODD                      = 1 << 3,   // Odd parity               r/w     0
        PARNONE                     = 4 << 3,   // No parity                r/w     0
        NBSTOP2                     = 2 << 6,   // 2 stop bits              r/w     0
        NBSTOP1                     = 0 << 6,   // 1 stop bit               r/w     0
        CHMODENORM                  = 0 << 8,   // Normal mode              r/w     0
        CHMODELB                    = 2 << 8    // Loopback mode            r/w     0
    };

    // Useful bits in the INTRPT_EN_REG0, and INTRPT_DIS_REG0 registers
    enum {                                      // Description              Type    Value after reset
        INTRPT_RTRIG                = 1 << 0,   // Receiver FIFO empty      wo      0
        INTRPT_TTRIG                = 1 << 10   // Transmitter FIFO trigger wo      0
    };

    // Useful bits in the CHANNEL_STS_REG0 register
    enum {                                      // Description              Type    Value after reset
        STS_RTRIG                   = 1 << 0,   // Receiver FIFO trigger    ro      0
        STS_TFUL                    = 1 << 4    // Transmitter FIFO full    ro      0
    };

public:
    Zynq_UART(unsigned int unit, unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits)
        : _base(reinterpret_cast<Log_Addr *>(unit ? UART1_BASE : UART0_BASE)) {
        assert(unit < UNITS);
        config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        // Configure the number of stop bits, data bits, and the parity
        Reg32 mode = reg(MODE_REG0) & ~0xff;

        mode |= stop_bits == 2 ? NBSTOP2 : NBSTOP1;
        mode |= data_bits == 8 ? CHRL8 : data_bits == 7 ? CHRL7 : CHRL6;
        mode |= parity == 2 ? PAREVEN : parity == 1 ? PARODD : PARNONE;
        reg(MODE_REG0) = mode;

        // Set the baud rate
        Reg32 br = CLOCK / (7 * baud_rate);
        reg(BAUD_RATE_DIVIDER_REG0) = 6;
        reg(BAUD_RATE_GEN_REG0) = br;

        // Set the receiver trigger level to 1
        reg(RCVR_FIFO_TRIGGER_LEVEL0) = 1;

        // Enable and reset RX and TX data paths
        reg(CONTROL_REG0) |= RXRES | TXRES | RXEN | TXEN;
    }

    unsigned char rxd() { return reg(TX_RX_FIFO0); }
    void txd(unsigned char ch) { reg(TX_RX_FIFO0) = ch; }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(INTRPT_EN_REG0) |= (receive ? INTRPT_RTRIG : 0) | (send ? INTRPT_TTRIG : 0);
        reg(INTRPT_DIS_REG0) &= ~(receive ? INTRPT_RTRIG : 0) & ~(send ? INTRPT_TTRIG : 0);
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(INTRPT_EN_REG0) &= ~(receive ? INTRPT_RTRIG : 0) & ~(send ? INTRPT_TTRIG : 0);
        reg(INTRPT_DIS_REG0) |= (receive ? INTRPT_RTRIG : 0) | (send ? INTRPT_TTRIG : 0);
    }

    void loopback(bool flag) {
        Reg32 mode = reg(MODE_REG0) & ~0x300;

        if(flag)
            mode |= CHMODELB;
        else
            mode |= CHMODENORM;

        reg(MODE_REG0) = mode;
    }

    bool rxd_ok() { return reg(CHANNEL_STS_REG0) & STS_RTRIG; }
    bool txd_ok() { return !(reg(CHANNEL_STS_REG0) & STS_TFUL); }

private:
    volatile Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile Reg32*>(_base)[o / sizeof(Reg32)]; }

private:
    volatile Log_Addr * _base;
};

typedef Zynq_UART Cortex_A_Model_UART;

__END_SYS

#endif

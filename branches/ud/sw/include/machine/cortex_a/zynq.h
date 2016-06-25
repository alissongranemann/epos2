// EPOS Zynq-7000 (Cortex-A9) Mediator Declarations

#ifndef __zynq_h
#define __zynq_h

#include <cpu.h>
#include <tsc.h>

__BEGIN_SYS

class Zynq
{
public:
    // Base address for memory-mapped System Control Registers
    enum {
        UART0_BASE = 0xE0000000,
        UART1_BASE = 0xE0001000
    };

    // Register addresses relative to base
    enum {
        UART_CONTROL_REG0               = 0x00,
        UART_MODE_REG0                  = 0x04,
        UART_INTRPT_EN_REG0             = 0x08,
        UART_INTRPT_DIS_REG0            = 0x0C,
        UART_BAUD_RATE_GEN_REG0         = 0x18,
        UART_RCVR_FIFO_TRIGGER_LEVEL0   = 0x20,
        UART_CHANNEL_STS_REG0           = 0x2C,
        UART_TX_RX_FIFO0                = 0x30,
        UART_BAUD_RATE_DIVIDER_REG0     = 0x34,
    };

    // Useful bits in the UART_CONTROL_REG0 register
    enum {
        UART_CONTROL_REG0_RXRES   = 0,
        UART_CONTROL_REG0_TXRES   = 1,
        UART_CONTROL_REG0_RXEN    = 2,
        UART_CONTROL_REG0_RXDIS   = 3,
        UART_CONTROL_REG0_TXEN    = 4,
        UART_CONTROL_REG0_TXDIS   = 5,
        UART_CONTROL_REG0_RSTTO   = 6,
        UART_CONTROL_REG0_STTBRK  = 7,
        UART_CONTROL_REG0_STPBRK  = 8
    };

    // Useful bits in the UART_MODE_REG0 register
    enum {
        UART_MODE_REG0_CLKS     = 0,
        UART_MODE_REG0_CHRL     = 1,
        UART_MODE_REG0_PAR      = 3,
        UART_MODE_REG0_NBSTOP   = 6,
        UART_MODE_REG0_CHMODE   = 8
    };

    // Useful bits in the UART_INTRPT_EN_REG0, and UART_INTRPT_DIS_REG0
    // registers
    enum {
        UART_INTRPT_REG0_RTRIG = 0,
        UART_INTRPT_REG0_TTRIG = 10,
    };

    // Useful bits in the UART_CHANNEL_STS_REG0 register
    enum {
        UART_CHANNEL_STS_REG0_RTRIG     = 0,
        UART_CHANNEL_STS_REG0_REMPTY    = 1,
        UART_CHANNEL_STS_REG0_RFUL      = 2,
        UART_CHANNEL_STS_REG0_TEMPTY    = 3,
        UART_CHANNEL_STS_REG0_TFUL      = 4,
        UART_CHANNEL_STS_REG0_RACTIVE   = 10,
        UART_CHANNEL_STS_REG0_TACTIVE   = 11,
        UART_CHANNEL_STS_REG0_FDELT     = 12,
        UART_CHANNEL_STS_REG0_TTRIG     = 13,
        UART_CHANNEL_STS_REG0_TNFUL     = 14
    };
};

typedef Zynq Cortex_A_Model;

class Zynq_UART: protected Zynq
{
public:
    Zynq_UART(unsigned int unit, unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits)
        : _base(unit ? UART1_BASE : UART0_BASE) {
        //assert(unit < UNITS);
        config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        // Divide baud rate by 6+1
        reg(UART_BAUD_RATE_DIVIDER_REG0, 0x6);

        // Set baud rate clock divisor to 62
        reg(UART_BAUD_RATE_GEN_REG0, 0x3E);

        // Enable and reset RX/TX data paths
        reg(UART_CONTROL_REG0, reg(UART_CONTROL_REG0) | 1<<UART_CONTROL_REG0_RXRES | 1<<UART_CONTROL_REG0_TXRES | 1<<UART_CONTROL_REG0_RXEN |
            1<<UART_CONTROL_REG0_TXEN);

        // Set 1 stop bit, no parity and 8 data bits
        reg(UART_MODE_REG0, reg(UART_MODE_REG0) | 0x0<<UART_MODE_REG0_CHRL | 0x4<<UART_MODE_REG0_PAR | 0x0<<UART_MODE_REG0_NBSTOP);

        // Set the receiver trigger level to 1
        reg(UART_RCVR_FIFO_TRIGGER_LEVEL0, 1);
    }

    unsigned char rxd() { return reg(UART_TX_RX_FIFO0); }
    void txd(unsigned char ch) { reg(UART_TX_RX_FIFO0, (Reg32)ch); }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(UART_INTRPT_EN_REG0, reg(UART_INTRPT_EN_REG0) | (receive << UART_INTRPT_REG0_RTRIG) | (send << UART_INTRPT_REG0_TTRIG));
        reg(UART_INTRPT_DIS_REG0, reg(UART_INTRPT_DIS_REG0) & ~(receive << UART_INTRPT_REG0_RTRIG) & ~(send << UART_INTRPT_REG0_TTRIG));
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(UART_INTRPT_EN_REG0, reg(UART_INTRPT_EN_REG0) & ~(receive << UART_INTRPT_REG0_RTRIG) & ~(send << UART_INTRPT_REG0_TTRIG));
        reg(UART_INTRPT_DIS_REG0, reg(UART_INTRPT_DIS_REG0) | (receive << UART_INTRPT_REG0_RTRIG) | (send << UART_INTRPT_REG0_TTRIG));
    }

    void loopback(bool flag) {
        Reg32 mode = reg(UART_MODE_REG0) & ~0x300;

        if(flag)
            mode |= 2 << UART_MODE_REG0_CHMODE;
        else
            mode |= 0 << UART_MODE_REG0_CHMODE;

        reg(UART_MODE_REG0, mode);
    }

    bool rxd_ok() { return reg(UART_CHANNEL_STS_REG0) & (1 << UART_CHANNEL_STS_REG0_RTRIG); }
    bool txd_ok() { return !(reg(UART_CHANNEL_STS_REG0) & (1<<UART_CHANNEL_STS_REG0_TFUL)); }

private:
    typedef CPU::Reg32 Reg32;

private:
    Reg32 reg(Reg32 addr) { return CPU::in32(_base + addr); }
    void reg(Reg32 addr, Reg32 value) { CPU::out32(_base + addr, value); }

private:
    Reg32 _base;
};

typedef Zynq_UART Cortex_A_Model_UART;

__END_SYS

#endif

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
};

typedef Zynq Cortex_A_Model;

class Zynq_UART: protected Zynq
{
private:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Reg32 Reg32;

private:
    // Register addresses relative to base
    enum {
        CONTROL_REG0                = 0x00,
        MODE_REG0                   = 0x04,
        INTRPT_EN_REG0              = 0x08,
        INTRPT_DIS_REG0             = 0x0C,
        BAUD_RATE_GEN_REG0          = 0x18,
        RCVR_FIFO_TRIGGER_LEVEL0    = 0x20,
        CHANNEL_STS_REG0            = 0x2C,
        TX_RX_FIFO0                 = 0x30,
        BAUD_RATE_DIVIDER_REG0      = 0x34
    };

    // Useful bits in the CONTROL_REG0 register
    enum {
        RXRES   = 0,
        TXRES   = 1,
        RXEN    = 2,
        TXEN    = 4
    };

    // Useful bits in the MODE_REG0 register
    enum {
        CHRL    = 1,
        PAR     = 3,
        NBSTOP  = 6,
        CHMODE  = 8
    };

    // Useful bits in the INTRPT_EN_REG0, and INTRPT_DIS_REG0 registers
    enum {
        INTRPT_RTRIG = 0,
        INTRPT_TTRIG = 10
    };

    // Useful bits in the CHANNEL_STS_REG0 register
    enum {
        STS_RTRIG   = 0,
        STS_TFUL    = 4
    };

public:
    Zynq_UART(unsigned int unit, unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits)
        : _base(reinterpret_cast<Log_Addr *>(unit ? UART1_BASE : UART0_BASE)) {
        //assert(unit < UNITS);
        config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        // Divide baud rate by 6+1
        reg(BAUD_RATE_DIVIDER_REG0) = 0x6;

        // Set baud rate clock divisor to 62
        reg(BAUD_RATE_GEN_REG0) = 0x3E;

        // Enable and reset RX/TX data paths
        reg(CONTROL_REG0) |= (1 << RXRES) | (1 << TXRES) | (1 << RXEN) | (1 << TXEN);

        // Set 1 stop bit, no parity and 8 data bits
        reg(MODE_REG0) |= (0x0 << CHRL) | (0x4 << PAR) | (0x0 << NBSTOP);

        // Set the receiver trigger level to 1
        reg(RCVR_FIFO_TRIGGER_LEVEL0) = 1;
    }

    unsigned char rxd() { return reg(TX_RX_FIFO0); }
    void txd(unsigned char ch) { reg(TX_RX_FIFO0) = (Reg32)ch; }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(INTRPT_EN_REG0) |= (receive << INTRPT_RTRIG) | (send << INTRPT_TTRIG);
        reg(INTRPT_DIS_REG0) &= ~(receive << INTRPT_RTRIG) & ~(send << INTRPT_TTRIG);
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(INTRPT_EN_REG0) &= ~(receive << INTRPT_RTRIG) & ~(send << INTRPT_TTRIG);
        reg(INTRPT_DIS_REG0) |= (receive << INTRPT_RTRIG) | (send << INTRPT_TTRIG);
    }

    void loopback(bool flag) {
        Reg32 mode = reg(MODE_REG0) & ~0x300;

        if(flag)
            mode |= 2 << CHMODE;
        else
            mode |= 0 << CHMODE;

        reg(MODE_REG0) = mode;
    }

    bool rxd_ok() { return reg(CHANNEL_STS_REG0) & (1 << STS_RTRIG); }
    bool txd_ok() { return !(reg(CHANNEL_STS_REG0) & (1 << STS_TFUL)); }

private:
    volatile Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile Reg32*>(_base)[o / sizeof(Reg32)]; }

private:
    volatile Log_Addr * _base;
};

typedef Zynq_UART Cortex_A_Model_UART;

__END_SYS

#endif

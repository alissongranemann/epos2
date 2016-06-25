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
        BAUD_RATE_DIVIDER_REG0      = 0x34,
    };

    // Useful bits in the CONTROL_REG0 register
    enum {
        CONTROL_REG0_RXRES  = 0,
        CONTROL_REG0_TXRES  = 1,
        CONTROL_REG0_RXEN   = 2,
        CONTROL_REG0_RXDIS  = 3,
        CONTROL_REG0_TXEN   = 4,
        CONTROL_REG0_TXDIS  = 5,
        CONTROL_REG0_RSTTO  = 6,
        CONTROL_REG0_STTBRK = 7,
        CONTROL_REG0_STPBRK = 8
    };

    // Useful bits in the MODE_REG0 register
    enum {
        MODE_REG0_CLKS      = 0,
        MODE_REG0_CHRL      = 1,
        MODE_REG0_PAR       = 3,
        MODE_REG0_NBSTOP    = 6,
        MODE_REG0_CHMODE    = 8
    };

    // Useful bits in the INTRPT_EN_REG0, and INTRPT_DIS_REG0
    // registers
    enum {
        INTRPT_REG0_RTRIG = 0,
        INTRPT_REG0_TTRIG = 10,
    };

    // Useful bits in the CHANNEL_STS_REG0 register
    enum {
        CHANNEL_STS_REG0_RTRIG      = 0,
        CHANNEL_STS_REG0_REMPTY     = 1,
        CHANNEL_STS_REG0_RFUL       = 2,
        CHANNEL_STS_REG0_TEMPTY     = 3,
        CHANNEL_STS_REG0_TFUL       = 4,
        CHANNEL_STS_REG0_RACTIVE    = 10,
        CHANNEL_STS_REG0_TACTIVE    = 11,
        CHANNEL_STS_REG0_FDELT      = 12,
        CHANNEL_STS_REG0_TTRIG      = 13,
        CHANNEL_STS_REG0_TNFUL      = 14
    };

public:
    Zynq_UART(unsigned int unit, unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits)
        : _base(unit ? UART1_BASE : UART0_BASE) {
        //assert(unit < UNITS);
        config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        // Divide baud rate by 6+1
        reg(BAUD_RATE_DIVIDER_REG0, 0x6);

        // Set baud rate clock divisor to 62
        reg(BAUD_RATE_GEN_REG0, 0x3E);

        // Enable and reset RX/TX data paths
        reg(CONTROL_REG0, reg(CONTROL_REG0) | 1<<CONTROL_REG0_RXRES | 1<<CONTROL_REG0_TXRES | 1<<CONTROL_REG0_RXEN |
            1<<CONTROL_REG0_TXEN);

        // Set 1 stop bit, no parity and 8 data bits
        reg(MODE_REG0, reg(MODE_REG0) | 0x0<<MODE_REG0_CHRL | 0x4<<MODE_REG0_PAR | 0x0<<MODE_REG0_NBSTOP);

        // Set the receiver trigger level to 1
        reg(RCVR_FIFO_TRIGGER_LEVEL0, 1);
    }

    unsigned char rxd() { return reg(TX_RX_FIFO0); }
    void txd(unsigned char ch) { reg(TX_RX_FIFO0, (Reg32)ch); }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(INTRPT_EN_REG0, reg(INTRPT_EN_REG0) | (receive << INTRPT_REG0_RTRIG) | (send << INTRPT_REG0_TTRIG));
        reg(INTRPT_DIS_REG0, reg(INTRPT_DIS_REG0) & ~(receive << INTRPT_REG0_RTRIG) & ~(send << INTRPT_REG0_TTRIG));
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(INTRPT_EN_REG0, reg(INTRPT_EN_REG0) & ~(receive << INTRPT_REG0_RTRIG) & ~(send << INTRPT_REG0_TTRIG));
        reg(INTRPT_DIS_REG0, reg(INTRPT_DIS_REG0) | (receive << INTRPT_REG0_RTRIG) | (send << INTRPT_REG0_TTRIG));
    }

    void loopback(bool flag) {
        Reg32 mode = reg(MODE_REG0) & ~0x300;

        if(flag)
            mode |= 2 << MODE_REG0_CHMODE;
        else
            mode |= 0 << MODE_REG0_CHMODE;

        reg(MODE_REG0, mode);
    }

    bool rxd_ok() { return reg(CHANNEL_STS_REG0) & (1 << CHANNEL_STS_REG0_RTRIG); }
    bool txd_ok() { return !(reg(CHANNEL_STS_REG0) & (1<<CHANNEL_STS_REG0_TFUL)); }

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

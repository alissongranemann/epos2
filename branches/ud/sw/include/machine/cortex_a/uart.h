#ifndef __cortex_a_uart_h__
#define __cortex_a_uart_h__

#include <cpu.h>
#include <uart.h>

__BEGIN_SYS

class Cortex_A_UART: public UART_Common
{
public:
    Cortex_A_UART(unsigned int baud, unsigned int data_bits, unsigned int parity,
            unsigned int stop_bits, unsigned int unit = 0) {}

    // The default unit is 1 because Serial_Display instantiate it without
    // paremeters and we want it to use UART 1
    Cortex_A_UART(unsigned int unit = 1) {
        if(unit == 0)
            _base = UART0_BASE;
        else if(unit == 1)
            _base = UART1_BASE;

        // Divide baud rate by 6+1
        reg(BAUD_RATE_DIVIDER_REG0, 0x6);

        // Set baud rate clock divisor to 62
        reg(BAUD_RATE_GEN_REG0, 0x3E);

        // Enable and reset RX/TX data paths
        reg(CONTROL_REG0, reg(CONTROL_REG0) | 1<<RXRES | 1<<TXRES | 1<<RXEN |
            1<<TXEN);

        // Set 1 stop bit, no parity and 8 data bits
        reg(MODE_REG0, reg(MODE_REG0) | 0x0<<CHRL | 0x4<<PAR | 0x0<<NBSTOP);
    }

    void put(unsigned char ch) {
        while((reg(CHANNEL_STS_REG0) & 1<<TFUL) != 0);
        reg(TX_RX_FIFO0, (Reg32)ch);
    }

    unsigned char get() {
        while((reg(CHANNEL_STS_REG0) & 1<<RTRIG) != 1);
        return reg(TX_RX_FIFO0);
    }

private:
    typedef CPU::Reg32 Reg32;

    // Base address of each UART
    enum {
        UART0_BASE = 0xE0000000,
        UART1_BASE = 0xE0001000
    };

    // Register addresses relative to base
    enum {
        CONTROL_REG0            = 0x00,
        MODE_REG0               = 0x04,
        BAUD_RATE_GEN_REG0      = 0x18,
        CHANNEL_STS_REG0        = 0x2C,
        TX_RX_FIFO0             = 0x30,
        BAUD_RATE_DIVIDER_REG0  = 0x34,
    };

    // CONTROL_REG0 bits
    enum {
        RXRES   = 0,
        TXRES   = 1,
        RXEN    = 2,
        RXDIS   = 3,
        TXEN    = 4,
        TXDIS   = 5,
        RSTTO   = 6,
        STTBRK  = 7,
        STPBRK  = 8
    };

    // MODE_REG0 bits
    enum {
        CLKS    = 0,
        CHRL    = 1,
        PAR     = 3,
        NBSTOP  = 6,
        CHMODE  = 8
    };

    // CHANNEL_STS_REG0 bis
    enum {
        RTRIG   = 0,
        REMPTY  = 1,
        RFUL    = 2,
        TEMPTY  = 3,
        TFUL    = 4,
        RACTIVE = 10,
        TACTIVE = 11,
        FDELT   = 12,
        TTRIG   = 13,
        TNFUL   = 14
    };

private:
    Reg32 reg(Reg32 addr) { return CPU::in32(_base + addr); }
    void reg(Reg32 addr, Reg32 value) { CPU::out32(_base + addr, value); }

private:
    Reg32 _base;
};

__END_SYS

#endif

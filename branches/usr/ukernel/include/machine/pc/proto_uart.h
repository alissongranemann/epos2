#ifndef __proto_uart_h
#define __proto_uart_h

#include <architecture/ia32/cpu.h>

__BEGIN_SYS

class Proto_UART
{
    typedef CPU::IO_Port IO_Port;
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;

    static const unsigned int CLOCK = 1843200 / 16;

    static const unsigned int DEF_BAUD_RATE = 115200;
    static const unsigned int DEF_DATA_BITS = 8;
    static const unsigned int DEF_PARITY = 0; // none
    static const unsigned int DEF_STOP_BITS = 1;

    static const unsigned int COM1 = 0x3f8; // to 0x3ff, IRQ4
    static const unsigned int COM2 = 0x2f8; // to 0x2ff, IRQ3
    static const unsigned int COM3 = 0x3e8; // to 0x3ef, no IRQ
    static const unsigned int COM4 = 0x2e8; // to 0x2ef, no IRQ

    static const IO_Port _port = COM1;

    // Register Addresses (relative to base I/O port)
    typedef Reg8 Address;
    enum {
        THR = 0, // Transmit Holding    W,   DLAB = 0
        RBR = 0, // Receive Buffer  R,   DLAB = 0
        IER = 1, // Interrupt Enable    R/W, DLAB = 0 [0=DR,1=THRE,2=LI,3=MO]
        FCR = 2, // FIFO Control    W   [0=EN,1=RC,2=XC,3=RDY,67=TRG]
        IIR = 2, // Interrupt Id    R   [0=PEN,12=ID,3=FIFOTO,67=1]
        LCR = 3, // Line Control    R/W [01=DL,2=SB,345=P,6=BRK,7=DLAB]
        MCR = 4, // Modem Control   R/W [0=DTR,1=RTS,2=OUT1,3=OUT2,4=LB]
        LSR = 5, // Line Status     R/W [0=DR,1=OE,2=PE,3=FE,4=BI,5=THRE,
                 //              6=TEMT,7=FIFOE]
        MSR = 6, // Modem Status    R/W [0=CTS,1=DSR,2=RI,3=DCD,4=LBCTS,
                 //                  5=LBDSR,6=LBRI,7=LBDCD]
        SCR = 7, // Scratch         R/W
        DLL = 0, // Divisor Latch LSB   R/W, DLAB = 1
        DLH = 1  // Divisor Latch MSB   R/W, DLAB = 1
    };

public:
    static void config() {
        unsigned int brate = DEF_BAUD_RATE;
        unsigned int dbits = DEF_DATA_BITS;
        unsigned int par = DEF_PARITY;
        unsigned int sbits = DEF_STOP_BITS;
        // Disable all interrupts
        reg(IER, 0);

        // Set clock divisor
        unsigned int div = CLOCK / brate;
        dlab(true);
        reg(DLL, div);
        reg(DLH, div >> 8);
        dlab(false);

        // Set data word length (5, 6, 7 or 8)
        Reg8 lcr = dbits - 5;

        // Set parity (0 [no], 1 [odd], 2 [even])
        if(par) {
            lcr |= 1 << 3;
            lcr |= (par - 1) << 4;
        }

        // Set stop-bits (1, 2 or 3 [1.5])
        lcr |= (sbits > 1) ? (1 << 2) : 0;

        reg(LCR, lcr);

        // Enables Tx and Rx FIFOs, clear them, set trigger to 14 bytes
        reg(FCR, 0xc7);

        // Set DTR, RTS and OUT2 of MCR
        reg(MCR, reg(MCR) | 0x0b);
    }


    static bool txd_ok() { return reg(LSR) & (1 << 5); }

    static void txd(Reg8 c) { reg(THR, c); }

    static void put(char c) { while(!txd_ok()); txd(c); }

private:
    static Reg8 reg(Address addr) { return CPU::in8(_port + addr); }
    static void reg(Address addr, Reg8 value) { CPU::out8(_port + addr, value); }
    static void dlab(bool f) { reg(LCR, (reg(LCR) & 0x7f) | (f << 7)); }
};

__END_SYS

#endif

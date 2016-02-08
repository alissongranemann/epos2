// EPOS Cortex-M IEEE 802.15.4 NIC Mediator Declarations

#ifndef __cortex_m_radio_h
#define __cortex_m_radio_h

#include <tstp_mac.h>
#include <ic.h>
#include <mmu.h>

__BEGIN_SYS

// CC2538 IEEE 802.15.4 RF Transceiver
class CC2538RF
{
protected:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef CPU::IO_Irq IO_Irq;
    typedef MMU::DMA_Buffer DMA_Buffer;

    // Bases
    enum
    {
        FFSM_BASE = 0x40088500,
        XREG_BASE = 0x40088600,
        SFR_BASE  = 0x40088800,
        ANA_BASE  = 0x40088800,
        RXFIFO    = 0x40088000,
        TXFIFO    = 0x40088200,
    };

    // Useful FFSM register offsets
    enum
    {
        SRCRESINDEX = 0x8c,
        PAN_ID0     = 0xc8,
        PAN_ID1     = 0xcc,
        SHORT_ADDR0 = 0xd0,
        SHORT_ADDR1 = 0xd4,
    };

    // ANA_REGS register
    enum
    {
        IVCTRL    = 0x04,
    };

    // Useful XREG register offsets
    enum
    {
        FRMFILT0  = 0x000,
        FRMFILT1  = 0x004,
        SRCMATCH  = 0x008,
        FRMCTRL0  = 0x024,
        FRMCTRL1  = 0x028,
        RXMASKSET = 0x030,
        FREQCTRL  = 0x03C,
        FSMSTAT1  = 0x04C,
        FIFOPCTRL = 0x050,
        RXFIFOCNT = 0x06C,
        TXFIFOCNT = 0x070,
        RFIRQM0   = 0x08c,
        RFIRQM1   = 0x090,
        CSPT      = 0x194,
        AGCCTRL1  = 0x0c8,
        TXFILTCFG = 0x1e8,
        FSCAL1    = 0x0b8,
        CCACTRL0  = 0x058,
        TXPOWER   = 0x040,
        RSSI      = 0x060,
        RSSISTAT  = 0x064,
    };

    // Useful SFR register offsets
    enum
    {
        RFDATA  = 0x28,
        RFERRF  = 0x2c,
        RFIRQF1 = 0x30,
        RFIRQF0 = 0x34,
        RFST    = 0x38,
    };


    // Radio commands
    enum
    {
        STXON       = 0xd9,
        SFLUSHTX    = 0xde,
        ISSTART     = 0xe1,
        ISRXON      = 0xe3,
        ISTXON      = 0xe9,
        ISTXONCCA   = 0xea,
        ISSAMPLECCA = 0xeb,
        ISFLUSHRX   = 0xed,
        ISFLUSHTX   = 0xee,
        ISRFOFF     = 0xef,
        ISCLEAR     = 0xff,
    };

    // Useful bits in RSSISTAT
    enum 
    {
        RSSI_VALID = 1 << 0,
    };    
    // Useful bits in XREG_FRMFILT0
    enum
    {
        MAX_FRAME_VERSION = 1 << 2,
        PAN_COORDINATOR   = 1 << 1,
        FRAME_FILTER_EN   = 1 << 0,
    };
    // Useful bits in XREG_FRMFILT1
    enum
    {
        ACCEPT_FT3_MAC_CMD = 1 << 6,
        ACCEPT_FT2_ACK     = 1 << 5,
        ACCEPT_FT1_DATA    = 1 << 4,
        ACCEPT_FT0_BEACON  = 1 << 3,
    };
    // Useful bits in XREG_SRCMATCH
    enum
    {
        SRC_MATCH_EN   = 1 << 0,
    };

    // Useful bits in XREG_FRMCTRL0
    enum
    {
        APPEND_DATA_MODE = 1 << 7,
        AUTO_CRC         = 1 << 6,
        AUTO_ACK         = 1 << 5,
        ENERGY_SCAN      = 1 << 4,
        RX_MODE          = 1 << 2,
        TX_MODE          = 1 << 0,
    };

    // Bit set by hardware in FCS field when AUTO_CRC is set
    enum
    {
        AUTO_CRC_OK = 0x80,
    };

    // Useful bits in XREG_FRMCTRL1
    enum
    {
        PENDING_OR         = 1 << 2,
        IGNORE_TX_UNDERF   = 1 << 1,
        SET_RXENMASK_ON_TX = 1 << 0,
    };

    // Useful bits in XREG_FSMSTAT1
    enum XREG_FSMSTAT1
    {
        FIFO        = 1 << 7,
        FIFOP       = 1 << 6,
        SFD         = 1 << 5,
        CCA         = 1 << 4,
        SAMPLED_CCA = 1 << 3,
        LOCK_STATUS = 1 << 2,
        TX_ACTIVE   = 1 << 1,
        RX_ACTIVE   = 1 << 0,
    };

    // Useful bits in SFR_RFIRQF1
    enum
    {
        TXDONE = 1 << 1,
    };

    // RFIRQF0 Interrupts
    enum
    {
        INT_RXMASKZERO      = 1 << 7,
        INT_RXPKTDONE       = 1 << 6,
        INT_FRAME_ACCEPTED  = 1 << 5,
        INT_SRC_MATCH_FOUND = 1 << 4,
        INT_SRC_MATCH_DONE  = 1 << 3,
        INT_FIFOP           = 1 << 2,
        INT_SFD             = 1 << 1,
        INT_ACT_UNUSED      = 1 << 0,
    };

    // RFIRQF1 Interrupts
    enum
    {
        INT_CSP_WAIT   = 1 << 5,
        INT_CSP_STOP   = 1 << 4,
        INT_CSP_MANINT = 1 << 3,
        INT_RFIDLE     = 1 << 2,
        INT_TXDONE     = 1 << 1,
        INT_TXACKDONE  = 1 << 0,
    };

protected:
    volatile Reg32 & ana (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(ANA_BASE + offset)); }
    volatile Reg32 & xreg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(XREG_BASE + offset)); }
    volatile Reg32 & ffsm (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(FFSM_BASE + offset)); }
    volatile Reg32 & sfr  (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(SFR_BASE  + offset)); }

    volatile bool _rx_done() { return (xreg(FSMSTAT1) & FIFOP); }
};


// CC2538 TSTP Radio Mediator
class CC2538: public TSTP_MAC, private CC2538RF
{
    friend class TSTP_MAC;

    template <int unit> friend void call_init();

private:
    // Transmit and Receive Ring sizes
    static const unsigned int UNITS = Traits<CC2538>::UNITS;
    static const unsigned int TX_BUFS = Traits<CC2538>::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Traits<CC2538>::RECEIVE_BUFFERS;


    /*
    // Size of the DMA Buffer that will host the ring buffers and the init block
    static const unsigned int DMA_BUFFER_SIZE = ((sizeof(Init_Block) + 15) & ~15U) +
        RX_BUFS * ((sizeof(Rx_Desc) + 15) & ~15U) + TX_BUFS * ((sizeof(Tx_Desc) + 15) & ~15U) +
        RX_BUFS * ((sizeof(Buffer) + 15) & ~15U) + TX_BUFS * ((sizeof(Buffer) + 15) & ~15U); // align128() cannot be used here
    */
    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer);


    // Interrupt dispatching binding
    struct Device {
        CC2538 * device;
        unsigned int interrupt;
    };

protected:
    CC2538(unsigned int unit, IO_Irq irq, DMA_Buffer * dma_buf);

public:
    typedef void (Frame_Handler)(Buffer *);

    void set_channel(unsigned int channel);

    void off();
    void reset();
    void receive(const Frame_Handler & handler);

    ~CC2538();

    int send(Buffer * buf);
    bool channel_busy();

    Buffer * alloc(NIC * nic, unsigned int payload);

    void free(Buffer * buf);

    const Address & address() { return _address; }
    void address(const Address & address);

    const Statistics & statistics() { return _statistics; }

    static CC2538 * get(unsigned int unit = 0) { return get_by_unit(unit); }
    void set_tx(); //TODO:REMOVE
    void clear_tx(); //TODO:REMOVE
    void set_rx(); //TODO:REMOVE
    void clear_rx(); //TODO:REMOVE

private:
    Frame_Handler * fifop_handler;

    Statistics _statistics;

    void handle_int();
    bool copy_from_rxfifo(Buffer * buf);
    bool rxfifo_crc_check();
    void clear_rxfifo() { sfr(RFST) = ISFLUSHRX; }

    static void int_handler(const IC::Interrupt_Id & interrupt);

    static CC2538 * get_by_unit(unsigned int unit) {
        if(unit >= UNITS) {
            db<CC2538>(WRN) << "Radio::get: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _devices[unit].device;
    }

    static CC2538 * get_by_interrupt(unsigned int interrupt) {
        for(unsigned int i = 0; i < UNITS; i++)
            if(_devices[i].interrupt == interrupt) {
                return _devices[i].device;
            }

        return 0;
    };

    static void init(unsigned int unit);

    // Send a message and wait for it to be correctly sent
    bool send_and_wait(bool ack);

    bool do_send();

private:
    unsigned int _unit;

    Address _address;

    IO_Irq _irq;

    DMA_Buffer * _dma_buf;

    int _rx_cur;
    int _tx_cur;

    Buffer * _rx_buffer[RX_BUFS];
    Buffer * _tx_buffer[TX_BUFS];

    static Device _devices[UNITS];
    GPIO * _tx_pin;
    GPIO * _rx_pin;
};

__END_SYS

#endif

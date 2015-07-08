// EPOS eMote3 IEEE 802.15.4 NIC Mediator Declarations

#ifndef __emote3_radio_h
#define __emote3_radio_h

#include <ieee802_15_4.h>
#include "machine.h"

__BEGIN_SYS

// eMote3 IEEE 802.15.4 Radio
class eMote3_IEEE802_15_4
{
protected:

    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef CPU::IO_Irq IO_Irq;
    typedef MMU::DMA_Buffer DMA_Buffer;
    typedef IEEE802_15_4::Address MAC_Address;


    // Bases
    enum
    {
        FFSM_BASE = 0x40088500,
        XREG_BASE = 0x40088600,
        SFR_BASE  = 0x40088800,            
        RXFIFO    = 0x40088000,
        TXFIFO    = 0x40088200,
    };

    // Useful FFSM register offsets
    enum
    {
        SRCRESINDEX = 0x8c,
        PAN_ID0     = 0xc8,
        PAN_ID1     = 0xcc,
    };

    // Useful XREG register offsets
    enum
    {
        FRMFILT0  = 0x000,
        FRMFILT1  = 0x004,
        SRCMATCH  = 0x008,
        FRMCTRL0  = 0x024,
        FRMCTRL1  = 0x028,
        FREQCTRL  = 0x03C,
        FSMSTAT1  = 0x04C,
        FIFOPCTRL = 0x050,
        RXFIFOCNT = 0x06C,
        TXFIFOCNT = 0x070,
	    RFIRQM0   = 0x08c,
	    RFIRQM1   = 0x090,
	    CSPT      = 0x194,
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
        STXON     = 0xd9,
        SFLUSHTX  = 0xde,
        ISRXON    = 0xe3,
        ISTXON    = 0xe9,
        ISFLUSHRX = 0xed,
        ISFLUSHTX = 0xee,
        ISRFOFF   = 0xef,
        ISCLEAR   = 0xff,
    };

    // Useful bits in XREG_FRMFILT0
    enum
    {
        MAX_FRAME_VERSION = 1 << 2,
        PAN_COORDINATOR   = 1 << 1,
        FRAME_FILTER_EN   = 1 << 0,
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

    // Useful bits in XREG_FRMCTRL1
    enum
    {
        PENDING_OR         = 1 << 2,
        IGNORE_TX_UNDERF   = 1 << 1,
        SET_RXENMASK_ON_TX = 1 << 0,
    };

    // Useful bits in XREG_FSMSTAT1
    enum
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
};

// eMote3 Radio NIC
class eMote3_Radio: public IEEE802_15_4, public IEEE802_15_4::Observed, private eMote3_IEEE802_15_4
{
    template <int unit> friend void call_init();

private:
    // Transmit and Receive Ring sizes
    static const unsigned int UNITS = Traits<eMote3_Radio>::UNITS;
    static const unsigned int TX_BUFS = Traits<eMote3_Radio>::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Traits<eMote3_Radio>::RECEIVE_BUFFERS;

    // Size of the DMA Buffer that will host the ring buffers and the init block
    /*
    static const unsigned int DMA_BUFFER_SIZE = ((sizeof(Init_Block) + 15) & ~15U) +
        RX_BUFS * ((sizeof(Rx_Desc) + 15) & ~15U) + TX_BUFS * ((sizeof(Tx_Desc) + 15) & ~15U) +
        RX_BUFS * ((sizeof(Buffer) + 15) & ~15U) + TX_BUFS * ((sizeof(Buffer) + 15) & ~15U); // align128() cannot be used here
        */



    // Interrupt dispatching binding
    struct Device
    {
        eMote3_Radio * device;
        unsigned int interrupt;
    };
        
protected:
    eMote3_Radio(unsigned int unit, IO_Irq irq);

public:
    ~eMote3_Radio();

    int send(const Address & dst, const Protocol & prot, const void * data, unsigned int size);
    int receive(Address * src, Protocol * prot, void * data, unsigned int size);

    Buffer * alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload);
    void free(Buffer * buf);
    int send(Buffer * buf);

    const Address & address() { return _address; }
    void address(const Address & address) { _address = address; }

    const Statistics & statistics() { return _statistics; }

    void reset();

    void set_channel(unsigned int channel);

    static eMote3_Radio * get(unsigned int unit = 0) { return get_by_unit(unit); }

    void listen(); 

    void stop_listening();

private:
    void handle_int();

    static void int_handler(const IC::Interrupt_Id & interrupt);

    static eMote3_Radio * get_by_unit(unsigned int unit) {
        if(unit >= UNITS) {
            db<eMote3_Radio>(WRN) << "eMote3_Radio::get: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _devices[unit].device;
    }

    static eMote3_Radio * get_by_interrupt(unsigned int interrupt) {
        for(unsigned int i = 0; i < UNITS; i++)
            if(_devices[i].interrupt == interrupt)
        	return _devices[i].device;

        return 0;
    };

    static void init(unsigned int unit);

    volatile Reg32 & xreg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(XREG_BASE + offset)); }
    volatile Reg32 & ffsm (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(FFSM_BASE + offset)); }
    volatile Reg32 & sfr  (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(SFR_BASE  + offset)); }

    // Trigger an immediate send poll
    void _cmd_send_now() { sfr(RFST) = ISTXON; }

    // Checks whether the TXDONE interrupt is set and if so, clears it
    volatile bool _tx_done() 
    { 
        volatile bool ret = sfr(RFIRQF1) & TXDONE; 
        if(ret)
            sfr(RFIRQF1) &= ~TXDONE;
        return ret;
    }
    volatile bool _rx_done() { return (xreg(FSMSTAT1) & FIFOP); }

private:
    unsigned int _unit;

    Address _address;
    Statistics _statistics;

    IO_Irq _irq;
//    DMA_Buffer * _dma_buf;

    int _rx_cur;
    int _tx_cur;

    Buffer * _rx_buffer[RX_BUFS];
    Buffer * _tx_buffer[TX_BUFS];

    static Device _devices[UNITS];
};

__END_SYS

#endif

// EPOS Cortex-M IEEE 802.15.4 NIC Mediator Declarations

#ifndef __cortex_m_radio_h
#define __cortex_m_radio_h

#include <ieee802_15_4.h>

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
    typedef IEEE802_15_4::Address MAC_Address;

public:
    // Initialization block
    struct Init_Block {
        Reg16 mode;             // (pg 120)
        Reg8 rlen;              // log2(n buf), e.g. 4 -> 16 Rx_Desc
        Reg8 tlen;              // log2(n buf), e.g. 3 ->  8 Tx_Desc
        MAC_Address  mac_addr;  // MAC address
        Reg16 reserved;
        Reg32 filter1;
        Reg32 filter2;
        Reg32 rx_ring;          // Tx Ring DMA physical address
        Reg32 tx_ring;          // Rx Ring DMA physical address
    };

    // Transmit and Receive Descriptors (in the Ring Buffers)
    struct Desc {
        enum {
            OWN = 0x8000,
            ERR = 0x4000,
            STP = 0x0200,
            ENP = 0x0100,
            BPE = 0x0080
        };

        Reg32 phy_addr;
        volatile Reg16 size; // 2's complement
        volatile Reg16 status;
        volatile Reg32 misc;
        volatile Reg32 reserved;
    };

    // Receive Descriptor
    struct Rx_Desc: public Desc {
        enum {
            BUFF = 0x0400,
            CRC  = 0x0800,
            OFLO = 0x1000,
            FRAM = 0x2000
        };

        friend Debug & operator<<(Debug & db, const Rx_Desc & d) {
            db << "{" << hex << d.phy_addr << dec
               << "," << 65536 - d.size
               << "," << hex << d.status
               << "," << d.misc << dec << "}";
            return db;
        }
    };

    // Transmit Descriptor
    struct Tx_Desc: public Desc {
        friend Debug & operator<<(Debug & db, const Tx_Desc & d) {
            db << "{" << hex << d.phy_addr << dec
               << "," << 65536 - d.size
               << "," << hex << d.status
               << "," << d.misc << dec << "}";
            return db;
        }
    };

};

// CC2538 IEEE 802.15.4 Radio Mediator
class CC2538: public IEEE802_15_4, public IEEE802_15_4::Observed, private CC2538RF
{
    template <int unit> friend void call_init();

private:
    // Transmit and Receive Ring sizes
    static const unsigned int UNITS = Traits<CC2538>::UNITS;
    static const unsigned int TX_BUFS = Traits<CC2538>::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Traits<CC2538>::RECEIVE_BUFFERS;


    // Size of the DMA Buffer that will host the ring buffers and the init block
    static const unsigned int DMA_BUFFER_SIZE = ((sizeof(Init_Block) + 15) & ~15U) +
        RX_BUFS * ((sizeof(Rx_Desc) + 15) & ~15U) + TX_BUFS * ((sizeof(Tx_Desc) + 15) & ~15U) +
        RX_BUFS * ((sizeof(Buffer) + 15) & ~15U) + TX_BUFS * ((sizeof(Buffer) + 15) & ~15U); // align128() cannot be used here


    // Interrupt dispatching binding
    struct Device {
        CC2538 * device;
        unsigned int interrupt;
    };
        
protected:
    CC2538(unsigned int unit, IO_Irq irq, DMA_Buffer * dma_buf);

public:
    ~CC2538();

    int send(const Address & dst, const Protocol & prot, const void * data, unsigned int size);
    int receive(Address * src, Protocol * prot, void * data, unsigned int size);

    Buffer * alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload);
    void free(Buffer * buf);
    int send(Buffer * buf);

    const Address & address() { return _address; }
    void address(const Address & address) { _address = address; }

    const Statistics & statistics() { return _statistics; }

    void reset();

    static CC2538 * get(unsigned int unit = 0) { return get_by_unit(unit); }

private:
    void handle_int();

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
            if(_devices[i].interrupt == interrupt)
        	return _devices[i].device;

        return 0;
    };

    static void init(unsigned int unit);

private:
    unsigned int _unit;

    Address _address;
    Statistics _statistics;

    IO_Irq _irq;
    DMA_Buffer * _dma_buf;

    Init_Block * _iblock;
    Phy_Addr  _iblock_phy;

    int _rx_cur;
    Rx_Desc * _rx_ring;
    Phy_Addr _rx_ring_phy;

    int _tx_cur;
    Tx_Desc * _tx_ring;
    Phy_Addr _tx_ring_phy;

    Buffer * _rx_buffer[RX_BUFS];
    Buffer * _tx_buffer[TX_BUFS];

    static Device _devices[UNITS];
};

__END_SYS

#endif

// EPOS TI CC2538 IEEE 802.15.4 NIC Mediator Declarations

#ifndef __cc2538_h
#define __cc2538_h

#include <ieee802_15_4.h>
#include <ic.h>

__BEGIN_SYS

// IT CC2538 IEEE 802.15.4 RF Transceiver
class CC2538RF
{
    friend class One_Hop_MAC;
    friend class TSTP_MAC;

protected:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;
    typedef CPU::IO_Irq IO_Irq;
    typedef MMU::DMA_Buffer DMA_Buffer;

public:
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
        FRMFILT0    = 0x000,
        FRMFILT1    = 0x004,
        SRCMATCH    = 0x008,
        FRMCTRL0    = 0x024,
        FRMCTRL1    = 0x028,
        RXMASKSET   = 0x030,
        FREQCTRL    = 0x03C,
        FSMSTAT1    = 0x04C,
        FIFOPCTRL   = 0x050,
        RXFIRST     = 0x068,
        RXFIFOCNT   = 0x06C,
        TXFIFOCNT   = 0x070,
        RXFIRST_PTR = 0x074,
        RXLAST_PTR  = 0x078,
        RFIRQM0     = 0x08c,
        RFIRQM1     = 0x090,
        CSPT        = 0x194,
        AGCCTRL1    = 0x0c8,
        TXFILTCFG   = 0x1e8,
        FSCAL1      = 0x0b8,
        CCACTRL0    = 0x058,
        TXPOWER     = 0x040,
        RSSI        = 0x060,
        RSSISTAT    = 0x064,
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
    enum RX_MODES
    {
        RX_MODE_NORMAL = 0,
        RX_MODE_OUTPUT_TO_IOC,
        RX_MODE_CYCLIC,
        RX_MODE_NO_SYMBOL_SEARCH,
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

    CC2538RF() {
        // Enable clock to the RF CORE module
        Machine::radio_enable();

        // Disable device interrupts
        xreg(RFIRQM0) = 0;
        xreg(RFIRQM1) = 0;

        // Change recommended in the user guide (CCACTRL0 register description)
        xreg(CCACTRL0) = 0xF8;

        // Changes recommended in the user guide (Section 23.15 Register Settings Update)
        xreg(TXFILTCFG) = 0x09;
        xreg(AGCCTRL1) = 0x15;
        ana(IVCTRL) = 0x0b;
        xreg(FSCAL1) = 0x01;

        sfr(RFST) = ISFLUSHTX; // Clear TXFIFO
        sfr(RFST) = ISFLUSHRX; // Clear RXFIFO

        // Reset result of source matching (value undefined on reset)
        ffsm(SRCRESINDEX) = 0;

        // Set FIFOP threshold to maximum
        xreg(FIFOPCTRL) = 0xff;

        // Set TXPOWER (this is the value Contiki uses by default)
        xreg(TXPOWER) = 0xD5;

        rx_mode(RX_MODE_NORMAL);

        // Clear interrupts
        sfr(RFIRQF0) = 0;
        sfr(RFIRQF1) = 0;

        // Clear error flags
        sfr(RFERRF) = 0;
    }

    void off() { sfr(RFST) = ISRFOFF; clear_rxfifo(); sfr(RFIRQF0) = 0; }
    void rx() { sfr(RFST) = ISRXON; }
    void tx() { sfr(RFST) = ISTXON; }
    volatile bool cca() { return xreg(FSMSTAT1) & CCA; }
    volatile bool cca_valid() { return xreg(RSSISTAT) & RSSI_VALID; }
    void start_cca() { rx_mode(RX_MODE_NO_SYMBOL_SEARCH); rx(); }
    void end_cca() { rx_mode(RX_MODE_NORMAL); }
    bool valid_frame() { return frame_in_rxfifo(); }

    void setup_tx(char * f, unsigned int size) {
        sfr(RFDATA) = size + sizeof(IEEE802_15_4::CRC);
        for(auto i=0u; i < size; i++)
            sfr(RFDATA) = f[i];
    }
    volatile bool tx_ok() {
        volatile bool ret = (sfr(RFIRQF1) & INT_TXDONE);
        if(ret)
            sfr(RFIRQF1) &= ~INT_TXDONE;
        return ret;
    }

    void channel(unsigned int c) {
        if((c > 10) and (c < 27)) {
            /*
               The carrier frequency is set by programming the 7-bit frequency word in the FREQ[6:0] bits of the
               FREQCTRL register. Changes take effect after the next recalibration. Carrier frequencies in the range
               from 2394 to 2507 MHz are supported. The carrier frequency f C , in MHz, is given by
               f C = (2394 + FREQCTRL.FREQ[6:0]) MHz, and is programmable in 1-MHz steps.
               IEEE 802.15.4-2006 specifies 16 channels within the 2.4-GHz band. These channels are numbered 11
               through 26 and are 5 MHz apart. The RF frequency of channel k is given by Equation 1.
               f c = 2405 + 5(k –11) [MHz] k [11, 26]
               (1)
               For operation in channel k, the FREQCTRL.FREQ register should therefore be set to
               FREQCTRL.FREQ = 11 + 5 (k – 11).
               */
            frequency(11+5*(c-11));
        }
    }

protected:
    bool tx_if_cca() { sfr(RFST) = ISTXONCCA; return (xreg(FSMSTAT1) & SAMPLED_CCA); }
    void rx_mode(RX_MODES m) {
        xreg(FRMCTRL0) = (xreg(FRMCTRL0) & ~(3 * RX_MODE)) | (m * RX_MODE);
    }
    void copy_from_rxfifo(IEEE802_15_4::Frame * c);
    bool frame_in_rxfifo();
    void clear_rxfifo() { sfr(RFST) = ISFLUSHRX; }
    void frequency(unsigned int freq) { xreg(FREQCTRL) = freq; }
    void clear_txfifo() {
        sfr(RFST) = ISFLUSHTX;
        while(xreg(TXFIFOCNT) != 0);
    }

    volatile Reg32 & ana (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(ANA_BASE + offset)); }
    volatile Reg32 & xreg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(XREG_BASE + offset)); }
    volatile Reg32 & ffsm (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(FFSM_BASE + offset)); }
    volatile Reg32 & sfr  (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(SFR_BASE  + offset)); }

    volatile bool _rx_done() { return (xreg(FSMSTAT1) & FIFOP); }
};


// CC2538 IEEE 802.15.4 EPOSMote III NIC Mediator
class CC2538: public IEEE802_15_4, public IEEE802_15_4::Observed, private CC2538RF
{
    template <int unit> friend void call_init();

private:
    // Transmit and Receive Ring sizes
    static const unsigned int UNITS = Traits<CC2538>::UNITS;
    static const unsigned int TX_BUFS = Traits<CC2538>::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Traits<CC2538>::RECEIVE_BUFFERS;

    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer) + TX_BUFS * sizeof(Buffer);

    static const unsigned int CSMA_CA_MIN_BACKOFF_EXPONENT = 3;
    static const unsigned int CSMA_CA_MAX_BACKOFF_EXPONENT = 5;
    static const unsigned int CSMA_CA_UNIT_BACKOFF_PERIOD = 320; // us
    static const unsigned int CSMA_CA_MAX_TRANSMISSION_TRIALS = 4;

//    typedef CPU::Log_Addr Log_Addr;
//    typedef CPU::Phy_Addr Phy_Addr;
//    static const unsigned int MTU = IEEE802_15_4::MTU;

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
    void address(const Address & address);

    const Statistics & statistics() { return _statistics; }

    void reset();

    void listen();
    void stop_listening();
    unsigned int channel() const { return _channel; }
    void channel(unsigned int c) {
        if((c > 10) && (c < 27)) {
            _channel = c;
            CC2538RF::channel(_channel);
        }
    }

    static CC2538 * get(unsigned int unit = 0) { return get_by_unit(unit); }

private:
    unsigned int _channel;
    void handle_int();

    static void int_handler(const IC::Interrupt_Id & interrupt);

    static CC2538 * get_by_unit(unsigned int unit) {
        if(unit >= UNITS) {
            db<NIC>(WRN) << "NIC::get: requested unit (" << unit << ") does not exist!" << endl;
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

    bool wait_for_ack();

    bool backoff_and_send();

private:
    volatile bool _acked;
    unsigned int _unit;

    Address _address;
    Statistics _statistics;

    IO_Irq _irq;
    DMA_Buffer * _dma_buf;

    int _rx_cur;
    Buffer * _rx_buffer[RX_BUFS];

    static Device _devices[UNITS];
};

__END_SYS

#endif

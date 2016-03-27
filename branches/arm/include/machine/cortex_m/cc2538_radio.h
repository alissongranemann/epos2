// EPOS Cortex-M IEEE 802.15.4 NIC Mediator Declarations

#ifndef __cortex_m_radio_h
#define __cortex_m_radio_h

#include <ic.h>
#include <ieee802_15_4.h>

__BEGIN_SYS

// CC2538 IEEE 802.15.4 RF Transceiver
class CC2538RF
{
protected:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;

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

protected:
    volatile Reg32 & ana (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(ANA_BASE + offset)); }
    volatile Reg32 & xreg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(XREG_BASE + offset)); }
    volatile Reg32 & ffsm (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(FFSM_BASE + offset)); }
    volatile Reg32 & sfr  (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(SFR_BASE  + offset)); }

    volatile bool _rx_done() { return (xreg(FSMSTAT1) & FIFOP); }
};

// Dedicated MAC Timer present in CC2538
class MAC_Timer
{
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;

    const static unsigned int CLOCK = 32 * 1000 * 1000; // 32MHz

    public:
    static unsigned int frequency() { return CLOCK; }

    private:
    enum
    {
        MAC_TIMER_BASE = 0x40088800,
    };

    enum {     //Offset   Description                               Type    Value after reset
        CSPCFG = 0x00, // MAC Timer event configuration              RW     0x0
        CTRL   = 0x04, // MAC Timer control register                 RW     0x2
        IRQM   = 0x08, // MAC Timer interrupt mask                   RW     0x0
        IRQF   = 0x0C, // MAC Timer interrupt flags                  RW     0x0
        MSEL   = 0x10, // MAC Timer multiplex select                 RW     0x0
        M0     = 0x14, // MAC Timer multiplexed register 0           RW     0x0
        M1     = 0x18, // MAC Timer multiplexed register 1           RW     0x0
        MOVF2  = 0x1C, // MAC Timer multiplexed overflow register 2  RW     0x0
        MOVF1  = 0x20, // MAC Timer multiplexed overflow register 1  RW     0x0
        MOVF0  = 0x24, // MAC Timer multiplexed overflow register 0  RW     0x0
    };

    enum CTRL {           //Offset   Description                                                             Type    Value after reset
        CTRL_LATCH_MODE = 1 << 3, // 0: Reading MTM0 with MTMSEL.MTMSEL = 000 latches the high               RW      0
                                  // byte of the timer, making it ready to be read from MTM1. Reading
                                  // MTMOVF0 with MTMSEL.MTMOVFSEL = 000 latches the two
                                  // most-significant bytes of the overflow counter, making it possible to
                                  // read these from MTMOVF1 and MTMOVF2.
                                  // 1: Reading MTM0 with MTMSEL.MTMSEL = 000 latches the high
                                  // byte of the timer and the entire overflow counter at once, making it
                                  // possible to read the values from MTM1, MTMOVF0, MTMOVF1, and MTMOVF2.
        CTRL_STATE      = 1 << 2, // State of MAC Timer                                                      RO      0
                                  // 0: Timer idle
                                  // 1: Timer running
        CTRL_SYNC       = 1 << 1, // 0: Starting and stopping of timer is immediate; that is, synchronous    RW      1
                                  // with clk_rf_32m.
                                  // 1: Starting and stopping of timer occurs at the first positive edge of
                                  // the 32-kHz clock. For more details regarding timer start and stop,
                                  // see Section 22.4.
        CTRL_RUN        = 1 << 0, // Write 1 to start timer, write 0 to stop timer. When read, it returns    RW      0
                                  // the last written value.
    };
    enum MSEL {
        MSEL_MTMOVFSEL = 1 << 4, // See possible values below
        MSEL_MTMSEL    = 1 << 0, // See possible values below
    };
    enum MSEL_MTMOVFSEL {
        OVERFLOW_COUNTER  = 0x00,
        OVERFLOW_CAPTURE  = 0x01,
        OVERFLOW_PERIOD   = 0x02,
        OVERFLOW_COMPARE1 = 0x03,
        OVERFLOW_COMPARE2 = 0x04,
    };
    enum MSEL_MTMSEL {
        TIMER_COUNTER  = 0x00,
        TIMER_CAPTURE  = 0x01,
        TIMER_PERIOD   = 0x02,
        TIMER_COMPARE1 = 0x03,
        TIMER_COMPARE2 = 0x04,
    };

protected:
    static volatile Reg32 & reg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(MAC_TIMER_BASE + offset)); }

public:
    struct Timestamp
    {
        Timestamp() : overflow_count(0), timer_count(0) {}
        Timestamp(unsigned int val) : overflow_count(val >> 16), timer_count(val) {}
        Timestamp(Reg32 of, Reg16 ti) : overflow_count(of), timer_count(ti) {}

        Reg32 overflow_count;
        Reg16 timer_count;

        operator Reg32() { return (overflow_count << 16) + timer_count; }
    } __attribute__((packed));

    static Timestamp read()
    {
        Reg32 index = reg(MSEL);
        reg(MSEL) = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);

        Reg16 timer_count = reg(M0); // M0 must be read first
        timer_count += reg(M1) << 8;

        Reg32 overflow_count = (reg(MOVF2) << 16) + (reg(MOVF1) << 8) + reg(MOVF0);

        reg(MSEL) = index;

        return Timestamp(overflow_count, timer_count);
    }

    static void set(const Timestamp & t)
    {
        bool r = running();
        if(r) stop();
        Reg32 index = reg(MSEL);
        reg(MSEL) = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);

        reg(MOVF0) = t.overflow_count;
        reg(MOVF1) = t.overflow_count >> 8;
        reg(MOVF2) = t.overflow_count >> 16; // MOVF2 must be written last

        reg(M0) = t.timer_count; // M0 must be written first
        reg(M1) = t.timer_count >> 8;

        reg(MSEL) = index;
        if(r) start();
    }

    static void config()
    {
        reg(IRQM) = 0; // Disable interrupts
        stop();
        reg(CTRL) &= ~CTRL_SYNC; // We can't use the sync feature because we want to change
                                 // the count and overflow values when the timer is stopped
        reg(CTRL) |= CTRL_LATCH_MODE; // count and overflow will be latched at once
    }

    static void start() { reg(CTRL) |= CTRL_RUN; }
    static void stop()  { reg(CTRL) &= ~CTRL_RUN; }

    static bool running() { return reg(CTRL) & CTRL_STATE; }
};

// Standalone IEEE 802.15.4 PHY mediator
class CC2538_PHY : protected CC2538RF, public IEEE802_15_4_PHY
{
protected:
    typedef CPU::IO_Irq IO_Irq;

public:
    CC2538_PHY() {
        // Enable clock to the RF CORE module
        Cortex_M_Model::radio_enable();

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

        // Disable counting of MAC overflows
        xreg(CSPT) = 0xff;

        // Clear interrupts
        sfr(RFIRQF0) = 0;
        sfr(RFIRQF1) = 0;

        // Clear error flags
        sfr(RFERRF) = 0;
    }

    void off() { sfr(RFST) = ISRFOFF; }
    void rx() { sfr(RFST) = ISRXON; }
    void tx() { sfr(RFST) = ISTXON; }
    bool cca();//TODO
    void start_cca() { rx_mode(RX_MODE_NO_SYMBOL_SEARCH); rx(); }
    void end_cca() { rx_mode(RX_MODE_NORMAL); }
    bool valid_frame() { return frame_in_rxfifo(); }

protected:
    bool tx_if_cca() { sfr(RFST) = ISTXONCCA; return (xreg(FSMSTAT1) & SAMPLED_CCA); }
    void rx_mode(RX_MODES m) {
        xreg(FRMCTRL0) = (xreg(FRMCTRL0) & ~(3 * RX_MODE)) | (m * RX_MODE);
    }
    unsigned int copy_from_rxfifo(unsigned char * c);
    bool frame_in_rxfifo();
    void clear_rxfifo() { sfr(RFST) = ISFLUSHRX; }
    void frequency(unsigned int freq) { xreg(FREQCTRL) = freq; }
};

// CC2538 IEEE 802.15.4 Radio Mediator
class eMote3_IEEE802_15_4: public IEEE802_15_4, public IEEE802_15_4::Observed, private CC2538_PHY
{
    static const unsigned int TX_BUFS = Traits<eMote3_IEEE802_15_4>::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Traits<eMote3_IEEE802_15_4>::RECEIVE_BUFFERS;

    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer) + TX_BUFS * sizeof(Buffer);

    template <int unit> friend void call_init();

    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef MMU::DMA_Buffer DMA_Buffer;
    static const unsigned int MTU = IEEE802_15_4::MTU;
    typedef IEEE802_15_4::Frame Frame;
    typedef IEEE802_15_4::Header Header;

private:
    // Transmit and Receive Ring sizes
    static const unsigned int UNITS = Traits<eMote3_IEEE802_15_4>::UNITS;

    // Interrupt dispatching binding
    struct Device {
        eMote3_IEEE802_15_4 * device;
        unsigned int interrupt;
    };

protected:
    eMote3_IEEE802_15_4(unsigned int unit, IO_Irq irq, DMA_Buffer * dma_buf);

public:
    typedef IEEE802_15_4::Address MAC_Address;
    typedef MAC_Address Address;

    void listen();
    void stop_listening();
    void channel(unsigned int channel) { 
        if((channel < 11) or (channel > 26)) return;
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
        _channel = channel;
        frequency(11+5*(_channel-11));
    }
    unsigned int channel() { return _channel; }

    ~eMote3_IEEE802_15_4();

    int send(const Address & dst, const Protocol & prot, const void * data, unsigned int size);
    int receive(Address * src, Protocol * prot, void * data, unsigned int size);

    Buffer * alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload);
    void free(Buffer * buf);
    int send(Buffer * buf);

    const Address & address() { return _address; }
    void address(const Address & address);

    const Statistics & statistics() { return _statistics; }

    void reset();

    static eMote3_IEEE802_15_4 * get(unsigned int unit = 0) { return get_by_unit(unit); }

private:
    unsigned int _channel;
    void handle_int();

    static void int_handler(const IC::Interrupt_Id & interrupt);

    static eMote3_IEEE802_15_4 * get_by_unit(unsigned int unit) {
        if(unit >= UNITS) {
            db<eMote3_IEEE802_15_4>(WRN) << "Radio::get: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _devices[unit].device;
    }

    static eMote3_IEEE802_15_4 * get_by_interrupt(unsigned int interrupt) {
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

    /*
    Init_Block * _iblock;
    Phy_Addr  _iblock_phy;
    */

    int _rx_cur;
    /*
    Rx_Desc * _rx_ring;
    Phy_Addr _rx_ring_phy;
    */

    int _tx_cur;
    /*
    Tx_Desc * _tx_ring;
    Phy_Addr _tx_ring_phy;
    */

    Buffer * _rx_buffer[RX_BUFS];
    Buffer * _tx_buffer[TX_BUFS];

    static Device _devices[UNITS];
};

__END_SYS

#endif

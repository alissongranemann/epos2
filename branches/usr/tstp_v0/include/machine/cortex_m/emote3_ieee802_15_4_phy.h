// EPOS Cortex-M IEEE 802.15.4 NIC Mediator Declarations

#ifndef __emote3_ieee802_15_4_phy_h
#define __emote3_ieee802_15_4_phy_h

#include <ic.h>
#include <ieee802_15_4_phy.h>

__BEGIN_SYS

// CC2538 IEEE 802.15.4 RF Transceiver
class CC2538RF
{
    friend class One_Hop_MAC;
    friend class TSTP_MAC;
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

    typedef long long Timestamp;
    typedef Timestamp Microsecond;

    static Timestamp us_to_ts(Microsecond us) { return us * static_cast<Timestamp>(frequency() / 1000000); }
    static Microsecond ts_to_us(Timestamp ts) { return ts / static_cast<Timestamp>((frequency() / 1000000)); }
    static Microsecond read() { return ts_to_us(read_ts()); }
    static void set(Microsecond time) { set_ts(us_to_ts(time)); }

    private:
    enum {
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
    enum {
        INT_OVERFLOW_COMPARE2 = 1 << 5,
        INT_OVERFLOW_COMPARE1 = 1 << 4,
        INT_OVERFLOW_PER      = 1 << 3,
        INT_COMPARE2          = 1 << 2,
        INT_COMPARE1          = 1 << 1,
        INT_PER               = 1 << 0
    };


public:
    static void interrupt(const Microsecond & when, const IC::Interrupt_Handler & h) { interrupt_ts(us_to_ts(when), h); }

    static void interrupt_ts(const Timestamp & when, const IC::Interrupt_Handler & h) {
        int_set(0);
        _user_handler = h;
        reg(MSEL) = (OVERFLOW_COMPARE1 * MSEL_MTMOVFSEL) | (TIMER_COMPARE1 * MSEL_MTMSEL);
        reg(M0) = when;
        reg(M1) = when >> 8;
        reg(MOVF0) = when >> 16;
        reg(MOVF1) = when >> 24;
        reg(MOVF2) = when >> 32;
        _int_overflow_count_overflow = when >> 40ll;

        int_clear();
        Timestamp now = read_ts();
        if(when <= now) {
            int_enable(INT_OVERFLOW_PER);
            _user_handler(49);
        } else if((when >> 16ll) > (now >> 16ll)) {
            int_enable(INT_OVERFLOW_COMPARE1 | INT_OVERFLOW_PER);
        } else if(when > now) {
            int_enable(INT_COMPARE1 | INT_OVERFLOW_PER);
        }
    }

    static Timestamp last_sfd() {
        reg(MSEL) = (TIMER_CAPTURE * MSEL_MTMSEL);

        Timestamp ts = reg(M0); // M0 must be read first
        ts += reg(M1) << 8;
        ts += static_cast<long long>(reg(MOVF2)) << 32ll;
        ts += static_cast<long long>(reg(MOVF1)) << 24ll;
        ts += static_cast<long long>(reg(MOVF0)) << 16ll;
        ts += static_cast<long long>(_overflow_count_overflow) << 40ll;

        return ts_to_us(ts);
    }
    static void int_clear() {
        reg(IRQF) = 0;
    }
    static void int_disable() {
        reg(IRQM) = INT_OVERFLOW_PER;
    }
    static void int_enable(const Reg32 & interrupt) {
        reg(IRQM) |= interrupt;
    }
private:
    static void int_set(const Reg32 & interrupt) {
        reg(IRQM) = interrupt;
    }

private:
    static void interrupt_handler(const unsigned int & interrupt) {
        Reg32 ints = reg(IRQF);
        int_clear();
        if(ints & INT_OVERFLOW_PER) {
            _overflow_count_overflow++;
            if(_int_overflow_count_overflow == _overflow_count_overflow) {
                int_enable(INT_OVERFLOW_COMPARE1);
            }
        }
        if(ints & INT_OVERFLOW_COMPARE1) {
            int_set(INT_COMPARE1 | INT_OVERFLOW_PER);
        } else if(ints & INT_COMPARE1) {
            int_disable();
            _user_handler(interrupt);
        }
    }

protected:
    static IC::Interrupt_Handler _user_handler;

    static volatile Reg32 & reg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(MAC_TIMER_BASE + offset)); }
    
    static Reg32 _overflow_count_overflow;
    static Reg32 _int_overflow_count_overflow;

public:

    static Timestamp read_ts() {
        //Reg32 index = reg(MSEL);
        reg(MSEL) = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);

        Timestamp ts = reg(M0); // M0 must be read first
        ts += reg(M1) << 8;
        ts += static_cast<long long>(reg(MOVF2)) << 32ll;
        ts += static_cast<long long>(reg(MOVF1)) << 24ll;
        ts += static_cast<long long>(reg(MOVF0)) << 16ll;
        ts += static_cast<long long>(_overflow_count_overflow) << 40ll;

        //reg(MSEL) = index;
        return ts;
    }

    static void set_ts(const Timestamp & t) {
        bool r = running();
        if(r) stop();
        //Reg32 index = reg(MSEL);
        reg(MSEL) = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);

        reg(MOVF0) = t >> 16ll;
        reg(MOVF1) = t >> 24ll;
        reg(MOVF2) = t >> 32ll; // MOVF2 must be written last
        _overflow_count_overflow = t >> 40ll;

        reg(M0) = t; // M0 must be written first
        reg(M1) = t >> 8ll;

        //reg(MSEL) = index;
        if(r) start();
    }

    static void config() {
        stop();
        int_set(0);
        int_clear();        
        reg(CTRL) &= ~CTRL_SYNC; // We can't use the sync feature because we want to change
                                 // the count and overflow values when the timer is stopped
        reg(CTRL) |= CTRL_LATCH_MODE; // count and overflow will be latched at once
        IC::int_vector(49, &interrupt_handler);
        IC::enable(33);
        int_enable(INT_OVERFLOW_PER);
    }

    static void start() { reg(CTRL) |= CTRL_RUN; }
    static void stop()  { reg(CTRL) &= ~CTRL_RUN; }

    static bool running() { return reg(CTRL) & CTRL_STATE; }
};

// Standalone IEEE 802.15.4 PHY mediator
class eMote3_IEEE802_15_4_PHY : protected CC2538RF, public IEEE802_15_4_PHY
{
    friend class One_Hop_MAC;
    friend class TSTP_MAC;

protected:
    typedef CPU::IO_Irq IO_Irq;

public:
    typedef IEEE802_15_4_PHY::Frame Frame;

    eMote3_IEEE802_15_4_PHY() {
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
        sfr(RFDATA) = size + sizeof(CRC);
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
    unsigned int copy_from_rxfifo(unsigned char * c);
    bool frame_in_rxfifo();
    void clear_rxfifo() { sfr(RFST) = ISFLUSHRX; }
    void frequency(unsigned int freq) { xreg(FREQCTRL) = freq; }
    void clear_txfifo() { 
        sfr(RFST) = ISFLUSHTX;
        while(xreg(TXFIFOCNT) != 0);
    }
};

__END_SYS

#endif

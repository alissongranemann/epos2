// EPOS Cortex-M IEEE 802.15.4 NIC Mediator Declarations

#ifndef __cortex_m_radio_h
#define __cortex_m_radio_h

#include <tstp_mac.h>
#include <ic.h>
#include <utility/handler.h>

__BEGIN_SYS

// CC2538 TSTP MAC RF Transceiver
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
    typedef TSTP_MAC::Address MAC_Address;

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
        RXMASKCLR = 0x034,
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
        CSPX      = 0x188,
        CSPY      = 0x18C,
        CSPZ      = 0x190,
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
        SNOP      = 0xd0, // No operation
        LABEL     = 0xbb, // Sets next instruction as start of loop
        INCX      = 0xc0, // Increment register X
        INCY      = 0xc1, // Increment register Y
        INCZ      = 0xc2, // Increment register Z
        DECX      = 0xc3, // Decrement register X
        DECY      = 0xc4, // Decrement register Y
        DECZ      = 0xc5, // Decrement register Z

        SRFOFF        = 0xdf, // Disable RX or TX and frequency synthesizer       
        INT           = 0xba, // The interrupt IRQ_CSP_INT is asserted when this instruction executes
        SRXON         = 0xd3, // Enable and calibrate frequency synthesizer for RX
        WAITW         = 0x80, // Wait for W MAC timer overflows
        WEVENT1       = 0xb8, // Wait until MAC timer event 1
        WEVENT2       = 0xb9, // Wait until MAC timer event 2
        SSAMPLECCA    = 0xdb, // Sample the current CCA value to SAMPLED_CCA in xreg(FSMSTAT1)
        SRXMASKBITSET = 0xd4, // Set bit 5 in RXENABLE register
        SRXMASKBITCLR = 0xd5, // Clear bit 5 in RXENABLE register
        ISSTART       = 0xe1, // Start CSP program execution
        ISSTOP        = 0xe2, // Stop CSP program execution
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
    static volatile Reg32 & ana (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(ANA_BASE + offset)); }
    static volatile Reg32 & xreg (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(XREG_BASE + offset)); }
    static volatile Reg32 & ffsm (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(FFSM_BASE + offset)); }
    static volatile Reg32 & sfr  (unsigned int offset) { return *(reinterpret_cast<volatile Reg32*>(SFR_BASE  + offset)); }

};

// Dedicated MAC Timer present in CC2538
class MAC_Timer
{    
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;

    const static unsigned int CLOCK = 32 * 1000 * 1000; // 32MHz
    static const unsigned int IRQ_NUMBER = 33;

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
    enum EVENT {
        EVENT_PERIOD            = 0x00,
        EVENT_COMPARE1          = 0x01,
        EVENT_COMPARE2          = 0x02,
        EVENT_OVERFLOW_PERIOD   = 0x03,
        EVENT_OVERFLOW_COMPARE1 = 0x04,
        EVENT_OVERFLOW_COMPARE2 = 0x05,
      //EVENT_RESERVED          = 0x06,
        EVENT_NO_EVENT          = 0x07,
    };
    enum IRQ {
        OVF_COMPARE2 = 1 << 5,
        OVF_COMPARE1 = 1 << 4,
        OVF_PER      = 1 << 3,
        COMPARE2     = 1 << 2,
        COMPARE1     = 1 << 1,
        PER          = 1 << 0,
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

    static Timestamp us_to_ts(unsigned int us)
    {
        return us * (frequency() / 1000000);
    }


    static const unsigned int N_INTERRUPTS = 6;
    typedef void (* Interrupt_Handler)();
    static Interrupt_Handler _handler[N_INTERRUPTS];

    static void timer_compare(Timestamp when, Interrupt_Handler handler, bool index = 0);
    static void overflow_compare(unsigned int how_many, Interrupt_Handler handler, bool index = 0);

    static void event(unsigned int event_number, EVENT event)
    {
        assert((event_number >= 1) && (event_number <= 2));
        Reg8 aux = reg(CSPCFG) & ~(7 << (4 * (event_number-1)));
        reg(CSPCFG) = aux + (event << (4 * (event_number-1)));
    }

    static Timestamp read()
    {
        const Reg32 desired_index = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);
        Reg32 current_index = reg(MSEL);
        if(current_index != desired_index)
            reg(MSEL) = desired_index;

        Reg16 timer_count = reg(M0); // M0 must be read first
        timer_count += reg(M1) << 8;

        Reg32 overflow_count = (reg(MOVF2) << 16) + (reg(MOVF1) << 8) + reg(MOVF0); 

        if(current_index != desired_index)
            reg(MSEL) = current_index;

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

    static void init();

    static void start() { reg(CTRL) |= CTRL_RUN; }
    static void stop()  { reg(CTRL) &= ~CTRL_RUN; }

    static bool running() { return reg(CTRL) & CTRL_STATE; }

    private:
    static void int_handler(const unsigned int & int_id)
    {
        Reg32 interrupts = reg(IRQF) & reg(IRQM);
        for(unsigned int i=0; i<N_INTERRUPTS; i++)
            if(interrupts & (1 << i))
            {
                reg(IRQM) &= ~(1 << i);
                _handler[i]();
            }
    }
};

// CC2538 TSTP MAC Radio Mediator
class CC2538: public TSTP_MAC, public TSTP_MAC::Observed, private CC2538RF
{
    template <int unit> friend void call_init();

    private:
    // Transmit and Receive Ring sizes
    static const unsigned int UNITS = Traits<CC2538>::UNITS;
    static const unsigned int TX_BUFS = Traits<CC2538>::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Traits<CC2538>::RECEIVE_BUFFERS;

    typedef TSTP_MAC::Address Address;
    typedef CC2538RF::Reg32 Reg32;

    /*
    // Size of the DMA Buffer that will host the ring buffers and the init block
    static const unsigned int DMA_BUFFER_SIZE = ((sizeof(Init_Block) + 15) & ~15U) +
    RX_BUFS * ((sizeof(Rx_Desc) + 15) & ~15U) + TX_BUFS * ((sizeof(Tx_Desc) + 15) & ~15U) +
    RX_BUFS * ((sizeof(Buffer) + 15) & ~15U) + TX_BUFS * ((sizeof(Buffer) + 15) & ~15U); // align128() cannot be used here
    */

    // Interrupt dispatching binding
    struct Device {
        CC2538 * device;
        unsigned int interrupt;
    };

    protected:
    CC2538(unsigned int unit, IO_Irq irq) ;//, DMA_Buffer * dma_buf);

    public:
    void schedule_listen(TSC::Time_Stamp time);
    static void listen(const unsigned int & irq = 0);
    void off();

    static TSC::Time_Stamp woke_up_at;

    void set_channel(unsigned int channel);

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

    static CC2538 * get(unsigned int unit = 0) { return get_by_unit(unit); }

    bool channel_free() { return xreg(FSMSTAT1) & XREG_FSMSTAT1::CCA; }
    CPU::Reg8 rssi() { return xreg(RSSI); }

    private:
    void handle_int();

    void _send_microframe(Microframe & mf);
    int _send_frame(Buffer * buf);

    static void int_handler(const IC::Interrupt_Id & interrupt);
    static void end_of_listen();

    Function_Handler end_of_listen_handler;

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

    // Immediately send a message and wait for it to be correctly sent
    void _send_and_wait() 
    {
        sfr(RFST) = ISTXON; 
        while(!(sfr(RFIRQF1) & INT_TXDONE));
        sfr(RFIRQF1) &= ~INT_TXDONE;
    }

    volatile bool _rx_done() { return (xreg(FSMSTAT1) & FIFOP); }

    // Wait for any ongoing transmission/reception to finish and then disable the receiver    
    static void _soft_rxoff(const unsigned int & irq = 0);// { GPIO led('c',3,GPIO::OUTPUT); led.set(false); xreg(RXMASKCLR) = 0xff; }
private:
    bool copy_from_rxfifo(Buffer * buf);
    unsigned int _unit;

    Address _address;
    Statistics _statistics;

    IO_Irq _irq;
    /*
    DMA_Buffer * _dma_buf;

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

#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <system.h>
#include <utility/buffer.h>
#include <utility/random.h>
#include <mmu.h>
#include <cpu.h>
#include <ic.h>
#include <tstp_api.h>
#include <gpio.h>

__BEGIN_SYS

class TSTP_MAC : public TSTP_API
{
    typedef CPU::Reg32 Reg32;

    friend class TSTP;
    friend class TSTP_NIC;
    typedef Traits<TSTP>::MAC_Config<0, TSTP_MAC> Config;
    typedef Config::PHY_Layer PHY_Layer; // TODO: Polymorphic PHY
    typedef MMU::DMA_Buffer DMA_Buffer;
    typedef CPU::IO_Irq IO_Irq;

    typedef TSTP_API API;

    typedef CPU::Reg16 CRC;
    typedef unsigned short Frame_ID;

public:
    GPIO _tx_pin, _rx_pin;
    TSTP * tstp() { return _tstp; }

    class Buffer : public _UTIL::Buffer<TSTP, TSTP_API::Frame> {
        friend class TSTP_MAC;

        typedef _UTIL::Buffer<TSTP, TSTP_API::Frame> Base;
        typedef Ordered_List<Buffer, TSTP_API::Time> List;
        typedef List::Element Element;

    public:
        Buffer(void * s) : Base(s), _tx_link(this), _id(0) { }
        Buffer(TSTP * t, unsigned int s) : Base(t, s), _tx_link(this), _id(0) { }
        template<typename ... Tn>
        Buffer(TSTP * t, unsigned int s, Tn ... an): Base(t, s, an ...), _tx_link(this), _id(0) {}

        Element * tx_link() { return &_tx_link; }

        void set_id() { _id = (Random::random() & 0x7fff); } // TODO

        void id(TSTP_MAC::Frame_ID i) { _id = i; }
        TSTP_MAC::Frame_ID id() const { return _id; }

    private:
        Element _tx_link;
        TSTP_MAC::Frame_ID _id;
    };

private:
    enum STATE {
        RX_MF,
        RX_DATA,
    };

    typedef Ordered_List<Buffer, Time> TX_Schedule;

    TX_Schedule _tx_ready_schedule;
    TX_Schedule _tx_later_schedule;

    class State {
    public:
        void unschedule();
    protected:
        void schedule(const Time & when, Handler * what);
        void schedule(const Time & when, Handler * what, const Time & period, const Time & end);
        State(TSTP_MAC * mac) : _mac(mac) { }        
        Event _event;
        TSTP_MAC * _mac;
    };

    struct Check_TX_Schedule : public State, public Handler {
        Check_TX_Schedule(TSTP_MAC * mac) : State(mac) { }
        void operator()() { _mac->check_tx_schedule(); }
        void schedule(const Time & when) { State::schedule(when, this); }
    } _check_tx_schedule;

    struct Rx_Mf : public State, public Handler {
        Rx_Mf(TSTP_MAC * mac) : State(mac) { }
        void operator()() { _mac->rx_mf(); }
        void schedule(const Time & when) { State::schedule(when, this); }
    } _rx_mf;

    struct Rx_Data : public State, public Handler {
        Rx_Data(TSTP_MAC * mac) : State(mac) { }
        void operator()() { _mac->rx_data(); }
        void schedule(const Time & when) { State::schedule(when, this); }
    } _rx_data;

    struct Tx : public State, public Handler {
        Tx(TSTP_MAC * mac) : State(mac) { }
        void operator()() { _mac->tx(); }
        void schedule(const Time & when, const Time & period, const Time & end) { State::schedule(when, this, period, end); }
    } _tx;

    struct CCA : public State, public Handler {
        CCA(TSTP_MAC * mac) : State(mac) { }
        void operator()() { _mac->cca(); }
        void schedule(const Time & when) { State::schedule(when, this); }
    } _cca;

    STATE _rx_state;

    static const unsigned int TX_BUFS = Config::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Config::RECEIVE_BUFFERS;
    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer) + TX_BUFS * sizeof(Buffer);

    typedef unsigned char Count;

    class Microframe {
    public:
        Microframe(bool all_listen, const Frame_ID & id, const Count & count, const Address_Hint & hint) : 
            _al_id((id & 0x7fff) | (static_cast<unsigned int>(all_listen) << 15)), _count(count), _hint(hint) {}

        Microframe(bool all_listen, const Frame_ID & id, const Count & count) : 
            _al_id((id & 0x7fff) | (static_cast<unsigned int>(all_listen) << 15)), _count(count), _hint(0) {}

        Count count() const { return _count; }
        Count dec_count() { Count ret = _count--; return ret; }

        Frame_ID id() const { return _al_id & 0x7fff; }
        void id(Frame_ID id) { _al_id  = all_listen() | (id & 0x7fff); }

        void all_listen(bool all_listen) { _al_id = id() | (1 << 15); }
        bool all_listen() const { return _al_id & ~(0x7fff); }

        Address_Hint hint() const { return _hint; }

    private:
        unsigned short _al_id; // all_listen : 1 + id : 15
        Count _count;
        Address_Hint _hint;
        // CRC is handled by the interrupt handler
    }__attribute__((packed));

    bool is_ack(TX_Schedule::Element * el); 

    void bootstrap() { check_tx_schedule(); }
    // Bridge between the hardware-dependent PHY layer and hardware-independent TSTP
    void update(Buffer * buf);

    // == Methods for interacting with the PHY layer ==
    Buffer * _rx_buffer[RX_BUFS];
    Buffer * _tx_buffer[TX_BUFS];
    unsigned int _rx_cur;
    unsigned int _tx_cur;

    template<typename PHY = PHY_Layer>
    static void init(unsigned int unit);

    template<typename PHY = PHY_Layer>
    TSTP_MAC(PHY * phy, DMA_Buffer * dma_buf);

    // Interrupt dispatching binding
    struct MACS {
        TSTP_MAC * mac;
        unsigned int interrupt;
    };

    static const unsigned int UNITS = Traits<NIC>::UNITS;

    static MACS _macs[UNITS];

    static TSTP_MAC * get_by_unit(unsigned int unit) {
        if(unit >= UNITS) {
            db<TSTP_MAC>(WRN) << "TSTP_MAC::get: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _macs[unit].mac;
    }

    static TSTP_MAC * get_by_interrupt(unsigned int interrupt) {
        for(unsigned int i = 0; i < UNITS; i++) {
            if(_macs[i].interrupt == interrupt) {
                return _macs[i].mac;
            }
        }
        return 0;
    }

    template <typename PHY = typename Config::PHY_Layer>
    static void int_handler(const IC::Interrupt_Id & interrupt) {
        TSTP_MAC * mac = get_by_interrupt(interrupt);

        db<TSTP_MAC>(TRC) << "TSTP_MAC::int_handler(int=" << interrupt << ",mac=" << mac << ")" << endl;

        if(!mac)
            db<TSTP_MAC>(WRN) << "TSTP_MAC::int_handler: handler not assigned!" << endl;
        else
            mac->handle_int<PHY>();
    }

    template <typename PHY = PHY_Layer>
    void handle_int();

    template <typename PHY = PHY_Layer>
    void send_mf(Buffer * b);

    template <typename PHY = PHY_Layer>
    void send_frame(Buffer * b);

    // == TSTP -> TSTP_MAC interface ==
    Buffer * alloc(unsigned int size, Frame * f);
    Buffer * alloc(unsigned int size);
    void send(Buffer * b);
    void free(Buffer * b) {
        db<TSTP_MAC>(TRC) << "TSTP_MAC::free(b=" << b << ")" << endl;
        b->unlock();
    }

    PHY_Layer * _phy;
    TSTP * _tstp;

    void prepare_tx_mf();

    // State machine
    void check_tx_schedule();
    void cca();
    void rx_mf();
    void tx();
    void rx_data();

    void process_mf(Buffer * buf);
    void process_data(Buffer * buf);

    Frame_ID _receiving_data_id;
    TX_Schedule::Element * _tx_pending;
    Buffer * _sending_microframe;
    Frame_ID _last_fwd_id;
    Time _last_backoff;
    bool _acking;
    bool _channel_silent;
};

class One_Hop_MAC : public TSTP_API
{
    typedef CPU::Reg32 Reg32;

    friend class TSTP;
    friend class TSTP_NIC;
    typedef Traits<TSTP>::MAC_Config<0, One_Hop_MAC> Config;
    typedef Config::PHY_Layer PHY_Layer; // TODO: Polymorphic PHY
    typedef MMU::DMA_Buffer DMA_Buffer;
    typedef CPU::IO_Irq IO_Irq;

    typedef TSTP_API API;

    typedef CPU::Reg16 CRC;

public:
    typedef _UTIL::Buffer<TSTP, Frame> Buffer;

private:
    static const unsigned int TX_BUFS = Config::SEND_BUFFERS;
    static const unsigned int RX_BUFS = Config::RECEIVE_BUFFERS;
    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer) + TX_BUFS * sizeof(Buffer);

    // Frame to hold PHY packets. Will be cast to either Frame or Microframe
    class MAC_Frame {
    public:
        template<typename T>
        T* data() { return reinterpret_cast<T*>(_data); }
    private:
        unsigned char _data[MTU - sizeof(CRC)];
        CRC _crc;
    }__attribute__((packed));

    class Microframe {
    private:
        unsigned _all_listen : 1;
        unsigned _id : 15;
        unsigned _count : 8;
        Address_Hint _hint;
        // CRC is handled by the interrupt handler
    }__attribute__((packed));

    // Bridge between the hardware-dependent PHY layer and hardware-independent TSTP
    void update(Buffer * buf);

    // == Methods for interacting with the PHY layer ==
    Buffer * _rx_buffer[RX_BUFS];
    Buffer * _tx_buffer[TX_BUFS];
    unsigned int _rx_cur;
    unsigned int _tx_cur;

    template<typename PHY = PHY_Layer>
    static void init(unsigned int unit);

    template<typename PHY = PHY_Layer>
    One_Hop_MAC(PHY * phy, DMA_Buffer * dma_buf);

    // Interrupt dispatching binding
    struct MACS {
        One_Hop_MAC * mac;
        unsigned int interrupt;
    };

    static const unsigned int UNITS = Traits<NIC>::UNITS;

    static MACS _macs[UNITS];

    static One_Hop_MAC * get_by_unit(unsigned int unit) {
        if(unit >= UNITS) {
            db<One_Hop_MAC>(WRN) << "One_Hop_MAC::get: requested unit (" << unit << ") does not exist!" << endl;
            return 0;
        } else
            return _macs[unit].mac;
    }

    static One_Hop_MAC * get_by_interrupt(unsigned int interrupt) {
        for(unsigned int i = 0; i < UNITS; i++) {
            if(_macs[i].interrupt == interrupt) {
                return _macs[i].mac;
            }
        }
        return 0;
    }

    template <typename PHY = typename Config::PHY_Layer>
    static void int_handler(const IC::Interrupt_Id & interrupt) {
        One_Hop_MAC * mac = get_by_interrupt(interrupt);

        db<One_Hop_MAC>(TRC) << "One_Hop_MAC::int_handler(int=" << interrupt << ",mac=" << mac << ")" << endl;

        if(!mac)
            db<One_Hop_MAC>(WRN) << "One_Hop_MAC::int_handler: handler not assigned!" << endl;
        else
            mac->handle_int<PHY>();
    }

    template <typename PHY = PHY_Layer>
    void handle_int();

    template <typename PHY = PHY_Layer>
    void send_mf(Buffer * b);

    template <typename PHY = PHY_Layer>
    void send_frame(Buffer * b);

    // == TSTP -> One_Hop_MAC interface ==
    Buffer * alloc(unsigned int size, Frame * f);
    void send(Buffer * b);
    void free(Buffer * b) {
        b->unlock();
    }

    PHY_Layer * _phy;
    TSTP * _tstp;
};

__END_SYS

#endif

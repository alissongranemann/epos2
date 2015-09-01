// EPOS TSTP MAC Mediator Common Package

#include <nic.h>

#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <ieee802_15_4.h>
#include <cpu.h>
#include <tsc.h>
#include <nic.h>
#include <utility/list.h>
#include <utility/hash.h>
#include <utility/observer.h>
#include <utility/random.h>

__BEGIN_SYS

class TSTP_MAC: private NIC_Common
{
protected:
//    static const unsigned int HEADER_SIZE = 14;

public:
    static const unsigned int MTU = 127;
    // TODO: Geographic address
    typedef IF<Traits<TSTP_MAC>::geographic, void, NIC_Common::Address<2>>::Result Address;
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;    
    typedef CPU::Reg32 Reg32;    
    typedef TSC::Time_Stamp Time_Stamp;

    typedef Reg32 LH_Distance;

    // Frame types
    enum Frame_Type
    {
        BEACON  = 0,
        DATA    = 1,
        ACK     = 2,
        MAC_CMD = 3,
    };
    enum Addressing_Mode
    {
        ADDR_MODE_NOT_PRESENT = 0,
        ADDR_MODE_SHORT_ADDR  = 2,
        ADDR_MODE_EXT_ADDR    = 4,
    };
    enum
    {
        PAN_ID_BROADCAST = 0xffff
    };

    typedef unsigned short Protocol;
    enum
    {
        IP     = 0x0800,
        ARP    = 0x0806,
        RARP   = 0x8035,
        ELP    = 0x8888,
        PTP    = 0x88F7
    };

    typedef unsigned char Data[MTU];
    typedef NIC_Common::CRC16 CRC;

    Address _sink_address;
    Address _address;

    Address sink_address()
    {
        return _sink_address;
    }

    Address address()
    {
        return _address;
    }

    // The IEEE 802.15.4 PHR
    class Phy_Header
    {
    public:
        Phy_Header() {};
        Phy_Header(Reg8 len) : _frame_length(len) {};

        Reg8 frame_length() const { return _frame_length; }
        void frame_length(Reg8 len) { _frame_length = len; }
    
    protected:
        Reg8 _frame_length;        
    } __attribute__((packed, may_alias));

    class Microframe : public Phy_Header
    {
    public:
        Microframe() {}
        Microframe(Reg16 id) : 
        Phy_Header(sizeof(Microframe) - sizeof(Phy_Header)),
        _all_listen((!Traits<TSTP_MAC>::geographic) || (Traits<TSTP_MAC>::geographic && Traits<TSTP_MAC>::is_sink)),
       _id(id),
        _last_hop_distance(Traits<TSTP_MAC>::geographic ? 0 : 0),
        _count(Traits<TSTP_MAC>::n_microframes - 1)
       {};

        Microframe& operator--() { _count--; return *this; }            

        bool all_listen() { return _all_listen; }
        Reg8 count() { return _count; }
        Reg16 id() { return _id; }
        Reg32 last_hop_distance() { return _last_hop_distance; }

    protected:
        unsigned _all_listen : 1;
        unsigned _id : 15;
        LH_Distance _last_hop_distance;
        unsigned _count : 8;
        CRC _crc;
    } __attribute__((packed, may_alias));

    typedef IEEE802_15_4::Frame Frame;
    typedef IEEE802_15_4::Header Header;
    typedef IEEE802_15_4::Buffer Buffer;

    //TODO
//    template <typename Addr = Address>
//    Time_Stamp calculate_backoff(const LH_Distance & d) 
    Time_Stamp calculate_backoff(LH_Distance d = 0) 
    { 
        const auto g = Traits<TSTP_MAC>::g;
        const auto backoff = (Random::random() % ((Traits<TSTP_MAC>::sleep_time / g) + 1)) * g;
        return backoff;
    }

    class TX_Schedule_Entry;

    //Time_Stamp calculate_backoff(Microframe * m)
    //{
    //    //TODO
    //    return TSC::time_stamp();
    //}

    typedef Simple_Hash<TX_Schedule_Entry, Traits<TSTP_MAC>::TX_SCHEDULE_SIZE, Reg16> Hash;

    class TX_Schedule_Entry
    {
    public:
        bool is_new_message() { return _is_new_message; }
        bool is_retransmission() { return !is_new_message(); }

        Time_Stamp timeout() { return _timeout; }
        void timeout(const Time_Stamp & tmt) { _timeout = tmt; }

        void link(Hash::Element * l) { _link = l; }
        Reg16 id() { return _id; }

        Time_Stamp backoff() { return _backoff; }

        Buffer * payload() { return _payload; }

        // Constructor for new messages
        TX_Schedule_Entry(Buffer * pld, Address dest, Time_Stamp tmt = 0) 
            : _is_new_message(true),
              _timeout(tmt), 
              _destination(dest), 
              _id(Random::random()), 
              _payload(pld),
              _link(0)              
        { }

        // Constructor for retransmissions
        TX_Schedule_Entry(Microframe * m, TSTP_MAC * mac, Buffer * b)
            : _is_new_message(false),
              _backoff(mac->calculate_backoff(m->last_hop_distance())), 
              _timeout(TSC::time_stamp()), 
              _destination(b->frame()->header()->dst()), 
              _id(m->id()),
              _payload(b),
              _link(0)
        { }

        ~TX_Schedule_Entry()
        {
            //TODO
            //if(_payload)
            //    NIC::free(_payload);
            if(_link)
                delete _link;
        }

    private:
        bool _is_new_message;
        Time_Stamp _backoff;
        Time_Stamp _timeout;
        Address _destination;
        Reg16 _id;
        Buffer * _payload;
        Hash::Element * _link;
    };

    class TX_Schedule : private Hash
    {
    public:
        TX_Schedule(TSTP_MAC * t) : Hash(), _tstp_mac(t) {}
        ~TX_Schedule()
        {
            for(auto i = Hash::begin(); i < Hash::end(); i++)
            {
                delete (*i).object();
                delete &(*i);
            }
        }

        typedef Hash::Element Element;

        TX_Schedule_Entry * choose()
        {
            auto now = TSC::time_stamp();
            Iterator i = Hash::begin();
            TX_Schedule_Entry * ret = 0;
            for(i++; i != Hash::end(); i++)
            {
                TX_Schedule_Entry * obj = i->object();
                db<TSTP_MAC>(TRC) << "obj = " << obj << endl;
                db<TSTP_MAC>(TRC) << "now  " << now << endl;
                db<TSTP_MAC>(TRC) << "timeout  " << obj->timeout() << endl;
                if(obj->timeout() <= now)
                {
                    if(ret)
                    {
                        if(ret->is_retransmission())
                        {
                            if(obj->is_new_message())
                                continue;
                            else if(obj->timeout() >= ret->timeout())
                                continue;
                        }
                        else
                        {
                            if(obj->is_new_message() && (obj->timeout() >= ret->timeout()))
                                continue;
                        }
                    }
                    ret = obj;
                }
            }
            return ret;
        }

        void schedule_new(Buffer * pld, Address dest, Time_Stamp tmt = 0)
        {
            TX_Schedule_Entry * entry = new TX_Schedule_Entry(pld, dest, tmt);
            Element * el = new Element(entry, entry->id());
            Hash::insert(el);
        }

        void schedule_forwarding(Microframe * m, Buffer * b)
        {
            TX_Schedule_Entry * entry = new TX_Schedule_Entry(m, _tstp_mac, b);
            Element * el = new Element(entry, entry->id());
            Hash::insert(el);
        }

        bool remove(Reg16 id)
        {
            auto ret = Hash::remove_key(id);
            if(ret)
            {
                delete ret->object();
                delete ret;
                return true;
            }
            return false;
        }
    private:
        TSTP_MAC * _tstp_mac;
    };

    TX_Schedule _tx_schedule;
    TX_Schedule_Entry * _currently_sending;
    Microframe _currently_receiving;

public:
    enum STATE 
    {
        UPDATE_TX_SCHEDULE,
        SLEEP_S,
        RX_MF_RX,
        SLEEP_UNTIL_DATA,
        RX_DATA,        
        BACKOFF,
        RX_MF_TX,
        TX_MF,
        TX_DATA,
    };

private:
    STATE _state;
    void state(STATE s) 
    { 
        _state = s;
        db<TSTP_MAC>(TRC) << "TSTP_MAC::state(" << s << ")" << endl;
    }
public:
    STATE state() { return _state; }

protected:
    // Should be called only by the radio after waking up from a scheduled listen
    void next_state_after_wakeup()
    {
        //const auto s = state();
        //if((s == SLEEP_S) || (s == BACKOFF) || (s == SLEEP_UNTIL_DATA))
            state(static_cast<STATE>(static_cast<int>(state()) + 1));
    }

    void next_state()
    {
        //const auto s = state();
        //if((s == SLEEP_S) || (s == BACKOFF) || (s == SLEEP_UNTIL_DATA))
        if(state() == TX_DATA)
            _update_tx_schedule();
        else if(state() == TX_MF)
        {
            if(_currently_sending->is_new_message() || (!is_for_me(_currently_sending->payload())))
                state(TX_DATA);
            else
            {
                _tx_schedule.remove(_currently_sending->id());
                _currently_sending = 0;
                _update_tx_schedule();
            }
        }
        else
            state(static_cast<STATE>(static_cast<int>(state()) + 1));
    }

    void listen_timeout(bool channel_free)
    {
        db<TSTP_MAC>(TRC) << "listen_timeout " << channel_free << endl;
        if((state() == RX_MF_TX) && channel_free)
        {
            state(TX_MF);
        }
        // TODO: check if there is no other action necessary when on RX_DATA and timeout occurs
        else
        {
            _update_tx_schedule();
        }
        /*
        if(state() == RX_MF_RX)
        {
            _update_tx_schedule();
        }
        else if(state() == RX_MF_TX)
        {
            if(channel_free)
                state(TX_MF);
            else                
                _update_tx_schedule();
        }
        */
    }

    void _update_tx_schedule(TSC::Time_Stamp offset = 0)
    {
        TSC::Time_Stamp wake_up_time = TSC::time_stamp();
        state(UPDATE_TX_SCHEDULE);        
        db<TSTP_MAC>(TRC) << "_update_tx_schedule now = " << wake_up_time << " offset = " << offset << endl;

        if(_currently_sending)
            _currently_sending->timeout(wake_up_time + TSC::us_to_ts(Traits<TSTP_MAC>::timeout_for_retransmission));
        _currently_sending = _tx_schedule.choose();

        if(_currently_sending)
        {
            db<TSTP_MAC>(TRC) << "currently sending ";
            if(_currently_sending->is_new_message())
            {
                db<TSTP_MAC>(TRC) << "a new message" << endl;
                const auto g = Traits<TSTP_MAC>::g;
                wake_up_time += TSC::us_to_ts((Random::random() % ((Traits<TSTP_MAC>::sleep_time / g) + 1)) * g);
            }
            else
            {
                db<TSTP_MAC>(TRC) << "a retransmission" << endl;
                wake_up_time += TSC::us_to_ts(_currently_sending->backoff());
            }
            state(BACKOFF);
        }
        else
        {
            db<TSTP_MAC>(TRC) << "Sleeping S" << endl;
            wake_up_time += TSC::us_to_ts(Traits<TSTP_MAC>::sleep_time) + offset;
            state(SLEEP_S);
        }

        db<TSTP_MAC>(TRC) << "_update_tx_schedule wake_up_time = " << wake_up_time << endl;
        schedule_listen(wake_up_time);
    }

public:
    volatile bool waiting_for_microframe() { return (state() == RX_MF_RX) || (state() == RX_MF_TX); }
    volatile bool waiting_for_data() { return state() == RX_DATA; }

    /*
    void _sleep_s()
    {
        const auto ti = Traits<TSTP_MAC>::time_between_microframes;
        const auto ts = Traits<TSTP_MAC>::ts;
        auto wake_up_time = ti + m.count() * (ti + ts) - eMote3::PM2_EXIT_TIME;
        eMote3_TSC::wake_up_at(wake_up_time, &state_machine);
    }

    void state_machine()
    {
        switch(state())
        {
            case UPDATE_TX_SCHEDULE: _update_tx_schedule(); break;
            case SLEEP_S:            _sleep_s();            break;
            case RX_MF_RX:           _rx_mf_rx();           break;
            case SLEEP_UNTIL_DATA:   _sleep_until_data();   break;
            case RX_DATA:            _rx_data();            break;
            case BACKOFF:            _backoff();            break;
            case RX_MF_TX:           _rx_mf_tx();           break;
            case TX_MF:              _tx_mf();              break;
            case TX_DATA:            _tx_data();            break;
            default: break;
        }
    }
    */

    virtual void schedule_listen(TSC::Time_Stamp time) = 0;

    bool is_for_me(Buffer * b)
    {
        auto dst = b->frame()->header()->dst();
        return (dst == broadcast()) || (dst == address());
    }

    bool process_frame(Buffer * b)
    {
        _tx_schedule.schedule_forwarding(&_currently_receiving, b);
        return is_for_me(b);
    }

    void process_microframe(Microframe * m)
    {
        db<TSTP_MAC>(TRC) << "=============================Processing Microframe" << endl;
        auto removed = _tx_schedule.remove(m->id());

        auto now = TSC::time_stamp();
        const auto ti = Traits<TSTP_MAC>::time_between_microframes;
        const auto ts = Traits<TSTP_MAC>::ts;
        auto wake_up_time = ti + m->count() * (ti + ts);

        if(removed)
        {
            db<TSTP_MAC>(TRC) << "Removed id " << m->id() << " from schedule" << endl;
            _update_tx_schedule(TSC::us_to_ts(wake_up_time));
        }
        else
        {
            db<TSTP_MAC>(TRC) << "Didn't remove id " << m->id() << " from schedule" << endl;
            if((state() == RX_MF_RX) && 
                    (m->all_listen() ))//|| ) // TODO: acceptance conditions
            {
                _currently_receiving = *m;
                db<TSTP_MAC>(TRC) << "I'm interested in this frame!" << endl;
                state(SLEEP_UNTIL_DATA);
                schedule_listen(now + TSC::us_to_ts(wake_up_time - Traits<TSTP_MAC>::data_listen_margin));
            }
            else
            {
                _update_tx_schedule();
            }
        }
    }


    //class Frame: public Phy_Header
    //{
    //public:
    //    Frame() {}
    //    Frame(const void * data, Reg8 size) : Phy_Header(size+sizeof(CRC))
    //    {
    //        memcpy(_data, data, size);
    //    }

    //    Phy_Header * header() { return this; }

    //    template<typename T>
    //    T * data() { return reinterpret_cast<T *>(&_data); }

    //    friend Debug & operator<<(Debug & db, const Frame & f) {
    //        db << "{" << f._data << "}";
    //        return db;
    //    }
    //    
    //protected:
    //    Data _data;
    //} __attribute__((packed, may_alias));

    typedef Frame PDU;

    // Buffers used to hold frames across a zero-copy network stack
    //class Buffer: private Frame
    //{
    //public:
    //    typedef Simple_List<Buffer> List;
    //    typedef List::Element Element;

    //public:
    //    Buffer(void * back): _lock(false), _nic(0), _back(back), _size(sizeof(Frame)), _link(this) {}

    //    Frame * frame() { return this; }

    //    bool lock() { return !CPU::tsl(_lock); }
    //    void unlock() { _lock = 0; }

    //    NIC * nic() const { return _nic; }
    //    void nic(NIC * n) { _nic = n; }

    //    template<typename T>
    //    T * back() const { return reinterpret_cast<T *>(_back); }

    //    unsigned int size() const { return _size; }
    //    void size(unsigned int s) { _size = s; }

    //    Element * link() { return &_link; }

    //    friend Debug & operator<<(Debug & db, const Buffer & b) {
    //        db << "{nc=" << b._nic << ",lk=" << b._lock << ",sz=" << b._size << ",bl=" << b._back << "}";
    //        return db;
    //    }

    //private:
    //    volatile bool _lock;
    //    NIC * _nic;
    //    void * _back;
    //    unsigned int _size;
    //    Element _link;
    //};
//    } __attribute__((packed, may_alias));


public:
    // Observers of a protocol get a also a pointer to the received buffer
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;


    // Meaningful statistics for Ethernet
    struct Statistics: public NIC_Common::Statistics
    {
        Statistics(): rx_overruns(0), tx_overruns(0), frame_errors(0), carrier_errors(0), collisions(0) {}

        friend Debug & operator<<(Debug & db, const Statistics & s) {
            db << "{rxp=" << s.rx_packets
               << ",rxb=" <<  s.rx_bytes
               << ",rxorun=" <<  s.rx_overruns
               << ",txp=" <<  s.tx_packets
               << ",txb=" <<  s.tx_bytes
               << ",txorun=" <<  s.tx_overruns
               << ",frm=" <<  s.frame_errors
               << ",car=" <<  s.carrier_errors
               << ",col=" <<  s.collisions
               << "}";
            return db;
        }
        
        unsigned int rx_overruns;
        unsigned int tx_overruns;
        unsigned int frame_errors;
        unsigned int carrier_errors;
        unsigned int collisions;
    };

/*
    virtual void listen() = 0;
    virtual void stop_listening() = 0;
    */

protected:
    TSTP_MAC() : _tx_schedule(this), _currently_sending(0), _state(SLEEP_S) { }

public:
    static const unsigned int mtu() { return MTU; }
    static const Address broadcast() { return Address::BROADCAST; }

private:
};

__END_SYS

#endif

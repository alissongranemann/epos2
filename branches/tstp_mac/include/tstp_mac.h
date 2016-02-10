#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <nic.h>
#include <units.h>

#include <utility/list.h>
#include <utility/math.h>
#include <cpu.h>
#include <ieee802_15_4.h>
#include <timer.h>

__BEGIN_SYS

class TSTP_MAC : public NIC_Common, public TSTP_Common
{
public:
    typedef TSTP_Common::Address Address;
    typedef TSTP_Common::Statistics Statistics;
    typedef TSTP_Common::Interest Interest;

    static const Statistics & statistics() { return _statistics; }

    static void init();

    typedef CPU::Reg8 Reg8;

    static void address(const Address & a) { _address = a; }
    static Address address() { return _address; }
    static Address sink_address() { return _sink_address; }

    typedef IEEE802_15_4::Phy_Header Phy_Header;

    static Time time_now();

    // TSTP MAC Microframe
    class Microframe
    {
    public:
        Microframe(bool all_listen, Distance distance, Message_ID id, unsigned int count = Traits<TSTP_MAC>::N_MICROFRAMES - 1)
            : _all_listen(all_listen), _count(count), _last_hop_distance(distance), _id(id) {}

        unsigned int id() { return _id; }
        unsigned int count() { return _count; }
        Distance last_hop_distance() { return _last_hop_distance; }
        bool all_listen() { return _all_listen; }

        void operator--() { _count--; }

        friend Debug & operator<<(Debug & db, const Microframe & m) {
            db << "{all=" << m._all_listen << " ,c=" << m._count << " ,lhd=" << m._last_hop_distance  << " ,id=" << m._id  << "}";
            return db;
        }

    private:
    public: //TODO:REMOVE
        unsigned _all_listen : 1;
        unsigned _count : 8;
        unsigned _last_hop_distance : 32;
        unsigned _id : 15;
    } __attribute__((packed, may_alias));

    class Header
    {
    public:
        Header() {}
        Header(Message_Type t) : _message_type(t), _origin_address(TSTP_MAC::address())  {};

        Message_Type message_type() const { return static_cast<Message_Type>(_message_type); }
        Address last_hop_address() const { return _last_hop_address; }
        void last_hop_address(const Address & addr) { _last_hop_address = addr; }
        Time last_hop_time() const { return _last_hop_time; }
        void last_hop_time(const Time & t) { _last_hop_time = t; }
        Address origin_address() const { return _origin_address; }

    private:
        unsigned _message_type : 3;
        unsigned _time_request : 1;
        unsigned _spatial_scale : 2;
        unsigned _temporal_scale : 2;
        unsigned _location_confidence : 8;
        Address _last_hop_address;
        Time _last_hop_time;
        Address _origin_address;
        Time _origin_time;
    } __attribute__((packed, may_alias));

    typedef NIC_Common::CRC16 CRC;
    static const unsigned int MTU = 127 - sizeof(CRC);

    typedef unsigned char Data[MTU];

    class Interest_Message : private Header, public Interest
    {
    public:
        Interest_Message(const Interest & body) : 
            Header(INTEREST),
            Interest(body) { }

        Interest_Message(const Region & region, const Time & t0, const Time & dt, const Time & period, const Unit & unit, const unsigned int & precision, const RESPONSE_MODE & response_mode) : 
            Header(INTEREST),
            Interest(region, t0, dt, period, unit, precision, response_mode) { }

        Header * header() { return this; }

        Interest * interest() { 
            auto raw = reinterpret_cast<char *>(this);
            return reinterpret_cast<Interest*>(&(raw[sizeof(Header)]));
        }

    } __attribute__((packed, may_alias));

    // The IEEE802_15_4 Frame which encapsulates TSTP data
    class Frame: private Phy_Header
    {
    public:
        Frame() {}
        Frame(Reg8 data_size) : Phy_Header(data_size + sizeof(CRC)) { }
        Frame(const void * data, Reg8 data_size) : Phy_Header(data_size + sizeof(CRC)) {
            memcpy(_data, data, data_size);
        }

        Phy_Header * header() { return this; }
        const Phy_Header * header() const { return this; }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        template<typename T>
        const T * data() const { return reinterpret_cast<const T *>(&_data); }

        const CRC & crc () const {
            auto raw = data<char>();
            auto crc_offset = header()->frame_length() - sizeof(CRC);
            return *(reinterpret_cast<const CRC *>(&raw[crc_offset]));
        }

        void crc(const CRC & c) {
            auto raw = data<char>();
            auto crc_offset = header()->frame_length() - sizeof(CRC);
            reinterpret_cast<CRC &>(raw[crc_offset]) = c;
        }

        friend Debug & operator<<(Debug & db, const Frame & f) {
            db << "{" << f._data << "}";
            return db;
        }

    protected:
        Data _data;
        CRC _crc;
    } __attribute__((packed, may_alias));


    static bool send(const Interest * interest);

    typedef Frame PDU;

    // Buffers used to hold frames across a zero-copy network stack
    class Buffer: private Frame
    {
    public:
        typedef Simple_List<Buffer> List;
        typedef List::Element Element;

    public:
        Buffer(void * back): _lock(false), _nic(0), _back(back), _size(sizeof(Frame)), _link(this) {}
        Buffer(NIC * nic, unsigned int size, bool lock = false):
            Frame(size), _lock(lock), _nic(nic), _size(size), _link(this) {}

        Frame * frame() { return this; }

        template <typename T>
        T * raw() { return reinterpret_cast<T *>(this); }

        bool lock() { return !CPU::tsl(_lock); }
        void unlock() { _lock = 0; }

        NIC * nic() const { return _nic; }
        void nic(NIC * n) { _nic = n; }

        template<typename T>
        T * back() const { return reinterpret_cast<T *>(_back); }

        // For the upper layers, size will represent the size of frame->data<T>()
        unsigned int size() const { return _size; }
        void size(unsigned int s) { _size = s; }

        Element * link() { return &_link; }

        friend Debug & operator<<(Debug & db, const Buffer & b) {
            db << "{nc=" << b._nic << ",lk=" << b._lock << ",sz=" << b._size << ",bl=" << b._back << "}";
            return db;
        }

    private:
        volatile bool _lock;
        NIC * _nic;
        void * _back;
        unsigned int _size;
        Element _link;
    };

private:
    static Message_ID message_hash(const char * raw, unsigned int size) {
        Message_ID id = 0;
        unsigned int i;
        for(i = 0; (i+sizeof(Message_ID)) <= size; i += sizeof(Message_ID)) {
            id ^= *(reinterpret_cast<const Message_ID *>(&(raw[i])));
        }
        Message_ID rest = 0;
        for(; i < size; i++) {
            rest <<= sizeof(char) * 8;
            rest += raw[i];
        }
        id ^= rest;
        id &= ~(0x8000);
        return id ^ rest;
    }

public:
    template<typename T>
    static Message_ID id(T * t) {
        return message_hash(reinterpret_cast<const char *>(t), sizeof(T));
    }
    // Specialization below:
    // template<>
    // Message_ID id(Interest_Message * i);

private:
    class TX_Schedule 
    {
    public:
        TX_Schedule() : _n_entries(0) {}

        class TX_Schedule_Entry 
        {
        public:
            TX_Schedule_Entry() {}

            TX_Schedule_Entry(bool is_new, Message_ID id, Time transmit_at, Time backoff, Address destination, Buffer * buffer) : 
                _new(is_new), _id(id), _transmit_at(transmit_at), 
                _backoff(backoff), _dst(destination), _trials(0), 
                _buffer(buffer) 
            { }

            Message_ID id() { return _id; }
            Time transmit_at() { return _transmit_at; }
            void transmit_at(Time t) { _transmit_at = t; }
            Buffer * buffer() { return _buffer; }
            Time backoff() { return _backoff; }
            const Address & destination() { return _dst; }

            void free(); 

            void operator++() { _trials++; }
            unsigned int trials() { return _trials; }

        private:
            bool _new;
            Message_ID _id;
            Time _transmit_at;
            Time _backoff;
            Address _dst;
            unsigned int _trials;
            Buffer * _buffer;
        };

        TX_Schedule_Entry * tx_pending(const Time & time) {
            unsigned int min_i = -1u;
            auto min = time;

            for(auto i = 0u; i < _n_entries; i++) {
                if(_table[i].transmit_at() <= time) {
                    if(_table[i].trials() >= Traits<TSTP_MAC>::MAX_SEND_TRIALS) {
                        remove_by_index(i);
                        i--;
                    } else {
                        if(_table[i].transmit_at() < min) {
                            min = _table[i].transmit_at();
                            min_i = i;
                        }
                    }
                }
            }
            if(min_i == -1u) {
                return 0;
            } else {
                ++_table[min_i];
                return &_table[min_i];
            }
        }

        bool remove(Message_ID id) {
            for(auto i = 0u; i < _n_entries; i++) {
                if(_table[i].id() == id) {
                    _table[i].free();
                    if(i < _n_entries - 1) {
                        _table[i] = _table[_n_entries - 1];
                    }
                    _n_entries--;
                    return true;
                }
            }
            return false;
        }

        bool insert(bool is_new, Message_ID id, Time transmit_at, Time backoff, Address destination, Buffer * buffer) {
            db<TSTP_MAC>(TRC) << "TSTP_MAC::TX_Schedule::insert(new=" << is_new << ",id=" << id << ",at=" << transmit_at << ",bkf=" << backoff << ",dst=" << destination << ",buf=" << buffer << endl;

            if(_n_entries < Traits<TSTP_MAC>::TX_SCHEDULE_SIZE - 1) {
                new (&_table[_n_entries]) TX_Schedule_Entry(is_new, id, transmit_at, backoff, destination, buffer);
                db<TSTP_MAC>(TRC) << " => " << hex << &_table[_n_entries] << endl;
                _n_entries++;
                return true;
            }
            return false;
        }

        void update_timeout(TX_Schedule_Entry * e, Time timeout) {
            e->transmit_at(timeout);
        }

    private:
        void remove_by_index(unsigned int idx) {
            for(auto i = idx; i < _n_entries; i++) {
                _table[i].free();
                if(i < _n_entries - 1) {
                    _table[i] = _table[_n_entries - 1];
                }
                _n_entries--;
                return;
            }
        }

        TX_Schedule_Entry _table[Traits<TSTP_MAC>::TX_SCHEDULE_SIZE];
        unsigned int _n_entries;
    };

    typedef void (Timer_Handler)(const unsigned int & int_id);

    static void check_tx_schedule(const unsigned int & int_id = 0);
    static void timeout(Time time, const Timer_Handler & handler);
    static void clear_timeout();
    static void cca(const unsigned int & int_id = 0);
    static void prepare_tx_mf();
    static void tx_mf(const unsigned int & int_id = 0);
    static void rx_mf(const unsigned int & int_id = 0);
    static void tx_data(const unsigned int & int_id = 0);
    static void rx_data(const unsigned int & int_id = 0);
    static void process_mf(Buffer * b);
    static void process_data(Interest_Message * interest);
    static void parse_data(Buffer * b);
    static Microframe * to_microframe(Buffer * b);
    static bool relevant(Microframe * mf);
    static bool should_forward(Interest_Message * i);
    static Interest_Message * to_interest(Frame * f);

    static bool all_listen(Frame * f);
    static bool is_ack(TX_Schedule::TX_Schedule_Entry * e);
    static Time time_until_data(Microframe * mf);
    static Time backoff(const Address & destination, const Distance & last_hop_distance);
    static Time backoff();

    static Buffer * _tx_pending_mf_buffer;
    static Microframe * _tx_pending_mf;
    static TX_Schedule _tx_schedule;
    static TX_Schedule::TX_Schedule_Entry * _tx_pending_data;
    static NIC _radio;
    static Traits<TSTP_MAC>::Timer _timer;
    static Message_ID _receiving_data_id;
    static Address _address;
    static Address _sink_address;
    static Statistics _statistics;
};

template<>
inline TSTP_MAC::Message_ID TSTP_MAC::id(Interest_Message * i) {
    if(i->response_mode() == RESPONSE_MODE::SINGLE) {
        auto ret = id(i->interest());
        return ret + ((time_now() - i->t0()) / i->period());
    } else {
        return message_hash(reinterpret_cast<const char *>(i), sizeof(Interest_Message));
    }
}

__END_SYS

#endif

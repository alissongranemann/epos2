#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <nic.h>

#include <utility/list.h>
#include <utility/math.h>
#include <cpu.h>
#include <timer.h>

__BEGIN_SYS

class TSTP_MAC : public NIC_Common
{
public:
    static void init();

    typedef CPU::Reg8 Reg8;

    typedef short Message_ID;
    typedef int Microsecond;    
    typedef Microsecond Time;
    typedef int Distance;
    typedef int Unit;

    enum Message_Type {
        INTEREST = 0,
        DATA = 1,
        REPORT = 2,
        // 3 is unused
        BOOTSTRAP_0 = 4,
        BOOTSTRAP_1 = 5,
        BOOTSTRAP_2 = 6,
        BOOTSTRAP_3 = 7,
    };

    class Address 
    {
    public:
        int x, y, z;
        Address(int xi = 0, int yi = 0, int zi = 0) :x(xi), y(yi), z(zi) { }

        friend Debug & operator<<(Debug & db, const Address & a) {
            db << "{" << a.x << "," << a.y << "," << a.z << "}";
            return db;
        }

        operator int() const { return x; }

        bool operator==(const Address & rhs) {
            return x == rhs.x and y == rhs.y and z == rhs.z;
        }

        Distance operator-(const Address & rhs) const {
            int xx = rhs.x - x;
            int yy = rhs.y - y;
            int zz = rhs.z - z;
            //return Math::sqrt(xx*xx + yy*yy + zz*zz);
            return Math::abs(xx + yy + zz);
        }
    }__attribute__((packed, may_alias));

    static void address(const Address & a) { _address = a; }
    static Address address() { return _address; }

    // The IEEE 802.15.4 PHR
    class Phy_Header
    {
    public:
        Phy_Header() { }
        Phy_Header(Reg8 len) : _frame_length((len & (~0x7f)) ? 0x7f : len) { }

        Reg8 frame_length() const { return _frame_length; }
        void frame_length(Reg8 len) { _frame_length = ((len & (~0x7f)) ? 0x7f : len); }

    protected:
        Reg8 _frame_length;
    } __attribute__((packed, may_alias));

    // TSTP MAC Microframe
    class Microframe : private Phy_Header
    {
    public:
        Microframe(bool all_listen, unsigned int count, Distance distance, Message_ID id) 
            : Phy_Header(sizeof(Microframe)),
            _all_listen(all_listen), _count(count), _last_hop_distance(distance), _id(id)            {}

        unsigned int id() { return _id; }
        unsigned int count() { return _count; }
        Distance last_hop_distance() { return _last_hop_distance; }
        bool all_listen() { return _all_listen; }

        void operator--() { _count--; }

        friend Debug & operator<<(Debug & db, const Microframe & m) {
            db << "{all=" << m._all_listen << " ,c=" << m._count << " ,lhd=" << m._last_hop_distance  << " ,id=" << m._id  << " ,crc" << m._CRC << "}";
            return db;
        }

    private:
    public: //TODO:REMOVE
        unsigned _all_listen : 1;
        unsigned _count : 8;
        unsigned _last_hop_distance : 32;
        unsigned _id : 15;
        unsigned _CRC : 16;
    } __attribute__((packed, may_alias));

    class Payload_Header : public Phy_Header
    {
    public:
        Payload_Header() {}
        Payload_Header(Reg8 size) : Phy_Header(size) { }
        Payload_Header(Message_Type t, Reg8 size) : Phy_Header(size+sizeof(Payload_Header)), _message_type(t), _origin_address(TSTP_MAC::address())  {};

        Message_Type message_type() { return static_cast<Message_Type>(_message_type); }
        Address last_hop_address() { return _last_hop_address; }
        void last_hop_address(const Address & addr) { _last_hop_address = addr; }
        Time last_hop_time() { return _last_hop_time; }
        void last_hop_time(const Time & t) { _last_hop_time = t; }
        Address origin_address() { return _origin_address; }

    protected:
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

    static const unsigned int MTU = 127;
    typedef NIC_Common::CRC16 CRC;

    typedef unsigned char Data[MTU];

    class Interest : private Payload_Header
    {
    public:
        Interest(Address region, Time t0, Time dt, Time period, Unit unit, unsigned int precision, bool response_mode) : 
            Payload_Header(INTEREST, sizeof(Interest)),
            _region(region), _t0(t0), _dt(dt),
            _period(period), _unit(unit), _precision(precision), 
            _response_mode(response_mode) { }

        Address destination() const { return _region; }
        Address region() const { return destination(); }
        Time t0() const { return _t0; }
        Time period() const { return _period; }

        Payload_Header * header() { return this; }

        friend Debug & operator<<(Debug & db, const Interest & i) {
            db << "{region=" << i._region << ",t0=" << i._t0 << ",dt=" << i._dt << ",p=" << i._period << ",u=" << i._unit << ",pr=" << i._precision << ",rm=" << i._response_mode << "}";
            return db;
        }

    private:
    public: //TODO: REMOVE
        Address _region;
        Time _t0;
        Time _dt;
        Time _period;
        Unit _unit;
        unsigned _precision : 7;
        unsigned _response_mode : 1;
    } __attribute__((packed, may_alias));

    class Payload : private Payload_Header
    {
    public:
        Payload_Header * header() { return this; }

        Address destination() 
        {
            if(_message_type == INTEREST) {
                return reinterpret_cast<Interest*>(this)->destination();
            }
            else {
                return _sink_address;
            }
        }
    };

    // The TSTP Frame
    class Frame: private Phy_Header
    {
    public:
        Frame() {}
        Frame(Reg8 size) : Phy_Header(size) { } //+sizeof(CRC)) { }
        Frame(const void * data, Reg8 size) : Phy_Header(size) { //+sizeof(CRC)) {
            memcpy(_data, data, size);
        }

        Phy_Header * header() { return this; }

        Microframe * microframe() { return reinterpret_cast<Microframe *>(this); }

        Payload * payload() { return reinterpret_cast<Payload *>(this); }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Frame & f) {
            db << "{" << f._data << "}";
            return db;
        }

    protected:
        Data _data;
        //CRC _crc;
    } __attribute__((packed, may_alias));

    static Message_ID id(const Interest * i)
    {
        auto raw = reinterpret_cast<const char *>(i);
        unsigned int id = 0u;
        for(auto j = 0u; j < sizeof(Interest); j++)
            id ^= raw[j];
        id += (time_now() - i->t0()) / i->period();
        return id;
    }

    static void send(const Interest * interest);

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
    class TX_Schedule 
    {
    public:
        TX_Schedule() : _n_entries(0) { }

        class TX_Schedule_Entry 
        {
        public:
            TX_Schedule_Entry() {}

            TX_Schedule_Entry(bool is_new, Message_ID id, Time transmit_at, Time backoff, Address destination, Buffer * payload) : 
                _new(is_new), _id(id), _transmit_at(transmit_at), 
                _backoff(backoff), _dst(destination), _trials(0), 
                _payload(payload) 
            { }

            Message_ID id() { return _id; }
            Time transmit_at() { return _transmit_at; }
            void transmit_at(Time t) { _transmit_at = t; }
            Buffer * payload() { return _payload; }
            Time backoff() { return _backoff; }
            Address destination() { return _dst; }

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
            Buffer * _payload;
        };

        TX_Schedule_Entry * next_message()
        {
            auto min_i = 0u;
            bool ok;
            do {
                if(_n_entries == 0) {
                    return 0;
                }

                auto min = _table[0].transmit_at();

                for(auto i = 1u; i < _n_entries; i++) {
                    if(_table[i].transmit_at() <= min) {
                        min = _table[i].transmit_at();
                        min_i = i;
                    }
                }
                ok = _table[min_i].trials() < Traits<TSTP_MAC>::MAX_SEND_TRIALS;
                if(not ok) {
                    remove_by_index(min_i);
                    min_i = 0u;  
                }
            } while(not ok);

            return &_table[min_i];
        }

        bool remove(Message_ID id)
        {
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

        bool insert(bool is_new, Message_ID id, Time transmit_at, Time backoff, Address destination, Buffer * payload)
        {
            db<TSTP_MAC>(TRC) << "TSTP_MAC::TX_Schedule::insert(new=" << is_new << ",id=" << id << ",at=" << transmit_at << ",bkf=" << backoff << ",dst=" << destination << ",pld=" << payload << endl;

            if(_n_entries < Traits<TSTP_MAC>::TX_SCHEDULE_SIZE - 1) {
                new (&_table[_n_entries]) TX_Schedule_Entry(is_new, id, transmit_at, backoff, destination, payload);
                db<TSTP_MAC>(TRC) << " => " << hex << &_table[_n_entries] << endl;
                _n_entries++;
                return true;
            }
            return false;
        }

        void update_timeout(TX_Schedule_Entry * e, Time timeout) {
            ++(*e);
            e->transmit_at(timeout);
        }

    private:
        void remove_by_index(unsigned int idx) 
        {
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
    static TX_Schedule::TX_Schedule_Entry * tx_pending();
    static Time time_now();
    static void timeout(Time time, const Timer_Handler & handler);
    static void clear_timeout();
    static void cca(const unsigned int & int_id = 0);
    static void prepare_tx_mf();
    static void tx_mf(const unsigned int & int_id = 0);
    static void rx_mf(const unsigned int & int_id = 0);
    static void tx_data(const unsigned int & int_id = 0);
    static void rx_data(const unsigned int & int_id = 0);
    static void process_mf(Buffer * b);
    static void process_data(Buffer * b);
    static Microframe * to_microframe(Buffer * b);
    static Payload * to_payload(Buffer * b);
    static bool relevant(Microframe * mf);
    static bool should_forward(Payload * p);

    static bool all_listen(TX_Schedule::TX_Schedule_Entry * e);
    static bool is_destination(TX_Schedule::TX_Schedule_Entry * e);
    static Distance distance_to(TX_Schedule::TX_Schedule_Entry * e); 
    static Time time_until_data(Microframe * mf);
    static Time backoff(Payload * p);
    static Time backoff();

    static Buffer * _tx_pending_mf_buffer;
    static TX_Schedule _tx_schedule;
    static TX_Schedule::TX_Schedule_Entry * _tx_pending_data;
    static NIC _radio;
    static Traits<TSTP_MAC>::Timer _timer;
    static Message_ID _receiving_data_id;
    static Address _address;
    static Address _sink_address;
};

__END_SYS

#endif

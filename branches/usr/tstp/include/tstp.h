// EPOS Trustful SpaceTime Protocol Declarations

#ifndef __tstp_common_h
#define __tstp_common_h

#include <utility/geometry.h>
#include <rtc.h>

__BEGIN_SYS

class TSTP_Common
{
public:
    static const unsigned int PAN = 10; // Nodes
    static const unsigned int SAN = 100; // Nodes
    static const unsigned int LAN = 10000; // Nodes
    static const unsigned int NODES = Traits<Build>::NODES;

    typedef Traits<TSTP>::Time_Manager Time_Manager;
    typedef Traits<TSTP>::Locator Locator;

    // Version
    // This field is packed first and matches the Frame Type field in the Frame Control in IEEE 802.15.4 MAC.
    // A version number above 4 renders TSTP into the reserved frame type zone and should avoid interference.
    enum Version {
        V0 = 4
    };

    // Packet Types
    enum Type {
        INTEREST  = 0,
        RESPONSE  = 1,
        COMMAND   = 2,
        CONTROL   = 3
    };

    // Control Packet Types
    enum Control_Subtype {
        KEY_REQUEST = 0,
        KEY_RESPONSE = 1,
        AUTH_REQUEST = 2,
        AUTH_RESPONSE = 3,
        REPORT = 4,
    };

    // Scale for local network's geographic coordinates
    enum Scale {
        CMx50_8  = 0,
        CM_16    = 1,
        CMx25_16 = 2,
        CM_32    = 3
    };
    static const Scale SCALE = (NODES <= PAN) ? CMx50_8 : (NODES <= SAN) ? CM_16 : (NODES <= LAN) ? CMx25_16 : CM_32;

    // Time
    //typedef RTC::Microsecond Microsecond;
    typedef long long Microsecond;
    typedef unsigned int Time_Stamp; // Least significative bytes from the time manager's time stamp

    // Geographic Coordinates
    template<Scale S>
    struct _Coordinates: public Point<char, 3>
    {
        typedef char Number;

        _Coordinates(Number x = 0, Number y = 0, Number z = 0): Point<Number, 3>(x, y, z) {}
    } __attribute__((packed));
    typedef _Coordinates<SCALE> Coordinates;

    // Geographic Region in a time interval (not exactly Spacetime, but ...)
    template<Scale S>
    struct _Region: public Sphere<typename _Coordinates<S>::Number>
    {
        typedef typename _Coordinates<S>::Number Number;
        typedef Sphere<Number> Base;

        _Region(const Coordinates & c, const Number & r, const Microsecond & _t0, const Microsecond & _t1): Base(c, r), t0(_t0), t1(_t1) {}

        bool contains(const Coordinates & c, const Microsecond & t) const { return ((Base::center - c) <= Base::radius) && ((t >= t0) && (t <= t1)); }

        friend Debug & operator<<(Debug & db, const _Region & r) {
            db << "{" << reinterpret_cast<const Base &>(r) << ",t0=" << r.t0 << ",t1=" << r.t1 << "}";
            return db;
        }

        Microsecond t0;
        Microsecond t1;
    } __attribute__((packed));
    typedef _Region<SCALE> Region;

    // Packet Header
    template<Scale S>
    class _Header
    {
        // Format
        // Bit  7      5    3  2    0                0         0         0         0         0         0         0         0         0
        //      +------+----+--+----+----------------+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+
        //      | ver  |type|tr|scal|   confidence   |   o.x   |   o.y   |   o.z   |   l.x   |   l.y   |   l.z   |   l.t   | elapsed |
        //      +------+----+--+----+----------------+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+
        // Size(bits)     8                  8         8/16/32   8/16/32   8/16/32   8/16/32   8/16/32   8/16/32      32        32    

    public:
        _Header(const Type & t, bool tr = false, unsigned char c = 0, const Coordinates & o = 0, const Coordinates & l = 0, const Time_Stamp & lht = 0, const Microsecond & m = 0, const Version & v = V0)
        : _config(v << 5 | t << 3 | tr << 2 | S), _confidence(c), _origin(o), _last_hop(l), _last_hop_time(lht), _time(m) {}

        Version version() const { return static_cast<Version>((_config >> 5) & 0x07); }
        void version(const Version & v) { _config = (_config & 0x1f) | (v << 5); }

        Type type() const { return static_cast<Type>((_config >> 3) & 0x03); }
        void type(const Type & t) { _config = (_config & 0xe4) | (t << 3); }

        bool time_request() const { return (_config >> 2) & 0x01; }
        void time_request(bool tr) { _config = (_config & 0xfb) | (tr << 2); }

        Scale scale() const { return static_cast<Scale>(_config & 0x03); }
        void scale(const Scale & s) { _config = (_config & 0xfc) | s; }

        Microsecond time() const { return _time; }
        void time(const Microsecond & t) { _time = t; }

        const Coordinates & origin() const { return _origin; }
        void origin(const Coordinates & c) { _origin = c; }

        const Coordinates & last_hop() const { return _last_hop; }
        void last_hop(const Coordinates & c) { _last_hop = c; }

        Time_Stamp last_hop_time() const { return _last_hop_time; }
        void last_hop_time(const Time_Stamp & lht) { _last_hop_time = lht; }

        friend Debug & operator<<(Debug & db, const _Header & h) {
            db << "{v=" << h.version() - V0 << ",t=" << ((h.type() == INTEREST) ? 'I' :  (h.type() == RESPONSE) ? 'R' : (h.type() == COMMAND) ? 'C' : 'P') << ",tr=" << h.time_request() << ",s=" << h.scale() << ",t=" << h._time << ",o=" << h._origin << ",l=" << h._last_hop << "}";
            return db;
        }

    protected:
        unsigned char _config;
        unsigned char _confidence;
        Coordinates _origin;
        Coordinates _last_hop;
        Time_Stamp _last_hop_time;
        Microsecond _time;
    } __attribute__((packed));
    typedef _Header<SCALE> Header;


    // TSTP encodes SI Units similarly to IEEE 1451 TEDs
    class Unit
    {
    public:
        // Formats
        // Bit       31                                 16                                     0
        //         +--+----------------------------------+-------------------------------------+
        // Digital |0 | type                             | dev                                 |
        //         +--+----------------------------------+-------------------------------------+

        // Bit       31   29   27     24     21     18     15     12      9      6      3      0
        //         +--+----+----+------+------+------+------+------+------+------+------+------+
        // SI      |1 |NUM |MOD |sr+4  |rad+4 |m+4   |kg+4  |s+4   |A+4   |K+4   |mol+4 |cd+4  |
        //         +--+----+----+------+------+------+------+------+------+------+------+------+
        // Bits     1   2    2     3      3      3      3      3      3      3      3      3


        // Valid values for field SI
        enum {
            DIGITAL = 0 << 31, // The Unit is plain digital data. Subsequent 15 bits designate the data type. Lower 16 bits are application-specific, usually a device selector.
            SI      = 1 << 31  // The Unit is SI. Remaining bits are interpreted as specified here.
        };

        // Valid values for field NUM
        enum {
            I32 = 0 << 29, // Value is an integral number stored in the 32 last significant bits of a 32-bit big-endian integer.
            I64 = 1 << 29, // Value is an integral number stored in the 64 last significant bits of a 64-bit big-endian integer.
            F32 = 2 << 29, // Value is a real number stored as an IEEE 754 binary32 big-endian floating point.
            D64 = 3 << 29, // Value is a real number stored as an IEEE 754 binary64 big-endian doulbe precision floating point.
            NUM = D64      // AND mask to select NUM bits
        };

        // Valid values for field MOD
        enum {
            DIR     = 0 << 27, // Unit is described by the product of SI base units raised to the powers recorded in the remaining fields.
            DIV     = 1 << 27, // Unit is U/U, where U is described by the product SI base units raised to the powers recorded in the remaining fields.
            LOG     = 2 << 27, // Unit is log_e(U), where U is described by the product of SI base units raised to the powers recorded in the remaining fields.
            LOG_DIV = 3 << 27, // Unit is log_e(U/U), where U is described by the product of SI base units raised to the powers recorded in the remaining fields.
            MOD = D64          // AND mask to select MOD bits
        };

        // Masks to select the SI units
        enum {
            SR      = 7 << 24,
            RAD     = 7 << 21,
            M       = 7 << 18,
            KG      = 7 << 15,
            S       = 7 << 12,
            A       = 7 <<  9,
            K       = 7 <<  6,
            MOL     = 7 <<  3,
            CD      = 7 <<  0
        };

        // Typical SI Quantities
        enum Quantity {
             //                        si      | mod       | sr            | rad           |  m            |  kg           |  s            |  A            |  K            |  mol          |  cd
             Length                  = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 1) << 18 | (4 + 0) << 15 | (4 + 0) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 0),
             Mass                    = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 0) << 18 | (4 + 1) << 15 | (4 + 0) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 0),
             Time                    = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 0) << 18 | (4 + 0) << 15 | (4 + 1) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 0),
             Current                 = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 0) << 18 | (4 + 0) << 15 | (4 + 0) << 12 | (4 + 1) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 0),
             Electric_Current        = Current,
             Temperature             = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 0) << 18 | (4 + 0) << 15 | (4 + 0) << 12 | (4 + 0) << 9  | (4 + 1) << 6  | (4 + 0) << 3  | (4 + 0),
             Amount_of_Substance     = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 0) << 18 | (4 + 0) << 15 | (4 + 0) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 1) << 3  | (4 + 0),
             Liminous_Intensity      = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 0) << 18 | (4 + 0) << 15 | (4 + 0) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 1),
             Area                    = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 2) << 18 | (4 + 0) << 15 | (4 + 0) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 0),
             Volume                  = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 3) << 18 | (4 + 0) << 15 | (4 + 0) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 0),
             Speed                   = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 1) << 18 | (4 + 0) << 15 | (4 - 1) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 0),
             Velocity                = Speed,
             Acceleration            = 1 << 31 | DIR << 27 | (4 + 0) << 24 | (4 + 0) << 21 | (4 + 1) << 18 | (4 + 0) << 15 | (4 - 2) << 12 | (4 + 0) << 9  | (4 + 0) << 6  | (4 + 0) << 3  | (4 + 0)
         };

        // SI Factors
        typedef char Factor;
        enum {
         // Name           Code         Symbol    Factor
            ATTO        = (8 - 8), //     a       0.000000000000000001
            FEMTO       = (8 - 7), //     f       0.000000000000001
            PICO        = (8 - 6), //     p       0.000000000001
            NANO        = (8 - 5), //     n       0.000000001
            MICRO       = (8 - 4), //     μ       0.000001
            MILI        = (8 - 3), //     m       0.001
            CENTI       = (8 - 2), //     c       0.01
            DECI        = (8 - 1), //     d       0.1
            NONE        = (8    ), //     -       1
            DECA        = (8 + 1), //     da      10
            HECTO       = (8 + 2), //     h       100
            KILO        = (8 + 3), //     k       1000
            MEGA        = (8 + 4), //     M       1000000
            GIGA        = (8 + 5), //     G       1000000000
            TERA        = (8 + 6), //     T       1000000000000
            PETA        = (8 + 7)  //     P       1000000000000000
        };


        template<int N>
        struct Get { typedef typename SWITCH<N, CASE<I32, long, CASE<I64, long long, CASE<DEFAULT, long>>>>::Result Type; };

        template<typename T>
        struct GET { enum { NUM = I32 }; };

    public:
        Unit(unsigned long u) { _unit = u; }

        operator unsigned long() const { return _unit; }

        int sr()  const { return ((_unit & SR)  >> 24) - 4 ; }
        int rad() const { return ((_unit & RAD) >> 21) - 4 ; }
        int m()   const { return ((_unit & M)   >> 18) - 4 ; }
        int kg()  const { return ((_unit & KG)  >> 15) - 4 ; }
        int s()   const { return ((_unit & S)   >> 12) - 4 ; }
        int a()   const { return ((_unit & A)   >>  9) - 4 ; }
        int k()   const { return ((_unit & K)   >>  6) - 4 ; }
        int mol() const { return ((_unit & MOL) >>  3) - 4 ; }
        int cd()  const { return ((_unit & CD)  >>  0) - 4 ; }

        friend Debug & operator<<(Debug & db, const Unit & u) {
            if(u & SI) {
                db << "{SI";
                switch(u & MOD) {
                case DIR: break;
                case DIV: db << "[U/U]"; break;
                case LOG: db << "[log(U)]"; break;
                case LOG_DIV: db << "[log(U/U)]";
                };
                switch(u & NUM) {
                case I32: db << ".I32"; break;
                case I64: db << ".I64"; break;
                case F32: db << ".F32"; break;
                case D64: db << ".D64";
                }
                db << ':';
                if(u.sr())
                    db << "sr^" << u.sr();
                if(u.rad())
                    db << "rad^" << u.rad();
                if(u.m())
                    db << "m^" << u.m();
                if(u.kg())
                    db << "kg^" << u.kg();
                if(u.s())
                    db << "s^" << u.s();
                if(u.a())
                    db << "A^" << u.a();
                if(u.k())
                    db << "K^" << u.k();
                if(u.mol())
                    db << "mol^" << u.mol();
                if(u.cd())
                    db << "cdr^" << u.cd();
            } else
                db << "{D:" << "l=" <<  (u >> 16);
            db << "}";
            return db;
        }

    private:
        unsigned long _unit;
    } __attribute__((packed));

    // SI values (either integer32, integer64, float32, double64)
    template<int NUM>
    class Value
    {
    public:
        Value(long int v): _value(v) {}

        operator long int() { return _value; }

    private:
        long int _value;
    };

    // Precision or Error in SI values, expressed as 10^Error
    typedef char Precision;
    typedef char Error;

    // Observers choose whether they want to receive only packets that are destined to this node,
    // only packets that are NOT destined to this node, or all packets.
    // These values ensure that ALL observers (e.g. time/space sync) are going to be called before
    // FOR_ME observers (e.g. TSTP), so that necessary synchronizations are done first.
    class NIC_Observing_Condition {
    public:
        enum Code {
            ALL = 0, // All packets
            FOR_ME = 1, // Only packets destined to this node
            NOT_FOR_ME = 3 // Only packets not destined to this node
        };

        NIC_Observing_Condition(char c = ALL) : _code(c) { }

        bool operator==(const NIC_Observing_Condition & rhs) const { 
            // ALL == ALL, FOR_ME == FOR_ME, NOT_FOR_ME == NOT_FOR_ME,
            // FOR_ME != NOT_FOR_ME
            // ALL == FOR_ME, ALL == NOT_FOR_ME,
            return rhs._code == _code ? true : !(rhs._code & _code); 
        }

        operator char() const { return _code; }
        operator char&() { return _code; }

    private:
        char _code;
    };
};

template<>
struct TSTP_Common::_Coordinates<TSTP_Common::CM_16>: public Point<short, 3>
{
    typedef short Number;

    _Coordinates(Number x = 0, Number y = 0, Number z = 0): Point<Number, 3>(x, y, z) {}
} __attribute__((packed));

template<>
struct TSTP_Common::_Coordinates<TSTP_Common::CMx25_16>: public Point<short, 3>
{
    typedef short Number;

    _Coordinates(Number x = 0, Number y = 0, Number z = 0): Point<Number, 3>(x, y, z) {}
} __attribute__((packed));

template<>
struct TSTP_Common::_Coordinates<TSTP_Common::CM_32>: public Point<long, 3>
{
    typedef long Number;

    _Coordinates(Number x = 0, Number y = 0, Number z = 0): Point<Number, 3>(x, y, z) {}
} __attribute__((packed));

template<>
class TSTP_Common::Value<TSTP_Common::Unit::I64>
{
public:
    Value(long long int v): _value(v) {}

    operator long long int() { return _value; }

public:
    long long int _value;
};

template<>
class TSTP_Common::Value<TSTP_Common::Unit::F32>
{
public:
    Value(float v): _value(v) {}

    operator float() { return _value; }

private:
    float _value;
};

template<>
class TSTP_Common::Value<TSTP_Common::Unit::D64>
{
public:
    Value(double v): _value(v) {}

    operator double() { return _value; }

private:
    double _value;
};

typedef Traits<TSTP>::MAC TSTPNIC;

__END_SYS

#endif

#ifndef __tstp_h
#define __tstp_h

#include <utility/observer.h>
#include <utility/buffer.h>
#include <utility/hash.h>
#include <network.h>
#include <tstpoe.h>

__BEGIN_SYS

// Time Managers
#include <timer.h>
#include <utility/math.h>

// Passive Time Synchronization
class PTS : public TSTP_Common, private TSTPNIC::Observer
{
    friend class TSTP;
    friend class TSTPOE;

    typedef TSTP_Timer Timer;
    static const unsigned int TX_DELAY = TSTPNIC::TX_DELAY; 

    typedef TSTPNIC::Buffer Buffer;
    typedef TSTPNIC::Packet Packet;

    typedef TSTP_Timer::Time_Stamp Time_Stamp;
    static const IF<(static_cast<Time_Stamp>(-1) < static_cast<Time_Stamp>(0)), bool, void>::Result _time_stamp_sign_check; // TSTP_Timer::Time_Stamp must be signed!
    typedef TSTP_Common::Time_Stamp Short_Time_Stamp;
    typedef TSTP_Common::Microsecond Time;

public:
    // Called by TSTP
    PTS() {
        _timer.start();
        TSTPNIC::attach(this, NIC_Observing_Condition::ALL);
    }
    ~PTS() {
        _timer.stop();
        TSTPNIC::detach(this, NIC_Observing_Condition::ALL);
    }

    static Time now() { return ts_to_us(_timer.now()); }

    static bool bootstrap() {
        while(!_synced);
        return true; 
    }


    // Called by TSTPNIC
    static void cancel_interrupt() { _timer.cancel_interrupt(); }

    static void interrupt(const Time & when) {
        auto tnow = _timer.now();
        Time_Stamp w = us_to_ts(when);
        if(when <= tnow) {
            _timer.interrupt(tnow + 1000, &int_handler); //TODO
        } else {
            _timer.interrupt(w, &int_handler);
        }
    }

    static void stamp_receive(Buffer * buf) { buf->sfd_time(_timer.sfd()); }

    static void stamp_send(Buffer * buf) { buf->frame()->data<Packet>()->last_hop_time(_timer.now()); }

    // Called by Observer
    void update(TSTPNIC::Observed * obs, NIC_Observing_Condition c, Buffer * buf) {
        Time_Stamp adjust = 0;
        auto packet = buf->frame()->data<Packet>();
        switch(packet->type()) {
        case INTEREST: {
            Short_Time_Stamp sfd = buf->sfd_time();
            Short_Time_Stamp to_set = packet->last_hop_time() + TX_DELAY;
            Time_Stamp diff1 = sfd - to_set;
            Time_Stamp diff2 = to_set - sfd;
            adjust = _UTIL::abs(diff1) < _UTIL::abs(diff2) ? diff1 : diff2;
            _timer.adjust(adjust);
            _synced = true;
            kout << "sfd = " << sfd << endl;
            kout << "to_set = " << to_set << endl;
            kout << "diff1 = " << diff1 << endl;
            kout << "diff2 = " << diff2 << endl;
            kout << "adjust = " << adjust << endl;
        } break;
        default: break;
        }
    }

private:
    static void int_handler(const unsigned int & interrupt) {} //TODO
    static Time ts_to_us(const Time_Stamp & ts) { return ts * 1000000ll / _timer.frequency(); }
    static Time_Stamp us_to_ts(const Time & us) { return us * _timer.frequency() / 1000000ll; }

    static Timer _timer; // TODO: several units?
    static volatile bool _synced;
};


// Locators
class NIC_Locator : public TSTP_Common 
{
public:
    static bool bootstrap() { return true; }
    static Coordinates here() { return TSTPNIC::here(); }
    static Coordinates absolute(const Coordinates & coordinates) { return coordinates; }
};

class Static_Locator : public TSTP_Common 
{
public:
    static bool bootstrap() { return true; }
    static Coordinates here() { return _here; }
    static Coordinates absolute(const Coordinates & coordinates) { return coordinates; }

private:
    static Coordinates _here;
};


// TSTP
class TSTP: public TSTP_Common, private TSTPNIC::Observer
{
    template<typename> friend class Smart_Data;

private:
    typedef TSTPNIC NIC;

public:
    // Buffers received from the NIC
    typedef NIC::Buffer Buffer;
    typedef NIC::Packet Packet;

    static const unsigned int MTU = NIC::MTU;


    // TSTP observer/d conditioned to a message's address (ID)
    typedef Data_Observer<Buffer, int> Observer;
    typedef Data_Observed<Buffer, int> Observed;

    // Hash to store TSTP Observers by type
    static const unsigned int HASH_SIZE = 10;
    class Interested;
    typedef Hash<Interested, HASH_SIZE, Unit> Interests;
    class Responsive;
    typedef Hash<Responsive, HASH_SIZE, Unit> Responsives;


    // TSTP Messages
    // Each TSTP message is encapsulated in a single package. TSTP does not need nor supports fragmentation.

    // Interest/Response Modes
    enum Mode {
        // Response
        SINGLE = 0, // Only one response is desired for each interest job (desired, but multiple responses are still possible)
        ALL    = 1, // All possible responses (e.g. from different sensors) are desired
        // Interest
        DELETE = 2  // Revoke an interest
    };

    // Interest Message
    class Interest: public Header
    {
    public:
        Interest(const Region & region, const Unit & unit, const Mode & mode, const Error & precision, const Microsecond & expiry, const Microsecond & period = 0)
        : Header(INTEREST, 0, 0, here(), here(), 0), _region(region), _unit(unit), _mode(mode), _precision(0), _expiry(expiry), _period(period) {}

        const Unit & unit() const { return _unit; }
        const Region & region() const { return _region; }
        Microsecond period() const { return _period; }
        Microsecond expiry() const { return _expiry; }
        Mode mode() const { return static_cast<Mode>(_mode); }
        Error precision() const { return static_cast<Error>(_precision); }

        bool time_triggered() { return _period; }
        bool event_driven() { return !time_triggered(); }

        friend Debug & operator<<(Debug & db, const Interest & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",m=" << ((m._mode == ALL) ? 'A' : 'S') << ",e=" << int(m._precision) << ",x=" << m._expiry << ",re=" << m._region << ",p=" << m._period;
            return db;
        }

    protected:
        Region _region;
        Unit _unit;
        unsigned char _mode : 2;
        unsigned char _precision : 6;
        Microsecond _expiry;
        Microsecond _period;
    } __attribute__((packed));

    // Response (Data) Message
    class Response: public Header
    {
    private:
        typedef unsigned char Data[MTU - sizeof(Unit) - sizeof(Error) - sizeof(Microsecond)];

    public:
        Response(const Unit & unit, const Error & error = 0, const Microsecond & expiry = 0)
        : Header(RESPONSE, 0, 0, here(), here(), 0), _unit(unit), _error(error), _expiry(expiry) {}

        const Unit & unit() const { return _unit; }
        Microsecond expiry() const { return _expiry; }
        Error error() const { return _error; }

        template<typename T>
        void value(const T & v) { *reinterpret_cast<Value<Unit::GET<T>::NUM> *>(&_data) = v; }

        template<typename T>
        T value() { return *reinterpret_cast<Value<Unit::GET<T>::NUM> *>(&_data); }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Response & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",e=" << int(m._error) << ",x=" << m._expiry << ",d=" << hex << *const_cast<Response &>(m).data<unsigned>() << dec;
            return db;
        }

    protected:
        Unit _unit;
        Error _error;
        Microsecond _expiry;
        Data _data;
    } __attribute__((packed));

    // Command Message
    class Command: public Header
    {
    private:
        typedef unsigned char Data[MTU - sizeof(Region) - sizeof(Unit)];

    public:
        Command(const Unit & unit, const Region & region)
        : Header(COMMAND, 0, 0, here(), here(), 0), _region(region), _unit(unit) {}

        const Region & region() const { return _region; }
        const Unit & unit() const { return _unit; }

        template<typename T>
        T * command() { return reinterpret_cast<T *>(&_data); }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Command & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",reg=" << m._region;
            return db;
        }

    protected:
        Region _region;
        Unit _unit;
        Data _data;
    } __attribute__((packed));

    // Control Message
    class _Control : public Header
    {
    public:
        _Control(unsigned char st) : Header(CONTROL, 0, 0, here(), here(), 0, 0), _subtype(st) {}
    protected:
        static const unsigned int MTU = TSTP::MTU - sizeof(unsigned char);
        unsigned char _subtype;
    }__attribute__((packed));

    class Report : public _Control 
    {
    private:
        typedef unsigned char Data[_Control::MTU];
        static const Control_Subtype TYPE = Control_Subtype::REPORT;

    public:
        static unsigned int size(const Responsive * s) { 
            return _UTIL::min(s->_size - sizeof(Header) + sizeof(_Control), sizeof(Report));
        }

        Report(const Responsive * s)
            : _Control(TYPE) {
                memcpy(&_data, &(reinterpret_cast<const char *>(s)[sizeof(Header)]), size(s) - sizeof(_Control));
            }

    protected:
        Data _data;
    } __attribute__((packed));

    // Interested (binder between Interest messages and Smart Data)
    class Interested: public Interest
    {
    public:
        template<typename T>
        Interested(T * data, const Region & region, const Unit & unit, const Mode & mode, const Precision & precision, const Microsecond & expiry, const Microsecond & period = 0)
        : Interest(region, unit, mode, precision, expiry, period), _link(this, T::UNIT) {
            db<TSTP>(TRC) << "TSTP::Interested(d=" << data << ",r=" << region << ",p=" << period << ") => " << reinterpret_cast<const Interest &>(*this) << endl;
            _interested.insert(&_link);
            advertise();
        }
        ~Interested() {
            db<TSTP>(TRC) << "TSTP::~Interested(this=" << this << ")" << endl;
            _interested.remove(&_link);
            revoke();
        }

        void advertise() { send(); }
        void revoke() { _mode = DELETE; send(); }

    private:
        void send() {
            db<TSTP>(TRC) << "TSTP::Interested::send() => " << reinterpret_cast<const Interest &>(*this) << endl;
            Buffer * buf = NIC::alloc(sizeof(Interest));
            memcpy(buf->frame()->data<Interest>(), this, sizeof(Interest));
            NIC::send(buf);
        }

    private:
        Interests::Element _link;
    };

    // Responsive (binder between Smart Data (Sensors) and Response messages)
    class Responsive: public Response
    {
        friend class Report;
    public:
        template<typename T>
        Responsive(T * data, const Unit & unit, const Error & error, const Microsecond & expiry)
        : Response(unit, error, expiry), _size(sizeof(Response) - sizeof(Response::Data) + sizeof(typename T::Value)), _link(this, T::UNIT) {
            db<TSTP>(TRC) << "TSTP::Responsive(d=" << data << ",s=" << _size << ") => " << this << endl;
            db<TSTP>(INF) << "TSTP::Responsive() => " << reinterpret_cast<const Response &>(*this) << endl;
            _responsives.insert(&_link);
        }
        ~Responsive() {
            db<TSTP>(TRC) << "TSTP::~Responsive(this=" << this << ")" << endl;
            _responsives.remove(&_link);
        }


        using Header::time;
        using Header::origin;

        void respond(const Microsecond & expiry) { send(expiry); }

        void advertise() {
            Buffer * buf = NIC::alloc(Report::size(this));
            if(buf) {
                new (buf->frame()->data<Report>()) Report(this);
                db<TSTP>(INF) << "TSTP::Responsive::send:report=" << buf << " => " << reinterpret_cast<const Response &>(*this) << endl;
                NIC::send(buf);
            }
        }

    private:
        void send(const Microsecond & expiry) {
            db<TSTP>(TRC) << "TSTP::Responsive::send(x=" << expiry << ")" << endl;
            Buffer * buf = NIC::alloc(_size);
            memcpy(buf->frame()->data<Response>(), this, _size);
            db<TSTP>(INF) << "TSTP::Responsive::send:response=" << this << " => " << reinterpret_cast<const Response &>(*this) << endl;
            NIC::send(buf);
        }

    private:
        unsigned int _size;
        Responsives::Element _link;
    };

public:
    TSTP() : _time_manager(), _locator() {
        db<TSTP>(TRC) << "TSTP::TSTP()" << endl;
        NIC::attach(this, NIC_Observing_Condition::FOR_ME);
    }
    ~TSTP() {
        db<TSTP>(TRC) << "TSTP::~TSTP()" << endl;
        NIC::detach(this, NIC_Observing_Condition::FOR_ME);
    }

    static bool bootstrap() {
        bool ret = true;
        if((ret = Locator::bootstrap()))
            //if((ret = Router::bootstrap()))
                if((ret = Time_Manager::bootstrap()))
                    //if((ret = Security::bootstrap()))
                        ret = report();
        return ret;
    }

    static Microsecond now() { return Time_Manager::now(); }
    static Coordinates here() { return Locator::here(); }

    static void attach(Observer * obs, void * subject) { _observed.attach(obs, int(subject)); }
    static void detach(Observer * obs, void * subject) { _observed.detach(obs, int(subject)); }
    static bool notify(void * subject, Buffer * buf) { return _observed.notify(int(subject), buf); }

    static void init(unsigned int unit) {
        db<Init, TSTP>(TRC) << "TSTP::init()" << endl;
    }

private:
    static bool report() {
        for(unsigned int i = 0; i < HASH_SIZE; i++) {
            Responsives::List * list = _responsives[i];
            if(list)
                for(Responsives::Element * el = list->head(); el; el = el->next()) {
                    Responsive * responsive = el->object();
                    responsive->advertise();
                }
        }
        return true;
    }

    static Coordinates absolute(const Coordinates & coordinates) { return Locator::absolute(coordinates); }

    void update(NIC::Observed * obs, NIC_Observing_Condition c, Buffer * buf) {
        db<TSTP>(TRC) << "TSTP::update(obs=" << obs << ",buf=" << buf << ")" << endl;

        Packet * packet = buf->frame()->data<Packet>();
        switch(packet->type()) {
        case INTEREST: {
            Interest * interest = reinterpret_cast<Interest *>(packet);
            db<TSTP>(INF) << "TSTP::update:msg=" << interest << " => " << *interest << endl;
            // Check for local capability to respond and notify interested observers
            Responsives::List * list = _responsives[interest->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
            if(list)
                for(Responsives::Element * el = list->head(); el; el = el->next()) {
                    Responsive * responsive = el->object();
                    if(interest->region().contains(responsive->origin(), now())) {
                        notify(responsive, buf);
                    }
                }
        } break;
        case RESPONSE: {
            Response * response = reinterpret_cast<Response *>(packet);
            db<TSTP>(INF) << "TSTP::update:msg=" << response << " => " << *response << endl;
            // Check region inclusion and notify interested observers
            Interests::List * list = _interested[response->unit()];
            if(list)
                for(Interests::Element * el = list->head(); el; el = el->next()) {
                    Interested * interested = el->object();
                    if(interested->region().contains(response->origin(), response->time()))
                        notify(interested, buf);
                }
        } break;
        case COMMAND: {
            Command * command = reinterpret_cast<Command *>(packet);
            db<TSTP>(INF) << "TSTP::update:msg=" << command << " => " << *command << endl;
            // Check for local capability to respond and notify interested observers
            Responsives::List * list = _responsives[command->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
            if(list)
                for(Responsives::Element * el = list->head(); el; el = el->next()) {
                    Responsive * responsive = el->object();
                    if(command->region().contains(responsive->origin(), now()))
                        notify(responsive, buf);
                }
        } break;
        case CONTROL: break;
        }

        buf->nic()->free(buf);
    }

private:
    static Interests _interested;
    static Responsives _responsives;

    static Observed _observed; // Channel protocols are singletons

    Time_Manager _time_manager;
    Locator _locator;
};
/*
template<TSTP_Common::Scale S>
inline TSTP_Common::Time TSTP_Common::_Header<S>::time() const {
    return TSTP::now() + _elapsed;
}

template<TSTP_Common::Scale S>
inline void TSTP_Common::_Header<S>::time(const TSTP_Common::Time & t) {
    _elapsed = t - TSTP::now();
}
*/
__END_SYS

#endif

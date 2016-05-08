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

    typedef RTC::Microsecond Microsecond;
    typedef unsigned long long Time;
    typedef unsigned long Time_Offset;

    // Version
    // This field is packed first and matches the Frame Type field in the Frame Control in IEEE 802.15.4 MAC.
    // A version number above 4 renders TSTP into the reserved frame type zone and should avoid interfernce.
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

    // Scale for local network's geographic coordinates
    enum Scale {
        CMx50_8  = 0,
        CM_16    = 1,
        CMx25_16 = 2,
        CM_32    = 3
    };
    static const Scale SCALE = (NODES <= PAN) ? CMx50_8 : (NODES <= SAN) ? CM_16 : (NODES <= LAN) ? CMx25_16 : CM_32;

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

        _Region(const Coordinates & c, const Number & r, const Time & _t0, const Time & _t1): Base(c, r), t0(_t0), t1(_t1) {}

        bool contains(const Coordinates & c, const Time & t) const { return ((Base::center - c) <= Base::radius) && ((t >= t0) && (t <= t1)); }

        friend Debug & operator<<(Debug & db, const _Region & r) {
            db << "{" << reinterpret_cast<const Base &>(r) << ",t0=" << r.t0 << ",t1=" << r.t1 << "}";
            return db;
        }

        Time t0;
        Time t1;
    } __attribute__((packed));
    typedef _Region<SCALE> Region;

    // Packet Header
    template<Scale S>
    class _Header
    {
        // Format
        // Bit   7    5    3  2    0                0         0         0         0         0         0         0         0
        //     +------+----+--+----+----------------+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+
        //     | ver  |type|tr|scal|   confidence   | elapsed |   o.x   |   o.y   |   o.z   |   l.x   |   l.y   |   l.z   |
        //     +------+----+--+----+----------------+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+--- ~ ---+
        // Bits          8                  8            32     8/16/32   8/16/32   8/16/32   8/16/32   8/16/32   8/16/32

    public:
        _Header(const Type & t, bool tr = false, unsigned char c = 0, const Coordinates & o = 0, const Coordinates & l = 0, const Time_Offset & e = 0, const Version & v = V0)
        : _config(v << 5 | t << 3 | tr << 2 | S), _confidence(c), _origin(o), _last_hop(l), _elapsed(e) {}

        Version version() const { return static_cast<Version>((_config >> 5) & 0x07); }
        void version(const Version & v) { _config = (_config & 0x1f) | (v << 5); }

        Type type() const { return static_cast<Type>((_config >> 3) & 0x03); }
        void type(const Type & t) { _config = (_config & 0xe4) | (t << 3); }

        bool time_request() const { return (_config >> 2) & 0x01; }
        void time_request(bool tr) { _config = (_config & 0xfb) | (tr << 2); }

        Scale scale() const { return static_cast<Scale>(_config & 0x03); }
        void scale(const Scale & s) { _config = (_config & 0xfc) | s; }

        Time_Offset elapsed() const { return _elapsed; }
        void elapsed(const Time_Offset & e) { _elapsed = e; }

        const Coordinates & origin() const { return _origin; }
        void origin(const Coordinates & c) { _origin = c; }

        const Coordinates & last_hop() const { return _last_hop; }
        void last_hop(const Coordinates & c) { _last_hop = c; }

        Time time() const;

        friend Debug & operator<<(Debug & db, const _Header & h) {
            db << "{v=" << h.version() - V0 << ",t=" << ((h.type() == INTEREST) ? 'I' :  (h.type() == RESPONSE) ? 'R' : (h.type() == COMMAND) ? 'C' : 'P') << ",tr=" << h.time_request() << ",s=" << h.scale() << ",e=" << h._elapsed << ",o=" << h._origin << ",l=" << h._last_hop << "}";
            return db;
        }

    protected:
        unsigned char _config;
        unsigned char _confidence;
        Coordinates _origin;
        Coordinates _last_hop;
        Time_Offset _elapsed;
    } __attribute__((packed));
    typedef _Header<SCALE> Header;

    // Packet
    typedef unsigned char Data[];
    template<Scale S>
    class _Packet: public Header
    {
    public:
        _Packet() {}

        Header * header() { return this; }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const _Packet & p) {
            db << "{head=" << reinterpret_cast<const Header &>(p) << ",data=" << p._data << "}";
            return db;
        }

    private:
        Data _data;
    } __attribute__((packed));
    typedef _Packet<SCALE> Packet;


    // TSTP encodes SI Units similarly to IEEE 1451 TEDs
    struct Unit
    {
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
            I28 = 0 << 29, // Value is an integral number stored in the 28 last significant bits of a 32-bit big-endian integer. The 4 most significant bits encode an SI Factor.
            I60 = 1 << 29, // Value is an integral number stored in the 60 last significant bits of a 64-bit big-endian integer. The 4 most significant bits encode an SI Factor.
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

        // SI Factors
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

        template<int N>
        struct Get { typedef typename SWITCH<N, CASE<I28, long, CASE<I60, long long, CASE<DEFAULT, long>>>>::Result Type; };

        template<typename T>
        struct GET { enum { NUM = I28 }; };

        Unit(unsigned int u) { unit = u; }

        operator unsigned int() const { return unit; }

        int sr()  const { return ((unit & SR)  >> 24) - 4 ; }
        int rad() const { return ((unit & RAD) >> 21) - 4 ; }
        int m()   const { return ((unit & M)   >> 18) - 4 ; }
        int kg()  const { return ((unit & KG)  >> 15) - 4 ; }
        int s()   const { return ((unit & S)   >> 12) - 4 ; }
        int a()   const { return ((unit & A)   >>  9) - 4 ; }
        int k()   const { return ((unit & K)   >>  6) - 4 ; }
        int mol() const { return ((unit & MOL) >>  3) - 4 ; }
        int cd()  const { return ((unit & CD)  >>  0) - 4 ; }

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
                case I28: db << ".I28"; break;
                case I60: db << ".I60"; break;
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

        unsigned long unit;
    } __attribute__((packed));

    // SI values (either integer28, integer60, float32, double64)
    template<int NUM>
    struct Value
    {
        Value(long int v): exp(0), value(v) {}
        Value(int e, long int v): exp(e), value(v) {}

        operator long int() { return value & (0xf << 28); }

        long int exp:4;
        long int value:28;
    };

    // Precision or Error in SI values, expressed as 10^Error
    typedef char Precision;
    typedef char Error;
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
struct TSTP_Common::Value<TSTP_Common::Unit::I60>
{
    Value(long long int v): exp(0), value(v) {}
    Value(int e, long long int v): exp(e), value(v) {}

    operator long long int() { return value & (0xfULL << 60); }

    long long int exp:4;
    long long int value:60;
};

template<>
struct TSTP_Common::Value<TSTP_Common::Unit::F32>
{
    Value(float v): value(v) {}

    operator float() { return value; }

    float value;
};

template<>
struct TSTP_Common::Value<TSTP_Common::Unit::D64>
{
    Value(double v): value(v) {}

    operator double() { return value; }

    double value;
};

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

typedef TSTPOE TSTPNIC;

template<typename S>
class Smart_Data;

class TSTP: public TSTP_Common, private TSTPNIC::Observer
{
    template<typename> friend class Smart_Data;

public:
    // Buffers received from the NIC
    typedef TSTPNIC::Buffer Buffer;

    // TSTP observer/d conditioned to a message's address (ID)
    typedef Data_Observer<Packet, int> Observer;
    typedef Data_Observed<Packet, int> Observed;

    // Hash to store TSTP Observers by type
    class Interested;
    typedef Hash<Interested, 10, Unit> Interests;
    class Responsive;
    typedef Hash<Responsive, 10, Unit> Responsives;

    // Response Modes
    enum Mode {
        SINGLE = 0, // Only one response is desired for each interest job (desired, but multiple responses are still possible)
        ALL    = 1  // All possible responses (e.g. from different sensors) are desired
    };

    // TSTP Messages
    // Each TSTP message is encapsulated in a single package. TSTP does not need nor supports fragmentation.

    // Interest Message
    class Interest: public Header
    {
    public:
        Interest(const Region & region, const Unit & unit, const Mode & mode, const Error & precision, const Microsecond & expiry, const Microsecond & period = 0)
        : Header(INTEREST, 0, 0, here(), here(), 0), _region(region), _unit(unit), _mode(mode), _precision(0), _expiry(expiry), _period(period) {}

        const Unit & unit() const { return _unit; }
        const Region & region() const { return _region; }
        Microsecond period() const { return _period; }
        Time expiry() const { return _expiry; } // TODO: must return absolute time
        Mode mode() const { return static_cast<Mode>(_mode); }
        Error precision() const { return static_cast<Error>(_precision); }

        bool time_triggered() { return _period; }
        bool event_driven() { return !time_triggered(); }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Interest & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",m=" << ((m._mode == ALL) ? 'A' : 'S') << ",e=" << int(m._precision) << ",x=" << m._expiry << ",re=" << m._region << ",p=" << m._period;
            return db;
        }

    protected:
        Region _region;
        Unit _unit;
        unsigned char _mode : 2;
        unsigned char _precision : 6;
        Time_Offset _expiry;
        Microsecond _period;
        Data _data;
    } __attribute__((packed));

    // Response (Data) Message
    class Response: public Header
    {
    public:
        Response(const Unit & unit, const Error & error = 0, const Time & expiry = 0)
        : Header(RESPONSE, 0, 0, here(), here(), 0), _unit(unit), _error(error), _expiry(expiry) {}

        const Unit & unit() const { return _unit; }
        Time expiry() const { return _expiry; }
        Error error() const { return _error; }

        template<typename T>
        void value(const T & v) { *reinterpret_cast<Value<Unit::GET<T>::NUM> *>(&_data) = v; }

        template<typename T>
        T value() { return *reinterpret_cast<Value<Unit::GET<T>::NUM> *>(&_data); }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Response & m) {
            db << reinterpret_cast<const Header &>(m) << ",u=" << m._unit << ",e=" << int(m._error) << ",x=" << m._expiry;
            return db;
        }

        void send(const Time & expiry) {
            if(expiry)
                _expiry = expiry;
            Buffer * buf = TSTPNIC::alloc(sizeof(Response));
            db<TSTP>(TRC) << "TSTP::send() => " << *this << endl;
            memcpy(buf, this, sizeof(Response));
            TSTPNIC::send(buf);
        }

    private:
        Unit _unit;
        Error _error;
        Time _expiry;
        Data _data;
    } __attribute__((packed));

    // Command Message
    class Command: public Header
    {
    public:
        Command(const Unit & unit, const Region & region)
        : Header(COMMAND), _unit(unit), _region(region) {}

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
        Unit _unit;
        Region _region;
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
            _origin = _last_hop = here();
            _elapsed = 0;
            advertise();
        }
        ~Interested() {
            db<TSTP>(TRC) << "TSTP::~Interested(this=" << this << ")" << endl;
            _interested.remove(&_link);
            revoke();
        }

        void advertise() { send(); }
        void revoke() { _region.t1 = 0 ; send(); } // An interest with t1=0 deletes a previous interest for the same Unit in the same Region

    private:
        void send() {
            Buffer * buf = TSTPNIC::alloc(sizeof(Interest));
            db<TSTP>(TRC) << "TSTP::send() => " << reinterpret_cast<const Interest &>(*this) << endl;
            memcpy(buf->frame()->data<Interest>(), this, sizeof(Interest));
            db<TSTP>(TRC) << "TSTP::send() => " << reinterpret_cast<const Interest &>(*this) << endl;
            TSTPNIC::send(buf);
        }

    private:
        Interests::Element _link;
    };

    // Responsive (binder between Smart Data (Sensors) and Response messages)
    class Responsive: public Response
    {
    public:
        template<typename T>
        Responsive(T * data, const Unit & unit, const Time & expiry, const Error & error)
        : Response(unit, expiry, error), _link(this, T::UNIT) {
            db<TSTP>(TRC) << "TSTP::Responsive(d=" << data << ")" << endl;
            _responsives.insert(&_link);
        }
        ~Responsive() {
            db<TSTP>(TRC) << "TSTP::~Responsive(this=" << this << ")" << endl;
            _responsives.remove(&_link);
        }

        void respond(const Time & expiry = 0) { send(expiry); }

    private:
        Responsives::Element _link;
    };

 public:
    TSTP() {
        db<TSTP>(TRC) << "TSTP::TSTP()" << endl;
        TSTPNIC::attach(this);
    }
    ~TSTP() {
        db<TSTP>(TRC) << "TSTP::~TSTP()" << endl;
        TSTPNIC::detach(this);
    }

    static Time now() { return TSC::time_stamp() / 1000000; }
    static Coordinates here() { return Coordinates(0, 0, 0); }

    static void attach(Observer * obs, int interest) { _observed.attach(obs, interest); }
    static void detach(Observer * obs, int interest) { _observed.detach(obs, interest); }
    static bool notify(int interest, Packet * packet) { return _observed.notify(interest, packet); }

   static void init(unsigned int unit) {
        db<Init, TSTP>(TRC) << "TSTP::init()" << endl;
    }

private:
    static Coordinates absolute(const Coordinates & coordinates) { return coordinates; }

    void update(TSTPNIC::Observed * obs, Buffer * buf) {
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
                    db<TSTP>(INF) << "TSTP::update:msg=" << interest << " => " << *interest << endl;
                    if(interest->region().contains(responsive->origin(), now())) // TODO: revokation messagens with T1 = 0 won't match!
                        notify(int(responsive), packet);
                }
            db<TSTP>(INF) << "TSTP::update:msg=" << interest << " => " << *interest << endl;
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
                        notify(int(interested), packet);
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
                        notify(int(responsive), packet);
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
 };


template<TSTP_Common::Scale S>
inline TSTP_Common::Time TSTP_Common::_Header<S>::time() const {
    return TSTP::now() + _elapsed;
}


//// Digital Data Response Message
//template<>
//class TSTP::Response<TSTP::Unit::DIGITAL>: public _Response
//{
//public:
//    Response(const Unit & u, const Time & t = 0, const Error & e = 0): _Response(u, t, e) {}
//
//    template<typename T>
//    T * data() { return reinterpret_cast<T *>(&_data); }
//
//    friend Debug & operator<<(Debug & db, const Response & m) {
//        db << reinterpret_cast<const _Response &>(m) << ",d=" << m._data;
//        return db;
//    }
//
//protected:
//    Data _data;
//};
//
// SI I28 Response Message Marshalling
//template<>
//long TSTP::Response::value<long>() {
//    return reinterpret_cast<TSTP::Value<TSTP::Unit::I28> &>(_data);
//}
//
//// SI I60 Response Message
//template<>
//class TSTP::Response<TSTP::Unit::SI | TSTP::Unit::I60>: public _Response, public TSTP::Value<TSTP::Unit::I60>
//{
//public:
//    Response(const Unit & u, const Time & t = 0, const Error & e = 0): _Response(u, t, e) {}
//
//    friend Debug & operator<<(Debug & db, const Response & m) {
//        db << reinterpret_cast<const _Response &>(m) << ",v={e=" << m.exp << ",v=" << m.value;
//        return db;
//    }
//} __attribute__((packed));
//
//// SI F32 Response Message
//template<>
//class TSTP::Response<TSTP::Unit::SI | TSTP::Unit::F32>: public _Response, public TSTP::Value<TSTP::Unit::F32>
//{
//public:
//    Response(const Unit & u, const Time & t = 0, const Error & e = 0): _Response(u, t, e) {}
//
//    friend Debug & operator<<(Debug & db, const Response & m) {
//        db << reinterpret_cast<const _Response &>(m) << ",v={e=" << m.exp << ",v=" << m.value;
//        return db;
//    }
//} __attribute__((packed));
//
//// SI D64 Response Message
//template<>
//class TSTP::Response<TSTP::Unit::SI | TSTP::Unit::D64>: public _Response, public TSTP::Value<TSTP::Unit::D64>
//{
//public:
//    Response(const Unit & u, const Time & t = 0, const Error & e = 0): _Response(u, t, e) {}
//
//    friend Debug & operator<<(Debug & db, const Response & m) {
//        db << reinterpret_cast<const _Response &>(m) << ",v={e=" << m.exp << ",v=" << m.value;
//        return db;
//    }
//} __attribute__((packed));
//
__END_SYS

#endif

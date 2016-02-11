#ifndef __units_h
#define __units_h

#include <utility/debug.h>
#include <utility/ostream.h>
#include <utility/math.h>

__BEGIN_SYS

class Units
{
public:
    typedef int Microsecond;    
    typedef Microsecond Time;
    typedef int Centimeter;
    typedef Centimeter Distance;

    struct Coordinate
    {
        int x, y, z;

        Coordinate(int xi = 0, int yi = 0, int zi = 0) :x(xi), y(yi), z(zi) { }

        friend Debug & operator<<(Debug & db, const Coordinate & c) {
            db << "{" << c.x << "," << c.y << "," << c.z << "}";
            return db;
        }
        friend OStream & operator<<(OStream & os, const Coordinate & c) {
            os << "{" << c.x << "," << c.y << "," << c.z << "}";
            return os;
        }

        //operator int() const { return x ^ y ^ z; }

        bool operator==(const Coordinate & rhs) {
            return x == rhs.x and y == rhs.y and z == rhs.z;
        }

        Distance operator-(const Coordinate & rhs) const {
            int xx = rhs.x - x;
            int yy = rhs.y - y;
            int zz = rhs.z - z;
            //return Math::sqrt(xx*xx + yy*yy + zz*zz);
            return Math::abs(xx + yy + zz);
        }
    }__attribute__((packed));

    template<typename C = Coordinate, typename R = Distance>
    struct Sphere {
        Sphere(const C & c, const R & r = 0) : center(c), radius(r) { }
        Sphere(int xi = 0, int yi = 0, int zi = 0, const R & r = 0) : center(xi,yi,zi), radius(r) { }

        bool contains(const C & coord) const {
            return center - coord <= radius;
        }

        friend Debug & operator<<(Debug & db, const Sphere & s) {
            db << "c=" << s.center << ",r=" << s.radius;
            return db;
        }
        friend OStream & operator<<(OStream & os, const Sphere & s) {
            os << "c=" << s.center << ",r=" << s.radius;
            return os;
        }

        C center;
        R radius;
    }__attribute__((packed));
};

class TSTP_Common : public Units
{
public:
    typedef short Message_ID;
    typedef unsigned int Data;
    typedef int RSSI;

    struct Address : public Units::Coordinate {
        Address(int xi = 0, int yi = 0, int zi = 0) : Coordinate(xi, yi, zi) { }

        bool contains (const struct Address & addr) const {
            return ((*this) - addr) <= Traits<TSTP_MAC>::ADDRESS_MATCH_RADIUS;
        }

        friend Debug & operator<<(Debug & db, const Address & a) {
            db << "{" << a.x << "," << a.y << "," << a.z << "}";
            return db;
        }
        friend OStream & operator<<(OStream & os, const Address & a) {
            os << "{" << a.x << "," << a.y << "," << a.z << "}";
            return os;
        }
    }__attribute__((packed));

    typedef Units::Sphere<Address, Units::Centimeter> Region;
    typedef int Unit; // TODO

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

    enum RESPONSE_MODE {
        SINGLE = 0,
        ALL = 1,
    };

    struct Statistics : public NIC_Common::Statistics {
        Statistics(): NIC_Common::Statistics(), 
            dropped_rx_packets(0), dropped_rx_bytes(0), dropped_tx_packets(0),
            dropped_tx_bytes(0), rx_payload_frames(0), tx_payload_frames(0), dropped_payload_frames(0), waited_to_rx_payload(0) { }

        unsigned int dropped_rx_packets;
        unsigned int dropped_rx_bytes;
        unsigned int dropped_tx_packets;
        unsigned int dropped_tx_bytes;
        unsigned int rx_payload_frames;
        unsigned int tx_payload_frames;
        unsigned int dropped_payload_frames;
        unsigned int waited_to_rx_payload;
    };

    class Header
    {
    public:
        Header() {}
        Header(const Message_Type & t, const Address & origin) : _message_type(t), _origin_address(origin)  {};

        Message_Type message_type() const { return static_cast<Message_Type>(_message_type); }
        Address last_hop_address() const { return _last_hop_address; }
        void last_hop_address(const Address & addr) { _last_hop_address = addr; }
        Time last_hop_time() const { return _last_hop_time; }
        void last_hop_time(const Time & t) { _last_hop_time = t; }
        Address origin_address() const { return _origin_address; }

        friend Debug & operator<<(Debug & db, const Header & h) {
            db << "{type=" << h._message_type << ",tr=" << h._time_request << ",sscale=" << h._spatial_scale << ",tscale=" << h._temporal_scale << ",lconf=" << h._location_confidence << ",lhaddr=" << h._last_hop_address << ",lht=" << h._last_hop_time << ",oaddr=" << h._origin_address << ",ot=" << h._origin_time << "}" << endl;
            return db;
        }
        friend OStream & operator<<(OStream & os, const Header & h) {
            os << "{type=" << h._message_type << ",tr=" << h._time_request << ",sscale=" << h._spatial_scale << ",tscale=" << h._temporal_scale << ",lconf=" << h._location_confidence << ",lhaddr=" << h._last_hop_address << ",lht=" << h._last_hop_time << ",oaddr=" << h._origin_address << ",ot=" << h._origin_time << "}" << endl;
            return os;
        }

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

    class Interest {
    public:
        Interest(const Region & region, const Time & t0, const Time & dt, const Time & period, const Unit & unit, const unsigned int & precision, const RESPONSE_MODE & response_mode) : 
            _region(region), _t0(t0), _dt(dt),
            _period(period), _unit(unit), _precision(precision), 
            _response_mode(response_mode) { }

        Region region() const { return _region; }
        Time t0() const { return _t0; }
        Time period() const { return _period; }
        RESPONSE_MODE response_mode() const { return static_cast<RESPONSE_MODE>(_response_mode); }

        friend Debug & operator<<(Debug & db, const Interest & interest) {
            db << "{region=" << interest._region << ",t0=" << interest._t0 << ",dt=" << interest._dt << ",p=" << interest._period << ",u=" << interest._unit << ",pr=" << interest._precision << ",rm=" << interest._response_mode << "}" << endl;
            return db;
        }
        friend OStream & operator<<(OStream & os, const Interest & interest) {
            os << "{region=" << interest._region << ",t0=" << interest._t0 << ",dt=" << interest._dt << ",p=" << interest._period << ",u=" << interest._unit << ",pr=" << interest._precision << ",rm=" << interest._response_mode << "}" << endl;
            return os;
        }

    protected:
        Region _region;
        Time _t0;
        Time _dt;
        Time _period;
        Unit _unit;
        unsigned _precision : 7;
        unsigned _response_mode : 1;
    } __attribute__((packed, may_alias));
};

template<typename T>
inline T operator^(const T & a, const TSTP_Common::Address & addr) { return a ^ addr.x ^ addr.y ^ addr.z; }

__END_SYS

#endif

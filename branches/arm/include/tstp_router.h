#ifndef __tstp_router_h
#define __tstp_router_h

#include <utility/geometry.h>
#include <tstp_time.h>

__BEGIN_SYS

// Static Geographic Greedy Router
class SGGR 
{
    friend class TSTP;

    SGGR(unsigned int unit) : 
        _my_address(Traits<SGGR>::Address::X, Traits<SGGR>::Address::Y, Traits<SGGR>::Address::Z), 
        _sink_address(0, 0, 0), _my_distance_to_sink(_my_address - _sink_address)
    { }

    TSTP * _tstp;

    bool bootstrap();

    typedef Traits<TSTP>::Time_Manager::Time Time;
    typedef Traits<TSTP>::MAC_Config<> MAC_Config;

public:
    typedef int Distance;

    class Local_Address : public _UTIL::Point<3, Distance> {
    public:
        Local_Address(int x = 0, int y = 0, int z = 0) : _UTIL::Point<3, Distance>(x,y,z) {}

        // Point operators inheritance
        Distance operator-(const Local_Address & rhs) const {
            Distance xx = rhs.x - x;
            Distance yy = rhs.y - y;
            Distance zz = rhs.z - z;
            //return Math::sqrt(xx*xx + yy*yy + zz*zz); //TODO: fix when multiplication works on eMote3
            return Math::abs(xx) + Math::abs(yy) + Math::abs(zz);
        }
        bool operator==(const Local_Address& rhs) const { return x == rhs.x and y == rhs.y and z == rhs.z; }
        bool operator!=(const Local_Address& rhs) const { return !(*this == rhs); }

    } __attribute__((packed));

    class Remote_Address : public _UTIL::Sphere<Local_Address> {
    public:
        Remote_Address(int x = 0, int y = 0, int z = 0, int r = 0) : _UTIL::Sphere<Local_Address>(Local_Address(x,y,z), r) {}
    } __attribute__((packed));

    typedef Distance Hint;

    bool relevant(const Hint & hint) {
        return hint > _my_distance_to_sink;
    }

    Time backoff(char * header);
    Hint hint(const Remote_Address & dst) { return _my_address - dst.center; }
    Hint hint(const Local_Address & dst) { return _my_address - dst; }
    bool update(const Remote_Address & dst, const Local_Address & last_hop_addr);
    const Local_Address & my_address() { return _my_address; }
    const Local_Address & sink_address() { return _sink_address; }
    bool accept(const Remote_Address & dst) { return accept(dst, _my_address); }
    bool accept(const Remote_Address & dst, const Local_Address & src) { return dst.contains(src); }
    bool should_forward(const char * data, const Remote_Address & dst);
    bool should_forward(const char * data, const Local_Address & dst);

private:
    Time random_backoff() { return (Random::random() % ((MAC_Config::SLEEP_PERIOD - MAC_Config::CCA_TIME) / MAC_Config::G)) * MAC_Config::G; }

    const Local_Address _my_address;
    const Local_Address _sink_address;
    const Distance _my_distance_to_sink;
};

__END_SYS

#endif

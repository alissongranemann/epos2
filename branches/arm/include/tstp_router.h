#ifndef __tstp_router_h
#define __tstp_router_h

#include <utility/geometry.h>
#include <tstp_time.h>

__BEGIN_SYS

// Static Geographic Greedy Router
class SGGR 
{
    friend class TSTP;

    SGGR(unsigned int unit) : _my_address(
            EQUAL<Traits<TSTP>::Router, SGGR>::Result ? Traits<TSTP>::Router_Config<>::ADDRESS_X : 0,
            EQUAL<Traits<TSTP>::Router, SGGR>::Result ? Traits<TSTP>::Router_Config<>::ADDRESS_Y : 0,
            EQUAL<Traits<TSTP>::Router, SGGR>::Result ? Traits<TSTP>::Router_Config<>::ADDRESS_Z : 0)
    { }

    TSTP * _tstp;

    bool bootstrap();

    typedef Traits<TSTP>::Time_Manager::Time Time;

public:
    typedef _UTIL::Point<3, int> Point;
    typedef _UTIL::Sphere<Point> Sphere;

    class Local_Address : public Point {
    public:
        Local_Address(int x = 0, int y = 0, int z = 0) : Point(x,y,z) {}
    } __attribute__((packed));

    class Remote_Address : public Sphere {
    public:
        Remote_Address(int x = 0, int y = 0, int z = 0, int r = 0) : Sphere(Point(x,y,z), r) {}
    } __attribute__((packed));

    class Hint {
    };

    bool relevant(bool all_listen, const Hint & hint);
    bool is_destination(const Remote_Address & f);
    Time backoff(const Remote_Address & dst);
    bool hint(bool & all_listen, Hint & hint, const Remote_Address & dst);
    bool update(const Remote_Address & dst, const Local_Address & last_hop_addr);
    const Local_Address & my_address() { return _my_address; }
    bool accept(const Remote_Address & dst) { return accept(dst, _my_address); }
    bool accept(const Remote_Address & dst, const Local_Address & src) { return dst.contains(src); }

private:
    const Local_Address _my_address;
};

__END_SYS

#endif

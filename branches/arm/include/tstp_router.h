#ifndef __tstp_router_h
#define __tstp_router_h

#include <utility/geometry.h>
#include <tstp_time.h>

__BEGIN_SYS

class Geo_Greedy_Router 
{
    friend class TSTP;

    Geo_Greedy_Router(unsigned int unit) { }

    TSTP * _tstp;

    bool bootstrap();

    typedef Traits<TSTP>::Time_Manager::Time Time;

public:
    typedef _UTIL::Point<3, int> Point;
    typedef _UTIL::Sphere<Point> Sphere;

    class Local_Address : public Point {
        unsigned char _confidence : 7;
    } __attribute__((packed));

    class Remote_Address : public Sphere {
    public:
        Remote_Address() {}
        Remote_Address(int x, int y, int z, int r) : Sphere(Point(x,y,z), r) {}
    } __attribute__((packed));

    class Hint {
    };

    bool relevant(bool all_listen, const Hint & hint);
    bool is_destination(const Remote_Address & f);
    Time backoff(const Remote_Address & dst);
    bool hint(bool & all_listen, Hint & hint, const Remote_Address & dst);
    bool update(const Remote_Address & dst, const Local_Address & last_hop_addr);
    Local_Address & my_address() { return _my_address; }

private:
    Local_Address _my_address;
};

__END_SYS

#endif

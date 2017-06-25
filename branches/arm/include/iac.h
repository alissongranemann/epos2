#ifndef __iac_h
#define __iac_h

#include <tstp.h>
#include <utility/observer.h>
#include <serial_port.h>

__BEGIN_SYS

class IAC_Common
{
public:
    
    typedef unsigned int Type;
    enum {

        NEW_NODE = 0,
        NEW_INTEREST = 1

    };

    struct New_Node {

        New_Node(long _x, long _y, long _z)
        : x(_x), y(_y), z(_z), r(TSTP::RADIO_RANGE) {}

        long x;
        long y;
        long z;
        unsigned long r;

     }__attribute__((packed));

    struct New_Interest {

        New_Interest(long _x, long _y, long _z, long _r, unsigned long long _period, unsigned long long _expiry)
        : x(_x), y(_y), z(_z), r(_r), period(_period), expiry(_expiry) {}

        long x;
        long y;
        long z;
        unsigned long r;
        unsigned long long period;
        unsigned long long expiry;

        friend OStream & operator<<(OStream & os, const New_Interest & d) {
            os << d.x << "," << d.y << "," << d.z;
            return os;
        }

    }__attribute__((packed));

};

class IAC: private TSTP::Observer
{

private:

    static Serial_Port * serial_port;

public:

    IAC();

    ~IAC();

    static void init();
    static void new_interest(Serial_Port::Observer * obs, TSTP::Interest * interest);

    void update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf);

    friend Debug & operator<<(Debug & db, const IAC & d) {
        db << "IAC";
        return db;
    }
   
    IAC& operator=(const IAC& iac){
        return *this;
    }

};

__END_SYS

#endif

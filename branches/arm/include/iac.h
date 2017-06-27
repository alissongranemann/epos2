#ifndef __iac_h
#define __iac_h

#include <tstp.h>
#include <utility/observer.h>
#include <iac_serial_port_communication.h>
#include "machine/common/tstp_mac.h"

__BEGIN_SYS

class IAC_Common
{
public:
    
    typedef unsigned int Type;
    enum {
        NEW_NODE = 0,
        NEW_INTEREST = 1,
        CONFIG
    };

    struct New_Node {

        New_Node(long _x, long _y, long _z)
        : x(_x), y(_y), z(_z), r(TSTP::RADIO_RANGE) {}

        long x;
        long y;
        long z;
        unsigned long r;

        friend OStream & operator<<(OStream & os, const New_Node & d) {
            os << "coord(" << d.x << "," << d.y << "," << d.z << "),r=" << d.r;
            return os;
        }

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
            os << "coord(" << d.x << "," << d.y << "," << d.z << "),r=" << d.r << ",p=" << d.period << ", e=" << d.expiry;
            return os;
        }

    }__attribute__((packed));

    struct Config {

        Config(unsigned int _period)
        : r(TSTP::RADIO_RANGE), period(_period) {}

        unsigned long r;
        unsigned int period;

     }__attribute__((packed));

};

class IAC: private TSTP::Observer
{

public:

    typedef IAC_Common::New_Interest New_Interest;
    typedef IAC_Common::New_Node New_Node;
    typedef IAC_Common::Config Config;
    typedef Iac_Serial_Port_Communication::Message<New_Interest> New_Interest_Message;
    typedef Iac_Serial_Port_Communication::Message<New_Node> New_Node_Message;
    typedef Iac_Serial_Port_Communication::Message<Config> Config_Message;
    typedef IAC_Observer Observer;

private:

    typedef IF<EQUAL<Traits<Network>::NETWORKS::Get<Traits<NIC>::NICS::Find<CC2538>::Result>::Result, _SYS::TSTP>::Result, TSTP_MAC<CC2538RF>, IEEE802_15_4_MAC<CC2538RF>>::Result MAC;

    static Iac_Serial_Port_Communication * serial_port;

public:

    IAC();

    ~IAC();

    static void init();
    static void new_interest(Observer * obs, TSTP::Interest * interest);

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

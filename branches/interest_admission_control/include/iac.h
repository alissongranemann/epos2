#ifndef __iac_h
#define __iac_h

#include <tstp.h>
#include <utility/observer.h>
#include "iac_serial_manager.h"

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

        New_Interest(long _x, long _y, long _z, long _r, unsigned long long _period, unsigned long long _expiry, void * _ref)
        : x(_x), y(_y), z(_z), r(_r), period(_period), expiry(_expiry), ref(int(_ref)) {}

        long x;
        long y;
        long z;
        unsigned long r;
        unsigned long long period;
        unsigned long long expiry;
        int ref;

        friend OStream & operator<<(OStream & os, const New_Interest & d) {
            os << "coord(" << d.x << "," << d.y << "," << d.z << "),r=" << d.r << ",p=" << d.period << ", e=" << d.expiry << ", ref=" << d.ref;;
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
    typedef IAC_Serial_Manager::Message<New_Interest> New_Interest_Message;
    typedef IAC_Serial_Manager::Message<New_Node> New_Node_Message;
    typedef IAC_Serial_Manager::Message<Config> Config_Message;
    typedef Data_Observer<bool, int> Observer;
    typedef Data_Observed<bool, int> Observed;

    static const unsigned int Tr = 1457;

private:

    void update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf);

public:

    IAC();

    ~IAC();

    static void init();
    static void new_interest(IAC::Observer * obs, TSTP::Interested * interested);

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

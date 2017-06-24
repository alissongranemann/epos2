#ifndef __iac_h
#define __iac_h

#include <tstp.h>
#include <utility/observer.h>
#include <serial_port.h>

__BEGIN_SYS

class IAC_Common
{
public:
    
   struct Message {
        long x;
        long y;
        long z;

        friend OStream & operator<<(OStream & os, const Message & d) {
            os << d.x << "," << d.y << "," << d.z;
            return os;
        }
    }__attribute__((packed));

};

class IAC: private TSTP::Observer
{

private:

    typedef Serial_Port::Message Message;

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

#ifndef __iac_h
#define __iac_h

#include <tstp.h>

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

    IF<Traits<USB>::enabled, USB, UART>::Result io;
    OStream cout;

public:

    IAC();

    ~IAC();

    static void init();

    friend Debug & operator<<(Debug & db, const IAC & d) {
        db << "Interest_Admission_Control";
        return db;
    }
   
    void update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf) {
        db<TSTP>(TRC) << "Interest_Admission_Control::update: obs(" << obs << ",buf=" << buf << ")" << endl;
//        TSTP::Packet * packet = buf->frame()->data<TSTP::Packet>();
//        TSTP::Map * response = reinterpret_cast<TSTP::Map *>(packet);
//        TSTP::Coordinates coord = response->origin();
//        print(coord);
    }

    void print(const TSTP::Coordinates & c){
       
    }

    IAC& operator=(const IAC& iac){
        return *this;
    }

};

__END_SYS

#endif

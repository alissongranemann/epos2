#ifndef __iac_h
#define __iac_h

#include <tstp.h>
#include <utility/observer.h>

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

    typedef Data_Observed<bool> Observed;
    typedef Data_Observer<bool> Observer;

private:

    IF<Traits<USB>::enabled, USB, UART>::Result io;
    OStream cout;
    static Observed _observed; // Channel protocols are singletons

public:

    IAC();

    ~IAC();

    static void init();
    static void new_interest(Observer * obs);
    static void detach(Observer * obs) { _observed.detach(obs); }
    static bool notify(bool * result) { return _observed.notify(result); }

    friend Debug & operator<<(Debug & db, const IAC & d) {
        db << "Interest_Admission_Control";
        return db;
    }
   
    //new node
    void update(TSTP::Observed * obs, int subject, TSTP::Buffer * buf) {
        db<TSTP>(TRC) << "Interest_Admission_Control::update: obs(" << obs << ",buf=" << buf << ")" << endl;
//        TSTP::Packet * packet = buf->frame()->data<TSTP::Packet>();
//        TSTP::Map * response = reinterpret_cast<TSTP::Map *>(packet);
//        TSTP::Coordinates coord = response->origin();
//        print(coord);
    }

    IAC& operator=(const IAC& iac){
        return *this;
    }

};

__END_SYS

#endif

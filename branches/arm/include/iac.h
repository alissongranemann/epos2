#ifndef __iac_h
#define __iac_h

#include <tstp.h>
#include <iac_handler.h>
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

    static IAC_Handler _handler;
    static Observed _observed;

public:

    IAC();

    ~IAC();

    static void init();
    static void new_interest(Observer * obs);
    static void detach(Observer * obs) { _observed.detach(obs); }
    static bool notify(bool * result) { return _observed.notify(result); }

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

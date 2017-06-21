#include "iac.h"

__BEGIN_SYS

IAC::Observed IAC::_observed;

IAC::IAC(){
    db<TSTP>(TRC) << "TSTP::IAC()" << endl;
}

IAC::~IAC() {
    db<TSTP>(TRC) << "TSTP::~IAC()" << endl;
    TSTP::detach(this, reinterpret_cast<void *>(0));
}

void IAC::init(){
    db<TSTP>(TRC) << "IAC::init()" << endl;
	IAC * iac = new IAC();
    TSTP::attach(iac, reinterpret_cast<void *>(0));
}

void IAC::new_interest(Observer * obs){
    db<TSTP>(TRC) << "IAC::new_interest()" << endl;
    _observed.attach(obs);
    //write usb
    bool * result = new bool(true);
    notify(result);
}

__END_SYS

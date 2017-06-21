#include "iac.h"

__BEGIN_SYS

IAC::IAC(){
    db<TSTP>(TRC) << "TSTP::IAC()" << endl;
}

IAC::~IAC() {
    db<TSTP>(TRC) << "TSTP::~IAC()" << endl;
    //TODO TSTP::detach(this, reinterpret_cast<void *>(NEW_NODE));
}

void IAC::init(){
    db<TSTP>(TRC) << "IAC::init()" << endl;
	IAC * iac = new IAC();
    TSTP::attach(iac, reinterpret_cast<void *>(0));
}

__END_SYS

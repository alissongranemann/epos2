// EPOS TSTP Component Initialization

#include <utility/malloc.h>
#include <tstp.h>

__BEGIN_SYS

TSTP * TSTP::instance;

void TSTP::init()
{
    db<Init, TSTP>(TRC) << "TSTP::init()" << endl;
    instance = new TSTP();
    PTP::init();
}

TSTP::TSTP()
{
    TSTP_MAC::_tstp = this;
    //_nic.attach(this, Traits<TSTP>::PROTOCOL_ID); 
}

__END_SYS

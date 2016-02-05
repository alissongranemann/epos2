// EPOS TSTP Component Initialization

#include <utility/malloc.h>
#include <tstp.h>

__BEGIN_SYS

TSTP * TSTP::instance;

void TSTP::init()
{
    db<Init, TSTP>(TRC) << "TSTP::init()" << endl;
    instance = new TSTP();
}

TSTP::TSTP()
{
    //_nic.attach(this, Traits<TSTP>::PROTOCOL_ID); 
}

__END_SYS

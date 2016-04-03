// EPOS TSTP Component Initialization

#include <tstp.h>
#include <tstp_nic.h>
#include <utility/malloc.h>

__BEGIN_SYS

TSTP * TSTP::_network[Traits<NIC>::UNITS];

template<>
void TSTP::init(unsigned int unit)
{
    db<Init, TSTP>(TRC) << "TSTP::init()" << endl;
    auto mac = TSTP::MAC::get_by_unit(unit);
    auto time = new (SYSTEM) TSTP::Time_Manager(unit);
    auto router = new (SYSTEM) TSTP::Router(unit);
    auto security = new (SYSTEM) TSTP::Security(unit);

    _network[unit] = new (SYSTEM) TSTP(mac, time, router, security, unit);
}

void TSTP_NIC::init(unsigned int unit) {
    Traits<TSTP>::MAC::init(unit); 
}

__END_SYS

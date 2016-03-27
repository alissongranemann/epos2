// EPOS TSTP Component Initialization

#include <tstp.h>
#include <tstp_nic.h>
#include <utility/malloc.h>

__BEGIN_SYS

TSTP * TSTP::_network[Traits<NIC>::UNITS];

//TODO: this was called at nic_init. It's not called anywhere now
void TSTP::init(unsigned int unit)
{
    db<Init, TSTP>(TRC) << "TSTP::init()" << endl;
    auto mac = TSTP_MAC::get_by_unit(unit);
    auto time = new (SYSTEM) TSTP::Time_Manager(unit);
    auto router = new (SYSTEM) TSTP::Router(unit);
    auto security = new (SYSTEM) TSTP::Security(unit);

    _network[unit] = new (SYSTEM) TSTP(mac, time, router, security, unit);
}

template<>
void TSTP_NIC::init<0>(unsigned int unit) { TSTP_MAC::init<0, Traits<TSTP>::MAC_Config<0>::PHY_Layer>(0); TSTP::init(0); }
template<>
void TSTP_NIC::init<1>(unsigned int unit) { TSTP_MAC::init<1, Traits<TSTP>::MAC_Config<1>::PHY_Layer>(1); TSTP::init(1); }
template<>
void TSTP_NIC::init<2>(unsigned int unit) { TSTP_MAC::init<2, Traits<TSTP>::MAC_Config<2>::PHY_Layer>(2); TSTP::init(2); }
template<>
void TSTP_NIC::init<3>(unsigned int unit) { TSTP_MAC::init<3, Traits<TSTP>::MAC_Config<3>::PHY_Layer>(3); TSTP::init(3); }

TSTP::TSTP()
{
    //_nic.attach(this, Traits<TSTP>::PROTOCOL_ID);
}

__END_SYS

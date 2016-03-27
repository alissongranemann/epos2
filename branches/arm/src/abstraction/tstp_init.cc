// EPOS TSTP Component Initialization

#include <utility/malloc.h>
#include <tstp.h>
#include <tstp_nic.h>
#include <tstp_mac.h>
#include <cpu.h>

__BEGIN_SYS

TSTP * TSTP::instance;

//TODO: this was called at nic_init. It's not called anywhere now
void TSTP::init()
{
    //db<Init, TSTP>(TRC) << "TSTP::init()" << endl;
    //instance = new TSTP();
}

template<>
void TSTP_NIC::init<0>(unsigned int unit) { TSTP_MAC::init<0, Traits<TSTP>::MAC_Config<0>::PHY_Layer>(0); }
template<>
void TSTP_NIC::init<1>(unsigned int unit) { TSTP_MAC::init<1, Traits<TSTP>::MAC_Config<1>::PHY_Layer>(1); }
template<>
void TSTP_NIC::init<2>(unsigned int unit) { TSTP_MAC::init<2, Traits<TSTP>::MAC_Config<2>::PHY_Layer>(2); }
template<>
void TSTP_NIC::init<3>(unsigned int unit) { TSTP_MAC::init<3, Traits<TSTP>::MAC_Config<3>::PHY_Layer>(3); }

TSTP::TSTP()
{
    //_nic.attach(this, Traits<TSTP>::PROTOCOL_ID);
}

__END_SYS

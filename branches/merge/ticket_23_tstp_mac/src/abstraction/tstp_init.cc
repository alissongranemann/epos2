// EPOS Trustful SpaceTime Protocol Initialization

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>

__BEGIN_SYS

TSTP_Locator::TSTP_Locator()
{
    db<TSTP_Locator>(TRC) << "TSTP_Locator::TSTP_Locator()" << endl;
    TSTP::_nic->attach(this, NIC::TSTP);
}

TSTP_Time_Manager::TSTP_Time_Manager()
{
    db<TSTP_Time_Manager>(TRC) << "TSTP_Time_Manager::TSTP_Time_Manager()" << endl;
    TSTP::_nic->attach(this, NIC::TSTP);
}

TSTP_Router::TSTP_Router()
{
    db<TSTP_Router>(TRC) << "TSTP_Router::TSTP_Router()" << endl;
    TSTP::_nic->attach(this, NIC::TSTP);
}

TSTP_Security_Manager::TSTP_Security_Manager()
{
    db<TSTP_Security_Manager>(TRC) << "TSTP_Security_Manager::TSTP_Security_Manager()" << endl;
    TSTP::_nic->attach(this, NIC::TSTP);
}

TSTP::TSTP()
{
    db<TSTP>(TRC) << "TSTP::TSTP()" << endl;
    _nic->attach(this, NIC::TSTP);
}

void TSTP_Locator::bootstrap()
{
    db<TSTP_Locator>(TRC) << "TSTP_Locator::bootstrap()" << endl;
}

void TSTP_Time_Manager::bootstrap()
{
    db<TSTP_Time_Manager>(TRC) << "TSTP_Time_Manager::bootstrap()" << endl;
}

void TSTP_Router::bootstrap()
{
    db<TSTP_Router>(TRC) << "TSTP_Router::bootstrap()" << endl;
}

void TSTP_Security_Manager::bootstrap()
{
    db<TSTP_Security_Manager>(TRC) << "TSTP_Security_Manager::bootstrap()" << endl;
}

void TSTP::init(unsigned int unit)
{
    db<Init, TSTP>(TRC) << "TSTP::init(u=" << unit << ")" << endl;
    _nic = new (SYSTEM) NIC(unit);
    new (SYSTEM) TSTP_Locator;
    new (SYSTEM) TSTP_Time_Manager;
    new (SYSTEM) TSTP_Router;
    new (SYSTEM) TSTP_Security_Manager;
    new (SYSTEM) TSTP;

    TSTP_Locator::bootstrap();
    TSTP_Time_Manager::bootstrap();
    TSTP_Router::bootstrap();
    TSTP_Security_Manager::bootstrap();
}

__END_SYS

#endif

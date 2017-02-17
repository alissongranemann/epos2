// EPOS Trustful SpaceTime Protocol Initialization

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>

__BEGIN_SYS

TSTP::TSTP()
{
    db<TSTP>(TRC) << "TSTP::TSTP()" << endl;
}

void TSTP::Locator::bootstrap()
{
    db<TSTP>(TRC) << "TSTP::Locator::bootstrap()" << endl;

    _confidence = 0;

    //_confidence = 100;

    //if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x1a\x84\x0d\x06", 8)) // Router 1
        //_here = Coordinates(-6000,4500,0);
    //else
        //_here = TSTP::sink();

    /*
    // TODO: we need a better way to define static locations

    // LISHA Testbed
    if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xee\x0e\x16\x06", 8))
        _here = TSTP::sink();
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x7f\x0e\x16\x06", 8)) // Dummy 0
        _here = Coordinates(10,5,0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x69\x0e\x16\x06", 8)) // Dummy 1
        _here = Coordinates(10,10,0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xca\x0e\x16\x06", 8)) // Dummy 2
        _here = Coordinates(5,15,0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x67\x83\x0d\x06", 8)) // Dummy 3
        _here = Coordinates(0,15,0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xec\x82\x0d\x06", 8)) // Dummy 4
        _here = Coordinates(-5,10,0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x97\x0e\x16\x06", 8)) // Dummy 5
        _here = Coordinates(-5,5,0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x11\x83\x0d\x06", 8)) // Outlet 0 (B0)
        _here = Coordinates(460-730, -250-80, -15);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x74\x82\x0d\x06", 8)) // Outlet 1 (B1)
        _here = Coordinates(-5-730, -30-80, -15);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x5e\x83\x0d\x06", 8)) // Lights 1 (A1)
        _here = Coordinates(305-730, -170-80, 220);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x0b\x0f\x16\x06", 8)) // Luminosity sensor
        _here = Coordinates(10-730,-10-80, 0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x1a\x84\x0d\x06", 8)) // Router 1
        _here = Coordinates(505-730,120-80,0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xb0\x0e\x16\x06", 8)) // Router 2
        _here = Coordinates(505-730,20-80,10);



    //if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x1a\x84\x0d\x06", 8))
    //    _here = TSTP::sink();
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xec\x82\x0d\x06", 8))
    //    _here = Coordinates(  0,100, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xae\x82\x0d\x06", 8))
    //    _here = Coordinates(100,100, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x67\x83\x0d\x06", 8))
    //    _here = Coordinates(100, 0, 0);

    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x97\x0e\x16\x06", 8))
    //    _here = TSTP::sink();
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x7f\x0e\x16\x06", 8))
    //    _here = Coordinates(  0,100, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x7f\x0e\x16\x06", 8))
    //    _here = Coordinates(100,10, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xca\x0e\x16\x06", 8))
    //    _here = Coordinates(100,100, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xee\x0e\x16\x06", 8))
    //    _here = Coordinates(100, 0, 0);

    // Sniffer
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x6e\x82\x0d\x06", 8))
    //    _here = Coordinates(120, 120, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x7f\x0e\x16\x06", 8))
    //    _here = Coordinates(120,120, 0);

    // LISHA Smart Room
    //if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x0b\x0f\x16\x06", 8)) // Test Sink
    //    _here = Coordinates(0, 0, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x11\x83\x0d\x06", 8)) // Outlet 0 (B0)
    //    _here = Coordinates(460, -250, -15);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x74\x82\x0d\x06", 8)) // Outlet 1 (B1)
    //    _here = Coordinates(-5, -30, -15);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x5e\x83\x0d\x06", 8)) // Lights 1 (A1)
    //    _here = Coordinates(305, -170, 220);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x69\x0e\x16\x06", 8)) // Sniffer
    //    _here = Coordinates(120,120,0);
    ////else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x97\x0e\x16\x06", 8)) // Presence sensor
    ////    _here = Coordinates(10,-10, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xca\x0e\x16\x06", 8)) // Test GPIO sensor
    //    _here = Coordinates(675,-100,80);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xee\x0e\x16\x06", 8)) // Router
    //    _here = Coordinates(505,120,70);

    // SSB
    //if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x0b\x0f\x16\x06", 8)) // Test Sink
    //    _here = Coordinates(0, 0, 0);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x1a\x83\x0d\x06", 8)) // NOT SURE Lights 0
    //    _here = Coordinates(430, 50, 250);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xd3\x82\x0d\x06", 8)) // NOT SURE Lights 1
    //    _here = Coordinates(150, 50, 250);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xcb\x82\x0d\x06", 8)) // Lights 2
    //    _here = Coordinates(-140, 50, 250);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x04\x84\x0d\x06", 8)) // Lights 3
    //    _here = Coordinates(430, 300, 250);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x1e\x83\x0d\x06", 8)) // Lights 4
    //    _here = Coordinates(150, 300, 250);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x98\x83\x0d\x06", 8)) // Lights 5
    //    _here = Coordinates(-140, 300, 250);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xf6\x82\x0d\x06", 8)) // Lights 7
    //    _here = Coordinates(150, 500, 250);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xfa\x82\x0d\x06", 8)) // Lights 8
    //    _here = Coordinates(-140, 500, 250);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x84\x82\x0d\x06", 8)) // NOT SURE Outlet 3
    //    _here = Coordinates(-140, -10, -40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x0b\x83\x0d\x06", 8)) // Outlet 4
    //    _here = Coordinates(-250, -10, -40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x4c\x83\x0d\x06", 8)) // Outlet 5
    //    _here = Coordinates(-260, 200, -40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x98\x82\x0d\x06", 8)) // Outlet 6
    //    _here = Coordinates(-260, 400, -40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x8b\x82\x0d\x06", 8)) // NOT SURE Outlet 9
    //    _here = Coordinates(5, 570, -40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x90\x82\x0d\x06", 8)) // NOT SURE Outlet 11
    //    _here = Coordinates(185, 570, -40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x03\x83\x0d\x06", 8)) // Outlet 12
    //    _here = Coordinates(345, 570, -40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x22\x83\x0d\x06", 8)) // Lux 0
    //    _here = Coordinates(300, 320, 40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xa4\x82\x0d\x06", 8)) // Temperature 0
    //    _here = Coordinates(-130, 320, 40);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x51\x82\x0d\x06", 8)) // Air Conditioner 0
    //    _here = Coordinates(430, -10, 220);
    //else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x47\x83\x0d\x06", 8)) // Air Conditioner 1
    //    _here = Coordinates(150, -10, 220);

    else
        _confidence = 0;
    */

    if(Traits<Radio>::promiscuous) {
        _here = Coordinates(12,12,12);
        _confidence = 100;
    }

    TSTP::_nic->attach(this, NIC::TSTP);

    // Wait for spatial localization
    while(_confidence < 80)
        Thread::self()->yield();
}

void TSTP::Timekeeper::bootstrap()
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::bootstrap()" << endl;

    TSTP::_nic->attach(this, NIC::TSTP);

    if((TSTP::here() != TSTP::sink()) && (!Traits<Radio>::promiscuous)) { // TODO
        // Wait for time synchronization
        while(!_t1)
            Thread::self()->yield();
    }
}

void TSTP::Router::bootstrap()
{
    db<TSTP>(TRC) << "TSTP::Router::bootstrap()" << endl;
    TSTP::_nic->attach(this, NIC::TSTP);
}

void TSTP::Security::bootstrap()
{
    db<TSTP>(TRC) << "TSTP::Security::bootstrap()" << endl;

    TSTP::_nic->attach(this, NIC::TSTP);

    if((TSTP::here() != TSTP::sink()) && (!Traits<Radio>::promiscuous)) { // TODO
        Peer * peer = new (SYSTEM) Peer(_id, Region(TSTP::sink(), 0, 0, -1));
        _pending_peers.insert(peer->link());

        // Wait for key establishment
        while(_trusted_peers.size() == 0)
            Thread::self()->yield();
    }
}

template<unsigned int UNIT>
void TSTP::init(const NIC & nic)
{
    db<Init, TSTP>(TRC) << "TSTP::init(u=" << UNIT << ")" << endl;

    _nic = new (SYSTEM) NIC(nic);
    TSTP::Locator * locator = new (SYSTEM) TSTP::Locator;
    TSTP::Timekeeper * timekeeper = new (SYSTEM) TSTP::Timekeeper;
    TSTP::Router * router = new (SYSTEM) TSTP::Router;
    //TSTP::Security * security = new (SYSTEM) TSTP::Security;
    TSTP * tstp = new (SYSTEM) TSTP;

    locator->bootstrap();
    timekeeper->bootstrap();
    router->bootstrap();
    //security->bootstrap();

    _nic->attach(tstp, NIC::TSTP);
}

template void TSTP::init<0>(const NIC & nic);
template void TSTP::init<1>(const NIC & nic);
template void TSTP::init<2>(const NIC & nic);
template void TSTP::init<3>(const NIC & nic);
template void TSTP::init<4>(const NIC & nic);
template void TSTP::init<5>(const NIC & nic);
template void TSTP::init<6>(const NIC & nic);
template void TSTP::init<7>(const NIC & nic);

__END_SYS

#endif

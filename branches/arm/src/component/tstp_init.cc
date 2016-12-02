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

    // TODO: we need a better way to define static locations
    if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x1a\x84\x0d\x06", 8))
        _here = TSTP::sink();
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xec\x82\x0d\x06", 8))
        _here = Coordinates(  0,100, 0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xae\x82\x0d\x06", 8))
        _here = Coordinates(100,100, 0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\x67\x83\x0d\x06", 8))
        _here = Coordinates(100, 0, 0);

    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xca\x0e\x16\x06", 8))
        _here = Coordinates(100,100, 0);
    else if(!memcmp(Machine::id(), "\x00\x4b\x12\x00\xee\x0e\x16\x06", 8))
        _here = Coordinates(100, 0, 0);

    else
        _here = Coordinates(-1, -1, -1);

    TSTP::_nic->attach(this, NIC::TSTP);
}

void TSTP::Timekeeper::bootstrap()
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::bootstrap()" << endl;

    TSTP::_nic->attach(this, NIC::TSTP);

    //if(!Traits<TSTP_MAC<void>>::sniffer && (TSTP::here() != TSTP::sink())) { // TODO
    if(TSTP::here() != TSTP::sink()) {
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

    //if(!Traits<TSTP_MAC<void>>::sniffer && (TSTP::here() != TSTP::sink())) { // TODO
    if(TSTP::here() != TSTP::sink()) {
        Peer * peer = new Peer(_id, Region(TSTP::sink(), 0, 0, -1));
        _pending_peers.insert(peer->link());

        // Wait for key establishment
        while(_trusted_peers.size() == 0)
            Thread::self()->yield();
    }
}

void TSTP::init(unsigned int unit)
{
    db<Init, TSTP>(TRC) << "TSTP::init(u=" << unit << ")" << endl;

    _nic = new (SYSTEM) NIC(unit);
    TSTP::Locator * locator = new (SYSTEM) TSTP::Locator;
    TSTP::Timekeeper * timekeeper = new (SYSTEM) TSTP::Timekeeper;
    TSTP::Router * router = new (SYSTEM) TSTP::Router;
    TSTP::Security * security = new (SYSTEM) TSTP::Security;
    TSTP * tstp = new (SYSTEM) TSTP;

    locator->bootstrap();
    timekeeper->bootstrap();
    router->bootstrap();
    security->bootstrap();

    _nic->attach(tstp, NIC::TSTP);
}

__END_SYS

#endif

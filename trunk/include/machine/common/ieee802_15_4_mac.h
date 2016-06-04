// EPOS IEEE 802.15.4 MAC Declarations

#ifndef __ieee802_15_4_mac_h
#define __ieee802_15_4_mac_h

#include <ieee802_15_4.h>

__BEGIN_SYS

template<typename Radio>
class IEEE802_15_4_MAC: public IEEE802_15_4, public IEEE802_15_4::Observed, public Radio
{
private:
    static const unsigned int CSMA_CA_MIN_BACKOFF_EXPONENT = 3;
    static const unsigned int CSMA_CA_MAX_BACKOFF_EXPONENT = 5;
    static const unsigned int CSMA_CA_UNIT_BACKOFF_PERIOD = 320; // us
    static const unsigned int CSMA_CA_MAX_TRANSMISSION_TRIALS = 4;

//    typedef CPU::Log_Addr Log_Addr;
//    typedef CPU::Phy_Addr Phy_Addr;
//    static const unsigned int MTU = IEEE802_15_4::MTU;

public:
    using IEEE802_15_4::Address;

protected:
    IEEE802_15_4_MAC() {}

public:
    // Send a message and wait for it to be correctly sent
    bool send_and_wait(bool ack);

    bool wait_for_ack();

    bool backoff_and_send();

private:
    volatile bool _acked;
    unsigned int _unit;
};

__END_SYS

#endif

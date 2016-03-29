// Wrapper to make TSTP comply with EPOS' API

#ifndef __tstp_nic_h
#define __tstp_nic_h

#include <ethernet.h>
#include <ieee802_15_4.h>

__BEGIN_SYS

class TSTP_NIC : public Traits<TSTP_NIC>::MIMIC, public Traits<TSTP_NIC>::MIMIC::Observed {
    friend class TSTP;
    typedef Traits<TSTP_NIC>::MIMIC Base;
public:
    typedef Base::Address Address;
    typedef Base::Protocol Protocol;
    typedef Base::Statistics Statistics;
    typedef Base::Buffer Buffer;
    TSTP_NIC(unsigned int unit = 0) {}
    ~TSTP_NIC() {}
    int send(const Address & dst, const Protocol & prot, const void * data, unsigned int size) { return 0; }
    int receive(Address * src, Protocol * prot, void * data, unsigned int size) { return 0; }
    Buffer * alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload) { return 0; }
    int send(Buffer * buf) { return 0; }
    void free(Buffer * buf) { }
    const unsigned int mtu() { return 0; };
    const Address broadcast() { return _address; }
    const Address & address() { return _address; }
    void address(const Address &) { }
    const Statistics & statistics() { return _statistics; }
    void reset() { }
    const unsigned int channel() { return 0; }
    void channel(unsigned int channel) { }
    void listen() { }
    void stop_listening() { }
    static TSTP_NIC * get(unsigned int unit = 0) { return 0; }

    template<unsigned int UNIT = 0>
    static void init(unsigned int unit = UNIT); 
private:
    Address _address;
    Statistics _statistics;
};

__END_SYS

#endif

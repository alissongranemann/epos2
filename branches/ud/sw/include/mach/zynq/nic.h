#ifndef __zynq_nic_h
#define __zynq_nic_h

#include <ethernet.h>

__BEGIN_SYS

class Zynq_NIC: public Ethernet
{
public:

    Zynq_NIC(unsigned int u = 0) {
    }

    ~Zynq_NIC() { }

    int send(const Address & dst, const Protocol & prot,
             const void * data, unsigned int size) {
        return 0;
    }

    int receive(Address * src, Protocol * prot,
                void * data, unsigned int size) {
        return 0;
    }

    void reset() { }

    unsigned int mtu() const { return 0; }

    const Address & address() { return _address; }

    const Statistics & statistics() { return _statistics; }

    static void init();

private:
    Address _address;
    Statistics _statistics;
};

__END_SYS

#endif

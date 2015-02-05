// EPOS Zynq NIC Mediator Declarations

#ifndef __zynq_nic_h
#define __zynq_nic_h

#include <ethernet.h>

__BEGIN_SYS

class Zynq_NIC: public Ethernet
{
public:
    Zynq_NIC(unsigned int u = 0) {}

    ~Zynq_NIC() {}

    // TODO: alloc(), send() and free() implementations are just workarounds to
    // force EPOS compilation
    Buffer * alloc(NIC * nic, const Address & dst, const Protocol & prot,
            unsigned int once, unsigned int always, unsigned int payload) {
        Buffer * buf = 0;

        return buf;
    }
    int send(Buffer * buf) { return 0; }
    void free(Buffer * buf) {}

    int send(const Address & dst, const Protocol & prot, const void * data,
            unsigned int size) {
        return 0;
    }
    int receive(Address * src, Protocol * prot, void * data, unsigned int size) {
        return 0;
    }

    unsigned int mtu() const { return 0; }

    const Address & address() { return _address; }

    const Statistics & statistics() { return _statistics; }

    void reset() {}

    void attach(Observer * obs, const Protocol & prot) {}
    void detach(Observer * obs, const Protocol & prot) {}
    void notify(const Protocol & prot, Buffer * buf) {}

private:
    static void init();

private:
    Address _address;
    Statistics _statistics;
};

__END_SYS

#endif

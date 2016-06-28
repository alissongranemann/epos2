// EPOS Cortex-A NIC Mediator Declarations

#ifndef __cortex_a_nic_h
#define __cortex_a_nic_h

#include <ethernet.h>
#include <system.h>
#include "machine.h"
#include "gem.h"

__BEGIN_SYS

class Cortex_A_Ethernet: public Ethernet::Base
{
    friend class Zynq;

private:
    typedef Traits<Cortex_A_Ethernet>::NICS NICS;
    static const unsigned int UNITS = NICS::Length;

public:
    typedef Data_Observer<Buffer, Protocol> Observer;
    typedef Data_Observed<Buffer, Protocol> Observed;

public:
    template<unsigned int UNIT = 0>
    Cortex_A_Ethernet(unsigned int u = UNIT) {}
    ~Cortex_A_Ethernet() { _dev = 0; }

    int send(const Address & dst, const Protocol & prot, const void * data, unsigned int size) { return 0; }
    int receive(Address * src, Protocol * prot, void * data, unsigned int size) { return 0; }

    Buffer * alloc(const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload) { return 0; }
    int send(Buffer * buf) { return 0; }
    void free(Buffer * buf) {}

    const Address & address() { Address * a = new (SYSTEM) Address; return *a; }
    void address(const Address & address) {}

    const Statistics & statistics() { Statistics * s = new (SYSTEM) Statistics; return *s; }

    void reset() {}

    void attach(Observer * obs, const Protocol & prot) {}
    void detach(Observer * obs, const Protocol & prot) {}
    void notify(const Protocol & prot, Buffer * buf) {}

    Ethernet::NIC * device() { return 0; }

private:
    static void init();

private:
    Ethernet::NIC * _dev;
};

__END_SYS

#endif

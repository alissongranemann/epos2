#ifndef __ether_channel_h
#define __ether_channel_h

#include <ethernet.h>

__BEGIN_SYS

class Ether_Channel: private NIC::Observer
{
public:
    static const bool connectionless = true;

public:
    static int receive(NIC::Buffer * buf, void * data, unsigned int size);

private:
    void update(NIC::Observed * obs, NIC::Protocol prot, NIC::Buffer * buf);
};

__END_SYS

#endif

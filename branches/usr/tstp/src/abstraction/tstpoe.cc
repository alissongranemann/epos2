// EPOS TSTP over Ethernet Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstpoe.h>
#include <tstp.h>

__BEGIN_SYS

// Class attributes
TSTPOE * TSTPOE::_networks[];
TSTPOE::Observed TSTPOE::_observed;

// Methods
void TSTPOE::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf) 
{
    Time_Manager::stamp_receive(buf);
    db<TSTPOE>(TRC) << "TSTPOE::update(obs=" << obs << ",prot=" << hex << prot << dec << ",buf=" << buf << ")" << endl;
    buf->nic(&_nic);
    // TSTPOE assumes any received buffer is indeed for this node
    if(!notify(buf, NIC_Observing_Condition::FOR_ME))
        _nic.free(buf);
}

int TSTPOE::send(Buffer * buf)
{
    db<TSTPOE>(TRC) << "TSTPOE::send(buf=" << buf << ")" << endl;

    Time_Manager::stamp_send(buf);
    return nic()->send(buf); // implicitly releases the buffer
}

__END_SYS

#endif

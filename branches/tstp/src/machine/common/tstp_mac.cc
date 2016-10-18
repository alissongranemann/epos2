// EPOS Trustful SpaceTime Protocol MAC Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <nic.h>

#ifdef __tstp_mac_h

__BEGIN_SYS

// Class attributes

// Methods
template<typename Radio>
void TSTP_MAC<Radio>::free(Buffer * b) { b->nic()->free(reinterpret_cast<NIC::Buffer*>(b)); }

template void TSTP_MAC<CC2538RF>::free(Buffer * b);

__END_SYS

#endif

#endif

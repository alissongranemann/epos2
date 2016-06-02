// EPOS Trustful SpaceTime Protocol MAC Declarations

#ifndef __tstp_mac_h
#define __tstp_mac_h

#include <ieee802_15_4.h>

__BEGIN_SYS

template<typename Radio>
class TSTP_MAC: public Radio {};

__END_SYS

#endif

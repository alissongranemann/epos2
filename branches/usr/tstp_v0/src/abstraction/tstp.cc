// EPOS Trustful SpaceTime Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>
#include <tstp_mac.h>

__BEGIN_SYS

// Class attributes
TSTP::Interests TSTP::_interested;
TSTP::Responsives TSTP::_responsives;

TSTP::Observed TSTP::_observed;

// Methods
Greedy_Geographic_Router::Greedy_Geographic_Router() : _period(TSTP_Timer::us_to_ts(TSTP_MAC::PERIOD)) {
    TSTPNIC::attach(this);
}

__END_SYS

#endif

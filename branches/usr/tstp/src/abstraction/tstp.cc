// EPOS Trustful SpaceTime Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>

__BEGIN_SYS

// Class attributes
TSTP::Interests TSTP::_interested;
TSTP::Responsives TSTP::_responsives;

TSTP::Observed TSTP::_observed;

// PTS class attributes
TSTP_Timer PTS::_timer;

// Static_Locator class attributes
Static_Locator::Coordinates Static_Locator::_here(Traits<Static_Locator>::X, Traits<Static_Locator>::Y, Traits<Static_Locator>::Z);


// Methods

__END_SYS

#endif

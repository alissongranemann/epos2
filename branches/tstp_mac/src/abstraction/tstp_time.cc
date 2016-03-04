#include <tstp.h>
#include <timer.h>

__BEGIN_SYS

PTP::Time PTP::_t0;
PTP::Time PTP::_t1;

void PTP::set() { 
    MAC_Timer::set(_t0 + Traits<TSTP_MAC>::TX_UNTIL_PROCESS_DATA_DELAY);
}

void PTP::adjust() { 
    auto offset = MAC_Timer::us_to_ts(_t1 - _t0);
    MAC_Timer::set(offset + MAC_Timer::read()); 
}

void PTP::init() {
    MAC_Timer::config();
    MAC_Timer::enable();    
}

__END_SYS

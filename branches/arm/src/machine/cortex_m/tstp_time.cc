#include <tstp.h>
#include <machine/cortex_m/cc2538_phy.h>

__BEGIN_SYS

PTP::PTP(unsigned int unit) {
    MAC_Timer::config();
    MAC_Timer::start();
}

PTP::Time PTP::time_now() {
    return MAC_Timer::read();
}

__END_SYS

#include <tstp.h>
#include <ic.h>
#include <machine/cortex_m/cc2538_phy.h>

__BEGIN_SYS

PTP * PTP::_ptp;

PTP::PTP(unsigned int unit) {
    MAC_Timer::config();
    MAC_Timer::start();
    _ptp = this; // TODO
}

PTP::Time PTP::time_now() {
    return MAC_Timer::read();
}

void PTP::int_handler(const unsigned int & interrupt) {
    db<PTP>(TRC) << "PTP::int_handler(interrupt=" << interrupt << ")" << endl;
    _ptp->_tstp->process_event();//TODO
}

void PTP::interrupt(const Time & when) {
    db<PTP>(TRC) << "PTP::interrupt(when=" << when << ")" << endl;
    db<PTP>(TRC) << "now=" << time_now() << endl;
    MAC_Timer::interrupt(when, &int_handler);
}

__END_SYS

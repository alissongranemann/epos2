#include <tstp.h>
#include <ic.h>
#include <machine/cortex_m/cc2538_phy.h>

__BEGIN_SYS

PTS * PTS::_pts;

void PTS::update_interest(char * header) {
    _timer.set(reinterpret_cast<TSTP_API::Header*>(header)->last_hop_time() + TX_DELAY);
}

void PTS::do_update_data(char * header) {
    auto t0 = reinterpret_cast<TSTP_API::Header*>(header)->last_hop_time() + TX_DELAY;
    _timer.set((t0 + _t1) / 2);
}

void PTS::int_handler(const unsigned int & interrupt) {
    db<PTS>(TRC) << "PTS::int_handler(interrupt=" << interrupt << ")" << endl;
    _pts->_tstp->process_event();//TODO: several instances if multiple TSTP networks
}

__END_SYS

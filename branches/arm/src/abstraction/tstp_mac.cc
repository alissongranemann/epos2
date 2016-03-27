#include <tstp_mac.h>

__BEGIN_SYS

void TSTP_MAC::int_handler(const IC::Interrupt_Id & interrupt)
{
    TSTP_MAC * mac = get_by_interrupt(interrupt);

    db<TSTP_MAC>(TRC) << "TSTP_MAC::int_handler(int=" << interrupt << ",mac=" << mac << ")" << endl;

    if(!mac)
        db<TSTP_MAC>(WRN) << "TSTP_MAC::int_handler: handler not assigned!" << endl;
    else
        mac->handle_int();
}

__END_SYS

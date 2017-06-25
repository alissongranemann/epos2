#include <serial_port.h>

__BEGIN_SYS

Serial_Port::Observed Serial_Port::_observed;

void Serial_Port::handle_transmission_complete() {
//    if (!m_pTransmitQueue.empty()) {
//        Message pMsg = m_pTransmitQueue.remove()->object()->msg;
//        handle_tx_message(pMsg);
//    }
}

__END_SYS

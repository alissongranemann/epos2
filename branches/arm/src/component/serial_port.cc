#include <serial_port.h>

__BEGIN_SYS

Serial_Port::Observed Serial_Port::_observed;

void Serial_Port::handle_transmission_complete() {
    if (!m_pTransmitQueue.empty()) {
        Message pMsg = m_pTransmitQueue.remove()->object()->msg;
        //m_transmitBuffer.length = pMsg.length;
        //m_transmitBuffer.current_index = 0;

        //memcpy(m_transmitBuffer.data, pMsg, pMsg.length);
        handle_tx_message(pMsg);
    }
}

__END_SYS

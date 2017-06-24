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

void Serial_Port::handle_receive_complete(){
    //Message pMsg;
    //memcpy(pMsg, m_receiveBuffer.data, m_receiveBuffer.length);
    //pMsg->length = m_receiveBuffer.length;
    // Pass the message to the higher layers
    //m_pReceiveQueue->insert(&pMsg.e);

    //m_receiveBuffer.current_index = 0;
    //notify()
}

__END_SYS

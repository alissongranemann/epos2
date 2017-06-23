#ifndef __serial_port_h
#define __serial_port_h

#include <system/config.h>
#include <utility/queue.h>
#include <machine.h>

__BEGIN_SYS

#define PACKETSIZE sizeof(Message)

class Serial_Port
{

    typedef Data_Observed<bool> Observed;
    typedef Data_Observer<bool> Observer;

public:

    static const unsigned int BUFFER_SIZE = 64;

    struct Message
    {
        Message()
        : type(0), e(this) {}

        unsigned int type;
        char data[BUFFER_SIZE];
        Queue<Message>::Element e;

    };

private:

    IF<Traits<USB>::enabled, USB, UART>::Result io;
    static Observed _observed;

    Queue<Message> m_pReceiveQueue;
    Queue<Message> m_pTransmitQueue;

    Message m_receiveBuffer;
    Message m_transmitBuffer;

public:
    Serial_Port()
    {
        //m_receiveBuffer.length = 0;
        //m_receiveBuffer.current_index = 0;
        //m_transmitBuffer.length = 0;
        //m_transmitBuffer.current_index = 0;

        //io_write(m_interruptStatusRegister, ENABLE_RX_DISABLE_TX_MASK);

        m_pTransmitQueue = Queue<Message>();
        m_pReceiveQueue = Queue<Message>();
    }

    ~Serial_Port()
    {
        //io_write(m_interruptStatusRegister, DISABLE_RX_DISABLE_TX_MASK);
    }

    void epoch(){
        //TSTP::Time epoch = 0;
        char c = io.get();
        if(c != 'X') {
            //epoch += c - '0';
            c = io.get();
            while(c != 'X') {
               //epoch *= 10;
               //epoch += c - '0';
               c = io.get();
               db<TSTP>(TRC) << "Serial_Port::epoch:" << endl;
            }
            //TSTP::epoch(epoch);
        }
    }

    void handle_transmission_complete();

    void handle_receive_complete();

    void handle_tx_message(Message * new_msg) {
        db<TSTP>(TRC) << "Serial_Port::handle_tx_message:" << endl;

        if (!m_pTransmitQueue.empty()) {
            m_pTransmitQueue.insert(&new_msg->e);
            Message * msg = m_pTransmitQueue.remove()->object();
            for(unsigned int i = 0; i < sizeof(Message); i++)
                io.put(reinterpret_cast<const char *>(&msg)[i]);
        } else {
            for(unsigned int i = 0; i < sizeof(Message); i++)
                io.put(reinterpret_cast<const char *>(&new_msg)[i]);
        }
        handle_transmission_complete();
    }

    void handle_rx_message() {
        db<TSTP>(TRC) << "Serial_Port::handle_rx_message:" << endl;

        //receive
        handle_receive_complete();
    }

    static void detach(Observer * obs) { _observed.detach(obs); }
    static bool notify(bool * result) { return _observed.notify(result); }

};

__END_SYS

#endif

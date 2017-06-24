#ifndef __serial_port_h
#define __serial_port_h

#include <system/config.h>
#include <utility/queue.h>
#include <machine.h>

__BEGIN_SYS

#define PACKETSIZE sizeof(Message)

class Serial_Port
{

public:

    typedef Data_Observed<bool> Observed;
    typedef Data_Observer<bool> Observer;

    enum HEADER {

        START = 0x01,
        END = 0x02

    };

    struct Message
    {

        Message() : type(0), x(0), y(0), z(0), r(0), period(0), expiry(0) {}

        unsigned int type;
        long x;
        long y;
        long z;
        unsigned long r;
        unsigned long long period;
        unsigned long long expiry;

    }__attribute__((packed));

    struct MessageWrapper
    {
        MessageWrapper(Message _msg)
        : msg(_msg), e(this) {}

        Message msg;
        Queue<MessageWrapper>::Element e;
    };

private:

    IF<Traits<USB>::enabled, USB, UART>::Result io;
    static Observed _observed;

    Queue<MessageWrapper> m_pReceiveQueue;
    Queue<MessageWrapper> m_pTransmitQueue;

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

        m_pTransmitQueue = Queue<MessageWrapper>();
        m_pReceiveQueue = Queue<MessageWrapper>();
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

    void handle_tx_message(const Message & new_msg) {
        db<TSTP>(TRC) << "Serial_Port::handle_tx_message:" << endl;

        if (!m_pTransmitQueue.empty()) {
            MessageWrapper msgWrapper(new_msg);
            m_pTransmitQueue.insert(&msgWrapper.e);
            Message msg = m_pTransmitQueue.remove()->object()->msg;
            send_message(msg);
        } else {
            send_message(new_msg);
        }
        handle_transmission_complete();
    }

    void send_message(const Message & msg){
        io.put('^');
        for(unsigned int i = 0; i < sizeof(Message); i++)
            io.put(reinterpret_cast<const char *>(&msg)[i]);
        io.put('$');
    }

    void handle_rx_message() {
        db<TSTP>(TRC) << "Serial_Port::handle_rx_message:" << endl;
        char c = io.get();
        notify(new bool(c - '0'));
        handle_receive_complete();
    }

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }
    static bool notify(bool * result) { return _observed.notify(result); }

};

__END_SYS

#endif

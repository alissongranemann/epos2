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

    typedef unsigned char HEADER;
    enum {

        START = '^',
        END = '$'

    };

    template <typename MessageType>
    struct Message
    {
        Message(int _type, MessageType _msg) : type(_type), length(sizeof(MessageType)), msg(_msg) {}

        int type;
        int length;
        MessageType msg;

    }__attribute__((packed));

//    template <typename MessageType>
//    struct MessageWrapper
//    {
//        MessageWrapper(Message _msg)
//        : msg(_msg), e(this) {}
//
//        Message<MessageType> msg;
//        Queue<MessageWrapper<MessageType>>::Element e;
//    };

private:

    IF<Traits<USB>::enabled, USB, UART>::Result io;
    static Observed _observed;

    //Queue<MessageWrapper> m_pTransmitQueue;

public:
    Serial_Port()
    {
        //m_receiveBuffer.length = 0;
        //m_receiveBuffer.current_index = 0;
        //m_transmitBuffer.length = 0;
        //m_transmitBuffer.current_index = 0;

        //io_write(m_interruptStatusRegister, ENABLE_RX_DISABLE_TX_MASK);

       // m_pTransmitQueue = Queue<MessageWrapper>();
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

    template <typename MessageType>
    void handle_tx_message(const Message<MessageType> & new_msg) {
        db<TSTP>(TRC) << "Serial_Port::handle_tx_message:" << endl;

        //if (!m_pTransmitQueue.empty()) {
         //   MessageWrapper msgWrapper(new_msg);
         //   m_pTransmitQueue.insert(&msgWrapper.e);
         //   Message msg = m_pTransmitQueue.remove()->object()->msg;
         //   send_message(msg);
       // } else {
        send_message(new_msg);
       // }
        handle_transmission_complete();
    }

    template <typename MessageType>
    void send_message(const Message<MessageType> & msg){
        CPU::int_disable();
        io.put(START);
        for(unsigned int i = 0; i < sizeof(msg); i++)
            io.put(reinterpret_cast<const char *>(&msg)[i]);
        io.put(END);
        CPU::int_enable();
    }

    void handle_rx_message() {
        db<TSTP>(TRC) << "Serial_Port::handle_rx_message:" << endl;
        char c = io.get();
        notify(new bool(c - '0'));
    }

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }
    static bool notify(bool * result) { return _observed.notify(result); }

};

__END_SYS

#endif

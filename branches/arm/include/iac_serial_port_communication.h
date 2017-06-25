#ifndef __iac_serial_port_communication_h
#define __iac_serial_port_communication_h

#include <system/config.h>
#include <utility/queue.h>
#include <machine.h>

__BEGIN_SYS

#define PACKETSIZE sizeof(Message)

class Iac_Serial_Port_Communication
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

private:

    IF<Traits<USB>::enabled, USB, UART>::Result io;
    static Observed _observed;

public:
    Iac_Serial_Port_Communication();

    ~Iac_Serial_Port_Communication();

    void epoch();

    template <typename MessageType>
    void handle_tx_message(const Message<MessageType> & msg) {
        db<TSTP>(TRC) << "Serial_Port::handle_tx_message:" << endl;
        CPU::int_disable();
        io.put(START);
        for(unsigned int i = 0; i < sizeof(msg); i++)
        io.put(reinterpret_cast<const char *>(&msg)[i]);
        io.put(END);
        CPU::int_enable();
    }

    void handle_rx_message();

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }
    static bool notify(bool * result) { return _observed.notify(result); }

};

__END_SYS

#endif

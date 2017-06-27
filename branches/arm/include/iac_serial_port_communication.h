#ifndef __iac_serial_port_communication_h
#define __iac_serial_port_communication_h

#include <system/config.h>
#include <machine.h>
#include <utility/observer.h>

__BEGIN_SYS

#define PACKETSIZE sizeof(Message)

class Iac_Serial_Port_Communication
{

public:

    typedef unsigned char HEADER;
    enum {
        START = '^',
        END = '$'
    };

    template <typename MessageType>
    struct Message
    {
        Message(int _type, MessageType _msg)
        : start_header(START), type(_type), length(sizeof(MessageType)), end_header(END), msg(_msg) {}

        char start_header;
        int type;
        int length;
        char end_header;
        MessageType msg;

        friend OStream & operator<<(OStream & os, const Message & d) {
            os << "type=" << d.type << ",length" << d.length << ",msg=" < d.msg;
            return os;
        }

    }__attribute__((packed));

private:

    IF<Traits<USB>::enabled, USB, UART>::Result io;
    static Data_Observed<bool, int> _observed;

public:
    Iac_Serial_Port_Communication();

    ~Iac_Serial_Port_Communication();

    void epoch();

    template <typename MessageType>
    void handle_tx_message(const Message<MessageType> & msg) {
        db<TSTP>(TRC) << "Serial_Port::handle_tx_message:" << endl;
        CPU::int_disable();
        for(unsigned int i = 0; i < sizeof(msg); i++)
        io.put(reinterpret_cast<const char *>(&msg)[i]);
        CPU::int_enable();
    }

    void handle_rx_message() {
        db<TSTP>(TRC) << "Serial_Port::handle_rx_message:" << endl;
        char c = io.get();
        int subject = 0;
        if(c != 'X') {
            subject += c - '0';
            c = io.get();
            while(c != 'X') {
                subject *= 10;
                subject += c - '0';
                c = io.get();
            }
        }
        c = io.get();
        notify(subject, new bool(c - '0'));
    }

    static void attach(Data_Observer<bool, int> * obs, void * subject) {_observed.attach(obs, int(subject));}
    static void detach(Data_Observer<bool, int> * obs, void * subject) {_observed.detach(obs, int(subject));}
    static bool notify(int subject, bool * result) {return _observed.notify(subject, result);}

};

__END_SYS

#endif

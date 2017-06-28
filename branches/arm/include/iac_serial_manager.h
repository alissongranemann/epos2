#ifndef __iac_serial_port_communication_h
#define __iac_serial_port_communication_h

#include <system/config.h>
#include <machine.h>
#include <utility/observer.h>

__BEGIN_SYS

#define PACKETSIZE sizeof(Message)

class IAC_Serial_Manager
{

public:

    typedef unsigned char HEADER;
    enum
    {
        START = '^', END = '$'
    };

    template<typename MessageType>
    struct Message
    {
        Message(int _type, MessageType _msg)
        : start_header(START),
        type(_type),
        length(sizeof(MessageType)),
        end_header(END),
        msg(_msg)
        {
        }

        char start_header;
        int type;
        int length;
        char end_header;
        MessageType msg;

        friend OStream & operator<<(OStream & os, const Message & d)
        {
            os << "type=" << d.type << ",length" << d.length << ",msg=" < d.msg;
            return os;
        }

    }__attribute__((packed));

private:

    typedef IF<Traits<USB>::enabled, USB, UART>::Result IO;

    static IO io;
    static Data_Observed<bool, int> _observed;

public:
    IAC_Serial_Manager();

    ~IAC_Serial_Manager();

    static unsigned long long epoch();

    template<typename MessageType>
    static void handle_tx_message(const Message<MessageType> & msg)
    {
        db < TSTP > (TRC) << "Serial_Port::handle_tx_message:" << endl;
        CPU::int_disable();
        for(unsigned int i = 0; i < sizeof(msg); i++)
        io.put(reinterpret_cast<const char *>(&msg)[i]);
        CPU::int_enable();
    }

    static void handle_rx_message()
    {
        db < TSTP > (TRC) << "Serial_Port::handle_rx_message:" << endl;

        read_header();
    }

    static void attach(Data_Observer<bool, int> * obs, int subject)
    {
        _observed.attach(obs, subject);
    }
    static void detach(Data_Observer<bool, int> * obs, void * subject)
    {
        _observed.detach(obs, int(subject));
    }
    static bool notify(int subject, bool * result)
    {
        return _observed.notify(subject, result);
    }

private:

    static void read_header() {
            int msgs = 0;
            while(io.get() != START) {}
            char c = io.get();
            while(c != END) {
                msgs *= 10;
                msgs += c - '0';
                c = io.get();
            }
            read_msg(msgs);
        }

        static void read_msg(int times) {
            if(times > 0) {
                int subject = 0;
                char c = io.get();
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
                read_msg(--times);
            }
        }

};

__END_SYS

#endif

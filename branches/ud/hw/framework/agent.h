#ifndef __agent_hw_h
#define __agent_hw_h

#include <system/config.h>
#include <framework/agent.h>

__BEGIN_SYS

class Message
{
private:
    typedef Serializer<8> SerDes;
    typedef SerDes::Packet Packet;

public:
    enum {
        COMPONENT = 0x10,

        ADDER_ADD = COMPONENT,

        GET_INST_ID,
        SET_INST_ID,

        UNDEFINED = -1
    };

public:
    Message(Channel & rx_ch, Channel & tx_ch): _serdes(), _rx_ch(rx_ch),
        _tx_ch(tx_ch) {}

    template<unsigned int N_ARGS, typename T0, typename T1>
    void in2(T0 & a0, T1 & a1) {
        Packet * buff = _serdes.get_pkt_buf();

        _serdes.reset();

        for (unsigned int i = 0; i < N_ARGS; i++) {
            _rx_ch.read(_msg);
            buff[i] = _msg.payload;
        }

        _serdes.reset();

        _serdes.deserialize(a0);
        _serdes.deserialize(a1);
    }

    template<unsigned int N_RET, typename T0>
    void ret(T0 & t0) {
        Packet * buff = _serdes.get_pkt_buf();

        _serdes.reset();
        _serdes.serialize(t0);

        // _msg.addr.{x,y,local} and _msg.header.{type_id, inst_id} are the same
        // from the received message
        _msg.header.type = RMI_Msg::TYPE_RESP_DATA;

        for (unsigned int i = 0; i < N_RET; i++) {
            _msg.payload = buff[i];
            _tx_ch.write(_msg);
        }
    }

    unsigned int method() {
        _rx_ch.read(_msg);
        return _msg.payload;
    }

private:
    SerDes _serdes;
    Channel & _rx_ch;
    Channel & _tx_ch;
    RMI_Msg _msg;
};

__END_SYS

#define HLS_TOP_LEVEL(T)\
void T##_Top(Channel & rx_ch, Channel & tx_ch) {\
    static Agent<T> agent(rx_ch, tx_ch);\
\
    agent.exec();\
}

#endif

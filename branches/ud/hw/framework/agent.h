#ifndef __agent_hw_h
#define __agent_hw_h

#include <system/config.h>
#include <framework/serializer.h>
#include <framework/agent.h>

__BEGIN_SYS

class Message
{
private:
    typedef Serializer::Packet Packet;

    static const unsigned int MAX_PARAMETERS_SIZE = 20;

public:
    enum {
        COMPONENT = 0x10,

        ADDER_ADD = COMPONENT,

        GET_INST_ID,
        SET_INST_ID,

        UNDEFINED = -1
    };

public:
    Message(Channel & rx_ch, Channel & tx_ch): _rx_ch(rx_ch), _tx_ch(tx_ch) {}

    unsigned int method() {
        _rx_ch.read(_msg);
        return _msg.payload;
    }

    template<typename T0, typename T1>
    void in2(T0 & a0, T1 & a1) {
        for (unsigned int i = 0; i < type_to_npkt_2<T0, T1>::Result; i++) {
            _rx_ch.read(_msg);
            _parms[i] = _msg.payload;
        }

        Serializer::deserialize(&_parms[0], 0, a0);
        Serializer::deserialize(&_parms[0], type_to_npkt_1<T0>::Result, a1);
    }

    template<typename T0>
    void ret(T0 & a0) {
        // _msg.addr.{x,y,local} and _msg.header.{type_id, inst_id} are the same
        // from the received message
        _msg.header.type = RMI_Msg::TYPE_RESP_DATA;

        Serializer::serialize(&_parms[0], 0, a0);

        for (unsigned int i = 0; i < type_to_npkt_1<T0>::Result; i++) {
            _msg.payload = _parms[i];
            _tx_ch.write(_msg);
        }
    }

private:
    Channel & _rx_ch;
    Channel & _tx_ch;
    RMI_Msg _msg;
    Packet _parms[MAX_PARAMETERS_SIZE];
};

__END_SYS

#define HLS_TOP_LEVEL(T)\
void T##_Top(Channel & rx_ch, Channel & tx_ch) {\
    static Agent<T> agent(rx_ch, tx_ch);\
\
    agent.exec();\
}

#endif

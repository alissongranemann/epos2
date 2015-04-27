#ifndef __message_hw_h
#define __message_hw_h

#include <system/config.h>
#include <framework/rtsnoc.h>
#include <ac_channel.h>

#include "serializer.h"

__BEGIN_SYS

class Message
{
private:
    typedef Serializer::Buffer Buffer;
    typedef RTSNoC::Packet Packet;

    static const unsigned int MAX_PARAMETERS_SIZE = 20;

public:
    // The message's members order above will yield the following packet
    // organization:
    // 79...72 71...64 63...56 55...48 47...49 39...32 31...0
    // local   y       x       type_id inst_id type    payload
    typedef ac_channel<Packet> Channel;

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
        for (unsigned int i = 0; i < Serializer::npkt2<T0, T1>::Result; i++) {
            _rx_ch.read(_msg);
            _parms[i] = _msg.payload;
        }

        Serializer::deserialize(&_parms[0], 0, a0);
        Serializer::deserialize(&_parms[0], Serializer::npkt1<T0>::Result, a1);
    }

    template<typename T0>
    void ret(T0 & a0) {
        // _msg.addr.{x,y,local} and _msg.header.{type_id, inst_id} are the same
        // from the received message
        _msg.header.type = RTSNoC::RESP_DATA;

        Serializer::serialize(&_parms[0], 0, a0);

        for (unsigned int i = 0; i < Serializer::npkt1<T0>::Result; i++) {
            _msg.payload = _parms[i];
            _tx_ch.write(_msg);
        }
    }

private:
    Channel & _rx_ch;
    Channel & _tx_ch;
    Packet _msg;
    Buffer _parms[MAX_PARAMETERS_SIZE];
};

__END_SYS

#define HLS_TOP_LEVEL(T)\
void T##_Top(Message::Channel & rx_ch, Message::Channel & tx_ch) {\
    static Agent<T> agent(rx_ch, tx_ch);\
\
    agent.exec();\
}

#endif

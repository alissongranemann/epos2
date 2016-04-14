#ifndef __message_h
#define __message_h

#include <hls_stream.h>
#include <ap_int.h>

#include <system/config.h>

#include "../../unified/rtsnoc.h"
#include "serializer.h"

__BEGIN_SYS

class Message
{
private:
    typedef Serializer::Buffer Buffer;
    typedef RTSNoC::Packet Packet;

    static const unsigned int MAX_PARAMETERS_SIZE = 20;

public:
    // FIXME: This should be shared between software and hardware Message to
    // avoid inconsistencies
    enum {
        CREATE,
        CREATE1,
        CREATE2,
        CREATE3,
        CREATE4,
        CREATE5,
        CREATE6,
        CREATE7,
        CREATE8,
        DESTROY,
        SELF,
        COMPONENT = 0x10,

        ADDER_ADD = COMPONENT,
        ADDER_REST_ST,
        ADDER_SAVE_ST,
        ADDER_GET_ST_LEN,

        GET_INST_ID,
        SET_INST_ID,

        UNDEFINED = -1
    };

    typedef int Method;
    typedef Method Result;

    typedef ap_uint<80> u80;
    typedef hls::stream<u80> Channel;

public:
    Message(Channel & rx_ch, Channel & tx_ch): _rx_ch(rx_ch), _tx_ch(tx_ch) {}

    unsigned int method() {
        _rx_ch.read(_u);
        pack(_u, _msg);
        return _msg.payload;
    }

    template<typename T0, typename T1>
    void in2(T0 & a0, T1 & a1) {
        for (unsigned int i = 0; i < Serializer::NPKT2<T0, T1>::Result; i++) {
            _rx_ch.read(_u);
            pack(_u, _msg);
            _parms[i] = _msg.payload;
        }

        Serializer::deserialize(&_parms[0], 0, a0);
        Serializer::deserialize(&_parms[0], Serializer::NPKT1<T0>::Result, a1);
    }

    template<typename T0>
    void ret(T0 & a0) {
        // _msg.addr.{x,y,local} and _msg.header.{type_id, inst_id} are the same
        // from the received message
        _msg.header.type = RTSNoC::RESP_DATA;

        Serializer::serialize(&_parms[0], 0, a0);

        for (unsigned int i = 0; i < Serializer::NPKT1<T0>::Result; i++) {
            _msg.payload = _parms[i];
            unpack(_u, _msg);
            _tx_ch.write(_u);
        }
    }

private:
    void pack(u80 &u, Packet &msg) {
        msg.payload = u.range(31, 0);
        msg.header.type = u.range(39, 32);
        msg.header.inst_id = u.range(47, 40);
        msg.header.type_id = u.range(55, 48);
        msg.addr._x = u.range(63, 56);
        msg.addr._y = u.range(71, 64);
        msg.addr._local = u.range(79, 72);
    }

    void unpack(u80 &u, Packet &msg) {
        u(31, 0) = msg.payload;
        u(39, 32) = msg.header.type;
        u(47, 40) = msg.header.inst_id;
        u(55, 48) = msg.header.type_id;
        u(63, 56) = msg.addr._x;
        u(71, 64) = msg.addr._y;
        u(79, 72) = msg.addr._local;
    }

private:
    Channel & _rx_ch;
    Channel & _tx_ch;
    Packet _msg;
    u80 _u;
    Buffer _parms[MAX_PARAMETERS_SIZE];
};

__END_SYS

#endif

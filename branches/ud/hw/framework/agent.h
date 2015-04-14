#ifndef __agent_hw_h
#define __agent_hw_h

#include <system/config.h>
#include <framework/agent.h>

#include "catapult.h"

__BEGIN_SYS

template<class T>
class Agent_Common<T, true>: public Serializer<Traits<T>::serdes_buffer>
{
public:
    Agent_Common(Channel_t & rx_ch, Channel_t & tx_ch): Base(), _call_ch(rx_ch),
        _return_ch(tx_ch) {}

    template<unsigned int N_ARGS>
    bool read_args() {
        typename Base::pkt * buff = Base::get_pkt_buf();

        for (int i = 0; i < N_ARGS; i++) {
            _call_ch.read(_msg);
            buff[i] = _msg.payload;
        }

        Base::set_pkt_cnt(N_ARGS);

        return true;
    }

    template<unsigned int N_RET>
    void write_return() {
        typename Base::pkt * buff = Base::get_pkt_buf();

        // _msg.addr.{x,y,local} and _msg.header.{type_id, inst_id} are the same
        // from the received message
        _msg.header.type = MSG_TYPE_RESP_DATA;

        for (unsigned int i = 0; i < N_RET; i++) {
            _msg.payload = buff[i];
            _return_ch.write(_msg);
        }
    }

    void finish() {}

    void top_level() { static_cast<Agent<T>*>(this)->dispatch(read_header()); }

private:
    unsigned int read_header() {
        _call_ch.read(_msg);

        return _msg.payload;
    }

private:
    typedef Serializer<Traits<T>::serdes_buffer> Base;

    Channel_t & _call_ch;
    Channel_t & _return_ch;

    Catapult::RMI_Msg _msg;
};

template<typename T>
class Agent_Dummy
{
public:
    Agent_Dummy(Channel_t & rx_ch, Channel_t & tx_ch): _rx_ch(rx_ch),
        _tx_ch(tx_ch) {}

    void top_level() {
        _rx_ch.read(_msg);
        _tx_ch.write(_msg);
    }

private:
    Channel_t & _rx_ch;
    Channel_t & _tx_ch;

    Catapult::RMI_Msg _msg;
};

__END_SYS

#define HLS_TOP_LEVEL(T)\
void T##_Top(_SYS::Channel_t &rx_ch, _SYS::Channel_t &tx_ch) {\
    static _SYS::IF<_SYS::Traits<_SYS::T>::hardware,\
                    _SYS::Agent<_SYS::T>,\
                    _SYS::Agent_Dummy<_SYS::T> >::Result\
        agent(rx_ch, tx_ch);\
\
    agent.top_level();\
}

#endif

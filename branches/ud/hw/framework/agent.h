#ifndef __agent_hw_h
#define __agent_hw_h

#include <system/config.h>
#include <framework/agent.h>

__BEGIN_SYS

template<class T>
class Agent_Common<T, true>: public Serializer<Traits<T>::serdes_buffer>
{
public:
    Agent_Common(Channel & rx_ch, Channel & tx_ch): Base(), _call_ch(rx_ch),
        _return_ch(tx_ch) {}

    template<unsigned int N_ARGS>
    bool read_args() {
        Base::pkt * buff = Base::get_pkt_buf();

        for (unsigned int i = 0; i < N_ARGS; i++) {
            _call_ch.read(_msg);
            buff[i] = _msg.payload;
        }

        return true;
    }

    template<unsigned int N_RET>
    void write_return() {
        Base::pkt * buff = Base::get_pkt_buf();

        // _msg.addr.{x,y,local} and _msg.header.{type_id, inst_id} are the same
        // from the received message
        _msg.header.type = RMI_Msg::TYPE_RESP_DATA;

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

    Channel & _call_ch;
    Channel & _return_ch;

    RMI_Msg _msg;
};

template<typename T>
class Agent_Dummy
{
public:
    Agent_Dummy(Channel & rx_ch, Channel & tx_ch): _rx_ch(rx_ch),
        _tx_ch(tx_ch) {}

    void top_level() {
        _rx_ch.read(_msg);
        _tx_ch.write(_msg);
    }

private:
    Channel & _rx_ch;
    Channel & _tx_ch;

    RMI_Msg _msg;
};

__END_SYS

#define HLS_TOP_LEVEL(T)\
void T##_Top(_SYS::Channel &rx_ch, _SYS::Channel &tx_ch) {\
    static _SYS::IF<_SYS::Traits<_SYS::T>::hardware,\
                    _SYS::Agent<_SYS::T>,\
                    _SYS::Agent_Dummy<_SYS::T> >::Result\
        agent(rx_ch, tx_ch);\
\
    agent.top_level();\
}

#endif

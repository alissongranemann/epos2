#ifndef __agent_hw_h
#define __agent_hw_h

#include "catapult.h"
#include "../../unified/framework/meta.h"
#include "../../unified/framework/agent.h"

namespace Implementation {

template<class Component>
class Agent_Common<Component, Configurations::EPOS_SOC_Catapult, true>:
        public Serializer<Traits<Component>::serdes_buffer>
{
public:
    Agent_Common(Channel_t & rx_ch, Channel_t & tx_ch, unsigned int inst_id):
            Base(), _call_ch(rx_ch), _return_ch(tx_ch), _inst_id(inst_id),
            _last_call_X(0xFF), _last_call_Y(0xFF), _last_call_local(0xFF) {}

    template<unsigned int N_ARGS>
    bool read_args() {
        typename Base::pkt_type * buff = Base::get_pkt_buffer();

        for (int i = 0; i < N_ARGS; i++) {
            _call_ch.read(_msg);
            buff[i] = _msg.phy_data.payload;
        }

        Base::set_pkt_cnt(N_ARGS);

        return true;
    }

    void finish() {}

    template<unsigned int N_RET>
    void write_return() {
        _msg.phy_addr.X = _last_call_X;
        _msg.phy_addr.Y = _last_call_Y;
        _msg.phy_addr.local = _last_call_local;

        _msg.phy_data.header.type_id = Type2Id<Component>::ID;
        _msg.phy_data.header.instance_id = _inst_id;
        //FIXME RESP initiation msgs are not really needed
        //ret_msg.header.msg_type = MSG_TYPE_RESP;
        //Msg_Payload_Init init(ret_msg.payload); init.set_op_id(op_id);
        //return_ch.write(ret_msg);
        _msg.phy_data.header.msg_type = MSG_TYPE_RESP_DATA;

        typename Base::pkt_type * buff = Base::get_pkt_buffer();

        for (int i = 0; i < N_RET; i++) {
            _msg.phy_data.payload = buff[i];
            _return_ch.write(_msg);
        }
    }

    void top_level() { static_cast<Agent<Component>*>(this)->dispatch(read_header()); }

protected:
    unsigned int _inst_id;

private:
    unsigned int read_header() {
        _call_ch.read(_msg);

        _last_call_X = _msg.phy_addr.X;
        _last_call_Y = _msg.phy_addr.Y;
        _last_call_local = _msg.phy_addr.local;

        return _msg.phy_data.payload.to_uint();
    }

private:
    typedef Serializer<Traits<Component>::serdes_buffer> Base;

    Channel_t & _call_ch;
    Channel_t & _return_ch;

    unsigned char _last_call_X;
    unsigned char _last_call_Y;
    unsigned char _last_call_local;

    Catapult::RMI_Msg _msg;
};

template<typename Component>
class Agent_Dummy
{
public:
    Agent_Dummy(Channel_t & rx_ch, Channel_t & tx_ch, unsigned int inst_id):
            _rx_ch(rx_ch), _tx_ch(tx_ch) {}

    void top_level() {
        _rx_ch.read(_msg);
        _tx_ch.write(_msg);
    }

private:
    Channel_t & _rx_ch;
    Channel_t & _tx_ch;

    Catapult::RMI_Msg _msg;
};

};

#define HLS_TOP_LEVEL(Component)\
void Component (Implementation::Channel_t &rx_ch, Implementation::Channel_t &tx_ch, unsigned int inst_id) {\
    static Implementation::IF<Implementation::Traits<Implementation::Component>::hardware,\
                              Implementation::Agent<Implementation::Component>,\
                              Implementation::Agent_Dummy<Implementation::Component> >::Result\
        agent(rx_ch, tx_ch, inst_id);\
\
    agent.top_level();\
}

#endif

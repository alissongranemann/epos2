#ifndef __agent_sw_h
#define __agent_sw_h

#include <component_controller.h>
#include "../../../unified/framework/agent.h"

using EPOS::Component_Controller;

namespace Implementation {

template<class T>
class Agent_Common<T, Configurations::EPOS_SOC_Catapult, false>:
        public Serializer<Traits<T>::serdes_buffer>
{
public:
    typedef Serializer<Traits<T>::serdes_buffer> Base;

public:
    Agent_Common(Channel_t &rx_ch, Channel_t &tx_ch, unsigned int inst_id):
            Base(), _idle(true), _op_id(0), _buffer(0) {}

    void state_machine(Component_Controller::agent_call_info &call_info) {
        if(_idle) {
            _idle = false;
            _op_id = Component_Controller::receive_call(call_info.buffer);
            _buffer = call_info.buffer;
        } else {
            typename Base::pkt_type * data_buffer = Base::get_pkt_buffer();
            data_buffer[Base::get_pkt_cnt()] =
                Component_Controller::receive_call_data(_buffer);
            Base::set_pkt_cnt(Base::get_pkt_cnt() + 1);
        }
    }

    template<unsigned int ARGS>
    bool read_args() { return ARGS == Base::get_pkt_cnt(); }

    void finish() { _idle = true; }

    template<unsigned int N_RET>
    void write_return() {
        typename Base::pkt_type * data_buffer = Base::get_pkt_buffer();

        Component_Controller::send_return_data(_buffer, N_RET, data_buffer);

        Base::set_pkt_cnt(0);
    }

     void top_level(Component_Controller::agent_call_info &call_info) {
         state_machine(call_info);
         static_cast<Agent<T> *>(this)->dispatch(_op_id);
     }

     static void static_top_level(Component_Controller::agent_call_info &call_info) {
        reinterpret_cast<Agent<T> *>((void *)call_info.object_address)->top_level(call_info);
    }

protected:
    unsigned int _inst_id;

private:
    bool _idle;
    unsigned int _op_id;
    unsigned int _buffer;
};

};

#endif

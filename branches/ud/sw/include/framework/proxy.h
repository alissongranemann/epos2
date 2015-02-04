#ifndef __proxy_sw_h
#define __proxy_sw_h

#include <system/ctti.h>
#include <system/types.h>
#include <component_manager.h>

using EPOS::Component_Manager;
using EPOS::SYSTEM;

namespace Implementation {

template<typename Component>
class Proxy_Common<Component, Configurations::EPOS_SOC_Catapult, false>:
    Serializer<Traits<Component>::serdes_buffer>
{
public:
    // No arguments, return
    template<unsigned int OP, typename RET>
    RET call_r() {
        Component_Manager::call(_buf, OP, 0, type_to_npkt_1<RET>::Result,
            Base::get_pkt_buffer());
        Base::set_pkt_cnt(type_to_npkt_1<RET>::Result);
        Base::reset();
        RET ret;
        Base::deserialize(ret);

        return ret;
    }

    // No arguments, no return
    template<unsigned int OP>
    void call() {
        Component_Manager::call(_buf, OP, 0, 0, 0);
    }

    // One argument, return
    template<unsigned int OP, typename RET, typename ARG0>
    RET call_r(ARG0 &arg0) {
        Base::reset();
        Base::serialize(arg0);
        Component_Manager::call(_buf, OP, type_to_npkt_1<ARG0>::Result,
            type_to_npkt_1<RET>::Result, Base::get_pkt_buffer());
        Base::set_pkt_cnt(type_to_npkt_1<RET>::Result);
        Base::reset();
        RET ret;
        Base::deserialize(ret);

        return ret;
    }

    // Two arguments, return
    template<unsigned int OP, typename RET, typename ARG0, typename ARG1>
    RET call_r(ARG0 &arg0, ARG1 &arg1) {
        Base::reset();
        Base::serialize(arg0);
        Base::serialize(arg1);
        Component_Manager::call(_buf, OP,
            type_to_npkt_2<ARG0,ARG1>::Result, type_to_npkt_1<RET>::Result,
            Base::get_pkt_buffer());
        Base::set_pkt_cnt(type_to_npkt_1<RET>::Result);
        Base::reset();
        RET ret;
        Base::deserialize(ret);

        return ret;
    }

    // Four arguments, return
    template<unsigned int OP, typename RET, typename ARG0, typename ARG1,
        typename ARG2, typename ARG3>
    RET call_r(ARG0 &arg0, ARG1 &arg1, ARG2 &arg2, ARG3 &arg3) {
        Base::reset();
        Base::serialize(arg0);
        Base::serialize(arg1);
        Base::serialize(arg2);
        Base::serialize(arg3);
        Component_Manager::call(_buf, OP,
            type_to_npkt_4<ARG0,ARG1,ARG2,ARG3>::Result,
            type_to_npkt_1<RET>::Result, Base::get_pkt_buffer());
        Base::set_pkt_cnt(type_to_npkt_1<RET>::Result);
        Base::reset();
        RET ret;
        Base::deserialize(ret);

        return ret;
    }

    // Eight arguments, return
    template<unsigned int OP, typename RET, typename ARG0, typename ARG1,
        typename ARG2, typename ARG3, typename ARG4, typename ARG5,
        typename ARG6, typename ARG7>
    RET call_r(ARG0 &arg0, ARG1 &arg1, ARG2 &arg2, ARG3 &arg3, ARG4 &arg4,
            ARG5 &arg5, ARG6 &arg6, ARG7 &arg7) {
        Base::reset();
        Base::serialize(arg0);
        Base::serialize(arg1);
        Base::serialize(arg2);
        Base::serialize(arg3);
        Base::serialize(arg4);
        Base::serialize(arg5);
        Base::serialize(arg6);
        Base::serialize(arg7);
        Component_Manager::call(_buf, OP,
            type_to_npkt_8<ARG0,ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7>::Result,
            type_to_npkt_1<RET>::Result, Base::get_pkt_buffer());
        Base::set_pkt_cnt(type_to_npkt_1<RET>::Result);
        Base::reset();
        RET ret;
        Base::deserialize(ret);

        return ret;
    }

    // One argument, no return
    template<unsigned int OP, typename ARG0>
    void call(ARG0 &arg0) {
        Base::reset();
        Base::serialize(arg0);
        Component_Manager::call(_buf, OP, type_to_npkt_1<ARG0>::Result, 0,
            Base::get_pkt_buffer());
    }

    // Two arguments, no return
    template<unsigned int OP, typename ARG0, typename ARG1>
    void call(ARG0 &arg0, ARG1 &arg1) {
        Base::reset();
        Base::serialize(arg0);
        Base::serialize(arg1);
        Component_Manager::call(_buf, OP,
            type_to_npkt_2<ARG0,ARG1>::Result, 0, Base::get_pkt_buffer());
    }

protected:
    Proxy_Common(Channel_t &rx_ch, Channel_t &tx_ch, unsigned int inst_id) {
        _buf = new (SYSTEM) Component_Manager::Buffer(Type2Id<Component>::ID, inst_id);

        Component_Manager::recfg(_buf, Type2Id<Component>::ID);

        // TODO: The Component_Manager should handle inst_ids, not the Proxy,
        // fix it
        set_inst_id(inst_id);
    }

    ~Proxy_Common() { delete _buf; }

private:
    typedef Serializer<Traits<Component>::serdes_buffer> Base;

private:
    // The methods are implemented by all agents
    void set_inst_id(unsigned int id) { call<Component::OP_SET_INST_ID, unsigned int>(id); }
    unsigned int get_inst_id() { return call_r<Component::OP_GET_INST_ID, unsigned int>(); }

private:
    Component_Manager::Buffer * _buf;
};

};

#endif

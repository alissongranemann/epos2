#ifndef __agent_h
#define __agent_h

#include <system/config.h>
#include <framework/scenario_adapter.h>
#include <components/adder.h>

#include "../../hw/framework/agent.h"

__BEGIN_SYS

template<typename Component>
class Agent;

// EPOS Component Framework - Component Agent

template<typename Component>
class Agent_Common: public Message
{
public:
    Agent_Common(Channel & rx_ch, Channel & tx_ch): Message(rx_ch, tx_ch), T() {}

public:
    Scenario_Adapter<Component> T;
};

template<>
class Agent<Adder>: public Agent_Common<Adder>
{
public:
    Agent(Channel & rx_ch, Channel & tx_ch): Agent_Common<Adder>(rx_ch, tx_ch) {}

    void exec() {
        switch(method()) {
        case ADDER_ADD: {
            int a0, a1, res;
            in2(a0, a1);
            res = T.add(a0, a1);
            ret(res);
            break;
        }
        default:
            break;
        }
    }
};

__END_SYS

// Helpers
#define AGENT_BEGIN(name)\
template<>\
class Agent<name>: public Message<name>\
{\
private:\
    typedef Message<name> Base;\
\
private:\
    Scenario_Adapter<name> T;\
\
public:\
    Agent(Channel &rx_ch, Channel &tx_ch): Message<name>(rx_ch, tx_ch), T(rx_ch, tx_ch) {}\
\
    void exec(unsigned int op) {\
        Base::reset();\
        switch (op) {

#define AGENT_END \
        default:\
            break;\
        }\
    }\
};

#define D_CALL_R_4(operation, OP_ID, ret_type_0, type_0, type_1, type_2, type_3)\
case OP_ID: {\
    if(!Base::read_args<type_to_npkt_4<type_0,type_1,type_2,type_3>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    type_1 t1; Base::deserialize(t1);\
    type_2 t2; Base::deserialize(t2);\
    type_3 t3; Base::deserialize(t3);\
    ret_type_0 resp = T.operation(t0,t1,t2,t3);\
    Base::reset();\
    Base::serialize(resp);\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    \
    break;\
}

#define D_CALL_R_8(operation, OP_ID, ret_type_0, type_0, type_1, type_2, type_3, type_4, type_5, type_6, type_7)\
case OP_ID: {\
    if(!Base::read_args<type_to_npkt_8<type_0,type_1,type_2,type_3,type_4,type_5,type_6,type_7>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    type_1 t1; Base::deserialize(t1);\
    type_2 t2; Base::deserialize(t2);\
    type_3 t3; Base::deserialize(t3);\
    type_4 t4; Base::deserialize(t4);\
    type_5 t5; Base::deserialize(t5);\
    type_6 t6; Base::deserialize(t6);\
    type_7 t7; Base::deserialize(t7);\
    ret_type_0 resp = T.operation(t0,t1,t2,t3,t4,t5,t6,t7);\
    Base::reset();\
    Base::serialize(resp);\
\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    break;\
}

#define D_CALL_R_2(operation, OP_ID, ret_type_0, type_0, type_1)\
case OP_ID: {\
    if(!Base::read_args<type_to_npkt_2<type_0,type_1>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    type_1 t1; Base::deserialize(t1);\
    ret_type_0 resp = T.operation(t0,t1);\
    Base::reset();\
    Base::serialize(resp);\
\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    break;\
}

#define D_CALL_2(operation, OP_ID, type_0, type_1)\
case OP_ID: {\
    if(!Base::read_args<type_to_npkt_2<type_0,type_1>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    type_1 t1; Base::deserialize(t1);\
    T.operation(t0,t1);\
    Base::finish();\
    break;\
}

#define D_CALL_R_1(operation, OP_ID, ret_type_0, type_0)\
case OP_ID: {\
    if(!Base::read_args<type_to_npkt_1<type_0>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    ret_type_0 resp = T.operation(t0);\
    Base::reset();\
    Base::serialize(resp);\
\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    break;\
}

#define D_CALL_1(operation, OP_ID, type_0)\
case OP_ID: {\
    if(!Base::read_args<type_to_npkt_1<type_0>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    T.operation(t0);\
    Base::finish();\
    break;\
}

#define D_CALL_R_0(operation, OP_ID, ret_type_0)\
case OP_ID: {\
    ret_type_0 resp = T.operation();\
    Base::reset();\
    Base::serialize(resp);\
\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    break;\
}

#define D_CALL_0(operation, OP_ID)\
case OP_ID: {\
    T.operation();\
    Base::finish();\
    break;\
}

#endif

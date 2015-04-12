#ifndef __agent_h
#define __agent_h

#include <system/config.h>
#include <framework/serializer.h>
#include <framework/scenario_adapter.h>

__BEGIN_SYS

template<class T>
class Agent;

template<class T, bool hardware>
class Agent_Common;

__END_SYS

// Helpers
#define AGENT_BEGIN(name)\
template<>\
class Agent<name>: public Scenario_Adapter<name>,\
        public Agent_Common<name, Traits<Build>::hardware_domain> {\
public:\
    Agent(Channel_t &rx_ch, Channel_t &tx_ch, unsigned int inst_id):\
            Scenario_Adapter<name>(rx_ch, tx_ch, inst_id),\
            Agent_Common<name, Traits<Build>::hardware_domain>(rx_ch, tx_ch, inst_id) {}\
\
    typedef name _T;\
    typedef Scenario_Adapter<name> T;\
    typedef Agent_Common<name, Traits<Build>::hardware_domain> Base;\
\
    void dispatch(unsigned int op) {\
        Base::reset();\
        switch (op) {

#define AGENT_END \
        case _T::OP_SET_INST_ID: {\
            if(!Base::read_args<type_to_npkt_1<unsigned int>::Result>())\
                return;\
\
            Base::reset();\
            unsigned int id;\
            Base::deserialize(id);\
            _inst_id = id;\
            Base::finish();\
            break;\
        }\
        case _T::OP_GET_INST_ID: {\
            Base::reset();\
            Base::serialize(_inst_id);\
\
            Base::write_return<type_to_npkt_1<unsigned int>::Result>();\
            Base::finish();\
            break;\
        }\
        default:\
            break;\
        }\
    }\
};

#define D_CALL_R_4(operation, OP_ID, ret_type_0, type_0, type_1, type_2, type_3)\
case _T::OP_ID: {\
    if(!Base::read_args<type_to_npkt_4<type_0,type_1,type_2,type_3>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    type_1 t1; Base::deserialize(t1);\
    type_2 t2; Base::deserialize(t2);\
    type_3 t3; Base::deserialize(t3);\
    ret_type_0 resp = T::operation(t0,t1,t2,t3);\
    Base::reset();\
    Base::serialize(resp);\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    \
    break;\
}

#define D_CALL_R_8(operation, OP_ID, ret_type_0, type_0, type_1, type_2, type_3, type_4, type_5, type_6, type_7)\
case _T::OP_ID: {\
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
    ret_type_0 resp = T::operation(t0,t1,t2,t3,t4,t5,t6,t7);\
    Base::reset();\
    Base::serialize(resp);\
\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    break;\
}

#define D_CALL_R_2(operation, OP_ID, ret_type_0, type_0, type_1)\
case _T::OP_ID: {\
    if(!Base::read_args<type_to_npkt_2<type_0,type_1>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    type_1 t1; Base::deserialize(t1);\
    ret_type_0 resp = T::operation(t0,t1);\
    Base::reset();\
    Base::serialize(resp);\
\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    break;\
}

#define D_CALL_2(operation, OP_ID, type_0, type_1)\
case _T::OP_ID: {\
    if(!Base::read_args<type_to_npkt_2<type_0,type_1>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    type_1 t1; Base::deserialize(t1);\
    T::operation(t0,t1);\
    Base::finish();\
    break;\
}

#define D_CALL_R_1(operation, OP_ID, ret_type_0, type_0)\
case _T::OP_ID: {\
    if(!Base::read_args<type_to_npkt_1<type_0>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    ret_type_0 resp = T::operation(t0);\
    Base::reset();\
    Base::serialize(resp);\
\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    break;\
}

#define D_CALL_1(operation, OP_ID, type_0)\
case _T::OP_ID: {\
    if(!Base::read_args<type_to_npkt_1<type_0>::Result>())\
        return;\
\
    Base::reset();\
    type_0 t0; Base::deserialize(t0);\
    T::operation(t0);\
    Base::finish();\
    break;\
}

#define D_CALL_R_0(operation, OP_ID, ret_type_0)\
case _T::OP_ID: {\
    ret_type_0 resp = T::operation();\
    Base::reset();\
    Base::serialize(resp);\
\
    Base::write_return<type_to_npkt_1<ret_type_0>::Result>();\
    Base::finish();\
    break;\
}

#define D_CALL_0(operation, OP_ID)\
case _T::OP_ID: {\
    T::operation();\
    Base::finish();\
    break;\
}

#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/framework/agent.h"
#else
#include "../../sw/include/framework/agent.h"
#endif

#endif

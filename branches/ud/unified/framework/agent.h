#ifndef __agent_h
#define __agent_h

#include <system/config.h>
#include <framework/scenario_adapter.h>
#include <components/adder.h>

#include "../../hw/framework/message.h"

__BEGIN_SYS

// EPOS Component Framework - Component Agent

template<typename Component>
class Agent_Common: public Message
{
public:
    Agent_Common(Channel & rx_ch, Channel & tx_ch): Message(rx_ch, tx_ch), T() {}

public:
    Scenario_Adapter<Component> T;
};

template<typename Component>
class Agent;

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

#endif

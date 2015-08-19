#ifndef __agent_h
#define __agent_h

#include <system/config.h>
#include <components/adder.h>

#include "adapter.h"
#include "message.h"

__BEGIN_SYS

// EPOS Component Framework - Component Agent

template<typename Component>
class Agent_Common: public Message
{
protected:
    typedef Message::Channel Channel;

public:
    Agent_Common(Channel & rx_ch, Channel & tx_ch): Message(rx_ch, tx_ch), T() {}

public:
    Adapter<Component> T;
};

template<typename Component>
class Agent;

template<>
class Agent<Adder>: public Agent_Common<Adder>
{
public:
    Agent(Channel & rx_ch, Channel & tx_ch): Agent_Common<Adder>(rx_ch, tx_ch) {}

    void exec() {
        Result res = 0;

        switch(method()) {
        case ADDER_ADD: {
            int a0, a1;
            in2(a0, a1);
            res = T.add(a0, a1);
            break;
        }
        case ADDER_SAVE_ST: {
            res = T.save_st();
            break;
        }
        case ADDER_GET_ST_LEN: {
            res = T.get_st_len();
            break;
        }
        default:
            break;
        }

        ret(res);
    }
};

__END_SYS

#endif

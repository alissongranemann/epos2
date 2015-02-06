// EPOS Adder Abstraction Declarations

#ifndef __adder_unified_h
#define __adder_unified_h

#include "component.h"

namespace Implementation {

class Adder: public Component
{
public:
    enum {
        OP_ADD = 0xF0
    };

public:
    Adder(Channel_t &rx_ch, Channel_t &tx_ch, unsigned int inst_id):
            Component(rx_ch, tx_ch, inst_id) {}

    unsigned int add(unsigned int a, unsigned int b) {
        return (a + b);
    }
};

PROXY_BEGIN(Adder)
    unsigned int add(unsigned int a, unsigned int b) {
        return Base::call_r<Adder::OP_ADD, unsigned int>(a, b);
    }
PROXY_END

HANDLE_BEGIN(Adder)
    unsigned int add(unsigned int a, unsigned int b) {
        unsigned int result;

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->add(a, b);
        else
            result = Base::_adapter->add(a, b);

        return result;
    }
HANDLE_END

AGENT_BEGIN(Adder)
    D_CALL_R_2(add, OP_ADD, unsigned int, unsigned int, unsigned int)
AGENT_END

};

DECLARE_RECFG_COMPONENT(Adder);

#endif

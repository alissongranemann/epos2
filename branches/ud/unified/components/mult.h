// EPOS Mult Abstraction Declarations

#ifndef __mult_unified_h
#define __mult_unified_h

#include "component.h"

namespace Implementation {

class Mult: public Component{

public:
    enum {
        OP_MULT         = 0xF0,
        OP_MULT_SQUARE  = 0xF1
    };

public:
    Mult(Channel_t &rx_ch, Channel_t &tx_ch, unsigned int inst_id):
            Component(rx_ch, tx_ch, inst_id) {}

    unsigned int mult(unsigned int a, unsigned int b) {
        return (a*b);
    }

    unsigned int mult_square(unsigned int a, unsigned int b) {
        unsigned int acc = mult(a,b);
        unsigned int square = mult(acc,acc);

        return square;
    }
};

PROXY_BEGIN(Mult)
    unsigned int mult(unsigned int a, unsigned int b) {
        return Base::call_r<Mult::OP_MULT,unsigned int>(a,b);
    }

    unsigned int mult_square(unsigned int a, unsigned int b) {
        return Base::call_r<Mult::OP_MULT_SQUARE,unsigned int>(a,b);
    }
PROXY_END

HANDLE_BEGIN(Mult)
    unsigned int mult(unsigned int a, unsigned int b) {
        unsigned int result;

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->mult(a, b);
        else
            result = Base::_adapter->mult(a, b);

        return result;
    }

    unsigned int mult_square(unsigned int a, unsigned int b) {
        unsigned int result;

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->mult_square(a, b);
        else
            result = Base::_adapter->mult_square(a, b);

        return result;
    }
HANDLE_END

AGENT_BEGIN(Mult)
    D_CALL_R_2(mult, OP_MULT, unsigned int, unsigned int, unsigned int)
    D_CALL_R_2(mult_square, OP_MULT_SQUARE, unsigned int, unsigned int, unsigned int)
AGENT_END

};

DECLARE_RECFG_COMPONENT(Mult);

#endif

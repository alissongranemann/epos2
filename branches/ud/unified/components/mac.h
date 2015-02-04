// EPOS MAC Abstraction Declarations

#ifndef __mac_unified_h
#define __mac_unified_h

#include "component.h"

namespace Implementation {

class MAC : public Component {
public:
    enum {
        OP_MAC          = 0xF0,
        OP_SAVE_ACC     = 0xF1,
        OP_RESTORE_ACC  = 0xF2
    };

public:
    MAC(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<MAC>::n_ids])
        : Component(rx_ch, tx_ch, iid[0]), _acc(0) { }

    unsigned int mac(unsigned int a, unsigned int b) {
        _acc += a*b;

        return _acc;
    }

    unsigned int save_acc() {
        return _acc;
    }

    void restore_acc(unsigned int acc) {
        _acc = acc;
    }

private:
    unsigned int _acc;
};

HANDLE_BEGIN(MAC)
    unsigned int mac(unsigned int a, unsigned int b) {
        return Base::call_r<MAC::OP_MAC, unsigned int>(a, b);
    }

    unsigned int save_acc() {
        return Base::call_r<MAC::OP_SAVE_ACC, unsigned int>();
    }

    void restore_acc(unsigned int acc) {
        Base::call<MAC::OP_RESTORE_ACC, unsigned int>(acc);
    }
HANDLE_END

PROXY_BEGIN(MAC)
    unsigned int mac(unsigned int a, unsigned int b) {
        return Base::call_r<MAC::OP_MAC, unsigned int>(a, b);
    }

    unsigned int save_acc() {
        return Base::call_r<MAC::OP_SAVE_ACC, unsigned int>();
    }

    void restore_acc(unsigned int acc) {
        Base::call<MAC::OP_RESTORE_ACC, unsigned int>(acc);
    }
PROXY_END

AGENT_BEGIN(MAC)
    D_CALL_R_2(mac, OP_MAC, unsigned int, unsigned int, unsigned int)
    D_CALL_R_0(save_acc, OP_SAVE_ACC, unsigned int)
    D_CALL_1(restore_acc, OP_RESTORE_ACC, unsigned int)
AGENT_END

};

DECLARE_RECFG_COMPONENT(MAC);

#endif

// EPOS AES Abstraction Declarations

#ifndef __aes_unified_h
#define __aes_unified_h

#include "component.h"
#include "src/aes.h"

namespace Implementation {

class AES: public Component, public AES_Common
{
public:
    enum {
        OP_CIPHER       = 0xF0,
        OP_DECIPHER     = 0xF1,
        OP_SET_KEY      = 0xF2
    };

public:
    AES(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char inst_id):
            Component(rx_ch, tx_ch, inst_id) {}

    void set_key(safe_pkt_t key) {
        for (unsigned int i = 0; i < CIPHER_SIZE; i++)
            _key[i] = key.data[i];

        _expandKey(_key);
    }

    safe_pkt_t cipher(safe_pkt_t pkt) {
        _cipher(pkt.data, _key);

        return pkt;
    }

    void set_state(unsigned int * buf, unsigned int len) {
        for(int i = 0; i < len; i++)
            _key[i] = *(buf++);
    }

    unsigned int get_state() {
        static unsigned int i = 0;

        return _key[i++];
    }

    unsigned int get_state_len() { return EXPANDED_SIZE; }

private:
    unsigned char _key[EXPANDED_SIZE];

};

PROXY_BEGIN(AES)
    void set_key(safe_pkt_t pkt) { Base::call<AES::OP_SET_KEY, safe_pkt_t>(pkt); }

    safe_pkt_t cipher(safe_pkt_t pkt) {
        return Base::call_r<AES::OP_CIPHER, safe_pkt_t, safe_pkt_t>(pkt);
    }

    unsigned int get_state(unsigned int * buf) {
        unsigned int len = Base::call_r<AES::OP_GET_STATE_LEN, unsigned int>();

        for(unsigned int i = 0; i < len; i++)
            *(buf++) = Base::call_r<AES::OP_GET_STATE, unsigned int>();

        return len;
    }
PROXY_END

HANDLE_BEGIN(AES)
    void set_key(safe_pkt_t key) {
        if(_domain == Component_Manager::HARDWARE)
            Base::_proxy->set_key(key);
        else
            Base::_adapter->set_key(key);
    }

    safe_pkt_t cipher(safe_pkt_t pkt) {
        safe_pkt_t result;

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->cipher(pkt);
        else
            result = Base::_adapter->cipher(pkt);

        return result;
    }

    unsigned int get_state(unsigned int * buf) {
        return Base::_proxy->get_state(buf);
    }

    void set_state(unsigned int * buf, unsigned int len) {
        Base::_adapter->set_state(buf, len);
    }
HANDLE_END

AGENT_BEGIN(AES)
    D_CALL_1(set_key, OP_SET_KEY, safe_pkt_t)
    D_CALL_R_1(cipher, OP_CIPHER, safe_pkt_t, safe_pkt_t)
    D_CALL_R_0(get_state, OP_GET_STATE, unsigned int)
    D_CALL_R_0(get_state_len, OP_GET_STATE_LEN, unsigned int)
AGENT_END

};

DECLARE_RECFG_COMPONENT(AES);

#endif

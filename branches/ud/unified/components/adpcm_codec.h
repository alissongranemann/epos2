// EPOS ADPCM Codec Abstraction Declarations

#ifndef __adpcm_codec_unified_h
#define __adpcm_codec_unified_h

#include "component.h"
#include "adpcm_encoder.h"
#include "adpcm_decoder.h"

namespace Implementation {

class ADPCM_Codec: public ADPCM_Encoder, public ADPCM_Decoder, public Component
{
public:
	enum {
		OP_ENCODE = 0xF0,
		OP_DECODE = 0xF1
	};

public:
    ADPCM_Codec(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char inst_id):
        ADPCM_Encoder(), ADPCM_Decoder(), Component(rx_ch, tx_ch, inst_id) {}

public:
    int decode(unsigned int adpcm_in) { return ADPCM_Decoder::decode(adpcm_in); }

    unsigned int encode(int pcm_in) { return ADPCM_Encoder::encode(pcm_in); }
};

PROXY_BEGIN(ADPCM_Codec)
    int decode(unsigned int adpcm_in) {
        return Base::call_r<ADPCM_Codec::OP_DECODE, int>(adpcm_in);
    }

    unsigned int encode(int pcm_in) {
        return Base::call_r<ADPCM_Codec::OP_ENCODE, unsigned int>(pcm_in);
    }
PROXY_END

HANDLE_BEGIN(ADPCM_Codec)
    int decode(unsigned int adpcm_in) {
        int result;

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->decode(adpcm_in);
        else
            result = Base::_adapter->decode(adpcm_in);

        return result;
    }

    unsigned int encode(int pcm_in) {
        unsigned int result;

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->encode(pcm_in);
        else
            result = Base::_adapter->encode(pcm_in);

        return result;
    }

    // ADPCM_Codec holds no internal state
    void get_state() {}

    void set_state() {}
HANDLE_END

AGENT_BEGIN(ADPCM_Codec)
    D_CALL_R_1(decode, OP_DECODE, int, unsigned int)
    D_CALL_R_1(encode, OP_ENCODE, unsigned int, int)
AGENT_END

};

DECLARE_RECFG_COMPONENT(ADPCM_Codec);

#endif

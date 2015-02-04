#ifndef __rsp_adpcm_unified_h
#define __rsp_adpcm_unified_h

#include "component.h"
#include "adpcm_decoder.h"
#include "rsp_dtmf.h"
#ifndef HIGH_LEVEL_SYNTHESIS
#include <utility/ostream.h>
#endif

namespace Implementation {

class RSP_ADPCM : public Component, private ADPCM_Decoder {

public:
    enum {
        OP_DECODE = 0xF1
    };

private:
    System::RSP_DTMF dtmf;

    decoded_pkt_t dec_pkt;

public:

    RSP_ADPCM(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<RSP_ADPCM>::n_ids])
        :Component(rx_ch, tx_ch, iid[0]),
         ADPCM_Decoder(),
         dtmf(rx_ch, tx_ch, &iid[1]){}


    void decode(safe_pkt_t pkt){
        /*#ifndef HIGH_LEVEL_SYNTHESIS
        System::OStream cout;
        cout << "RSP_ADPCM::decode(" << (void*)pkt.data[0] << ",...,"<< (void*)pkt.data[15] << ")\n";
        #endif*/

        adpcm_unfold: for (int i = 0; i < 16; ++i) {
            unsigned int s1 = pkt.data[i] & 0xF;
            unsigned int s2 = pkt.data[i] >> 4;

            dec_pkt.data[i*2] = ADPCM_Decoder::decode(s1);
            dec_pkt.data[(i*2)+1] = ADPCM_Decoder::decode(s2);
        }

        dtmf.add_sample(dec_pkt);
    }

};

PROXY_BEGIN(RSP_ADPCM)
    void decode(safe_pkt_t pkt){
        Base::call<RSP_ADPCM::OP_DECODE>(pkt);
    }
PROXY_END

AGENT_BEGIN(RSP_ADPCM)
    D_CALL_1(decode,OP_DECODE, safe_pkt_t)
AGENT_END

};

DECLARE_COMPONENT(RSP_ADPCM);

#endif

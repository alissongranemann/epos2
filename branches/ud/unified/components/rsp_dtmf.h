#ifndef __rsp_dtmf_unified_h
#define __rsp_dtmf_unified_h

#include "component.h"
#include "rsp_controller.h"
#include "dtmf_algorithm.h"
#ifndef HIGH_LEVEL_SYNTHESIS
#include <utility/ostream.h>
#endif

namespace Implementation {

class RSP_DTMF : public Component{

public:
    enum {
        OP_ADD_SAMPLE = 0xF2
    };

private:

    System::RSP_Controller controller;

    enum{
        FRAMESIZE = 32*22,
        TONES = DTMF_Algorithm::TONES
    };

    typedef DTMF_Algorithm::sample_t sample_t;


    unsigned short buffer_idx;
    sample_t samples[FRAMESIZE];
    bool tone_responses[TONES];

public:

    RSP_DTMF(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<RSP_DTMF>::n_ids])
        :Component(rx_ch, tx_ch, iid[0]),
         controller(rx_ch, tx_ch, &iid[1]),
         buffer_idx(0){

        for (int i = 0; i < FRAMESIZE; ++i) samples[i] = 0;
        for (int i = 0; i < TONES; ++i) tone_responses[i] = false;

    }


    void add_sample(decoded_pkt_t pkt){
        /*#ifndef HIGH_LEVEL_SYNTHESIS
        System::OStream cout;
        cout << "RSP_DTMF::add_sample(" << (void*)pkt.data[0] << ",...,"<< (void*)pkt.data[31] << ")\n";
        #endif*/

        for (int i = 0; i < 32; ++buffer_idx, ++i) {
            samples[buffer_idx] = pkt.data[i];
        }
        if(buffer_idx == FRAMESIZE){

            /*#ifndef HIGH_LEVEL_SYNTHESIS
            System::OStream cout;
            cout << "RSP_DTMF::add_sample() - doing detection\n";
            #endif*/

            GOERTZEL: for(int i = 0; i < TONES; ++i){
                tone_responses[i] = DTMF_Algorithm::goertzel(i,samples);
            }

            unsigned char detected_signal = DTMF_Algorithm::analyze_responses(tone_responses);

            // convert from [0, 15] to [1,2,3,..,A,B,..,F] using a lookup table
            // 'I' for invalid signal
            unsigned char tone = 'I';
            if(detected_signal != DTMF_Algorithm::INVAL) {
                tone = DTMF_Algorithm::button_names[detected_signal];
            }

            buffer_idx = 0;

            controller.tone_detected(tone);

        }
    }

};

PROXY_BEGIN(RSP_DTMF)
    void add_sample(decoded_pkt_t pkt){
        Base::call<RSP_DTMF::OP_ADD_SAMPLE>(pkt);
    }
PROXY_END

AGENT_BEGIN(RSP_DTMF)
    D_CALL_1(add_sample,OP_ADD_SAMPLE, decoded_pkt_t)
AGENT_END

};

DECLARE_COMPONENT(RSP_DTMF);

#endif

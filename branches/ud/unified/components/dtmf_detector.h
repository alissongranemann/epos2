// EPOS DTMF Detector Abstraction Declarations

#ifndef __dtmf_detector_unified_h
#define __dtmf_detector_unified_h

#include "component.h"
#include "dtmf_algorithm.h"

namespace Implementation {

class DTMF_Detector: public Component
{
public:
    enum {
        OP_ADD_SAMPLE   = 0xF0,
        OP_DETECT       = 0xF1
    };

    typedef DTMF_Algorithm::sample_t sample_t;

public:
    DTMF_Detector(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char inst_id):
            Component(rx_ch, tx_ch, inst_id), _i(0) {}

    unsigned char add_sample(sample_t sample) {
        _samples[_i] = sample;

        if(_i == (DTMF_Algorithm::FRAMESIZE - 1)) {
            _i = 0;
            return true;
        } else {
            _i++;
            return false;
        }
    }

    char detect() {
        bool _tone_responses[DTMF_Algorithm::TONES];

        GOERTZEL: for(int i = 0; i < DTMF_Algorithm::TONES; i++)
            _tone_responses[i] = DTMF_Algorithm::goertzel(i, _samples);

        unsigned char detected_signal =
            DTMF_Algorithm::analyze_responses(_tone_responses);

        // Convert from [0...15] to [1...F] using a lookup table. 'I' for
        // invalid signal.
        if(detected_signal == DTMF_Algorithm::INVAL)
            return 'I';
        else
            return DTMF_Algorithm::button_names[detected_signal];
    }

    void set_state(unsigned int * buf, unsigned int len) {
        for(int i = 0; i < len; i++)
            _samples[i] = *(buf++);
    }

    unsigned int get_state() {
        static unsigned int i = 0;

        return _samples[i++];
    }

    unsigned int get_state_len() { return _i; }

private:
    sample_t _samples[DTMF_Algorithm::FRAMESIZE];
    unsigned int _i;
};

PROXY_BEGIN(DTMF_Detector)
    unsigned char add_sample(DTMF_Detector::sample_t sample) {
        return Base::call_r<DTMF_Detector::OP_ADD_SAMPLE, unsigned char>(sample);
    }

    unsigned char detect() {
        return Base::call_r<DTMF_Detector::OP_DETECT, unsigned char>();
    }

    unsigned int get_state(unsigned int * buf) {
        unsigned int len = Base::call_r<DTMF_Detector::OP_GET_STATE_LEN, unsigned int>();

        for(unsigned int i = 0; i < len; i++)
            *(buf++) = Base::call_r<DTMF_Detector::OP_GET_STATE, unsigned int>();

        return len;
    }
PROXY_END

HANDLE_BEGIN(DTMF_Detector)
    unsigned char add_sample(DTMF_Detector::sample_t sample) {
        unsigned char result;

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->add_sample(sample);
        else
            result = Base::_adapter->add_sample(sample);

        return result;
    }

    unsigned char detect() {
        unsigned char result;

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->detect();
        else
            result = Base::_adapter->detect();

        return result;
    }

    unsigned int get_state(unsigned int * buf) {
        return Base::_proxy->get_state(buf);
    }

    void set_state(unsigned int * buf, unsigned int len) {
        Base::_adapter->set_state(buf, len);
    }
HANDLE_END

AGENT_BEGIN(DTMF_Detector)
    D_CALL_R_1(add_sample, OP_ADD_SAMPLE, unsigned char, sample_t)
    D_CALL_R_0(detect, OP_DETECT, unsigned char)
    D_CALL_R_0(get_state, OP_GET_STATE, unsigned int)
    D_CALL_R_0(get_state_len, OP_GET_STATE_LEN, unsigned int)
AGENT_END

};

DECLARE_RECFG_COMPONENT(DTMF_Detector);

#endif

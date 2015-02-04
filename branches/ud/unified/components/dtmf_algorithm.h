// EPOS DTMF Algorithm Abstraction Declarations

#ifndef __dtmf_algorithm_unified_h
#define __dtmf_algorithm_unified_h

namespace Implementation {

class DTMF_Algorithm
{
public:
    enum {
        THRESHOLD   = 42, // Bogus, define it better later
        TONES       = 8,
        FRAMESIZE   = 700,
        SRATE       = 32000,

        MAX_UCHAR   = 255,
        MAX_SHORT   = 32768,
        INVAL       = 255,

        SINCOS_SIZE = 17,
        SINCOS_FP   = 16,

        SAMPLE_SIZE = 17,
        SAMPLE_FP   = 16,

        WITH_SIGNAL = true
    };

    typedef int sincos_t;
    typedef int sample_t;

    static const sincos_t SINES[8];
    static const sincos_t COSINES[8];
    static const unsigned char RESPONSE_LOOKUP[MAX_UCHAR + 1];
    static const char button_names[16];

public:
    static bool goertzel(int tone_index, sample_t samples[FRAMESIZE]);
    static unsigned char analyze_responses(bool tone_responses[TONES]);
};

};

#endif

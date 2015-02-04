#include "../dtmf_algorithm.h"

namespace Implementation {

const DTMF_Algorithm::sincos_t DTMF_Algorithm::SINES[8] = {697, 770, 852, 941,
    1209, 1336, 1477, 1633};
const DTMF_Algorithm::sincos_t DTMF_Algorithm::COSINES[8] = {-697, -770, -852,
    -941, -1209, -1336, -1477, -1633};

// Sparse array, indexed with 8 bits indices, provides a 0(1) lookup to the
// valid tone combinations
const unsigned char DTMF_Algorithm::RESPONSE_LOOKUP[MAX_UCHAR + 1] = {INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, 13, 15, INVAL, 0, INVAL, INVAL, INVAL,
    14, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, 12, 9, INVAL, 8,
    INVAL, INVAL, INVAL, 7, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, 11, 6, INVAL, 5, INVAL, INVAL,
    INVAL, 4, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, 10,3, INVAL, 2, INVAL, INVAL, INVAL, 1, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL,
    INVAL, INVAL, INVAL, INVAL, INVAL, INVAL, INVAL};

const char DTMF_Algorithm::button_names[16] = {'0', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', 'A', 'B', 'C', 'D', '*', '#'};

bool DTMF_Algorithm::goertzel(int tone_index, sample_t samples[FRAMESIZE]) {
    sincos_t cosine = COSINES[tone_index];
    sincos_t sine = SINES[tone_index];
    sample_t coeff = 2*cosine;

    sample_t real = 0;
    sample_t imag = 0;
    sample_t magnitude_square = 0;

    sample_t q0 = 0, q1 = 0, q2 = 0;

   //GOERTZEL_ACC: for(int i = 0; i < FRAMESIZE; ++i) {
   for(int i = 0; i < FRAMESIZE; ++i) {
        q0 = (coeff*q1) - (q2 + samples[i]);
        q2 = q1;
        q1 = q0;
    }

    real = q1 - q2*cosine;
    imag = q2*sine;
    magnitude_square = (real*real) + (imag*imag);

    return (magnitude_square > THRESHOLD);
}

unsigned char DTMF_Algorithm::analyze_responses(bool tone_responses[TONES]) {
    unsigned char index = 0;

    //ANALYZE_SHIFT: for(int i = 0; i < 8; i++)
    for(int i = 0; i < 8; i++)
        index |= (tone_responses[i] << i);

    return RESPONSE_LOOKUP[index];
}

};



#include "../adpcm_encoder.h"
#include "../adpcm_decoder.h"

namespace Implementation {

const int ADPCM_Common::INDEX_TABLE[16] = {-1, -1, -1, -1, 2, 4, 6, 8, -1, -1,
    -1, -1, 2, 4, 6, 8};

const int ADPCM_Common::STEP_SIZE_TABLE[89] = {7, 8, 9, 10, 11, 12, 13, 14, 16,
    17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97,
    107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408,
    449, 494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552,
    1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871,
    5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 15289,
    16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};

int ADPCM_Decoder::decode(unsigned int adpcm_in) {
    int pred_sample = _prev_sample;
    int step = ADPCM_Common::STEP_SIZE_TABLE[_prev_index];
    int diffq = step >> 3;
    unsigned int code = adpcm_in;

    if (code & 4)
        diffq = diffq + step;
    if (code & 2)
        diffq = diffq + (step >> 1);
    if (code & 1)
        diffq = diffq + (step >> 2);

    if (code & 8)
        pred_sample = pred_sample - diffq;
    else
        pred_sample = pred_sample + diffq;

    if (pred_sample > 32767)
        pred_sample = 32767;
    else if (pred_sample < -32768)
        pred_sample = -32768;

    int index = _prev_index;
    index = index + ADPCM_Common::INDEX_TABLE[code];

    if (index >= 89)
        index = 88;
    else if (index < 0)
        index = 0;

    _prev_sample = pred_sample;
    _prev_index = index & 0x000000FF;

    return pred_sample;
}

unsigned int ADPCM_Encoder::encode(int pcm_in) {
    int pred_sample = _prev_sample;
    int step = ADPCM_Common::STEP_SIZE_TABLE[_prev_index];
    int diff = pcm_in - pred_sample;

    unsigned int code = 0;

    if (diff >= 0)
        code = 0;
    else {
        code = 8;
        diff = -diff;
    }

    int temp_step = step;
    if (diff >= temp_step){
        code = code | 4;
        diff = diff - temp_step;
    }
    temp_step = temp_step >> 1;
    if (diff >= temp_step) {
        code = code | 2;
        diff = diff - temp_step;
    }
    temp_step = temp_step >> 1;
    if (diff >= temp_step)
        code = code | 1;

    int diffq = step >> 3;
    if (code & 4)
        diffq = diffq + step;
    if (code & 2)
        diffq = diffq + (step >> 1);
    if (code & 1)
        diffq = diffq + (step >> 2);

    if (code & 8)
        pred_sample = pred_sample - diffq;
    else
        pred_sample = pred_sample + diffq;

    if (pred_sample > 32767)
        pred_sample = 32767;
    else if (pred_sample < -32768)
        pred_sample = -32768;

    int index = _prev_index;
    index = index + ADPCM_Common::INDEX_TABLE[code];

    if (index >= 89)
        index = 88;
    else if (index < 0)
        index = 0;

    _prev_sample = pred_sample;
    _prev_index = index & 0x000000FF;

    return code;
}

};

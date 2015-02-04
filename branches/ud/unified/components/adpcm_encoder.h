#ifndef __adpcm_encoder_h
#define __adpcm_encoder_h

#include "adpcm_common.h"

namespace Implementation {

class ADPCM_Encoder
{
public:
	ADPCM_Encoder(): _prev_sample(0), _prev_index(0) {}

	unsigned int encode(int pcm_in);

private:
	int _prev_sample;
	unsigned int _prev_index;
};

};

#endif

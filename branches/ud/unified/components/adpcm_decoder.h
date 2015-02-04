#ifndef __adpcm_decoder_h
#define __adpcm_decoder_h

#include "adpcm_common.h"

namespace Implementation {

class ADPCM_Decoder
{
public:
	ADPCM_Decoder(): _prev_sample(0), _prev_index(0) {}

	int decode(unsigned int adpcm_in);

private:
	int _prev_sample;
	unsigned int _prev_index;
};

};

#endif

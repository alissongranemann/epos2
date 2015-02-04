#ifndef __adpcm_common_h
#define __adpcm_common_h

namespace Implementation {

class ADPCM_Common {
public:
	const static int INDEX_TABLE[16];
	const static int STEP_SIZE_TABLE[89];
};

};

#endif

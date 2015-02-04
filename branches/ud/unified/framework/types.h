// EPOS Internal Unified Components Type Management System

#ifndef __unified_types_h
#define __unified_types_h

namespace Implementation {

enum {
    MSG_TYPE_CALL = 0,
    MSG_TYPE_RESP,
    MSG_TYPE_CALL_DATA,
    MSG_TYPE_RESP_DATA,
    MSG_TYPE_ERROR
};

class Sys;

template<int>
class Dummy;

class Dummy_Callee;
class Dummy_Caller;
class Add;
class EC;
class MAC;
class AES;
class Mult;
class DTMF_Detector;
class ADPCM_Codec;
class RSP_Controller;
class RSP_DTMF;
class RSP_ADPCM;
class RSP_AES;
class RSP_ETH;
class Dummy_Component_For_Proxy;
class Dummy_Component_For_Agent;
class Dummy_HW_Area_Test;

template <class,class>
class Sched;

typedef struct {
    unsigned long long x;
    unsigned long long y;
    unsigned long long z;
} EC_Point_t;

typedef struct {
    unsigned char data[16];
} safe_pkt_t;

typedef struct {
    unsigned short data[32];
} decoded_pkt_t;

namespace Scheduling_Criteria {
    class Priority;
    class Round_Robin;
};

namespace Configurations {
    class EPOS_SOC_Catapult;
}

// System Components IDs
typedef unsigned int Type_Id;
enum {
    ADD_ID,
    EC_ID,
    MAC_ID,
    AES_ID,
    MULT_ID,
    DTMF_DETECTOR_ID,
    ADPCM_CODEC_ID,
    SCHED_ID ,//TODO there should be a different type for each template instantiation
    DUMMY_CALLEE_ID,
    DUMMY_CALLER_ID,
    RSP_CONTROLLER_ID,
    RSP_DTMF_ID,
    RSP_ADPCM_ID,
    RSP_AES_ID,
    RSP_ETH_ID,
    UNKNOWN_TYPE_ID,
    LAST_TYPE_ID                    = UNKNOWN_TYPE_ID - 1,
    DUMMY_HW_AREA_TEST_ID           = DUMMY_CALLER_ID,
    DUMMY_COMPONENT_FOR_PROXY_ID    = DUMMY_CALLEE_ID,
    DUMMY_COMPONENT_FOR_AGENT_ID    = DUMMY_CALLER_ID,
};

};

#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/system/types_hw.h"
#else
#include "../../sw/include/system/types_sw.h"
#endif

#endif

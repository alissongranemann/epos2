#ifndef __resource_table_unified_h
#define __resource_table_unified_h

#include "resource.h"
#include "meta.h"
#include "../traits.h"

namespace Implementation {

enum {
    NOC_SW_NODE_GENERIC = Address::LOCAL_NN,
    NOC_SW_NODE_MANAGER = Address::LOCAL_NE,
    NOC_SW_NODE         = NOC_SW_NODE_MANAGER,//TODO tmp, delete this constant
    NOC_FREE_NODE_0     = Address::LOCAL_NW,
    NOC_FREE_NODE_1     = Address::LOCAL_SS,
    NOC_FREE_NODE_2     = Address::LOCAL_WW,
    NOC_FREE_NODE_3     = Address::LOCAL_SE,
    NOC_FREE_NODE_4     = Address::LOCAL_SW,
    NOC_FREE_NODE_5     = Address::LOCAL_EE
};

template<class T, int IID>
class Resource_Table;

template<>
class Resource_Table<Add, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_0;
    static const unsigned int LOCAL =
        IF_INT<Traits<Add>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<Add>::n_ids];
};

template<>
class Resource_Table<MAC, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_0;
    static const unsigned int LOCAL =
        IF_INT<Traits<MAC>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<MAC>::n_ids];
};

template<>
class Resource_Table<AES, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_0;
    static const unsigned int LOCAL =
        IF_INT<Traits<AES>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<AES>::n_ids];
};

template<>
class Resource_Table<Mult, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_1;
    static const unsigned int LOCAL =
        IF_INT<Traits<Mult>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<Mult>::n_ids];
};

//TODO These 'void,void' are workarounds. Get rid of them
template<>
class Resource_Table<Sched<void, void>, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_3;
    static const unsigned int LOCAL =
        IF_INT<Traits<Sched<void, void> >::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<Sched<void, void> >::n_ids];
};

template<>
class Resource_Table<DTMF_Detector, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_0;
    static const unsigned int LOCAL =
        IF_INT<Traits<DTMF_Detector>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<DTMF_Detector>::n_ids];
};

template<>
class Resource_Table<ADPCM_Codec, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_0;
    static const unsigned int LOCAL =
        IF_INT<Traits<ADPCM_Codec>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<ADPCM_Codec>::n_ids];
};

template<>
class Resource_Table<Dummy_Callee, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_2;
    static const unsigned int LOCAL =
        IF_INT<Traits<Dummy_Callee>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<Dummy_Callee>::n_ids];
};

template<>
class Resource_Table<Dummy_Caller, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_3;
    static const unsigned int LOCAL =
        IF_INT<Traits<Dummy_Caller>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<Dummy_Caller>::n_ids];
};

template<>
class Resource_Table<RSP_Controller, 0>
{
    public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_1;
    static const unsigned int LOCAL =
        IF_INT<Traits<RSP_Controller>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<RSP_Controller>::n_ids];
};

template<>
class Resource_Table<RSP_DTMF, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_3;
    static const unsigned int LOCAL =
        IF_INT<Traits<RSP_DTMF>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<RSP_DTMF>::n_ids];
};

template<>
class Resource_Table<RSP_ADPCM, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_4;
    static const unsigned int LOCAL =
        IF_INT<Traits<RSP_ADPCM>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<RSP_ADPCM>::n_ids];
};

template<>
class Resource_Table<RSP_AES, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL= NOC_FREE_NODE_5;
    static const unsigned int LOCAL =
        IF_INT<Traits<RSP_AES>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<RSP_AES>::n_ids];
};

template<>
class Resource_Table<RSP_ETH, 0>
{
public:
    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int HW_LOCAL = NOC_FREE_NODE_0;
    static const unsigned int LOCAL =
        IF_INT<Traits<RSP_ETH>::hardware, HW_LOCAL, NOC_SW_NODE>::Result;
    static const unsigned char IID[Traits<RSP_ETH>::n_ids];
};

template<class T, int IID> struct Type2IDX;
template<> struct Type2IDX<Add, 0>{ enum {IDX = 0}; };
template<> struct Type2IDX<Mult, 0>{ enum {IDX = 1}; };
template<> struct Type2IDX<Sched<void, void>, 0>{ enum {IDX = 2}; };
template<> struct Type2IDX<DTMF_Detector, 0>{ enum {IDX = 3}; };
template<> struct Type2IDX<ADPCM_Codec, 0>{ enum {IDX = 4}; };
template<> struct Type2IDX<Dummy_Callee, 0>{ enum {IDX = 5}; };
template<> struct Type2IDX<Dummy_Caller, 0>{ enum {IDX = 6}; };
template<> struct Type2IDX<RSP_Controller, 0>{ enum {IDX = 7}; };
template<> struct Type2IDX<RSP_DTMF, 0>{ enum {IDX = 8}; };
template<> struct Type2IDX<RSP_ADPCM, 0>{ enum {IDX = 9}; };
template<> struct Type2IDX<RSP_AES, 0>{ enum {IDX = 10}; };
template<> struct Type2IDX<RSP_ETH, 0>{ enum {IDX = 11}; };
template<> struct Type2IDX<MAC, 0>{ enum {IDX = 12}; };
template<> struct Type2IDX<AES, 0>{ enum {IDX = 13}; };

class PHY_Table {
public:
    static int type2IDX(int tid, int iid) {
        switch (tid) {
        case ADD_ID:
            switch (iid) {
            case 0:
                return Type2IDX<Add, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case MAC_ID:
            switch (iid) {
            case 0:
                return Type2IDX<MAC, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case AES_ID:
            switch (iid) {
            case 0:
                return Type2IDX<AES, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case MULT_ID:
            switch (iid) {
            case 0:
                return Type2IDX<Mult, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case SCHED_ID:
            switch (iid) {
            case 0:
                return Type2IDX<Sched<void, void>, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case DTMF_DETECTOR_ID:
            switch (iid) {
            case 0:
                return Type2IDX<DTMF_Detector, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case ADPCM_CODEC_ID:
            switch (iid) {
            case 0:
                return Type2IDX<ADPCM_Codec, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case DUMMY_CALLEE_ID:
            switch (iid) {
            case 0:
                return Type2IDX<Dummy_Callee, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case DUMMY_CALLER_ID:
            switch (iid) {
            case 0:
                return Type2IDX<Dummy_Caller, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case RSP_CONTROLLER_ID:
            switch (iid) {
            case 0:
                return Type2IDX<RSP_Controller, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case RSP_DTMF_ID:
            switch (iid) {
            case 0:
                return Type2IDX<RSP_DTMF, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case RSP_ADPCM_ID:
            switch (iid) {
            case 0:
                return Type2IDX<RSP_ADPCM, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case RSP_AES_ID:
            switch (iid) {
            case 0:
                return Type2IDX<RSP_AES, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        case RSP_ETH_ID:
            switch (iid) {
            case 0:
                return Type2IDX<RSP_ETH, 0>::IDX;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        return -1;
    }

public:
    static const unsigned int X[14];
    static const unsigned int Y[14];
    static const unsigned int LOCAL[14];
};

}

#endif

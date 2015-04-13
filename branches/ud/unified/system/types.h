// EPOS Internal Unified Components Type Management System

#ifndef __unified_types_h
#define __unified_types_h

// TODO: Find a better place to put this
enum {
    MSG_TYPE_CALL = 0,
    MSG_TYPE_RESP,
    MSG_TYPE_CALL_DATA,
    MSG_TYPE_RESP_DATA,
    MSG_TYPE_ERROR
};

__BEGIN_SYS

// System parts
class Build;

// Unified components
class Adder;

// System Components IDs
typedef unsigned int Type_Id;
enum {
    ADDER_ID = 0,
    UNKNOWN_TYPE_ID,
    LAST_TYPE_ID = UNKNOWN_TYPE_ID - 1,
};

// Type IDs for system components
template<typename T> struct Type { static const Type_Id ID = UNKNOWN_TYPE_ID; };

template<> struct Type<Adder> { static const Type_Id ID = ADDER_ID; };

__END_SYS

#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/system/types_hw.h"
#else
#include "../../sw/include/system/types_sw.h"
#endif

#endif

//EPOS Unified components Compile-time Type Information

#ifndef __unified_ctti_h
#define __unified_ctti_h

#include "types.h"

namespace Implementation {

// Type -> Id
template<typename T>struct Type2Id  { enum { ID = UNKNOWN_TYPE_ID }; };

template<> struct Type2Id<Add>  { enum { ID = ADD_ID }; };

}

#endif

#ifndef __dynamic_alloc_unified_h
#define __dynamic_alloc_unified_h

// TODO: Remove this mess
#ifdef HIGH_LEVEL_SYNTHESIS
__BEGIN_SYS

template<typename Obj_Type>
class Dynamic_Allocation {};

__END_SYS
#else
#include "../../sw/include/framework/dynamic_alloc.h"
#endif

#endif

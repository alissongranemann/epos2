#ifndef DYNAMIC_ALLOC2_H_
#define DYNAMIC_ALLOC2_H_

//TODO remover essa gabiarra
#ifdef HIGH_LEVEL_SYNTHESIS
namespace EPOS {
template<typename Obj_Type, bool system_heap>
class Dynamic_Allocation {};
};
#else
//EPOS_SoC Dynamic allocation
#include "../../sw/include/framework/dynamic_alloc.h"
#endif

namespace Implementation {

template<typename Obj_Type>
class Dynamic_Allocation : public EPOS::Dynamic_Allocation<Obj_Type,false>{};

template<typename Obj_Type>
class System_Dynamic_Allocation : public EPOS::Dynamic_Allocation<Obj_Type,true>{};

};


#endif

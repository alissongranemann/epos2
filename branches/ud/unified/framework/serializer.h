#ifndef __serializer_h
#define __serializer_h

#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/framework/serializer.h"
#else
#include "../../sw/include/framework/serializer.h"
#endif

namespace Implementation {

template<unsigned int BUFFER_SIZE>
class Serializer: public serializer_imp::Serializer<BUFFER_SIZE, Traits<Sys>::Platform, Traits<Sys>::hardware_domain> {};

};

#endif

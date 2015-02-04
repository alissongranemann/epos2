#ifndef __handle_h
#define __handle_h

#include "../traits.h"
#include "serializer.h"

namespace Implementation {

template<class Component>
class Handle;

template<class Component, class Platform, bool hardware>
class Handle_Common;

// Helpers
#define HANDLE_BEGIN(name)\
template<>\
class Handle<name>:\
    public Handle_Common<name, Traits<Sys>::Platform, Traits<Sys>::hardware_domain>\
{\
public:\
    typedef Handle_Common<name, Traits<Sys>::Platform, Traits<Sys>::hardware_domain> Base;\
\
    Handle(Channel_t &rx_ch, Channel_t &tx_ch): Base(rx_ch, tx_ch) {}

#define HANDLE_END };

};

#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/framework/handle.h"
#else
#include "../../sw/include/framework/handle.h"
#endif

#endif

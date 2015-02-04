#ifndef __proxy_h
#define __proxy_h

#include "../traits.h"
#include "serializer.h"

namespace Implementation {

template<class Component>
class Proxy;

template<class Component, class Platform, bool hardware>
class Proxy_Common;

// Helpers
#define PROXY_BEGIN(name)\
template<>\
class Proxy<name>:\
    public Proxy_Common<name, Traits<Sys>::Platform, Traits<Sys>::hardware_domain>\
{\
public:\
    typedef Proxy_Common<name, Traits<Sys>::Platform, Traits<Sys>::hardware_domain> Base;\
\
    Proxy(Channel_t &rx_ch, Channel_t &tx_ch, unsigned int inst_id):\
            Base(rx_ch, tx_ch, inst_id) {}

#define PROXY_END };

};

#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/framework/proxy.h"
#else
#include "../../sw/include/framework/proxy.h"
#endif

#endif

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
    Proxy(Channel &rx_ch, Channel &tx_ch):\
            Base(rx_ch, tx_ch) {}

#define PROXY_END };

};

#ifdef HIGH_LEVEL_SYNTHESIS
#include "../../hw/framework/proxy.h"
#else
#include "../../sw/include/framework/proxy.h"
#endif

#endif

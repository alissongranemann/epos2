#ifndef __handle_hw_h
#define __handle_hw_h

#include "../../unified/framework/handle.h"
#include "../../unified/framework/proxy.h"
#include "../../unified/framework/scenario_adapter.h"

// TODO: These two classes are workarounds. Catapult needs them to properly
// compile the unified designs. Remove them when possible.

namespace Implementation {

class Component_Manager {
public:
    typedef unsigned int Domain;
    enum {
        HARDWARE    = 0,
        SOFTWARE    = 1
    };
};

template<typename Component>
class Handle_Common<Component, Configurations::EPOS_SOC_Catapult, true>
{
protected:
    Handle_Common(Channel &rx_ch, Channel &tx_ch) {}

    void enter_recfg() {}
    void leave_recfg() {}

protected:
    Scenario_Adapter<Component> * _adapter;
    Proxy<Component> * _proxy;
    Component_Manager::Domain _domain;
};

};

#endif

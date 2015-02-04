#ifndef __component_unified_h
#define __component_unified_h

#include "../traits.h"
#include "../framework/handle.h"
#include "../framework/proxy.h"
#include "../framework/scenario_adapter.h"
#include "../framework/agent.h"

namespace Implementation {

class Component
{
public:
    // Built-in operations implemented in the Agent
    enum {
        OP_SET_INST_ID      = 0x00,
        OP_GET_INST_ID      = 0x01,
        OP_SET_STATE        = 0x02,
        OP_GET_STATE        = 0x03,
        OP_GET_STATE_LEN    = 0x04
    };

protected:
    Component(Channel_t &rx_ch, Channel_t &tx_ch, unsigned int inst_id) {}
};

};

#endif

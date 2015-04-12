#define HIGH_LEVEL_SYNTHESIS

#include <framework/agent.h>
#include "../../adder.h"

__USING_SYS

AGENT_BEGIN(Adder)
D_CALL_R_2(add, ADDER_ADD, int, int, int)
AGENT_END

#pragma hls_design top
HLS_TOP_LEVEL(Adder)

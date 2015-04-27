#define HIGH_LEVEL_SYNTHESIS

#include <framework/agent.h>
#include "../../XXNAMEXX.h"

__USING_SYS

#pragma hls_design top
void XXTOPXX_Top(Message::Channel & rx_ch, Message::Channel & tx_ch) {
    static Agent<XXTOPXX> agent(rx_ch, tx_ch);

    agent.exec();
}

#define HIGH_LEVEL_SYNTHESIS

#include <framework/agent.h>
#include <components/<<name>>.h>

__USING_SYS

void <<name>>_top(Message::Channel & rx_ch, Message::Channel & tx_ch) {
    Agent<<<Name>>> agent(rx_ch, tx_ch);
    //static Agent<XXTOPXX> agent(rx_ch, tx_ch);

    agent.exec();
}

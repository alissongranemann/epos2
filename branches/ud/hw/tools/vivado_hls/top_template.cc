#define HIGH_LEVEL_SYNTHESIS

#include <framework/agent.h>
#include <components/<<name>>.h>

__USING_SYS

void <<name>>_top(Message::Channel & rx, Message::Channel & tx) {
    Agent<<<Name>>> agent(rx, tx);
    //static Agent<XXTOPXX> agent(rx, tx);

    agent.exec();
}

/*
 * top_level.h
 *
 *  Created on: Feb 2, 2012
 *      Author: tiago
 */


#define HIGH_LEVEL_SYNTHESIS

/*
#include "../../src/sched.h"
using namespace System;

DECLARE_HLS_TOP_LEVEL(Sched_Thread)
*/

#include "../../agent.h"

class Agent_Dummy{

private:

    Implementation::Channel_t &rx_ch;
    Implementation::Channel_t &tx_ch;

    Catapult::RMI_Msg   msg;

public:

    Agent_Dummy(Implementation::Channel_t &_rx_ch, Implementation::Channel_t &_tx_ch, unsigned char _iid[1])
        :rx_ch(_rx_ch),
         tx_ch(_tx_ch){
    }

public:
    void top_level(){
        rx_ch.read(msg);
        tx_ch.write(msg);
    }

};

#pragma hls_design top
void Sched_Node (Implementation::Channel_t &rx_ch, Implementation::Channel_t &tx_ch, unsigned char iid[1]) {
    static Agent_Dummy agent(rx_ch,tx_ch,iid);
    agent.top_level();
}

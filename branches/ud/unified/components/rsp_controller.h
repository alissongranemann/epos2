#ifndef __rsp_controller_unified_h
#define __rsp_controller_unified_h

#include "component.h"

//Controller only in SW, so we use sw stuff
//But still uses #ifdef to avoid hw error
#ifndef HIGH_LEVEL_SYNTHESIS
#include <mach/epossoc/timer.h>
#endif

namespace Implementation {

class RSP_Controller : public Component{

public:
    enum {
        OP_TONE_DETECTED = 0xF2,
    };

public:

    RSP_Controller(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char iid[Traits<RSP_Controller>::n_ids])
        :Component(rx_ch, tx_ch, iid[0]){}

    void tone_detected(unsigned char tone){
#ifndef HIGH_LEVEL_SYNTHESIS
        volatile unsigned int * TIMER_REG = reinterpret_cast<volatile unsigned int *>(System::Traits<System::EPOSSOC_Timer>::BASE_ADDRESS);
        unsigned int next = *TIMER_REG + 400;
        while (next < *TIMER_REG );
        //System::Component_Manager::rsp_flag += 1;
#endif
    }

};

PROXY_BEGIN(RSP_Controller)
    void tone_detected(unsigned char tone){
        Base::call<RSP_Controller::OP_TONE_DETECTED>(tone);
    }
PROXY_END

AGENT_BEGIN(RSP_Controller)
    D_CALL_1(tone_detected,OP_TONE_DETECTED, unsigned char)
AGENT_END

};

DECLARE_COMPONENT(RSP_Controller);

#endif

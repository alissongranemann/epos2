#include <system/config.h>
#include __MODEL_H
#ifdef __emote3_h

#include <ic.h>

__USING_SYS

CC2538_TSC::Time_Stamp CC2538_TSC::_offset = 0;

void CC2538_TSC::wake_up_at(CC2538_TSC::Time_Stamp t, const CC2538_TSC::Interrupt_Handler & handler)
{    
    IC::unpend(SM_TIMER_IRQ);
    IC::disable(SM_TIMER_IRQ);
    IC::int_vector(IC::irq2int(SM_TIMER_IRQ), handler);

    // Won't take effect until wfi is asserted
    //eMote3::wake_up_on(eMote3::WAKE_UP_EVENT::SLEEP_MODE_TIMER);
    //eMote3::power_mode(eMote3::DEFAULT_SLEEP_MODE);

    //if(eMote3::DEFAULT_SLEEP_MODE == eMote3::POWER_MODE_1)
    //    t -= (eMote3::PM1_EXIT_TIME * frequency()) / 1000000;    
    //else if(eMote3::DEFAULT_SLEEP_MODE == eMote3::POWER_MODE_2)
    //    t -= (eMote3::PM2_EXIT_TIME * frequency()) / 1000000;

    while(!(reg(SMWDTHROSC_STLOAD) & STLOAD));

    reg(SMWDTHROSC_ST3) = t >> 24;
    reg(SMWDTHROSC_ST2) = t >> 16;
    reg(SMWDTHROSC_ST1) = t >> 8;
    reg(SMWDTHROSC_ST0) = t; // ST0 must be written last

    IC::enable(SM_TIMER_IRQ);
}

#endif

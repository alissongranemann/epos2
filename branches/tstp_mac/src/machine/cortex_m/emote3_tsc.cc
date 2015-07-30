#include <system/config.h>
#include __MODEL_H
#ifdef __emote3_h

#include <ic.h>

__USING_SYS

CC2538_TSC::Time_Stamp CC2538_TSC::_offset = 0;

void CC2538_TSC::wake_up_at(const CC2538_TSC::Time_Stamp & t, const CC2538_TSC::Interrupt_Handler & handler)
{    
    IC::unpend(SM_TIMER_IRQ);
    IC::disable(SM_TIMER_IRQ);
    IC::int_vector(IC::irq2int(SM_TIMER_IRQ), handler);

    while(!(reg(SMWDTHROSC_STLOAD) & STLOAD));

    reg(SMWDTHROSC_ST3) = t >> 24;
    reg(SMWDTHROSC_ST2) = t >> 16;
    reg(SMWDTHROSC_ST1) = t >> 8;
    reg(SMWDTHROSC_ST0) = t; // ST0 must be written last

    IC::enable(SM_TIMER_IRQ);
}

#endif

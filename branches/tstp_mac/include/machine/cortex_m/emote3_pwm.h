#ifndef __emote3_pwm_h_
#define __emote3_pwm_h_

#include "timer.h"

__BEGIN_SYS

class eMote3_PWM : private CC2538_GPTIMER, private Cortex_M_Model
{
public:
    const static unsigned int CLOCK = Traits<CPU>::CLOCK;

    typedef CPU::Reg32 Reg32;

    eMote3_PWM(unsigned int which_timer, unsigned int frequency_hertz, unsigned char duty_cycle_percent, char gpio_port = 'd', unsigned int gpio_pin = 2): 
        _base(reinterpret_cast<volatile Reg32*>(GPTIMER0_BASE + 0x1000 * (which_timer < 4 ? which_timer : 0)))
    {
        _frequency_hertz = frequency_hertz > CLOCK ? CLOCK : frequency_hertz;
        _duty_cycle_percent = duty_cycle_percent > 100 ? 100 : duty_cycle_percent;
        Reg32 period = CLOCK / _frequency_hertz;
        Reg32 time_low = period - ((period * _duty_cycle_percent) / 100);

        disable();
        Cortex_M_Model::config_PWM(which_timer, gpio_port, gpio_pin);
        reg(CFG) = 4; // 16-bit timer (only possible for PWM)
        reg(TAMR) |= TACMR;
        reg(TAMR) |= (2 * TAMR_TAMR) | TAAMS;
        if((_duty_cycle_percent == 0) || (_duty_cycle_percent == 100))
            reg(CTL) |= TAPWML;
        else
            reg(CTL) &= ~TAPWML;
        // 5. If a prescaler is to be used, write the prescale value to the GPTM Timer n Prescale Register (GPTIMER_TnPR).
        // 6. If PWM interrupts are used, configure the interrupt condition in the TnEVENT field in the GPTIMER_CTL register and enable the interrupts by setting the TnPWMIE bit in the GPTIMER_TnMR register.

        load_value(period);
        match_value(time_low);
    }

    ~eMote3_PWM()
    {
        disable();
    }

    volatile Reg32 read() { return reg(TAV); }

    void set(unsigned int frequency_hertz, unsigned char duty_cycle_percent)
    {
        _frequency_hertz = frequency_hertz > CLOCK ? CLOCK : frequency_hertz;
        _duty_cycle_percent = duty_cycle_percent > 100 ? 100 : duty_cycle_percent;
        Reg32 period = CLOCK / _frequency_hertz;
        Reg32 time_low;
        if(_duty_cycle_percent == 100)
            time_low = period;
        else
            time_low = period - ((period * _duty_cycle_percent) / 100);

        disable();
        if(_duty_cycle_percent == 0)
            reg(CTL) |= TAPWML;
        else
            reg(CTL) &= ~TAPWML;
        load_value(period);
        match_value(time_low);
        enable();
    }
    void disable()
    {
        reg(CTL) &= ~TAEN; // Disable timer A
    }
    void enable()
    {
        reg(CTL) |= TAEN; // Enable timer A
    }

    unsigned int frequency() { return _frequency_hertz; }
    unsigned int duty_cycle() { return _duty_cycle_percent; }

protected:
    volatile Reg32 & reg(unsigned int o) { return _base[o / sizeof(Reg32)]; }

    volatile Reg32* _base;

private:
    unsigned int _duty_cycle_percent;
    unsigned int _frequency_hertz;

    void load_value(const Reg32 & val)
    {
        reg(TAPR) = val >> 16;
        reg(TAILR) = val;
    }
    void match_value(const Reg32 & val)
    {
        reg(TAPMR) = val >> 16;
        reg(TAMATCHR) = val;
    }
};

__END_SYS

#endif

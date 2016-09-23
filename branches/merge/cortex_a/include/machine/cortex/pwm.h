// EPOS ARM Cortex PWM Mediator Declarations

#ifndef __cortex_pwm_h
#define __cortex_pwm_h

#include <timer.h>
#include __MODEL_H

__BEGIN_SYS

class PWM: private PWM_Common, private Machine_Model
{
public:
    PWM(User_timer * timer, GPIO * gpio, const Percent & duty_cycle)
    : _timer(timer), _gpio(gpio) {
        timer->pwm(duty_cycle);
        enable_pwm(timer->_channel, gpio->_port, gpio->_pin);
    }
    ~PWM() { disable_pwm(timer->_channel, gpio->_port, gpio->_pin); }

    void enable() { _timer->enable(); }
    void disable() { _timer->disable(); }
    void power(const Power_Mode & mode) { timer->power(mode); }
};

__END_SYS

#endif

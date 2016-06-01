// EPOS Cortex-A AXI Traffic Generator Mediator

#ifndef __cortex_a_traffic_gen_h
#define __cortex_a_traffic_gen_h

#include <traffic_gen.h>
#include "gpio.h"

__BEGIN_SYS

class Cortex_A_Traffic_Gen: public Traffic_Gen_Common
{
public:
    Cortex_A_Traffic_Gen(int start_pin, int stop_pin) {
        _start = new GPIO(start_pin);
        _stop = new GPIO(stop_pin);
        _start->output();
        _stop->output();
    }

    ~Cortex_A_Traffic_Gen() { disable(); }

    // TODO: Sometimes, even after fpga_reset(), enable() doesn't work, check
    // why
    void enable() {
        _stop->put(false);
        _start->put(true);
        Alarm::delay(10000);
    }

    // After disabling the peripheral, it cannot be enabled again
    void disable() {
        _stop->put(true);
        _start->put(false);
        Alarm::delay(10000);
    }

private:
    GPIO * _start;
    GPIO * _stop;
};

__END_SYS

#endif

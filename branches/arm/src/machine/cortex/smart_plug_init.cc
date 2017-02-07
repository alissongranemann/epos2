// EPOS Cortex Smart Plug Mediator Initialization

#include <smart_plug.h>

#ifdef __SMART_PLUG_H

#include <adc.h>

__BEGIN_SYS

void Smart_Plug::init()
{
    db<Init, Smart_Plug>(TRC) << "Smart_Plug::init()" << endl;

    _dev[0] = new (SYSTEM) Power_Meter(ADC::SINGLE_ENDED_ADC7, ADC::SINGLE_ENDED_ADC5, ADC::GND);
    _dev[1] = new (SYSTEM) Power_Meter(ADC::SINGLE_ENDED_ADC7, ADC::SINGLE_ENDED_ADC6, ADC::GND);
}

__END_SYS

#endif

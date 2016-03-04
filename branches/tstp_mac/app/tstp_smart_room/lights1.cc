#include <tstp.h>
#include <gpio.h>
#include <machine/cortex_m/emote3_power_meter.h>

using namespace EPOS;

OStream cout;
GPIO * led, * coil;
bool led_state = false;
bool coil_state = false;

Power_Meter * pm0;

typedef TSTP::Data Data;

int dummy = 0;
Data sense_power()
{
    return dummy++;
    auto ret = pm0->average();
    return ret;
}

int main()
{
    cout << "LISHA Smart Room Power meter" << endl;
    cout << "Pins" << endl
         << "   Power meters: PA7, PA6 and PA5" << endl;

    coil = new GPIO('d',3, GPIO::OUTPUT);

    pm0 = new Power_Meter(ADC::SINGLE_ENDED_ADC7, ADC::SINGLE_ENDED_ADC5, ADC::GND);

    coil_state = true;
    led->set(led_state);
    coil->set(coil_state);

    TSTP_MAC::address(TSTP_MAC::Address(320,-110,220));
    TSTP::Watt W;
    TSTP::Sensor s(W, &sense_power, 1, 500000);

    while(true);

    return 0;
}

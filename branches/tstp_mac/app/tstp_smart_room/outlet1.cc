#include <tstp.h>
#include <gpio.h>
#include <machine/cortex_m/emote3_power_meter.h>

using namespace EPOS;

OStream cout;
GPIO * led, * coil0, * coil1;
bool led_state = false;
bool coil0_state = false;
bool coil1_state = false;

Power_Meter * pm0, * pm1;

typedef TSTP::Data Data;

int dummy = 0;
Data sense_power0()
{
    return dummy++;
    auto ret = pm0->average();
    return ret;
}

Data sense_power1()
{
    auto ret = pm1->average();
    return ret;
}

int main()
{
    cout << "LISHA Smart Room Power meter" << endl;
    cout << "Pins" << endl
         << "   Power meters: PA7, PA6 and PA5" << endl;

    // FIXME: coil0 and coil1 are not being used, remove them
    coil0 = new GPIO('b',0, GPIO::OUTPUT);
    coil1 = new GPIO('b',1, GPIO::OUTPUT);

    pm0 = new Power_Meter(ADC::SINGLE_ENDED_ADC7, ADC::SINGLE_ENDED_ADC5, ADC::GND);
    pm1 = new Power_Meter(ADC::SINGLE_ENDED_ADC7, ADC::SINGLE_ENDED_ADC6, ADC::GND);

    led->set(led_state);
    coil0->set(coil0_state);
    coil1->set(coil1_state);

    TSTP_MAC::address(TSTP_MAC::Address(-30,-50,-40));
    TSTP::Watt W;
    TSTP::Sensor s(W, &sense_power0, 1, 500000);

    while(true);

    return 0;
}

#include <adc.h>
#include <tstp.h>

using namespace EPOS;

OStream cout;

TSTP::Data sense_temperature()
{
    const auto ADC_PIN = ADC::SINGLE_ENDED_ADC6;
    return ADC{ADC_PIN}.read();
}

int main()
{
    cout << "TSTP Sensor test" << endl;

    TSTP::Unit kelvin(TSTP::Unit::KELVIN);
    TSTP::Sensor s(kelvin, &sense_temperature, 1, 500000);

    while(true);

    return 0;
}

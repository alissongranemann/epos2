#include <adc.h>
#include <tstp.h>

using namespace EPOS;

OStream cout;

TSTP::Data sense_temperature()
{
    static TSTP::Data temp;
    cout << temp << endl;
    return temp++;
    const auto ADC_PIN = ADC::SINGLE_ENDED_ADC6;
    return ADC{ADC_PIN}.read();
}

int main()
{
    TSTP_MAC::address(TSTP_MAC::Address(123,123,0));

    cout << "TSTP Sensor test" << endl;

    TSTP::Kelvin kelvin;
    TSTP::Sensor s(kelvin, &sense_temperature, 1, 500000);

    while(true);

    return 0;
}

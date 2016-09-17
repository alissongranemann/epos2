#include <adc.h>
#include <alarm.h>
#include <utility/ostream.h>

using namespace EPOS;

int main()
{
    OStream cout;
    ADC adc1(ADC::SINGLE_ENDED_ADC5);
    ADC adc2(ADC::SINGLE_ENDED_ADC7);

    while(true) {
        cout << "ADC pin 5 = " << adc1.read() << ", ADC pin 7 = " << adc2.read() << endl;
        Alarm::delay(1000000);
    }

    return 0;
}

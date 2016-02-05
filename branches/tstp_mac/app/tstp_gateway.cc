#include <alarm.h>
#include <tstp.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "TSTP Gateway test" << endl;

    TSTP::Data temperature;
    TSTP::Unit kelvin(TSTP::Unit::KELVIN);
    TSTP::Interest in(kelvin, &temperature, 1, 1000000);

    while(true) {
        Alarm::delay(1000000);
        cout << temperature << endl;
    }

    return 0;
}

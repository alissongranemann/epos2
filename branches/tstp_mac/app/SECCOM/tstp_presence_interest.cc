#include <adc.h>
#include <tstp.h>
#include <gpio.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "TSTP Presence interest" << endl;

    GPIO led('c', 3, GPIO::OUTPUT);

    TSTP::Data presence;
    TSTP::Unit presence_unit(TSTP::Unit::PRESENCE);
    TSTP::Interest in(presence_unit, &presence, 1, 1000000);

    while(true) {
        led.set(presence);
    }

    return 0;
}

#include <adc.h>
#include <tstp.h>
#include <gpio.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "TSTP Lux interest" << endl;

    GPIO led('c', 3, GPIO::OUTPUT);

    TSTP::Data lux;
    TSTP::Unit lx(TSTP::Unit::CANDELA);
    lx.meter(-2);
    TSTP::Interest in(lx, &lux, 100, 1000000);

    while(true) {
        cout << lux << endl;
        Alarm::delay(1000000);
    }

    return 0;
}


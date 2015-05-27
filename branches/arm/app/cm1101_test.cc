#include <utility/ostream.h>
#include <alarm.h>

#include "../include/machine/cortex_m/uart.h"
#include "../include/machine/cortex_m/cm1101.h"

using namespace EPOS;

OStream cout;

int main()
{
    Cortex_M_UART u(9600, 8, 0, 1, 0);
    Cortex_M_CM1101 cm1101(&u);

    cout << "Cortex_M_CM1101 test" << endl;

    while(1) {
        cout << "Cortex_M_CM1101.sample() = " << cm1101.sample() << endl;
        Alarm::delay(1000000);
    }

    return 0;
}

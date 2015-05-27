#include <utility/ostream.h>
#include <alarm.h>

#include "../include/machine/cortex_m/uart.h"
#include "../include/machine/cortex_m/cm1101.h"

using namespace EPOS;

int main()
{
    Cortex_M_UART u(9600, 8, 0, 1, 0);
    Cortex_M_CM1101 cm1101(&u);
    NIC * nic = new NIC();
    char data[2];
    int co2;

    while(1) {
        co2 = cm1101.co2();

        data[0] = co2&0xff;
        data[1] = (co2>>8)&0xff;

        auto buf = nic->alloc(nic, nic->broadcast(), 0x1010, 0, 0, sizeof data);
        memcpy(buf->frame()->data<char>(), data, sizeof data);
        nic->send(buf);
        nic->free(buf);

        Alarm::delay(1000000);
    }

    return 0;
}

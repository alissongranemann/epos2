#include <tstp.h>
#include <gpio.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "LISHA Smart Room Replicator" << endl;

    GPIO led('c',3,GPIO::OUTPUT);
    led.set();

    TSTP_MAC::address(TSTP_MAC::Address(570,175,10));

    while(true);

    return 0;
}

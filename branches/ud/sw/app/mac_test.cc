#include <components/mac.h>
#include <utility/ostream.h>
#include <chronometer.h>
#include <alarm.h>
#include "mach/epossoc/pcap.h"

__USING_SYS

OStream cout;

void call_mac(MAC &mac, unsigned int a, unsigned int b) {
    Chronometer chrono;
    unsigned int result;

    cout << "Calling mac.mac(" << a << ", " << b << ")" << endl;

    chrono.reset();
    chrono.start();
    result = mac.mac(a,b);
    chrono.stop();

    cout << "Result = " << result << " (in " << chrono.ticks() << " cycles)" << endl;
}

void call_recfg(MAC &mac, int domain) {
    Chronometer chrono;

    cout << "Calling mac.recfg(" << domain << ")" << endl;

    chrono.reset();
    chrono.start();
    mac.recfg(domain);
    chrono.stop();

    cout << chrono.ticks() << " cycles" << endl;
}

int main()
{
    volatile unsigned int * gpio = (volatile unsigned int *)(Traits<Machine>::LEDS_ADDRESS);

    *gpio = (1<<31) | (1<<7);

    MAC mac(Component_Manager::dummy_channel, Component_Manager::dummy_channel, 0);

    cout << "MAC Test" << endl;

    call_mac(mac, 5, 10);
    call_recfg(mac, MAC::HARDWARE);
    call_mac(mac, 10, 10);
    call_recfg(mac, MAC::SOFTWARE);
    call_mac(mac, 3, 3);
    call_recfg(mac, MAC::HARDWARE);
    call_mac(mac, 5, 5);
    call_recfg(mac, MAC::SOFTWARE);

    cout << "The End" << endl;

    return 0;
}

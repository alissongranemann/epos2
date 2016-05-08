#include <smart_data.h>
#include <alarm.h>
#include <thread.h>

using namespace EPOS;

OStream cout;

void sensor(const NIC::Address & mac);
void sink(const NIC::Address & mac);

int main()
{
    cout << "TSTP test" << endl;

    Network::init();

    NIC nic;
    NIC::Address mac = nic.address();

    if(mac[5] % 2)
        sink(mac);
    else
        sensor(mac);

    cout << "I'm done, bye!" << endl;

    return 0;
}

void sink(const NIC::Address & mac)
{
    cout << "I'm the sink!" << endl;
    cout << "I'm running on a machine whose MAC address is " << mac << "." << endl;
    cout << "I'll now declare my interests ..." << endl;

    Acceleration a0(Region(Coordinates(1, 1, 1), 10, 20, 30), 1000000); // Remote, from a region centered at (1, 1, 1), with radius 10, from time 20 to 30, updated on each event, with expiration time of 1s.
    Acceleration a1(Region(Coordinates(1, 1, 1), 10, 20, 30), 1000000, 1000000); // Remote, from a region centered at (1, 1, 1), with radius 10, from time 20 to 30, updated every ten seconds, with expiration time of 1s.

//    Thread::yield();

    do {
        cout << "a0=" << a0 << endl;
        cout << "a1=" << a1 << endl;
        Delay (1000000);
    } while((a0 != 'a') && (a1 != 'a'));
}

void sensor(const NIC::Address & mac)
{
    cout << "I'm the sensor!" << endl;
    cout << "I'm running on a machine whose MAC address is " << mac << "." << endl;
    cout << "I'll now create sensors ..." << endl;

    Acceleration a0(0, 1000000); // Local, private, with expiration time of 1 s.
    Acceleration a1(1, 10000000, Acceleration::COMMANDED); // Local, commanded, with local expiration time of 10 s.

    while(1);
    Delay(30000000);
}


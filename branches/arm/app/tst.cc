// EPOS cout Test Program

#include <utility/ostream.h>
#include <utility/random.h>
#include <network.h>
#include <wiegand.h>

using namespace EPOS;

OStream cout;
TSTP * tstp;

Wiegand::Door_State_1 door_1;
Wiegand::Door_State_2 door_2;
Wiegand::Door_State_3 door_3;
Wiegand::Door_State_4 door_4;
Wiegand::RFID_1 rfid_1;
Wiegand::RFID_2 rfid_2;
Wiegand::RFID_3 rfid_3;
Wiegand::RFID_4 rfid_4;

TSTP::Meter m;

unsigned int val = 0;

void hello_sensor(TSTP::Sensor * s)
{
    cout << "Measuring!" << endl;
    cout << "measurement = " << val << endl;
    cout << "m = " << *(m.data<unsigned int>()) << endl;
    cout << "now = " << s->tstp()->time() << endl;
}

void hello_interest(TSTP::Interest * s)
{
    cout << "Received update!" << endl;
    cout << "Interest = " << *s << endl;
    cout << "Value = " << *(s->data<unsigned int>()) << endl;
    cout << "Time = " << s->last_update() << endl;
    cout << "now = " << s->tstp()->time() << endl;
}

void init()
{
    int n = 25000;
    while(n--) {
        cout << n << endl;
    }
    Network::init();
    tstp = TSTP::get_by_nic(0);
    tstp->bootstrap();
    if(!tstp) {
        while(1) {
            cout << "Waaaahhh" << endl;
        }
    }
}

void sensor()
{
    TSTP::Sensor id1(tstp, &rfid_1, 0, 0, 0);
    TSTP::Sensor ds1(tstp, &door_1, 0, 0, 0);
    for(int i = 0; ; i++) {
        Alarm::delay(2000000);
        auto d = rfid_1.data<Wiegand::ID_Code_Msg>();
        d->facility = i++;
        d->serial = i++;
        auto val = door_1.data<bool>();
        *val = !(*val);

        cout << "Event!" << endl; 
        cout << "facility = " << d->facility << endl;
        cout << "serial = " << d->serial << endl;
        cout << "door_1 = " << *val << endl;
        cout << "time = " << tstp->time() << endl;

        tstp->event(rfid_1);

        tstp->event(door_1);
    }
}

int main()
{
    init();
    sensor();
    while(true);

    return 0;
}

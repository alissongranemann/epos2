// EPOS cout Test Program

#include <utility/ostream.h>
#include <utility/random.h>
#include <network.h>

using namespace EPOS;

OStream cout;
TSTP * tstp;

template<unsigned int DEVICE>
class Door_State : public TSTP::User_Unit<1000 + DEVICE, 1> { };
template<unsigned int DEVICE>
class RFID : public TSTP::User_Unit<2000 + DEVICE, 3> { };

Door_State<1> door_1;
Door_State<2> door_2;
Door_State<3> door_3;
Door_State<4> door_4;
RFID<1> rfid_1;
RFID<2> rfid_2;
RFID<3> rfid_3;
RFID<4> rfid_4;

TSTP::Meter * m;

void hello_sensor(TSTP::Sensor * s) 
{
    cout << "Measuring!" << endl;
    auto val = Random::random();
    cout << "measurement = " << val << endl;
    (*m) = val;
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
    if(!tstp) {
        while(1) {
            cout << "Waaaahhh" << endl;
        }
    }
}

void sensor()
{
    m = new TSTP::Meter;
    auto s = new TSTP::Sensor(tstp, m, 0, 0, 0, &hello_sensor);
    cout << *reinterpret_cast<TSTP::Data_Message*>(s) << endl;
}

void interest()
{
    m = new TSTP::Meter;
    auto t0 = tstp->time();
    auto i = new TSTP::Interest(tstp, m, TSTP::Remote_Address(0, 0, 0, 0), t0 + 2000000, t0 + 100000000, 10000000, 1, TSTP::RESPONSE_MODE::SINGLE, &hello_interest);

    cout << *i << endl;
}

int main()
{
    init();
    //sensor();
    //while(true);
    interest();
    while(true);

    return 0;
}

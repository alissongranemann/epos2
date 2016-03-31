// EPOS cout Test Program

#include <utility/ostream.h>
#include <network.h>

using namespace EPOS;

OStream cout;

void hello(TSTP::Sensor * s) {
    cout << "Measuring!" << endl;
    cout << s->tstp()->time() << endl;
}

/*
void hello_timer(const unsigned int & int_n) {
    cout << "hello_timer(" << int_n << ") : " << MAC_Timer::read() << endl;
}
*/

int main()
{
    int n = 25000;
    while(n--) {
        cout << n << endl;
    }
    /*
    MAC_Timer::config();
    MAC_Timer::start();
    auto a = MAC_Timer::read() + 2300000;
    MAC_Timer::interrupt(MAC_Timer::read() + 2300000, hello_timer);
    while(1);
    */
    //while(1);

    Network::init();
    TSTP * tstp = TSTP::get_by_nic(0);
    if(!tstp) {
        while(1) {
            cout << "Waaaahhh" << endl;
        }
    }

    TSTP::Meter m;
    
    TSTP::Sensor s(tstp, &m, 0, 0, 0, &hello);

    cout << tstp->time() << endl;
    auto t0 = tstp->time();
    /*
    tstp->_time->interrupt(t0 + 2000000);
    //Alarm::delay(3000000);
    auto _t = tstp->time() / 1000000;

    while(1) {
        auto t = tstp->time() / 1000000;
        if(t > _t) {
            cout << tstp->time() << endl;
            _t = t;
        }
    }
    */

    TSTP::Interest i(tstp, &m, TSTP::Remote_Address(0, 0, 0, 0), t0 + 2000000, t0 + 10000000, 1000000, 1, TSTP::RESPONSE_MODE::SINGLE);

    cout << i << endl;

    tstp->_mac->update(tstp->_mac->alloc(sizeof(TSTP_API::Interest_Message), reinterpret_cast<TSTP_API::Frame *>(&i)));

    cout << "Interest sent" << endl;
    while(true);

    return 0;
}

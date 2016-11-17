// EPOS TI CC2538 IEEE 802.15.4 NIC Mediator Test Program

#include <smart_data.h>
#include <utility/ostream.h>
#include <nic.h>
#include <gpio.h>
#include <periodic_thread.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "TSTP MAC test" << endl;
    cout << "Configuration: " << endl;

    /*
    cout << "INT_HANDLING_DELAY = " << TSTP_MAC<CC2538RF>::INT_HANDLING_DELAY << endl;
    cout << "TX_DELAY = " << TSTP_MAC<CC2538RF>::TX_DELAY << endl;
    cout << "G = " << TSTP_MAC<CC2538RF>::G << endl;
    cout << "Tu = " << TSTP_MAC<CC2538RF>::Tu << endl;
    cout << "Ti = " << TSTP_MAC<CC2538RF>::Ti << endl;
    cout << "TIME_BETWEEN_MICROFRAMES = " << TSTP_MAC<CC2538RF>::TIME_BETWEEN_MICROFRAMES << endl;
    cout << "Ts = " << TSTP_MAC<CC2538RF>::Ts << endl;
    cout << "MICROFRAME_TIME = " << TSTP_MAC<CC2538RF>::MICROFRAME_TIME << endl;
    cout << "Tr = " << TSTP_MAC<CC2538RF>::Tr << endl;
    cout << "RX_MF_TIMEOUT = " << TSTP_MAC<CC2538RF>::RX_MF_TIMEOUT << endl;
    cout << "NMF = " << TSTP_MAC<CC2538RF>::NMF << endl;
    cout << "N_MICROFRAMES = " << TSTP_MAC<CC2538RF>::N_MICROFRAMES << endl;
    cout << "CI = " << TSTP_MAC<CC2538RF>::CI << endl;
    cout << "PERIOD = " << TSTP_MAC<CC2538RF>::PERIOD << endl;
    cout << "SLEEP_PERIOD = " << TSTP_MAC<CC2538RF>::SLEEP_PERIOD << endl;
    cout << "DUTY_CYCLE = " << TSTP_MAC<CC2538RF>::DUTY_CYCLE << endl;
    cout << "DATA_LISTEN_MARGIN = " << TSTP_MAC<CC2538RF>::DATA_LISTEN_MARGIN << endl;
    cout << "DATA_SKIP_TIME = " << TSTP_MAC<CC2538RF>::DATA_SKIP_TIME << endl;
    cout << "RX_DATA_TIMEOUT = " << TSTP_MAC<CC2538RF>::RX_DATA_TIMEOUT << endl;
    cout << "CCA_TIME = " << TSTP_MAC<CC2538RF>::CCA_TIME << endl;
    */

    cout << "sizeof(Microsecond) = " << sizeof(RTC_Common::Microsecond) << endl;
    cout << "sizeof(CC2538RF::Timer::Time_Stamp) = " << sizeof(CC2538RF::Timer::Time_Stamp) << endl;

    cout << "Machine::id() =";
    for(unsigned int i = 0; i < 8; i++)
        cout << " " << hex << Machine::id()[i];
    cout << endl;
    cout << "TSTP::here() = " << TSTP::here() << endl;
    Region dst(Coordinates(50, 50, 50), 10, 0, 20000000);

    cout << "dst.center - here = " << dst.center - TSTP::here() << endl;
    cout << "here - dst.center  = " << TSTP::here() - dst.center << endl;

    if(dst.contains(TSTP::here(), TSTP::now() + 0)) {
        cout << "Sensor" << endl;
        Acceleration a0(0, 2000000, Acceleration::ADVERTISED);
        while(true);
    } else {
        cout << "Sink" << endl;
        Acceleration a0(dst, 3000000, 1000000);
        while(true);
    }

    return 0;
}

#include <utility/ostream.h>
#include <nic.h>
#include <gpio.h>

using namespace EPOS;

int main()
{
    OStream cout;

    TSTP_MAC::address(TSTP_MAC::Address(1,1,0));

    eMote3_GPTM::delay(2000000);

    cout << "Hi, I'm the slave!" << endl;

    cout << endl;
    cout << endl;
    cout << endl;
    cout << "== TSTP MAC Configuration ==" << endl;
    cout << "MAX_SEND_TRIALS : " << Traits<TSTP_MAC>::MAX_SEND_TRIALS << endl;
    cout << "PERIOD : " << Traits<TSTP_MAC>::PERIOD << endl;
    cout << "ADDRESS_X : " << Traits<TSTP_MAC>::ADDRESS_X << endl;
    cout << "ADDRESS_Y : " << Traits<TSTP_MAC>::ADDRESS_Y << endl;
    cout << "ADDRESS_Z : " << Traits<TSTP_MAC>::ADDRESS_Z << endl;
    cout << "Tu : " << Traits<TSTP_MAC>::Tu << endl;
    cout << "G : " << Traits<TSTP_MAC>::G << endl;
    cout << "DATA_LISTEN_MARGIN : " << Traits<TSTP_MAC>::DATA_LISTEN_MARGIN << endl;
    cout << "Ts : " << Traits<TSTP_MAC>::Ts << endl;
    cout << "MICROFRAME_TIME : " << Traits<TSTP_MAC>::MICROFRAME_TIME << endl;
    cout << "N_MICROFRAMES : " << Traits<TSTP_MAC>::N_MICROFRAMES << endl;
    cout << "TIME_BETWEEN_MICROFRAMES : " << Traits<TSTP_MAC>::TIME_BETWEEN_MICROFRAMES << endl;
    cout << "DATA_SKIP_TIME : " << Traits<TSTP_MAC>::DATA_SKIP_TIME << endl;
    cout << "DATA_ACK_TIMEOUT : " << Traits<TSTP_MAC>::DATA_ACK_TIMEOUT << endl;
    cout << "RX_MF_TIMEOUT : " << Traits<TSTP_MAC>::RX_MF_TIMEOUT << endl;
    cout << "RX_DATA_TIMEOUT : " << Traits<TSTP_MAC>::RX_DATA_TIMEOUT << endl;
    cout << "SLEEP_PERIOD : " << Traits<TSTP_MAC>::SLEEP_PERIOD << endl;
    cout << "RADIO_RADIUS : " << Traits<TSTP_MAC>::RADIO_RADIUS << endl;
    cout << "DUTY_CYCLE : " << Traits<TSTP_MAC>::DUTY_CYCLE << endl;
    cout << endl;
    cout << endl;
    cout << endl;

    cout << "Just waiting" << endl;

    while(1);

    return 0;
}

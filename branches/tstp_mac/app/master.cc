#include <timer.h>
#include <utility/ostream.h>
#include <utility/math.h>
#include <nic.h>
#include <gpio.h>

using namespace EPOS;

NIC nic;
OStream cout;

template<typename X, typename Y>
bool _assert(X got, Y expected, unsigned int line) 
{
    bool ret = got == expected; 
    if(!ret) {
        cout << "==============Assertion in line " <<  line << " failed!==============" << endl;
        cout << "got: " << got << endl;
        cout << "expected: " << expected << endl;
        cout << "difference: " << Math::abs(expected - got) << endl;
        GPIO trigger('c',3,GPIO::OUTPUT);
        trigger.set();
    }
}

void print_statistics()
{
    static int times_called;
    TSTP_MAC::Statistics ts = TSTP_MAC::statistics();
    cout << "===" << endl;
    cout << "tx_payload_frames = " << ts.tx_payload_frames << endl;
    cout << "rx_payload_frames = " << ts.rx_payload_frames << endl;
    cout << "dropped_payload_frames = " << ts.dropped_payload_frames << endl;
    cout << "waited_to_rx_payload = " << ts.waited_to_rx_payload << endl;

    TSTP_MAC::Statistics s = nic.statistics();
    cout << "rx_packets = " << s.rx_packets << endl;
    cout << "rx_bytes = " << s.rx_bytes << endl;
    cout << "tx_packets = " << s.tx_packets << endl;
    cout << "tx_bytes = " << s.tx_bytes << endl;
    cout << "dropped_rx_packets = " << s.dropped_rx_packets << endl;
    cout << "dropped_rx_bytes = " << s.dropped_rx_bytes << endl;
    cout << "dropped_tx_packets = " << s.dropped_tx_packets << endl;
    cout << "dropped_tx_bytes = " << s.dropped_tx_bytes << endl;
    cout << "===" << endl;

    ++times_called;

    auto this_line = 49u;
    _assert(s.tx_packets, (times_called) * (Traits<TSTP_MAC>::N_MICROFRAMES + 1), ++this_line);
    _assert(s.rx_packets, (s.tx_packets / (Traits<TSTP_MAC>::N_MICROFRAMES + 1)) * 2, ++this_line);
    _assert(ts.rx_payload_frames, ts.tx_payload_frames, ++this_line);
    _assert(ts.dropped_payload_frames, 0, ++this_line);
    _assert(ts.waited_to_rx_payload, ts.rx_payload_frames, ++this_line);
}

int main()
{
    TSTP_MAC::address(TSTP_MAC::Address(0,0,0));

    eMote3_GPTM::delay(2000000);

    cout << "Hi, I'm the master!" << endl;

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

    int n_interests = 0;
    while(1) {
        cout << "Building Interest" << endl;
        int x_coord = Random::random();
        int y_coord = Random::random();
        x_coord %= (Traits<TSTP_MAC>::RADIO_RADIUS / 4);
        x_coord += Traits<TSTP_MAC>::RADIO_RADIUS / 4;
        y_coord %= (Traits<TSTP_MAC>::RADIO_RADIUS / 4);
        y_coord += Traits<TSTP_MAC>::RADIO_RADIUS / 4;
        TSTP_MAC::Interest i(TSTP_MAC::Address(x_coord, y_coord, 0), 1000000, 5000000, 1000000, 1, 100, 0);

        cout << "Sending Interest " << ++n_interests << endl;
        TSTP_MAC::send(&i);
        eMote3_GPTM::delay(Traits<TSTP_MAC>::PERIOD * 5);// + (Random::random() % 1000000));
        print_statistics();
    }

    cout << "Done!" << endl;

    return 0;
}

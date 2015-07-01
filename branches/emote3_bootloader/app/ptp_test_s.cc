// EPOS PTP Protocol Test Program

#include <utility/ostream.h>
#include <ptp.h>
#include <alarm.h>

const char Traits<Build>::ID[Traits<Build>::ID_SIZE] = {'A','1'};

using namespace EPOS;

OStream cout; 

int main() {
    bool master = false;
    cout << "PTP test" << endl;

    NIC nic;

    PTP * ptp;

    nic.address(master ? "42:42" : "43:43");

    cout << "  MAC: " << nic.address() << endl;

//     if(nic.address()[5] % 2) { // sender
    if(master)
    {
        cout << "Master:" << endl;

        ptp = new PTP(true);

        if (ptp->synchronize_slaves()) {
            cout << "  Synchronization successful!" << endl;
        } else {
            cout << "  Synchronization unsuccessful!" << endl;

            return -1;
        }
    } else {
        cout << "Slave:" << endl;

        ptp = new PTP(false);

//         Alarm::delay(1000 * 1000 * 3);
        while(1);

        cout << "Fin." << endl;
    }

    NIC::Statistics stat = nic.statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << endl;

    return stat.tx_bytes + stat.rx_bytes;
}

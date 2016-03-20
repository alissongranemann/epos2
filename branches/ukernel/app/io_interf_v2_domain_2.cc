/* Specification: see io_interf_v2.cc
 *
 * Implementation notes:
 * This file implements Domain 2.
 * Assuming Domain 0 is implemented in pc_loader.cc
 * Assuming Domain 1 is implemented in io_interf_v2.cc
 *
 * */

#include <utility/ostream.h>
#include <communicator.h>

using namespace EPOS;

const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_TASK_ITERATIONS;
const int PDU = 500;

OStream cout;

int main()
{
    cout << "I/O interference - Version 2" << endl;
    cout << "This is Domain 2" << endl;
    cout << "Task on Guest OS 2 (Domain 2) starting..." << endl;

    cout << "TCP Test" << endl;

    char data[PDU];
    TCP_Link * com;

    IP * ip = IP::get_by_nic(0);

    cout << "  IP: " << ip->address() << endl;
    cout << "  MAC: " << ip->nic()->address() << endl;

    cout << "Receiver:" << endl;

    IP::Address peer_ip = ip->address();
    peer_ip[3]++;

    db<void>(WRN) << "Will listen at: " << ip->address() << ":8000" << endl;
    com = new TCP_Link(TCP::Port(8000)); // listen
    db<void>(WRN) << "Some client has connected" << endl;

    for(int i = 0; i < ITERATIONS; i++) {
        int received = com->read(&data, sizeof(data));
        if(received == sizeof(data))
            cout << "  Data: " << data << endl;
        else
            cout << "  Data was not correctly received. It was " << sizeof(data) << " bytes long, but " << received << " bytes were received!"<< endl;
    }

    delete com;

    NIC::Statistics stat = ip->nic()->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.get_tx_packets() << "\n"
         << "Tx Bytes:   " << stat.get_tx_bytes() << "\n"
         << "Rx Packets: " << stat.get_rx_packets() << "\n"
         << "Rx Bytes:   " << stat.get_rx_bytes() << endl;

    cout << "Task on Guest OS 2 (Domain 2) finishing..." << endl;

    return stat.get_tx_bytes() + stat.get_rx_bytes();
}

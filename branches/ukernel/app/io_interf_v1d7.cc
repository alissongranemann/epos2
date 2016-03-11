/* Specification:
 *
 * This application is composed by Domain 0 and Domain 1 that executes a
 * BE task that sets up a TCP/IP server and prints data from the received
 * TCP/IP packets.
 *
 * Implementation notes:
 * This file implements Domain 1.
 * Assuming Domain 0 is implemented in pc_loader.cc
 *
 * */

#include <utility/ostream.h>
#include <scheduler.h>
#include <periodic_thread.h>

using namespace EPOS;

const int ITERATIONS = _SYS::Traits<Application>::GUEST_OS_TASK_ITERATIONS;
const int PDU = 500;

OStream cout;

int main()
{
    cout << "I/O interference - Version 1.7" << endl;
    cout << "This is Domain 1" << endl;
    cout << "Task on Guest OS 1 (Domain 1) starting..." << endl;

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
         << "Tx Packets: " << stat.tx_packets() << "\n"
         << "Tx Bytes:   " << stat.tx_bytes() << "\n"
         << "Rx Packets: " << stat.rx_packets() << "\n"
         << "Rx Bytes:   " << stat.rx_bytes() << endl;

    cout << "Task on Guest OS 1 (Domain 1) finishing..." << endl;

    return stat.tx_bytes() + stat.rx_bytes();
}

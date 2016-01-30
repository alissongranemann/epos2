// EPOS PC_NIC Test Program - Zero-Copy version

#include <utility/ostream.h>
#include <nic.h>
#include <alarm.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "  NIC Zero-Copy Sender Application" << endl;

    NIC nic;
    NIC::Buffer * buf = 0;

    NIC::Address self = nic.address();
    cout << "  MAC: " << self << endl;

    for(int i = 0; i < 10; i++) {
        buf = nic.alloc(&nic, nic.broadcast(), 0x8888, 0, 0, nic.mtu()); // Buffer * alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload);

        memset(buf->frame()->data<void>(), '0' + i, nic.mtu());
        memset(buf->frame()->data<void>() + nic.mtu() - 1, '\n', 1);

        nic.send(buf);
        nic.free(buf);
    }


    NIC::Statistics stat = nic.statistics();
    cout << "Statistics\n"
    << "Tx Packets: " << stat.tx_packets << "\n"
    << "Tx Bytes:   " << stat.tx_bytes << "\n"
    << "Rx Packets: " << stat.rx_packets << "\n"
    << "Rx Bytes:   " << stat.rx_bytes << "\n";

    cout << "Bye!" << endl;

    while(true);
}

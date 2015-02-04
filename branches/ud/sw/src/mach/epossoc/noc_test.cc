
#include <utility/ostream.h>
#include <ic.h>
#include <noc.h>
#include <semaphore.h>

__USING_SYS



OStream cout;
Semaphore rx_sem(0);

enum {
    ECHO_P0_LOCAL_ADDR = NOC::Address::LOCAL_SW,
    ECHO_P1_LOCAL_ADDR = NOC::Address::LOCAL_NW,

    N_PKTS = 16
};


void print_info(){

    NOC::Info const& info = NOC::get_instance().info();

    cout << "NoC info: "
         << info.local_addr << ", "
         << info.router_x_addr << ", "
         << info.router_y_addr << ", "
         << info.net_x_size << ", "
         << info.net_y_size << ", "
         << info.data_width << "\n";
}

unsigned int tx_idx = 0;
NOC::Packet tx_pkts[N_PKTS];

unsigned int rx_idx = 0;
NOC::Packet rx_pkts[N_PKTS];

void send_pkt(unsigned int dst_addr, unsigned int data){
    cout << "TX: Sending pkt: DST_L=" << dst_addr << " DATA=" << data << "\n";
    NOC::Address addr;
    NOC::Packet pkt;
    addr.router_x = 0;
    addr.router_y = 0;
    addr.local = dst_addr;
    pkt.data[0] = data;
    NOC::get_instance().send(&addr, &pkt);
    cout << "TX: Pkt sent\n";
    tx_pkts[tx_idx++] = pkt;
    Thread::yield();
}

int receive_pkt(){

    NOC::Address addr;
    NOC::Packet data;

    for (unsigned int var = 0; var < N_PKTS; ++var){
        cout << "RX: Waiting pkt... \n";
        rx_sem.p();
        NOC::get_instance().receive(&addr, &data);
        cout << "RX: Pkt received:\n";
        cout << "RX:     Header: "
                << "DST_X=" << addr.router_x << ", "
                << "DST_Y="<< addr.router_y << ", "
                << "DST_L="<< addr.local << "\n";
        cout << "RX:     Data: " << data.data[0] << "\n";
        rx_pkts[rx_idx++] = data;
    }

    return 0;
}

void int_handler(unsigned int interrupt){
    if(!NOC::get_instance().receive_available()) return;
    //receive_pkt();
    rx_sem.v();
}

int main() {

    cout << "RTSNoC test\n\n";

    cout << "Setup interrupt and rx thread\n";
    NOC::get_instance().receive_int(&int_handler);

    Thread *rx_thead = new Thread(&receive_pkt);

    print_info();
    cout << "\n";

    for (int i = 0; i < N_PKTS; ++i) {
        tx_pkts[i].data[0] = 0;
        tx_pkts[i].data[1] = 0;
        rx_pkts[i].data[0] = 0;
        rx_pkts[i].data[1] = 0;
    }

    for (unsigned int i = 0; i < N_PKTS/2; ++i) {
        send_pkt(ECHO_P0_LOCAL_ADDR, i);
        send_pkt(ECHO_P1_LOCAL_ADDR, ~i);
    }

    cout << "Waiting last packets\n";
    rx_thead->join();

    cout << "Comparing packets\n";
    for (int i = 0; i < N_PKTS; ++i) {
        bool diff = (tx_pkts[i].data[0] != rx_pkts[i].data[0]) || (tx_pkts[i].data[1] != rx_pkts[i].data[1]);
        if(diff){
            cout << "Error at pkt " << i << "\n";
        }
    }

    cout << "\nThe end!\n";

    *((volatile unsigned int*)0xFFFFFFFC) = 0;

    return 0;
}

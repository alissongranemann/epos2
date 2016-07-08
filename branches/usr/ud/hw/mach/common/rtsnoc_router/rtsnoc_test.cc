
#include <utility/ostream.h>
#include <ic.h>

__USING_SYS


OStream cout;

enum{
    NOC_BASE = 0x80001000,
};

enum{
    NOC_HEADER_DST_LOCAL_ADDR = 0,
    NOC_HEADER_DST_ROUTER_Y_ADDR,
    NOC_HEADER_DST_ROUTER_X_ADDR,
    NOC_HEADER_SRC_LOCAL_ADDR,
    NOC_HEADER_SRC_ROUTER_Y_ADDR,
    NOC_HEADER_SRC_ROUTER_X_ADDR,
    NOC_DATA,
    NOC_STATUS,
    NOC_LOCAL_ADDR,
    NOC_ROUTER_X_ADDR,
    NOC_ROUTER_Y_ADDR,
    NOC_NET_X_SIZE,
    NOC_NET_Y_SIZE,
    NOC_NET_DATA_WIDTH
};

enum {
    NOC_MY_LOCAL_ADDR  = 0x0,
    ECHO_P0_LOCAL_ADDR = 0x6,
    ECHO_P0_ROUTER_X = 0,
    ECHO_P0_ROUTER_Y = 0,
    ECHO_P1_LOCAL_ADDR = 0x4,
    ECHO_P1_ROUTER_X = 0,
    ECHO_P1_ROUTER_Y = 0,
};


inline unsigned int reg(unsigned int offset){
    volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(NOC_BASE);
    return aux[offset];
}

inline void reg(unsigned int offset, unsigned int val){
    volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(NOC_BASE);
    aux[offset] = val;
}

inline unsigned int noc_header_src_router_x_addr(){return reg(NOC_HEADER_SRC_ROUTER_X_ADDR);}
inline unsigned int noc_header_src_router_y_addr(){return reg(NOC_HEADER_SRC_ROUTER_Y_ADDR);}
inline unsigned int noc_header_src_local_addr(){return reg(NOC_HEADER_SRC_LOCAL_ADDR);}
inline unsigned int noc_header_dst_router_x_addr(){return reg(NOC_HEADER_DST_ROUTER_X_ADDR);}
inline unsigned int noc_header_dst_router_y_addr(){return reg(NOC_HEADER_DST_ROUTER_Y_ADDR);}
inline unsigned int noc_header_dst_local_addr(){return reg(NOC_HEADER_DST_LOCAL_ADDR);}

inline void noc_header_src_router_x_addr(unsigned int val){reg(NOC_HEADER_SRC_ROUTER_X_ADDR, val);}
inline void noc_header_src_router_y_addr(unsigned int val){reg(NOC_HEADER_SRC_ROUTER_Y_ADDR, val);}
inline void noc_header_src_local_addr(unsigned int val){reg(NOC_HEADER_SRC_LOCAL_ADDR, val);}
inline void noc_header_dst_router_x_addr(unsigned int val){reg(NOC_HEADER_DST_ROUTER_X_ADDR, val);}
inline void noc_header_dst_router_y_addr(unsigned int val){reg(NOC_HEADER_DST_ROUTER_Y_ADDR, val);}
inline void noc_header_dst_local_addr(unsigned int val){reg(NOC_HEADER_DST_LOCAL_ADDR, val);}

inline unsigned int noc_data(){ return reg(NOC_DATA);}
inline void noc_data(unsigned int val){ reg(NOC_DATA, val);}

inline void noc_wr(){ reg(NOC_STATUS, 0x1); }
inline void noc_rd(){ reg(NOC_STATUS, 0x2); }
inline bool noc_wait(){return reg(NOC_STATUS) & 0x4; }
inline bool noc_nd(){return reg(NOC_STATUS) & 0x8; }

inline unsigned int noc_local_addr(){return reg(NOC_LOCAL_ADDR);}
inline unsigned int noc_router_x_addr(){return reg(NOC_ROUTER_X_ADDR);}
inline unsigned int noc_router_y_addr(){return reg(NOC_ROUTER_Y_ADDR);}
inline unsigned int noc_net_x_size(){return reg(NOC_NET_X_SIZE);}
inline unsigned int noc_net_y_size(){return reg(NOC_NET_Y_SIZE);}
inline unsigned int noc_net_data_width(){return reg(NOC_NET_DATA_WIDTH);}

void print_info(){
    cout << "NoC info: "
         << noc_local_addr() << ", "
         << noc_router_x_addr() << ", "
         << noc_router_y_addr() << ", "
         << noc_net_x_size() << ", "
         << noc_net_y_size() << ", "
         << noc_net_data_width() << "\n";
}

void wait_noc(){
    cout << "TX: Waiting... \n";
    while(noc_wait());
    cout << "TX: done \n";
}

void send_pkt(unsigned int src_addr, unsigned int dst_addr, unsigned int data){
    cout << "TX: Sending pkt: SRC_L=" << src_addr << " DST_L=" << dst_addr << " DATA=" << data << "\n";
    wait_noc();
    noc_header_src_local_addr(src_addr);
    noc_header_dst_local_addr(dst_addr);
    noc_data(data);
    noc_wr();
    cout << "TX: Pkt sent\n";
}

void receive_pkt(){
    cout << "RX: Receiving pkt\n";
    cout << "RX: Waiting... \n";
    while(!noc_nd());
    cout << "RX: done\n";
    cout << "RX: Pkt received:\n";
    cout << "RX:     Header: "
         << "SRC_X=" << noc_header_src_router_x_addr() << ", "
         << "SRC_Y=" << noc_header_src_router_y_addr() << ", "
         << "SRC_L=" << noc_header_src_local_addr() << ", "
         << "DST_X=" << noc_header_dst_router_x_addr() << ", "
         << "DST_Y="<< noc_header_dst_router_y_addr() << ", "
         << "DST_L="<< noc_header_dst_local_addr() << "\n";
    cout << "RX:     Data: " << noc_data() << "\n";
    noc_rd();
}

void int_handler(unsigned int interrupt){
    receive_pkt();
}

int main() {

    cout << "RTSNoC test\n\n";

    cout << "Setup interrupt\n";
    CPU::int_disable();
    IC::disable(IC::IRQ_NOC);
    IC::int_vector(IC::IRQ_NOC, &int_handler);
    IC::enable(IC::IRQ_NOC);
    CPU::int_enable();

    print_info();
    cout << "\n";

    for (unsigned int i = 0; i < 8; ++i) {
        send_pkt(NOC_MY_LOCAL_ADDR, ECHO_P0_LOCAL_ADDR, i);
        send_pkt(NOC_MY_LOCAL_ADDR, ECHO_P1_LOCAL_ADDR, i);
    }

    cout << "\nThe end!\n";

    return 0;
}

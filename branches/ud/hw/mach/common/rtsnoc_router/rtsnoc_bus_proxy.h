/*
 * rtsnoc_bus_proxy.h
 *
 *  Created on: Jan 12, 2012
 *      Author: tiago
 */

#ifndef RTSNOC_BUS_PROXY_H_
#define RTSNOC_BUS_PROXY_H_

#include <systemc.h>
#include <arch/common/bus_slave_if.h>
#include <common/global.h>
#include <components/traits.h>

namespace Timed {

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE//How many registers are required to keep a NOC data element
>
class rtsnoc_bus_proxy : public sc_module,
    public bus_slave_if {

public:

    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,

        MASK_SIZE_DATA = 0xFFFFFFFF >> ((32*PROC_NOC_BUS_RATE)-SIZE_DATA),
        MASK_SIZE_X = ~((-1) << SIZE_X),
        MASK_SIZE_Y = ~((-1) << SIZE_Y),
        MASK_SIZE_LOCAL = 0x7,

        MASK_WR = 0x1,
        MASK_RD = 0x2,
    };

    sc_in<bool>     rst;
    sc_out<bool>    irq;

    sc_fifo_out<sc_biguint<BUS_SIZE> > din;
    sc_fifo_in<sc_biguint<BUS_SIZE> > dout;


    SC_HAS_PROCESS(rtsnoc_bus_proxy);
    rtsnoc_bus_proxy(sc_module_name nm_
            , unsigned int start_address_
            , unsigned int end_address_
            )
        : sc_module(nm_)
        , rst("rst")
        , irq("irq")
        , start_address(start_address_)
        , end_address(end_address_)
    {

         noc_tx_data = 0;
         noc_tx_local_dst = 0;
         noc_tx_Y_dst = 0;
         noc_tx_X_dst = 0;
         noc_tx_local_orig = 0;
         noc_tx_Y_orig = 0;
         noc_tx_X_orig = 0;

         wr = false;
         rd = false;
         wait = false;
         nd = false;

         SC_THREAD(rx_process);
         SC_THREAD(tx_process);

    }

    ~rtsnoc_bus_proxy() {

    }

    //interfaces
    void read(unsigned int *data, unsigned int address, int size);
    void write(unsigned int data, unsigned int address, int size);

    unsigned int get_start_address() const;
    unsigned int get_end_address() const;

    slave_type get_type() const {return DEVICE;}

    inline int max(int a, int b){ return (a > b) ? a : b; }
    inline int min(int a, int b){ return (a < b) ? a : b; }

private:

    unsigned int    start_address;
    unsigned int    end_address;

public:
    sc_biguint<BUS_SIZE> din_s;
    sc_biguint<BUS_SIZE> dout_s;

    bool wr;
    sc_event wr_event;
    bool rd;
    sc_event rd_event;
    bool wait;
    bool nd;

private:
    sc_biguint<SIZE_DATA>   noc_tx_data;
    sc_biguint<3>           noc_tx_local_dst;
    sc_biguint<SIZE_Y>      noc_tx_Y_dst;
    sc_biguint<SIZE_X>      noc_tx_X_dst;
    sc_biguint<3>           noc_tx_local_orig;
    sc_biguint<SIZE_Y>      noc_tx_Y_orig;
    sc_biguint<SIZE_X>      noc_tx_X_orig;

    sc_signal<bool> noc_nd;


    void rx_process(){
        irq = false;

        while(rst.read() == true) sc_module::wait(rst.negedge_event());

        while(true){
            while(dout.num_available() == 0) sc_module::wait(dout.data_written_event());
            dout.read(dout_s);

#ifdef debug_noc_proxy
            std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): Received:  " << dout_s.to_string(SC_HEX) << "\n";
#endif

            nd = true;
            irq = true;
            Global::delay_cycles<0>(this, 2);
            irq = false;

            while(!rd) sc_module::wait(rd_event);
            nd = false;
        }
    }

    void tx_process(){

        while(rst.read() == true) sc_module::wait(rst.negedge_event());

        while(true){

            while(!wr) sc_module::wait(wr_event);

            din_s = (noc_tx_X_orig,
                       noc_tx_Y_orig,
                       noc_tx_local_orig,
                       noc_tx_X_dst,
                       noc_tx_Y_dst,
                       noc_tx_local_dst,
                       noc_tx_data);

            wait = true;
            din.write(din_s);
            Global::delay_cycles<0>(this, 2);
            wait = false;

#ifdef debug_noc_proxy
           std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): Transmitted " << din_s.to_string(SC_HEX) << " to " << noc_tx_local_dst.to_string(SC_HEX) << "\n";
#endif
        }
    }

};


template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
void rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>::read(unsigned int *data, unsigned int address, int size) {

    sc_biguint<SIZE_DATA>   noc_rx_data;
    sc_biguint<3>           noc_rx_local_dst;
    sc_biguint<SIZE_Y>      noc_rx_Y_dst;
    sc_biguint<SIZE_X>      noc_rx_X_dst;
    sc_biguint<3>           noc_rx_local_orig;
    sc_biguint<SIZE_Y>      noc_rx_Y_orig;
    sc_biguint<SIZE_X>      noc_rx_X_orig;

    (noc_rx_X_orig,
     noc_rx_Y_orig,
     noc_rx_local_orig,
     noc_rx_X_dst,
     noc_rx_Y_dst,
     noc_rx_local_dst,
     noc_rx_data) = dout_s;

    sc_uint<4> status = 0;
    status[0] = wr;
    status[1] = rd;
    status[2] = wait;
    status[3] = nd;

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
        case 0: *data = LOCAL_ADDR; break;
        case 1: *data = X; break;
        case 2: *data = Y; break;
        case 3: *data = SIZE_X; break;
        case 4: *data = SIZE_Y; break;
        case 5: *data = SIZE_DATA; break;
        case 6: *data = status.to_uint(); break;
        case 7: *data = noc_rx_local_dst.to_uint(); break;
        case 8: *data = noc_rx_Y_dst.to_uint(); break;
        case 9: *data = noc_rx_X_dst.to_uint(); break;
        case 10: *data = noc_rx_local_orig.to_uint(); break;
        case 11: *data = noc_rx_Y_orig.to_uint(); break;
        case 12: *data = noc_rx_X_orig.to_uint(); break;
        case 13: *data = noc_rx_data.range(min(31,SIZE_DATA-1),0).to_uint(); break;
        case 14: *data = (PROC_NOC_BUS_RATE>1) ? noc_rx_data.range(min(63,SIZE_DATA-1),32).to_uint() : 0; break;
        case 15: *data = (PROC_NOC_BUS_RATE>2) ? noc_rx_data.range(min(95,SIZE_DATA-1),64).to_uint() : 0; break;
        case 16: *data = (PROC_NOC_BUS_RATE>3) ? noc_rx_data.range(min(127,SIZE_DATA-1),96).to_uint() : 0; break;
        default:
            break;
    }

#ifdef debug_noc_proxy
    std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): Reading " << (void*)*data << " from reg " << addr
              << " noc_rx_data=" << noc_rx_data << ",status(nd,wait,rd,wr)=" << status.to_string(SC_BIN) << "\n";
#endif

    Global::delay_cycles<0>(this, 1);

}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
void rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>::write(unsigned int data, unsigned int address, int size) {

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: break;
        case 4: break;
        case 5: break;
        case 6:
            wr = data & MASK_WR;
            rd = data & MASK_RD;
            wr_event.notify();
            rd_event.notify();
            break;
        case 7: noc_tx_local_dst =      data & MASK_SIZE_LOCAL; break;
        case 8: noc_tx_Y_dst =          data & MASK_SIZE_Y; break;
        case 9: noc_tx_X_dst =          data & MASK_SIZE_X; break;
        case 10: noc_tx_local_orig =    data & MASK_SIZE_LOCAL; break;
        case 11: noc_tx_Y_orig =        data & MASK_SIZE_Y; break;
        case 12: noc_tx_X_orig =        data & MASK_SIZE_X; break;
        case 13: noc_tx_data.range(min(31,SIZE_DATA-1),0) = (PROC_NOC_BUS_RATE<2) ? (data & MASK_SIZE_DATA) : data; break;
        case 14: noc_tx_data.range(min(63,SIZE_DATA-1),32) =    (PROC_NOC_BUS_RATE<3) ? (data & MASK_SIZE_DATA) : data; break;
        case 15: noc_tx_data.range(min(95,SIZE_DATA-1),64) =    (PROC_NOC_BUS_RATE<4) ? (data & MASK_SIZE_DATA) : data; break;
        case 16: noc_tx_data.range(min(127,SIZE_DATA-1),96) =   (PROC_NOC_BUS_RATE<5) ? (data & MASK_SIZE_DATA) : data; break;
        default:
            break;
    }

#ifdef debug_noc_proxy
    std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): Writing " << (void*)data << " to reg " << addr
              << " noc_tx_data=" << noc_tx_data.to_string(SC_HEX) << "\n";
#endif


    Global::delay_cycles<0>(this, 1);
    wr = false;
    rd = false;
    wr_event.notify();
    rd_event.notify();
    Global::delay_cycles<0>(this, 1);

}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
unsigned int rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>::get_start_address() const{
    return start_address;
}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
unsigned int rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>::get_end_address() const{
    return end_address;
}

};

namespace Untimed {

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE//How many registers are required to keep a NOC data element
>
class rtsnoc_bus_proxy : public sc_module,
    public bus_slave_if {

public:

    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,

        MASK_SIZE_DATA = 0xFFFFFFFF >> ((32*PROC_NOC_BUS_RATE)-SIZE_DATA),
        MASK_SIZE_X = ~((-1) << SIZE_X),
        MASK_SIZE_Y = ~((-1) << SIZE_Y),
        MASK_SIZE_LOCAL = 0x7,

        MASK_WR = 0x1,
        MASK_RD = 0x2,
    };

    sc_in<bool>     rst;
    sc_out<bool>    irq;

    sc_fifo_out<sc_biguint<BUS_SIZE> > din;
    sc_fifo_in<sc_biguint<BUS_SIZE> > dout;


    SC_HAS_PROCESS(rtsnoc_bus_proxy);
    rtsnoc_bus_proxy(sc_module_name nm_
            , unsigned int start_address_
            , unsigned int end_address_
            )
        : sc_module(nm_)
        , rst("rst")
        , irq("irq")
        , start_address(start_address_)
        , end_address(end_address_)
    {

         noc_tx_data = 0;
         noc_tx_local_dst = 0;
         noc_tx_Y_dst = 0;
         noc_tx_X_dst = 0;
         noc_tx_local_orig = 0;
         noc_tx_Y_orig = 0;
         noc_tx_X_orig = 0;

         wr = false;
         rd = false;
         wait = false;
         nd = false;

         SC_THREAD(rx_process);
         SC_THREAD(tx_process);

    }

    ~rtsnoc_bus_proxy() {

    }

    //interfaces
    void read(unsigned int *data, unsigned int address, int size);
    void write(unsigned int data, unsigned int address, int size);

    unsigned int get_start_address() const;
    unsigned int get_end_address() const;

    slave_type get_type() const {return DEVICE;}

    inline int max(int a, int b){ return (a > b) ? a : b; }
    inline int min(int a, int b){ return (a < b) ? a : b; }

private:

    unsigned int    start_address;
    unsigned int    end_address;

public:
    sc_biguint<BUS_SIZE> din_s;
    sc_biguint<BUS_SIZE> dout_s;

    bool wr;
    sc_event wr_event;
    bool rd;
    sc_event rd_event;
    bool wait;
    bool nd;

private:
    sc_biguint<SIZE_DATA>   noc_tx_data;
    sc_biguint<3>           noc_tx_local_dst;
    sc_biguint<SIZE_Y>      noc_tx_Y_dst;
    sc_biguint<SIZE_X>      noc_tx_X_dst;
    sc_biguint<3>           noc_tx_local_orig;
    sc_biguint<SIZE_Y>      noc_tx_Y_orig;
    sc_biguint<SIZE_X>      noc_tx_X_orig;

    sc_signal<bool> noc_nd;


    void rx_process(){
        irq = false;

        while(rst.read() == true) sc_module::wait(rst.negedge_event());

        while(true){
            while(dout.num_available() == 0) sc_module::wait(dout.data_written_event());
            dout.read(dout_s);

#ifdef debug_noc_proxy
            std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): Received:  " << dout_s.to_string(SC_HEX) << "\n";
#endif

            nd = true;
            irq = !irq.read();

            sc_module::wait(rd_event);
            nd = false;
        }
    }

    void tx_process(){

        while(rst.read() == true) sc_module::wait(rst.negedge_event());

        while(true){

            sc_module::wait(wr_event);

            din_s = (noc_tx_X_orig,
                       noc_tx_Y_orig,
                       noc_tx_local_orig,
                       noc_tx_X_dst,
                       noc_tx_Y_dst,
                       noc_tx_local_dst,
                       noc_tx_data);


            din.write(din_s);
#ifdef debug_noc_proxy
           std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): Transmitted " << din_s.to_string(SC_HEX) << " to " << noc_tx_local_dst.to_string(SC_HEX) << "\n";
#endif
        }
    }

};


template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
void rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>::read(unsigned int *data, unsigned int address, int size) {

    sc_biguint<SIZE_DATA>   noc_rx_data;
    sc_biguint<3>           noc_rx_local_dst;
    sc_biguint<SIZE_Y>      noc_rx_Y_dst;
    sc_biguint<SIZE_X>      noc_rx_X_dst;
    sc_biguint<3>           noc_rx_local_orig;
    sc_biguint<SIZE_Y>      noc_rx_Y_orig;
    sc_biguint<SIZE_X>      noc_rx_X_orig;

    (noc_rx_X_orig,
     noc_rx_Y_orig,
     noc_rx_local_orig,
     noc_rx_X_dst,
     noc_rx_Y_dst,
     noc_rx_local_dst,
     noc_rx_data) = dout_s;

    sc_uint<4> status = 0;
    status[0] = wr;
    status[1] = rd;
    status[2] = wait;
    status[3] = nd;

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
        case 0: *data = LOCAL_ADDR; break;
        case 1: *data = X; break;
        case 2: *data = Y; break;
        case 3: *data = SIZE_X; break;
        case 4: *data = SIZE_Y; break;
        case 5: *data = SIZE_DATA; break;
        case 6: *data = status.to_uint(); break;
        case 7: *data = noc_rx_local_dst.to_uint(); break;
        case 8: *data = noc_rx_Y_dst.to_uint(); break;
        case 9: *data = noc_rx_X_dst.to_uint(); break;
        case 10: *data = noc_rx_local_orig.to_uint(); break;
        case 11: *data = noc_rx_Y_orig.to_uint(); break;
        case 12: *data = noc_rx_X_orig.to_uint(); break;
        case 13: *data = noc_rx_data.range(min(31,SIZE_DATA-1),0).to_uint(); break;
        case 14: *data = (PROC_NOC_BUS_RATE>1) ? noc_rx_data.range(min(63,SIZE_DATA-1),32).to_uint() : 0; break;
        case 15: *data = (PROC_NOC_BUS_RATE>2) ? noc_rx_data.range(min(95,SIZE_DATA-1),64).to_uint() : 0; break;
        case 16: *data = (PROC_NOC_BUS_RATE>3) ? noc_rx_data.range(min(127,SIZE_DATA-1),96).to_uint() : 0; break;
        default:
            break;
    }

#ifdef debug_noc_proxy
    std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): Reading " << (void*)*data << " from reg " << addr
              << " noc_rx_data=" << noc_rx_data << ",status(nd,wait,rd,wr)=" << status.to_string(SC_BIN) << "\n";
#endif

}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
void rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>::write(unsigned int data, unsigned int address, int size) {

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: break;
        case 4: break;
        case 5: break;
        case 6:
            if(data & MASK_WR) wr_event.notify();
            if(data & MASK_RD) rd_event.notify();
            break;
        case 7: noc_tx_local_dst =      data & MASK_SIZE_LOCAL; break;
        case 8: noc_tx_Y_dst =          data & MASK_SIZE_Y; break;
        case 9: noc_tx_X_dst =          data & MASK_SIZE_X; break;
        case 10: noc_tx_local_orig =    data & MASK_SIZE_LOCAL; break;
        case 11: noc_tx_Y_orig =        data & MASK_SIZE_Y; break;
        case 12: noc_tx_X_orig =        data & MASK_SIZE_X; break;
        case 13: noc_tx_data.range(min(31,SIZE_DATA-1),0) = (PROC_NOC_BUS_RATE<2) ? (data & MASK_SIZE_DATA) : data; break;
        case 14: noc_tx_data.range(min(63,SIZE_DATA-1),32) =    (PROC_NOC_BUS_RATE<3) ? (data & MASK_SIZE_DATA) : data; break;
        case 15: noc_tx_data.range(min(95,SIZE_DATA-1),64) =    (PROC_NOC_BUS_RATE<4) ? (data & MASK_SIZE_DATA) : data; break;
        case 16: noc_tx_data.range(min(127,SIZE_DATA-1),96) =   (PROC_NOC_BUS_RATE<5) ? (data & MASK_SIZE_DATA) : data; break;
        default:
            break;
    }

#ifdef debug_noc_proxy
    std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): Writing " << (void*)data << " to reg " << addr
              << " noc_tx_data=" << noc_tx_data.to_string(SC_HEX) << "\n";
#endif
}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
unsigned int rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>::get_start_address() const{
    return start_address;
}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
unsigned int rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>::get_end_address() const{
    return end_address;
}

};

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int PROC_NOC_BUS_RATE
>
class rtsnoc_bus_proxy: public Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation,
                                Timed::rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>,
                                Untimed::rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE> >::Result {
public:
    typedef  typename Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation,
                                Timed::rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE>,
                                Untimed::rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE> >::Result Base;

    rtsnoc_bus_proxy(sc_module_name nm
            , unsigned int start_address_
            , unsigned int end_address_) :Base(nm,start_address_,end_address_){}
};

#endif /* RTSNOC_BUS_PROXY_H_ */

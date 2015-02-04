/*
 * router.h
 *
 *  Created on: Jan 12, 2012
 *      Author: tiago
 */

#ifndef RTSNOC_ROUTER_H_
#define RTSNOC_ROUTER_H_

#include <systemc.h>
#include <queue>
#include <common/global.h>
#include <components/traits.h>

namespace Timed {
template<
    unsigned int X,
    unsigned int Y,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y
>
SC_MODULE(rtsnoc_router) {
    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,

        ROUTER_NN = 0,
        ROUTER_NE = 1,
        ROUTER_EE = 2,
        ROUTER_SE = 3,
        ROUTER_SS = 4,
        ROUTER_SW = 5,
        ROUTER_WW = 6,
        ROUTER_NW = 7,

        NPORTS = 8
    };

    sc_fifo_in<sc_biguint<BUS_SIZE> > din[NPORTS];
    sc_fifo_out<sc_biguint<BUS_SIZE> > dout[NPORTS];

    SC_CTOR(rtsnoc_router){
        SC_THREAD(rx_process_0);
        SC_THREAD(rx_process_1);
        SC_THREAD(rx_process_2);
        SC_THREAD(rx_process_3);
        SC_THREAD(rx_process_4);
        SC_THREAD(rx_process_5);
        SC_THREAD(rx_process_6);
        SC_THREAD(rx_process_7);
    }

private:

    void rx_process_0() { handle_rx<0>(); }
    void rx_process_1() { handle_rx<1>(); }
    void rx_process_2() { handle_rx<2>(); }
    void rx_process_3() { handle_rx<3>(); }
    void rx_process_4() { handle_rx<4>(); }
    void rx_process_5() { handle_rx<5>(); }
    void rx_process_6() { handle_rx<6>(); }
    void rx_process_7() { handle_rx<7>(); }

    template<unsigned int port>
    void handle_rx(){
        unsigned int this_one = port; //for db only
        sc_biguint<BUS_SIZE> data;
        unsigned int dest;
        while(true){
            while(din[port].num_available() == 0) wait(din[port].data_written_event());
            din[port].read(data);
            dest = get_local_dest_addr(data);
            Global::delay_cycles<0>(this, 1);
            dout[dest].write(data);
        }
    }

    unsigned int get_local_dest_addr(const sc_biguint<BUS_SIZE> &data) const{
        return data.range(SIZE_DATA+2,SIZE_DATA).to_uint();
    }

}; };

namespace Untimed {
template<
    unsigned int X,
    unsigned int Y,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y
>
SC_MODULE(rtsnoc_router) {
    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,

        ROUTER_NN = 0,
        ROUTER_NE = 1,
        ROUTER_EE = 2,
        ROUTER_SE = 3,
        ROUTER_SS = 4,
        ROUTER_SW = 5,
        ROUTER_WW = 6,
        ROUTER_NW = 7,

        NPORTS = 8
    };

    sc_fifo_in<sc_biguint<BUS_SIZE> > din[NPORTS];
    sc_fifo_out<sc_biguint<BUS_SIZE> > dout[NPORTS];

    SC_CTOR(rtsnoc_router){
        SC_THREAD(rx_process_0);
        SC_THREAD(rx_process_1);
        SC_THREAD(rx_process_2);
        SC_THREAD(rx_process_3);
        SC_THREAD(rx_process_4);
        SC_THREAD(rx_process_5);
        SC_THREAD(rx_process_6);
        SC_THREAD(rx_process_7);
    }

private:

    void rx_process_0() { handle_rx<0>(); }
    void rx_process_1() { handle_rx<1>(); }
    void rx_process_2() { handle_rx<2>(); }
    void rx_process_3() { handle_rx<3>(); }
    void rx_process_4() { handle_rx<4>(); }
    void rx_process_5() { handle_rx<5>(); }
    void rx_process_6() { handle_rx<6>(); }
    void rx_process_7() { handle_rx<7>(); }

    template<unsigned int port>
    void handle_rx(){
        unsigned int this_one = port; //for db only
        sc_biguint<BUS_SIZE> data;
        unsigned int dest;
        while(true){
            while(din[port].num_available() == 0) wait(din[port].data_written_event());
            din[port].read(data);
            dest = get_local_dest_addr(data);
            dout[dest].write(data);
        }
    }

    unsigned int get_local_dest_addr(const sc_biguint<BUS_SIZE> &data) const{
        return data.range(SIZE_DATA+2,SIZE_DATA).to_uint();
    }

}; };

template<
    unsigned int X,
    unsigned int Y,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y
>
class rtsnoc_router: public Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation,
                                Timed::rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y>,
                                Untimed::rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y> >::Result {
public:
    typedef  typename Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation,
                                Timed::rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y>,
                                Untimed::rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y> >::Result Base;

    rtsnoc_router(sc_module_name nm) :Base(nm){}
};

#ifndef MTI_SYSTEMC

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y
>
SC_MODULE(rtsnoc_tlm_to_rtl) {
    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };


    sc_fifo_in<sc_biguint<BUS_SIZE> > fifo_din;
    sc_fifo_out<sc_biguint<BUS_SIZE> > fifo_dout;

    sc_in<sc_biguint<BUS_SIZE> > din;
    sc_out<sc_biguint<BUS_SIZE> > dout;
    sc_in<bool> wr;
    sc_in<bool> rd;
    sc_out<bool> wait;
    sc_out<bool> nd;

    sc_in<bool> rst;

    sc_out<bool> clk;

    SC_CTOR(rtsnoc_tlm_to_rtl)
    {
        SC_THREAD(rtl_rx_process);

        SC_THREAD(fifo_rx_process);

        clk.bind(Global::clk_signal<0>());
    }

private:
    void rtl_rx_process(){
        sc_biguint<BUS_SIZE> data;
        wait = false;

        while(rst.read()) sc_module::wait(rst.negedge_event());

        while(true){
            while(!wr.read()) sc_module::wait(wr.posedge_event());

            data = din.read();
            wait = true;
            fifo_dout.write(data);
            sc_module::wait(clk.posedge_event());
            wait = false;
            sc_module::wait(clk.posedge_event());
        }
    }

    void fifo_rx_process(){
        sc_biguint<BUS_SIZE> data = 0;
        nd = false;
        dout.write(data);

        while(rst.read()) sc_module::wait(rst.negedge_event());

        while(true){
            while(fifo_din.num_available() == 0) sc_module::wait(fifo_din.data_written_event());
            fifo_din.read(data);

            dout.write(data);
            nd = true;
            while(!rd.read()) sc_module::wait(clk.posedge_event());
            nd = false;
        }
    }

};

#endif

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y
>
SC_MODULE(rtsnoc_rtl_to_tlm) {
    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };


    sc_fifo_in<sc_biguint<BUS_SIZE> > fifo_din;
    sc_fifo_out<sc_biguint<BUS_SIZE> > fifo_dout;

    sc_out<sc_biguint<BUS_SIZE> > din;
    sc_in<sc_biguint<BUS_SIZE> > dout;
    sc_out<bool> wr;
    sc_in<bool> wait;
    sc_out<bool> rd;
    sc_in<bool> nd;

    sc_in<bool> rst;

    sc_in<bool> clk;

    SC_CTOR(rtsnoc_rtl_to_tlm)
    {
        SC_THREAD(rtl_rx_process);

        SC_THREAD(fifo_rx_process);
    }

private:
    void rtl_rx_process(){
        sc_biguint<BUS_SIZE> data;
        rd = false;

        while(rst.read()) sc_module::wait(rst.negedge_event());

        sc_module::wait(clk.posedge_event());
        while(true){
            while(!nd.read()) sc_module::wait(nd.posedge_event());

            sc_module::wait(clk.posedge_event());
            data = dout.read();
            rd = true;
            fifo_dout.write(data);
            sc_module::wait(clk.posedge_event());
            rd = false;
            sc_module::wait(clk.posedge_event());
        }
    }

    void fifo_rx_process(){
        sc_biguint<BUS_SIZE> data = 0;
        wr = false;
        din.write(data);

        while(rst.read()) sc_module::wait(rst.negedge_event());

        sc_module::wait(clk.posedge_event());
        while(true){
            while(fifo_din.num_available() == 0) sc_module::wait(fifo_din.data_written_event());
            fifo_din.read(data);

            sc_module::wait(clk.posedge_event());
            while(wait.read()) sc_module::wait(clk.posedge_event());
            din.write(data);
            wr = true;
            sc_module::wait(clk.posedge_event());
            wr = false;
            sc_module::wait(clk.posedge_event());
        }
    }

};


#endif /* ROUTER_H_ */

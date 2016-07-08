/*
 * rtsnoc_to_achannel_rtl.h
 *
 *  Created on: May 3, 2013
 *      Author: tiago
 */

#ifndef RTSNOC_TO_ACHANNEL_RTL_H_
#define RTSNOC_TO_ACHANNEL_RTL_H_

#include <systemc.h>

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
SC_MODULE(rtsnoc_to_achannel_rtl){

    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };

    sc_in<bool>     clk;
    sc_in<bool>     rst;

    sc_out<sc_biguint<BUS_SIZE> > din;
    sc_in<sc_biguint<BUS_SIZE> > dout;
    sc_out<bool> wr;
    sc_out<bool> rd;
    sc_in<bool> wait;
    sc_in<bool> nd;

    sc_out< sc_lv<RMI_MSG_SIZE> > rx_ch_z;
    sc_out< sc_logic > rx_ch_vz;
    sc_in< sc_logic > rx_ch_lz;

    sc_in< sc_lv<RMI_MSG_SIZE> > tx_ch_z;
    sc_out< sc_logic > tx_ch_vz;
    sc_in< sc_logic > tx_ch_lz;

    sc_in<unsigned long int> tsc;

    void rx_process();
    void tx_process();

    sc_signal<unsigned int> tx_state;
    sc_signal<unsigned int> rx_state;

    sc_signal<sc_biguint<RMI_MSG_SIZE> > rx_data;
    sc_signal<sc_biguint<RMI_MSG_SIZE> > tx_data;

    enum{
        STATE_AC,
        STATE_NOC,
    };

    unsigned int pkt_cnt;

    SC_CTOR(rtsnoc_to_achannel_rtl){
        pkt_cnt = 0;

        SC_METHOD(rx_process);
        sensitive << clk.pos();

        SC_METHOD(tx_process);
        sensitive << clk.pos();
    }


};

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
void rtsnoc_to_achannel_rtl<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE>::tx_process(){

    if(rst.read()){
        tx_ch_vz = sc_logic_0;
        wr = false;
        din = 0;
        tx_state = STATE_AC;
        rx_data = 0;
    }
    else{
        switch (tx_state) {
            case STATE_AC:
                wr = false;
                if(tx_ch_lz.read() == sc_logic_0){
                    tx_state = STATE_AC;
                }
                else{
                    tx_ch_vz = sc_logic_1;
                    rx_data = tx_ch_z.read();
                    tx_state = STATE_NOC;
                }
                break;

            case STATE_NOC:
                tx_ch_vz = sc_logic_0;
                if(wait.read()){
                    tx_state = STATE_NOC;
                }
                else{
                    sc_biguint<8> phyaddr_X = rx_data.read()(63,56).to_uint();
                    sc_biguint<8> phyaddr_Y = rx_data.read()(71,64).to_uint();
                    sc_biguint<8> phyaddr_local = rx_data.read()(79,72).to_uint();


                    sc_biguint<SIZE_DATA>   tx_data = rx_data.read();
                    sc_biguint<3>           tx_local_orig = LOCAL_ADDR;
                    sc_biguint<SIZE_Y>      tx_Y_orig = Y;
                    sc_biguint<SIZE_X>      tx_X_orig = X;
                    sc_biguint<3>           tx_local_dst = phyaddr_local;
                    sc_biguint<SIZE_Y>      tx_Y_dst = phyaddr_X;
                    sc_biguint<SIZE_X>      tx_X_dst = phyaddr_Y;

                    din = (tx_X_orig,
                            tx_Y_orig,
                            tx_local_orig,
                            tx_X_dst,
                            tx_Y_dst,
                            tx_local_dst,
                            tx_data);
                    cout << "pkts(" << pkt_cnt << ").tx <= '1'; " << "pkts(" << pkt_cnt << ").data <= \"" << (tx_X_orig,tx_Y_orig,tx_local_orig,tx_X_dst,tx_Y_dst,tx_local_dst,tx_data).to_string(SC_BIN) << "\";\n";
                    ++pkt_cnt;

                    wr = true;
                    tx_state = STATE_AC;
                }
                break;

            default:
                break;
        }

    }
}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int RMI_MSG_SIZE
>
void rtsnoc_to_achannel_rtl<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,RMI_MSG_SIZE>::rx_process(){

    if(rst.read()){
        rd = false;
        rx_ch_z = 0;
        rx_ch_vz = sc_logic_0;
        rx_state = STATE_NOC;
    }
    else{
        switch (rx_state.read()) {
            case STATE_NOC:
                rx_ch_vz = sc_logic_0;
                if(!nd.read()){
                    rx_state = STATE_NOC;
                }
                else{
                    rd = true;

                    sc_biguint<SIZE_DATA>   rx_data;
                    sc_biguint<3>           rx_local_dst;
                    sc_biguint<SIZE_Y>      rx_Y_dst;
                    sc_biguint<SIZE_X>      rx_X_dst;
                    sc_biguint<3>           rx_local_orig;
                    sc_biguint<SIZE_Y>      rx_Y_orig;
                    sc_biguint<SIZE_X>      rx_X_orig;
                    (rx_X_orig,
                            rx_Y_orig,
                            rx_local_orig,
                            rx_X_dst,
                            rx_Y_dst,
                            rx_local_dst,
                            rx_data) = dout.read();
                    cout << "pkts(" << pkt_cnt << ").tx <= '0'; " << "pkts(" << pkt_cnt << ").data <= \"" << dout.read().to_string(SC_BIN) << "\";\n";
                    ++pkt_cnt;

                    sc_biguint<8> local_orig(rx_local_orig);
                    sc_biguint<8> Y_orig(rx_Y_orig);
                    sc_biguint<8> X_orig(rx_X_orig);
                    sc_biguint<SIZE_DATA> data(rx_data);
                    tx_data = (local_orig, Y_orig, X_orig, data);

                    rx_state = STATE_AC;
                }

                break;
            case STATE_AC:
                rd = false;
                if(rx_ch_lz.read() == false){
                    rx_state = STATE_AC;
                }
                else{
                    sc_biguint<RMI_MSG_SIZE> aux0 = tx_data.read();
                    sc_lv<RMI_MSG_SIZE> aux1;
                    for (unsigned int i = 0; i < RMI_MSG_SIZE; ++i) aux1[i] = aux0[i].to_bool();
                    rx_ch_z = aux1;
                    rx_ch_vz = sc_logic_1;
                    rx_state = STATE_NOC;
                }
                break;
            default:
                break;
        }
    }
}

#endif /* RTSNOC_TO_ACHANNEL_RTL_H_ */

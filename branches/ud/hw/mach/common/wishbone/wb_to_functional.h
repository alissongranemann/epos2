/*
 * rtsnoc_bus_proxy.h
 *
 *  Created on: Jan 12, 2012
 *      Author: tiago
 */

#ifndef WB_MASTER_TO_FUNCTIONAL_H_
#define WB_MASTER_TO_FUNCTIONAL_H_

#include <systemc.h>
#include <arch/common/bus_slave_if.h>
#include <common/global.h>

template<unsigned int ADDRESS_SIZE, unsigned int DATA_SIZE>
SC_MODULE(wb_slave_to_functional){

    sc_in<bool> wb_clk_i;
    sc_in<bool> wb_rst_i;
    sc_in<bool> wb_cyc_i;
    sc_in<bool> wb_stb_i;
    sc_in<sc_uint<ADDRESS_SIZE> > wb_adr_i;
    sc_in<sc_uint<4> > wb_sel_i;
    sc_in<bool> wb_we_i;
    sc_in<sc_uint<DATA_SIZE> > wb_dat_i;
    sc_out<sc_uint<DATA_SIZE> > wb_dat_o;
    sc_out<bool> wb_ack_o;

    sc_port<bus_slave_if> functional;

    SC_CTOR(wb_slave_to_functional){
        SC_THREAD(bus_proces);
    }

    void bus_proces(){
        wb_ack_o = false;
        wb_dat_o = 0;

        while(wb_rst_i.read()) wait(wb_rst_i.negedge_event());

        wait(wb_clk_i.posedge_event());
        while(true){

            if(wb_stb_i.read() && wb_cyc_i){

                unsigned int data = wb_dat_i.read().to_uint();
                unsigned int addr = wb_adr_i.read().to_uint();
                if(wb_we_i.read()){
                    functional->write(data, addr, 4);
                }
                else{
                    functional->read(&data, addr, 4);
                    wb_dat_o = data;
                }

                wb_ack_o = true;
                wait(wb_clk_i.posedge_event());
            }
            wb_ack_o = false;
            wait(wb_clk_i.posedge_event());
        }
    }


};

#endif

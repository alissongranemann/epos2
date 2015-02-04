/*
 * rtsnoc_bus_proxy.h
 *
 *  Created on: Jan 12, 2012
 *      Author: tiago
 */

#ifndef COMP_MANAGER_H_
#define COMP_MANAGER_H_

#include <systemc.h>
#include <arch/common/bus_slave_if.h>
#include <common/global.h>
#include <common/debugs.h>
#include <components/traits.h>
#include <mach/common/wishbone/wb_to_functional.h>
#include <mach/common/rtsnoc_router/rtsnoc_router.h>
#include <iostream>

namespace Timed {

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int BUFFER_SIZE
>
class comp_manager : public sc_module,
    public bus_slave_if {

public:

    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };

    sc_in<bool>     rst;
    sc_out<bool>    irq;

    sc_fifo_out<sc_biguint<BUS_SIZE> > din;
    sc_fifo_in<sc_biguint<BUS_SIZE> > dout;


    SC_HAS_PROCESS(comp_manager);
    comp_manager(sc_module_name nm_
            , unsigned int start_address_
            , unsigned int end_address_
            )
        : sc_module(nm_)
        , rst("rst")
        , irq("irq")
        , start_address(start_address_)
        , end_address(end_address_)
    {

         for (int i = 0; i < BUFFER_SIZE; ++i) {
             buff_alloc_free[i] = true;
             buff_alloc_to_proxy_or_agent[i] = false;
             buff_tx[i] = false;
             buff_rx[i] = false;
         }

         tx_sem = new sc_semaphore(0);

         SC_THREAD(rx_process);
         SC_THREAD(tx_process);
         SC_THREAD(agent_ntf_process);

    }

    //interfaces
    void read(unsigned int *data, unsigned int address, int size){
        bus_read_write(data,address,size,false);
    }
    void write(unsigned int data, unsigned int address, int size){
        bus_read_write(&data,address,size,true);
    }

    unsigned int get_start_address() const {
        return start_address;
    }
    unsigned int get_end_address() const{
        return end_address;
    }

    slave_type get_type() const {return DEVICE;}


private:

    unsigned int    start_address;
    unsigned int    end_address;

private:

    void bus_read_write(unsigned int *data, unsigned int address, int size, bool write);

    void rx_process();

    void tx_process();

    void agent_ntf_process();


    bool status_agent_int;
    unsigned int status_agent_buffer;

    bool buff_alloc_free[BUFFER_SIZE];
    bool buff_alloc_to_proxy_or_agent[BUFFER_SIZE];

    sc_biguint<SIZE_X>  buff_proxy_phy_x[BUFFER_SIZE];
    sc_biguint<SIZE_Y>  buff_proxy_phy_y[BUFFER_SIZE];
    sc_biguint<3>       buff_proxy_phy_local[BUFFER_SIZE];

    sc_biguint<8>       buff_rx_msg_type[BUFFER_SIZE];
    sc_biguint<8>       buff_tx_msg_type[BUFFER_SIZE];
    sc_biguint<8>       buff_instance_id[BUFFER_SIZE];
    sc_biguint<8>       buff_type_id[BUFFER_SIZE];
    sc_biguint<32> buff_data_tx[BUFFER_SIZE];
    sc_biguint<32> buff_data_rx[BUFFER_SIZE];

    bool   buff_tx[BUFFER_SIZE];
    sc_semaphore *tx_sem;
    bool   buff_rx[BUFFER_SIZE];

    unsigned int buff_agent_disp_addr[BUFFER_SIZE];
    unsigned int buff_agent_disp_obj_addr[BUFFER_SIZE];
    sc_biguint<SIZE_X>  buff_agent_phy_x[BUFFER_SIZE];
    sc_biguint<SIZE_Y>  buff_agent_phy_y[BUFFER_SIZE];
    sc_biguint<3>       buff_agent_phy_local[BUFFER_SIZE];


    sc_event update_int;


};

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int BUFFER_SIZE
>
void comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>::bus_read_write(unsigned int *data, unsigned int address, int size, bool write) {

    unsigned int addr = (address & 0x0000003FF) >> 2;

    unsigned int addr_space_sel = (addr >> 7) & 0x1;
    unsigned int addr_ctrl_reg_sel = addr & 0x7f;
    unsigned int addr_buffer_reg_sel = (addr >> 3) & 0xf;
    unsigned int addr_buffer_sel = addr & 0x7;

    enum{
        ADDR_SPACE_CTRL = 0,
        ADDR_SPACE_BUFF = 1
    };

    enum{
        ADDR_CMD                 = 0,
        ADDR_CMD_RESULT,
        ADDR_CMD_IDLE,
        ADDR_STATUS_AGENT_INT,
        ADDR_STATUS_AGENT_BUFFER,
        ADDR_INFO_NOC_X,
        ADDR_INFO_NOC_Y,
        ADDR_INFO_NOC_LOCAL,
        ADDR_INFO_BUFFER_SIZE
    };

    enum{
        ADDR_PROXY_BUFF_PHY_X        = 0,
        ADDR_PROXY_BUFF_PHY_Y,
        ADDR_PROXY_BUFF_PHY_LOCAL,
        ADDR_BUFF_MSG_TYPE,
        ADDR_BUFF_INSTANCE_ID,
        ADDR_BUFF_TYPE_ID,
        ADDR_BUFF_DATA,
        ADDR_BUFF_TX,
        ADDR_BUFF_RX,
        ADDR_AGENT_DISP_ADDR,
        ADDR_AGENT_DISP_OBJ_ADDR
    };

    enum{
        CMD_ALLOC_PROXY_BUFFER = 0,
        CMD_ALLOC_AGENT_BUFFER = 1,
        CMD_RESULT_ERR = 0xFFFFFFFF
    };

    static unsigned int cmd_result = 0;


    switch (addr_space_sel) {
        case ADDR_SPACE_CTRL:
            switch (addr_ctrl_reg_sel) {
                case ADDR_CMD:
                    if(write){
                        bool found = false;
                        cmd_result = CMD_RESULT_ERR;
                        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
                            if(buff_alloc_free[i]){
                                cmd_result = i;
                                found = true;
                                break;
                            }
                        }
                        if(found){
                            if(*data == CMD_ALLOC_PROXY_BUFFER){
                                buff_alloc_free[cmd_result] = false;
                                buff_alloc_to_proxy_or_agent[cmd_result] = false;
                            }
                            else if(*data == CMD_ALLOC_AGENT_BUFFER){
                                buff_alloc_free[cmd_result] = false;
                                buff_alloc_to_proxy_or_agent[cmd_result] = true;
                            }
                            else{
                                cmd_result = CMD_RESULT_ERR;
                            }
                        }
                    }
                    else{
                        *data = 0;
                    }
                    break;
                case ADDR_CMD_RESULT:
                    if(!write) *data = cmd_result;
                    break;
                case ADDR_CMD_IDLE:
                    if(!write) *data = 1;
                    break;
                case ADDR_STATUS_AGENT_INT:
                    if(!write) *data = status_agent_int ? 1 : 0;
                    break;
                case ADDR_STATUS_AGENT_BUFFER:
                    if(!write) *data = status_agent_buffer;
                    break;
                case ADDR_INFO_NOC_X:
                    if(!write) *data = X;
                    break;
                case ADDR_INFO_NOC_Y:
                    if(!write) *data = Y;
                    break;
                case ADDR_INFO_NOC_LOCAL:
                    if(!write) *data = LOCAL_ADDR;
                    break;
                case ADDR_INFO_BUFFER_SIZE:
                    if(!write) *data = BUFFER_SIZE;
                    break;
                default:
                    break;
            }
            break;
        case ADDR_SPACE_BUFF:
            switch (addr_buffer_reg_sel) {
                case ADDR_PROXY_BUFF_PHY_X:
                    if(write) buff_proxy_phy_x[addr_buffer_sel] = *data;
                    else      *data = buff_proxy_phy_x[addr_buffer_sel].to_uint();
                    break;
                case ADDR_PROXY_BUFF_PHY_Y:
                    if(write) buff_proxy_phy_y[addr_buffer_sel] = *data;
                    else      *data = buff_proxy_phy_y[addr_buffer_sel].to_uint();
                    break;
                case ADDR_PROXY_BUFF_PHY_LOCAL:
                    if(write) buff_proxy_phy_local[addr_buffer_sel] = *data;
                    else      *data = buff_proxy_phy_local[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_MSG_TYPE:
                    if(write) buff_tx_msg_type[addr_buffer_sel] = *data;
                    else      *data = buff_rx_msg_type[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_INSTANCE_ID:
                    if(write) buff_instance_id[addr_buffer_sel] = *data;
                    else      *data = buff_instance_id[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_TYPE_ID:
                    if(write) buff_type_id[addr_buffer_sel] = *data;
                    else      *data = buff_type_id[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_DATA:
                    if(write) buff_data_tx[addr_buffer_sel] = *data;
                    else      *data = buff_data_rx[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_TX:
                    if(write && !buff_tx[addr_buffer_sel]) {
                        buff_tx[addr_buffer_sel] = (*data == 1);
                        if(buff_tx[addr_buffer_sel]) tx_sem->post();
                    }
                    else      *data = buff_tx[addr_buffer_sel] ? 1 : 0;
                    break;
                case ADDR_BUFF_RX:
                    if(write && buff_rx[addr_buffer_sel]) buff_rx[addr_buffer_sel] = !(*data == 0);
                    else      *data = buff_rx[addr_buffer_sel] ? 1 : 0;
                    update_int.notify();
                    break;
                case ADDR_AGENT_DISP_ADDR:
                    if(write) buff_agent_disp_addr[addr_buffer_sel] = *data;
                    else      *data = buff_agent_disp_addr[addr_buffer_sel];
                    break;
                case ADDR_AGENT_DISP_OBJ_ADDR:
                    if(write) buff_agent_disp_obj_addr[addr_buffer_sel] = *data;
                    else      *data = buff_agent_disp_obj_addr[addr_buffer_sel];
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }


#ifdef debug_catapult_nodes
    std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): ";
    if(!write)std::cout << "Reading from";
    else std::cout << "Writing to";
    std::cout << " addr_space_sel="<<addr_space_sel << " addr_ctrl_reg_sel="<<addr_ctrl_reg_sel << " addr_buffer_reg_sel="<<addr_buffer_reg_sel << " addr_buffer_sel="<<addr_buffer_sel<<"\n";
    std::cout << "\tdata = " << *data << "\n";
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
    unsigned int BUFFER_SIZE
>
void comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>::tx_process(){

    sc_biguint<SIZE_DATA>   noc_tx_data;
    sc_biguint<3>           noc_tx_local_dst;
    sc_biguint<SIZE_Y>      noc_tx_Y_dst;
    sc_biguint<SIZE_X>      noc_tx_X_dst;
    sc_biguint<3>           noc_tx_local_orig(LOCAL_ADDR);
    sc_biguint<SIZE_Y>      noc_tx_Y_orig(Y);
    sc_biguint<SIZE_X>      noc_tx_X_orig(X);

    while(rst.read() == true) sc_module::wait(rst.negedge_event());

    while(true){

        tx_sem->wait();

#ifdef debug_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): TX search \n";
#endif

        unsigned int idx = 0;
        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
            if(!buff_alloc_free[i] && buff_tx[i]){
                idx = i;
                break;
            }
        }

#ifdef debug_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): TX from buff " << idx << "\n";
#endif

        Global::delay_cycles<0>(this, 1);

        if(buff_alloc_to_proxy_or_agent[idx]){//agent
            noc_tx_local_dst = buff_agent_phy_local[idx];
            noc_tx_X_dst = buff_agent_phy_x[idx];
            noc_tx_Y_dst = buff_agent_phy_y[idx];
        }
        else{//proxy
            noc_tx_local_dst = buff_proxy_phy_local[idx];
            noc_tx_X_dst = buff_proxy_phy_x[idx];
            noc_tx_Y_dst = buff_proxy_phy_y[idx];

        }

        noc_tx_data = (buff_type_id[idx],
                       buff_instance_id[idx],
                       buff_tx_msg_type[idx],
                       buff_data_tx[idx]);

#ifdef profile_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): TX msg\n";
        std::cout << "\ttype = ";
        switch (buff_tx_msg_type[idx].to_uint()) {
            case Implementation::MSG_TYPE_CALL:
                std::cout << "MSG_TYPE_CALL";
                break;
            case Implementation::MSG_TYPE_CALL_DATA:
                std::cout << "MSG_TYPE_CALL_DATA";
                break;
            case Implementation::MSG_TYPE_RESP_DATA:
                std::cout << "MSG_TYPE_RESP_DATA";
                break;
            default:
                break;
        }
        std::cout << ", data = " << buff_data_tx[idx].to_string(SC_HEX_US) << "\n";
        std::cout << "\tTSC = " << Global::get_global_timer_ticks() << "\n";
#endif

        buff_tx[idx] = false;
        Global::delay_cycles<0>(this, 1);

        sc_biguint<BUS_SIZE>
        din_s = (noc_tx_X_orig,
                noc_tx_Y_orig,
                noc_tx_local_orig,
                noc_tx_X_dst,
                noc_tx_Y_dst,
                noc_tx_local_dst,
                noc_tx_data);


        din.write(din_s);

#ifdef debug_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): TX " << din_s.to_string(SC_HEX_US) << "\n";
#endif
    }
}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int BUFFER_SIZE
>
void comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>::rx_process(){

    sc_biguint<BUS_SIZE> dout_s;

    sc_biguint<SIZE_DATA>   noc_rx_data;
    sc_biguint<3>           noc_rx_local_dst;
    sc_biguint<SIZE_Y>      noc_rx_Y_dst;
    sc_biguint<SIZE_X>      noc_rx_X_dst;
    sc_biguint<3>           noc_rx_local_orig;
    sc_biguint<SIZE_Y>      noc_rx_Y_orig;
    sc_biguint<SIZE_X>      noc_rx_X_orig;

    sc_biguint<8>       rx_msg_type;
    sc_biguint<8>       rx_instance_id;
    sc_biguint<8>       rx_type_id;
    sc_biguint<32>      rx_data;

    while(rst.read() == true) sc_module::wait(rst.negedge_event());

    while(true){
        while(dout.num_available() == 0) sc_module::wait(dout.data_written_event());
        dout.read(dout_s);

#ifdef debug_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): RX " << dout_s.to_string(SC_HEX_US) << "\n";
#endif

        (noc_rx_X_orig,
         noc_rx_Y_orig,
         noc_rx_local_orig,
         noc_rx_X_dst,
         noc_rx_Y_dst,
         noc_rx_local_dst,
         noc_rx_data) = dout_s;

         (rx_type_id,
          rx_instance_id,
          rx_msg_type,
          rx_data) = noc_rx_data;

        Global::delay_cycles<0>(this, 1);

        unsigned int idx;
        bool found = false;
        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
            if((buff_instance_id[i] == rx_instance_id) &&
               (buff_type_id[i] == rx_type_id) &&
               !buff_alloc_free[i]){
                idx = i;
                found = true;
                break;
            }
        }

        if(found){

#ifdef debug_catapult_nodes
            std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): RX into buff " << idx << "\n";
#endif

            while(buff_rx[idx]) Global::delay_cycles<0>(this, 1);
            buff_rx[idx] = true;

            buff_data_rx[idx] = rx_data;
            buff_rx_msg_type[idx] = rx_msg_type;

            if(buff_alloc_to_proxy_or_agent[idx]){//agent
                if(rx_msg_type.to_uint() == Implementation::MSG_TYPE_CALL){
                    buff_agent_phy_x[idx] = noc_rx_X_orig;
                    buff_agent_phy_y[idx] = noc_rx_Y_orig;
                    buff_agent_phy_local[idx] = noc_rx_local_orig;
                }
            }

#ifdef profile_catapult_nodes
            std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): RX msg\n";
            std::cout << "\ttype = ";
            switch (buff_rx_msg_type[idx].to_uint()) {
            case Implementation::MSG_TYPE_CALL:
                std::cout << "MSG_TYPE_CALL";
                break;
            case Implementation::MSG_TYPE_CALL_DATA:
                std::cout << "MSG_TYPE_CALL_DATA";
                break;
            case Implementation::MSG_TYPE_RESP_DATA:
                std::cout << "MSG_TYPE_RESP_DATA";
                break;
            default:
                break;
            }
            std::cout << ", data = " << buff_data_rx[idx].to_string(SC_HEX_US) << "\n";
            std::cout << "\tTSC = " << Global::get_global_timer_ticks() << "\n";
#endif

            update_int.notify();

            Global::delay_cycles<0>(this, 1);
        }
        else{
            //TODO notify and buffer_not_found error and store the msg

#ifdef debug_catapult_nodes
            std::cout << "##ERROR NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): RX buffer not found\n";
#endif
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
    unsigned int BUFFER_SIZE
>
void comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>::agent_ntf_process(){

    status_agent_int = false;
    status_agent_buffer = 0;

    while(rst.read() == true) sc_module::wait(rst.negedge_event());


    while(true){
        wait(update_int);

        unsigned int idx;
        bool found = false;
        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
            if(buff_rx[i] && !buff_alloc_free[i] && buff_alloc_to_proxy_or_agent[i]){
                idx = i;
                found = true;
            }
        }

        status_agent_int = found;
        status_agent_buffer = idx;

        if(status_agent_int){
            irq = true;
            Global::delay_cycles<0>(this, 1);
            irq = false;
            Global::delay_cycles<0>(this, 1);
        }
    }

}

};

SC_MODULE(comp_manager_rtl_wrapped){

    static const unsigned int X = 0;
    static const unsigned int Y = 0;
    static const unsigned int LOCAL_ADDR = 1;
    static const unsigned int SIZE_DATA = 56;
    static const unsigned int SIZE_X = 1;
    static const unsigned int SIZE_Y = 1;
    static const unsigned int BUFFER_SIZE = 4;

    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };


    sc_in<bool> clk_i;
    sc_in<bool> rst_i;
    sc_in<bool> wb_cyc_i;
    sc_in<bool> wb_stb_i;
    sc_in<sc_uint<32> > wb_adr_i;
    sc_in<sc_uint<4> > wb_sel_i;
    sc_in<bool> wb_we_i;
    sc_in<sc_uint<32> > wb_dat_i;
    sc_out<sc_uint<32> > wb_dat_o;
    sc_out<bool> wb_ack_o;

    sc_out<sc_biguint<BUS_SIZE> > noc_din_o;
    sc_in<sc_biguint<BUS_SIZE> > noc_dout_i;
    sc_out<bool> noc_wr_o;
    sc_out<bool> noc_rd_o;
    sc_in<bool> noc_wait_i;
    sc_in<bool> noc_nd_i;

    sc_out<bool> noc_int_o;

    Timed::comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE> comp_manager;
    wb_slave_to_functional<32,32> wb_to_functional;
    rtsnoc_rtl_to_tlm<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y> noc_rtl_to_tlm;

    sc_fifo<sc_biguint<BUS_SIZE> > noc_din;
    sc_fifo<sc_biguint<BUS_SIZE> > noc_dout;

    SC_CTOR(comp_manager_rtl_wrapped)
        :comp_manager("manager",0,0),
         wb_to_functional("wb_to_functional"),
         noc_rtl_to_tlm("noc_rtl_to_tlm")
    {

        wb_to_functional.wb_clk_i(clk_i);
        wb_to_functional.wb_rst_i(rst_i);
        wb_to_functional.wb_adr_i(wb_adr_i);
        wb_to_functional.wb_stb_i(wb_stb_i);
        wb_to_functional.wb_cyc_i(wb_cyc_i);
        wb_to_functional.wb_sel_i(wb_sel_i);
        wb_to_functional.wb_we_i(wb_we_i);
        wb_to_functional.wb_dat_i(wb_dat_i);
        wb_to_functional.wb_dat_o(wb_dat_o);
        wb_to_functional.wb_ack_o(wb_ack_o);

        noc_rtl_to_tlm.clk(clk_i);
        noc_rtl_to_tlm.rst(rst_i);
        noc_rtl_to_tlm.din(noc_din_o);
        noc_rtl_to_tlm.dout(noc_dout_i);
        noc_rtl_to_tlm.wr(noc_wr_o);
        noc_rtl_to_tlm.wait(noc_wait_i);
        noc_rtl_to_tlm.rd(noc_rd_o);
        noc_rtl_to_tlm.nd(noc_nd_i);

        wb_to_functional.functional(comp_manager);

        comp_manager.rst(rst_i);
        comp_manager.din(noc_din);
        comp_manager.dout(noc_dout);
        comp_manager.irq(noc_int_o);

        noc_rtl_to_tlm.fifo_din(noc_din);
        noc_rtl_to_tlm.fifo_dout(noc_dout);

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
    unsigned int BUFFER_SIZE
>
class comp_manager : public sc_module,
    public bus_slave_if {

public:

    enum{
        BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,
    };

    sc_in<bool>     rst;
    sc_out<bool>    irq;

    sc_fifo_out<sc_biguint<BUS_SIZE> > din;
    sc_fifo_in<sc_biguint<BUS_SIZE> > dout;


    SC_HAS_PROCESS(comp_manager);
    comp_manager(sc_module_name nm_
            , unsigned int start_address_
            , unsigned int end_address_
            )
        : sc_module(nm_)
        , rst("rst")
        , irq("irq")
        , start_address(start_address_)
        , end_address(end_address_)
    {

         for (int i = 0; i < BUFFER_SIZE; ++i) {
             buff_alloc_free[i] = true;
             buff_alloc_to_proxy_or_agent[i] = false;
             buff_tx[i] = false;
             buff_rx[i] = false;
         }

         tx_sem = new sc_semaphore(0);

         SC_THREAD(rx_process);
         SC_THREAD(tx_process);
         SC_THREAD(agent_ntf_process);

    }

    //interfaces
    void read(unsigned int *data, unsigned int address, int size){
        bus_read_write(data,address,size,false);
    }
    void write(unsigned int data, unsigned int address, int size){
        bus_read_write(&data,address,size,true);
    }

    unsigned int get_start_address() const {
        return start_address;
    }
    unsigned int get_end_address() const{
        return end_address;
    }

    slave_type get_type() const {return DEVICE;}


private:

    unsigned int    start_address;
    unsigned int    end_address;

private:

    void bus_read_write(unsigned int *data, unsigned int address, int size, bool write);

    void rx_process();

    void tx_process();

    void agent_ntf_process();


    bool status_agent_int;
    unsigned int status_agent_buffer;

    bool buff_alloc_free[BUFFER_SIZE];
    bool buff_alloc_to_proxy_or_agent[BUFFER_SIZE];

    sc_biguint<SIZE_X>  buff_proxy_phy_x[BUFFER_SIZE];
    sc_biguint<SIZE_Y>  buff_proxy_phy_y[BUFFER_SIZE];
    sc_biguint<3>       buff_proxy_phy_local[BUFFER_SIZE];

    sc_biguint<8>       buff_rx_msg_type[BUFFER_SIZE];
    sc_biguint<8>       buff_tx_msg_type[BUFFER_SIZE];
    sc_biguint<8>       buff_instance_id[BUFFER_SIZE];
    sc_biguint<8>       buff_type_id[BUFFER_SIZE];
    sc_biguint<32> buff_data_tx[BUFFER_SIZE];
    sc_biguint<32> buff_data_rx[BUFFER_SIZE];

    bool   buff_tx[BUFFER_SIZE];
    sc_semaphore *tx_sem;
    bool   buff_rx[BUFFER_SIZE];

    unsigned int buff_agent_disp_addr[BUFFER_SIZE];
    unsigned int buff_agent_disp_obj_addr[BUFFER_SIZE];
    sc_biguint<SIZE_X>  buff_agent_phy_x[BUFFER_SIZE];
    sc_biguint<SIZE_Y>  buff_agent_phy_y[BUFFER_SIZE];
    sc_biguint<3>       buff_agent_phy_local[BUFFER_SIZE];


    sc_event update_int;


};

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int BUFFER_SIZE
>
void comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>::bus_read_write(unsigned int *data, unsigned int address, int size, bool write) {

    unsigned int addr = (address & 0x0000003FF) >> 2;

    unsigned int addr_space_sel = (addr >> 7) & 0x1;
    unsigned int addr_ctrl_reg_sel = addr & 0x7f;
    unsigned int addr_buffer_reg_sel = (addr >> 3) & 0xf;
    unsigned int addr_buffer_sel = addr & 0x7;

    enum{
        ADDR_SPACE_CTRL = 0,
        ADDR_SPACE_BUFF = 1
    };

    enum{
        ADDR_CMD                 = 0,
        ADDR_CMD_RESULT,
        ADDR_CMD_IDLE,
        ADDR_STATUS_AGENT_INT,
        ADDR_STATUS_AGENT_BUFFER,
        ADDR_INFO_NOC_X,
        ADDR_INFO_NOC_Y,
        ADDR_INFO_NOC_LOCAL,
        ADDR_INFO_BUFFER_SIZE
    };

    enum{
        ADDR_PROXY_BUFF_PHY_X        = 0,
        ADDR_PROXY_BUFF_PHY_Y,
        ADDR_PROXY_BUFF_PHY_LOCAL,
        ADDR_BUFF_MSG_TYPE,
        ADDR_BUFF_INSTANCE_ID,
        ADDR_BUFF_TYPE_ID,
        ADDR_BUFF_DATA,
        ADDR_BUFF_TX,
        ADDR_BUFF_RX,
        ADDR_AGENT_DISP_ADDR,
        ADDR_AGENT_DISP_OBJ_ADDR
    };

    enum{
        CMD_ALLOC_PROXY_BUFFER = 0,
        CMD_ALLOC_AGENT_BUFFER = 1,
        CMD_RESULT_ERR = 0xFFFFFFFF
    };

    static unsigned int cmd_result = 0;


    switch (addr_space_sel) {
        case ADDR_SPACE_CTRL:
            switch (addr_ctrl_reg_sel) {
                case ADDR_CMD:
                    if(write){
                        bool found = false;
                        cmd_result = CMD_RESULT_ERR;
                        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
                            if(buff_alloc_free[i]){
                                cmd_result = i;
                                found = true;
                                break;
                            }
                        }
                        if(found){
                            if(*data == CMD_ALLOC_PROXY_BUFFER){
                                buff_alloc_free[cmd_result] = false;
                                buff_alloc_to_proxy_or_agent[cmd_result] = false;
                            }
                            else if(*data == CMD_ALLOC_AGENT_BUFFER){
                                buff_alloc_free[cmd_result] = false;
                                buff_alloc_to_proxy_or_agent[cmd_result] = true;
                            }
                            else{
                                cmd_result = CMD_RESULT_ERR;
                            }
                        }
                    }
                    else{
                        *data = 0;
                    }
                    break;
                case ADDR_CMD_RESULT:
                    if(!write) *data = cmd_result;
                    break;
                case ADDR_CMD_IDLE:
                    if(!write) *data = 1;
                    break;
                case ADDR_STATUS_AGENT_INT:
                    if(!write) *data = status_agent_int ? 1 : 0;
                    break;
                case ADDR_STATUS_AGENT_BUFFER:
                    if(!write) *data = status_agent_buffer;
                    break;
                case ADDR_INFO_NOC_X:
                    if(!write) *data = X;
                    break;
                case ADDR_INFO_NOC_Y:
                    if(!write) *data = Y;
                    break;
                case ADDR_INFO_NOC_LOCAL:
                    if(!write) *data = LOCAL_ADDR;
                    break;
                case ADDR_INFO_BUFFER_SIZE:
                    if(!write) *data = BUFFER_SIZE;
                    break;
                default:
                    break;
            }
            break;
        case ADDR_SPACE_BUFF:
            switch (addr_buffer_reg_sel) {
                case ADDR_PROXY_BUFF_PHY_X:
                    if(write) buff_proxy_phy_x[addr_buffer_sel] = *data;
                    else      *data = buff_proxy_phy_x[addr_buffer_sel].to_uint();
                    break;
                case ADDR_PROXY_BUFF_PHY_Y:
                    if(write) buff_proxy_phy_y[addr_buffer_sel] = *data;
                    else      *data = buff_proxy_phy_y[addr_buffer_sel].to_uint();
                    break;
                case ADDR_PROXY_BUFF_PHY_LOCAL:
                    if(write) buff_proxy_phy_local[addr_buffer_sel] = *data;
                    else      *data = buff_proxy_phy_local[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_MSG_TYPE:
                    if(write) buff_tx_msg_type[addr_buffer_sel] = *data;
                    else      *data = buff_rx_msg_type[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_INSTANCE_ID:
                    if(write) buff_instance_id[addr_buffer_sel] = *data;
                    else      *data = buff_instance_id[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_TYPE_ID:
                    if(write) buff_type_id[addr_buffer_sel] = *data;
                    else      *data = buff_type_id[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_DATA:
                    if(write) buff_data_tx[addr_buffer_sel] = *data;
                    else      *data = buff_data_rx[addr_buffer_sel].to_uint();
                    break;
                case ADDR_BUFF_TX:
                    if(write && !buff_tx[addr_buffer_sel]) {
                        buff_tx[addr_buffer_sel] = (*data == 1);
                        if(buff_tx[addr_buffer_sel]) tx_sem->post();
                    }
                    else      *data = buff_tx[addr_buffer_sel] ? 1 : 0;
                    break;
                case ADDR_BUFF_RX:
                    if(write && buff_rx[addr_buffer_sel]) buff_rx[addr_buffer_sel] = !(*data == 0);
                    else      *data = buff_rx[addr_buffer_sel] ? 1 : 0;
                    update_int.notify();
                    break;
                case ADDR_AGENT_DISP_ADDR:
                    if(write) buff_agent_disp_addr[addr_buffer_sel] = *data;
                    else      *data = buff_agent_disp_addr[addr_buffer_sel];
                    break;
                case ADDR_AGENT_DISP_OBJ_ADDR:
                    if(write) buff_agent_disp_obj_addr[addr_buffer_sel] = *data;
                    else      *data = buff_agent_disp_obj_addr[addr_buffer_sel];
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }


#ifdef debug_catapult_nodes
    std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): ";
    if(!write)std::cout << "Reading from";
    else std::cout << "Writing to";
    std::cout << " addr_space_sel="<<addr_space_sel << " addr_ctrl_reg_sel="<<addr_ctrl_reg_sel << " addr_buffer_reg_sel="<<addr_buffer_reg_sel << " addr_buffer_sel="<<addr_buffer_sel<<"\n";
    std::cout << "\tdata = " << *data << "\n";
#endif
}


template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int BUFFER_SIZE
>
void comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>::tx_process(){

    sc_biguint<SIZE_DATA>   noc_tx_data;
    sc_biguint<3>           noc_tx_local_dst;
    sc_biguint<SIZE_Y>      noc_tx_Y_dst;
    sc_biguint<SIZE_X>      noc_tx_X_dst;
    sc_biguint<3>           noc_tx_local_orig(LOCAL_ADDR);
    sc_biguint<SIZE_Y>      noc_tx_Y_orig(Y);
    sc_biguint<SIZE_X>      noc_tx_X_orig(X);

    while(rst.read() == true) sc_module::wait(rst.negedge_event());

    while(true){

        tx_sem->wait();

#ifdef debug_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): TX search \n";
#endif

        unsigned int idx = 0;
        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
            if(!buff_alloc_free[i] && buff_tx[i]){
                idx = i;
                break;
            }
        }

#ifdef debug_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): TX from buff " << idx << "\n";
#endif

        if(buff_alloc_to_proxy_or_agent[idx]){//agent
            noc_tx_local_dst = buff_agent_phy_local[idx];
            noc_tx_X_dst = buff_agent_phy_x[idx];
            noc_tx_Y_dst = buff_agent_phy_y[idx];
        }
        else{//proxy
            noc_tx_local_dst = buff_proxy_phy_local[idx];
            noc_tx_X_dst = buff_proxy_phy_x[idx];
            noc_tx_Y_dst = buff_proxy_phy_y[idx];

        }

        noc_tx_data = (buff_type_id[idx],
                       buff_instance_id[idx],
                       buff_tx_msg_type[idx],
                       buff_data_tx[idx]);

        buff_tx[idx] = false;

        sc_biguint<BUS_SIZE>
        din_s = (noc_tx_X_orig,
                noc_tx_Y_orig,
                noc_tx_local_orig,
                noc_tx_X_dst,
                noc_tx_Y_dst,
                noc_tx_local_dst,
                noc_tx_data);


        din.write(din_s);

#ifdef debug_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): TX " << din_s.to_string(SC_HEX_US) << "\n";
#endif
    }
}

template<
    unsigned int X,
    unsigned int Y,
    unsigned int LOCAL_ADDR,
    unsigned int SIZE_DATA,
    unsigned int SIZE_X,
    unsigned int SIZE_Y,
    unsigned int BUFFER_SIZE
>
void comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>::rx_process(){

    sc_biguint<BUS_SIZE> dout_s;

    sc_biguint<SIZE_DATA>   noc_rx_data;
    sc_biguint<3>           noc_rx_local_dst;
    sc_biguint<SIZE_Y>      noc_rx_Y_dst;
    sc_biguint<SIZE_X>      noc_rx_X_dst;
    sc_biguint<3>           noc_rx_local_orig;
    sc_biguint<SIZE_Y>      noc_rx_Y_orig;
    sc_biguint<SIZE_X>      noc_rx_X_orig;

    sc_biguint<8>       rx_msg_type;
    sc_biguint<8>       rx_instance_id;
    sc_biguint<8>       rx_type_id;
    sc_biguint<32>      rx_data;

    while(rst.read() == true) sc_module::wait(rst.negedge_event());

    while(true){
        while(dout.num_available() == 0) sc_module::wait(dout.data_written_event());
        dout.read(dout_s);

#ifdef debug_catapult_nodes
        std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): RX " << dout_s.to_string(SC_HEX_US) << "\n";
#endif

        (noc_rx_X_orig,
         noc_rx_Y_orig,
         noc_rx_local_orig,
         noc_rx_X_dst,
         noc_rx_Y_dst,
         noc_rx_local_dst,
         noc_rx_data) = dout_s;

         (rx_type_id,
          rx_instance_id,
          rx_msg_type,
          rx_data) = noc_rx_data;

        unsigned int idx;
        bool found = false;
        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
            if((buff_instance_id[i] == rx_instance_id) &&
               (buff_type_id[i] == rx_type_id) &&
               !buff_alloc_free[i]){
                idx = i;
                found = true;
                break;
            }
        }

        if(found){

#ifdef debug_catapult_nodes
            std::cout << "##INFO NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): RX into buff " << idx << "\n";
#endif

            while(buff_rx[idx]) Global::delay_cycles<0>(this, 1);
            buff_rx[idx] = true;

            buff_data_rx[idx] = rx_data;
            buff_rx_msg_type[idx] = rx_msg_type;

            if(buff_alloc_to_proxy_or_agent[idx]){//agent
                if(rx_msg_type.to_uint() == Implementation::MSG_TYPE_CALL){
                    buff_agent_phy_x[idx] = noc_rx_X_orig;
                    buff_agent_phy_y[idx] = noc_rx_Y_orig;
                    buff_agent_phy_local[idx] = noc_rx_local_orig;
                }
            }

            update_int.notify();
        }
        else{
            //TODO notify and buffer_not_found error and store the msg

#ifdef debug_catapult_nodes
            std::cout << "##ERROR NODE (" << X << "," << Y << "," << LOCAL_ADDR << "): RX buffer not found\n";
#endif
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
    unsigned int BUFFER_SIZE
>
void comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>::agent_ntf_process(){

    status_agent_int = false;
    status_agent_buffer = 0;

    while(rst.read() == true) sc_module::wait(rst.negedge_event());


    while(true){
        wait(update_int);

        unsigned int idx;
        bool found = false;
        for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
            if(buff_rx[i] && !buff_alloc_free[i] && buff_alloc_to_proxy_or_agent[i]){
                idx = i;
                found = true;
            }
        }

        status_agent_int = found;
        status_agent_buffer = idx;

        if(status_agent_int){
            irq = !irq.read();
        }
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
    unsigned int BUFFER_SIZE
>
class comp_manager: public Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation,
                                Timed::comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>,
                                Untimed::comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE> >::Result {
public:
    typedef  typename Implementation::IF<Implementation::Traits<Implementation::Sys>::timed_simulation,
                                Timed::comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE>,
                                Untimed::comp_manager<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y,BUFFER_SIZE> >::Result Base;

    comp_manager(sc_module_name nm
            , unsigned int start_address_
            , unsigned int end_address_) :Base(nm,start_address_,end_address_){}
};

#endif /* RTSNOC_BUS_PROXY_H_ */

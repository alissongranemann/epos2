/*
 * scheduler_test.h
 *
 *  Created on: Jun 7, 2011
 *      Author: tiago
 */

#ifndef SCHEDULER_TEST_H_
#define SCHEDULER_TEST_H_

#include <systemc.h>
#include "bus_model.h"
#include <iostream>
#include "../../../../../unified/framework/types.h"

SC_MODULE(Comp_Manager_Mediator) {

	sc_port<Bus_Model_IF> bus;

	SC_CTOR(Comp_Manager_Mediator){

	}

private:

	enum {
		ADDR_BASE = 0x80001000,
		ADDR_BASE_CTRL = ADDR_BASE,
		ADDR_BASE_BUFF = ADDR_BASE | 0x00000200,

		CTRL_CMD                 = 0x00 << 2,
		CTRL_CMD_RESULT          = 0x01 << 2,
		CTRL_CMD_IDLE            = 0x02 << 2,
		CTRL_STATUS_AGENT_INT    = 0x03 << 2,
		CTRL_STATUS_AGENT_BUFFER = 0x04 << 2,
		CTRL_INFO_NOC_X          = 0x05 << 2,
		CTRL_INFO_NOC_Y          = 0x06 << 2,
		CTRL_INFO_NOC_LOCAL      = 0x07 << 2,
		CTRL_INFO_BUFFER_SIZE    = 0x08 << 2,

		BUFF_PROXY_PHY_X        = 0x0 << 5,
		BUFF_PROXY_PHY_Y        = 0x1 << 5,
		BUFF_PROXY_PHY_LOCAL    = 0x2 << 5,
		BUFF_MSG_TYPE           = 0x3 << 5,
		BUFF_INSTANCE_ID        = 0x4 << 5,
		BUFF_TYPE_ID            = 0x5 << 5,
		BUFF_DATA               = 0x6 << 5,
		BUFF_TX                 = 0x7 << 5,
		BUFF_RX                 = 0x8 << 5,
		BUFF_AGENT_DISP_ADDR    = 0x9 << 5
	};

	unsigned int read_ctrl_reg(unsigned int reg){
	    return bus->read(ADDR_BASE_CTRL | reg);
	}
	void write_ctrl_reg(unsigned int reg, unsigned int data){
        bus->write(ADDR_BASE_CTRL | reg, data);
    }
	unsigned int read_buff_reg(unsigned int buff, unsigned int reg){
        return bus->read(ADDR_BASE_BUFF | reg | (buff<<2));
    }
    void write_buff_reg(unsigned int buff, unsigned int reg, unsigned int data){
        bus->write(ADDR_BASE_BUFF | reg | (buff<<2), data);
    }

    void ctrl_cmd(unsigned int val){ write_ctrl_reg(CTRL_CMD, val); }
    unsigned int ctrl_cmd() { return read_ctrl_reg(CTRL_CMD); }

    unsigned int ctrl_cmd_result() { return read_ctrl_reg(CTRL_CMD_RESULT); }

    bool ctrl_cmd_idle() { return read_ctrl_reg(CTRL_CMD_IDLE) == 1; }

    bool ctrl_status_agent_int() { return read_ctrl_reg(CTRL_STATUS_AGENT_INT) == 1; }

    unsigned int ctrl_status_agent_buffer() { return read_ctrl_reg(CTRL_STATUS_AGENT_BUFFER); }

    unsigned int ctrl_info_noc_x() { return read_ctrl_reg(CTRL_INFO_NOC_X); }
    unsigned int ctrl_info_noc_y() { return read_ctrl_reg(CTRL_INFO_NOC_Y); }
    unsigned int ctrl_info_noc_local() { return read_ctrl_reg(CTRL_INFO_NOC_LOCAL); }
    unsigned int ctrl_info_buff_size() { return read_ctrl_reg(CTRL_INFO_BUFFER_SIZE); }

    void buff_proxy_phy_x(unsigned int buff, unsigned int val){ write_buff_reg(buff,BUFF_PROXY_PHY_X,val); }
    unsigned int buff_proxy_phy_x(unsigned int buff) { return read_buff_reg(buff,BUFF_PROXY_PHY_X); }
    void buff_proxy_phy_y(unsigned int buff, unsigned int val){ write_buff_reg(buff,BUFF_PROXY_PHY_Y,val); }
    unsigned int buff_proxy_phy_y(unsigned int buff) { return read_buff_reg(buff,BUFF_PROXY_PHY_Y); }
    void buff_proxy_phy_local(unsigned int buff, unsigned int val){ write_buff_reg(buff,BUFF_PROXY_PHY_LOCAL,val); }
    unsigned int buff_proxy_phy_local(unsigned int buff) { return read_buff_reg(buff,BUFF_PROXY_PHY_LOCAL); }

    void buff_msg_type(unsigned int buff, unsigned int val){ write_buff_reg(buff,BUFF_MSG_TYPE,val); }
    unsigned int buff_msg_type(unsigned int buff) { return read_buff_reg(buff,BUFF_MSG_TYPE); }

    void buff_instance_id(unsigned int buff, unsigned int val){ write_buff_reg(buff,BUFF_INSTANCE_ID,val); }
    unsigned int buff_instance_id(unsigned int buff) { return read_buff_reg(buff,BUFF_INSTANCE_ID); }

    void buff_type_id(unsigned int buff, unsigned int val){ write_buff_reg(buff,BUFF_TYPE_ID,val); }
    unsigned int buff_type_id(unsigned int buff) { return read_buff_reg(buff,BUFF_TYPE_ID); }

    void buff_data_tx(unsigned int buff, unsigned int val){ write_buff_reg(buff,BUFF_DATA,val); }
    unsigned int buff_data_rx(unsigned int buff) { return read_buff_reg(buff,BUFF_DATA); }

    void buff_tx(unsigned int buff, bool val){ write_buff_reg(buff,BUFF_TX,val); }
    bool buff_tx(unsigned int buff) { return read_buff_reg(buff,BUFF_TX); }

    void buff_rx(unsigned int buff, bool val){ write_buff_reg(buff,BUFF_RX,val); }
    bool buff_rx(unsigned int buff) { return read_buff_reg(buff,BUFF_RX); }

    void buff_agent_disp_addr(unsigned int buff, unsigned int val){ write_buff_reg(buff,BUFF_AGENT_DISP_ADDR,val); }
    unsigned int buff_agent_disp_addr(unsigned int buff) { return read_buff_reg(buff,BUFF_AGENT_DISP_ADDR); }


public:

	//Status
	enum {
		CMD_ALLOC_PROXY = 0,
		CMD_ALLOC_AGENT = 1,
		CMD_RESULT_ERR = 0xFFFFFFFF
	};

	unsigned int alloc_proxy(unsigned int phy_x,
	                         unsigned int phy_y,
	                         unsigned int phy_local,
	                         unsigned int type_id,
	                         unsigned int instance_id){
	    sc_cout() << "\tComp_Manager::alloc_proxy("
	              << (void*) phy_x << ", "
	              << (void*) phy_y << ", "
	              << (void*) phy_local << ", "
	              << (void*) type_id << ", "
	              << (void*) instance_id << ")\n";
	    while(!ctrl_cmd_idle());
	    ctrl_cmd(CMD_ALLOC_PROXY);
	    while(!ctrl_cmd_idle());
	    unsigned int buff = ctrl_cmd_result();
	    if(buff != CMD_RESULT_ERR){
	        buff_proxy_phy_x(buff, phy_x);
	        buff_proxy_phy_y(buff, phy_y);
	        buff_proxy_phy_local(buff, phy_local);

	        buff_type_id(buff, type_id);
	        buff_instance_id(buff, instance_id);

	        //check
	        if(buff_proxy_phy_x(buff) != phy_x) sc_cout() << "--ERR: phy_x check err \n";
	        if(buff_proxy_phy_y(buff) != phy_y) sc_cout() << "--ERR: phy_y check err \n";;
	        if(buff_proxy_phy_local(buff) != phy_local) sc_cout() << "--ERR: phy_local check err \n";;
	        if(buff_type_id(buff) != type_id) sc_cout() << "--ERR: type_id check err \n";;
            if(buff_instance_id(buff) != instance_id) sc_cout() << "--ERR: instance_id check err \n";;
	    }
	    else{
	        sc_cout() << "--ERR: couldn't allocate buffer\n";
	    }
	    sc_cout() << "\t= " << (void*)buff << "\n";

	    return buff;
	}

	unsigned int alloc_agent(unsigned int type_id,
	                         unsigned int instance_id,
	                         unsigned int dispatcher_address){
	    sc_cout() << "\tComp_Manager::alloc_agent("
	            << (void*) type_id << ", "
	            << (void*) instance_id << ", "
	            << (void*) dispatcher_address << ")\n";
	    while(!ctrl_cmd_idle());
	    ctrl_cmd(CMD_ALLOC_AGENT);
	    while(!ctrl_cmd_idle());
	    unsigned int buff = ctrl_cmd_result();
	    if(buff != CMD_RESULT_ERR){

	        buff_type_id(buff, type_id);
	        buff_instance_id(buff, instance_id);
	        buff_agent_disp_addr(buff, dispatcher_address);

	        //check
	        if(buff_type_id(buff) != type_id) sc_cout() << "--ERR: type_id check err \n";;
	        if(buff_instance_id(buff) != instance_id) sc_cout() << "--ERR: instance_id check err \n";;
	        if(buff_agent_disp_addr(buff) != dispatcher_address) sc_cout() << "--ERR: dispatcher_address check err \n";;
	    }
	    else{
	        sc_cout() << "--ERR: couldn't allocate buffer\n";
	    }
	    sc_cout() << "\t= " << (void*)buff << "\n";

	    return buff;
	}

	void send_call(unsigned int buff, unsigned int op_id){
	    sc_cout() << "\tComp_Manager::send_call("
                  << (void*) buff << ", "
                  << (void*) op_id << ")\n";

	    while(buff_tx(buff));

	    buff_msg_type(buff, Implementation::MSG_TYPE_CALL);
	    buff_data_tx(buff, op_id);

	    buff_tx(buff,true);
	}

	void send_call_data(unsigned int buff, unsigned int n_args, unsigned int *data){
	    sc_cout() << "\tComp_Manager::send_call_data("
	              << (void*) buff << ", "
	              << (void*) n_args << ", "
	              << "data(";
	             for (unsigned int i = 0; i < n_args; ++i)
	                     std::cout << (void*)data[i] << ", ";
	            std::cout << "))\n";

	    while(buff_tx(buff));
	    buff_msg_type(buff, Implementation::MSG_TYPE_CALL_DATA);

	    for (int i = 0; i < n_args; ++i) {
	        buff_data_tx(buff, data[i]);
	        buff_tx(buff,true);
	        while(buff_tx(buff));
        }
	}

	void send_return_data(unsigned int buff, unsigned int n_ret, unsigned int *data){
	    sc_cout() << "\tComp_Manager::send_return_data("
	            << (void*) buff << ", "
	            << (void*) n_ret << ", "
	            << "data(";
	    for (unsigned int i = 0; i < n_ret; ++i)
	        std::cout << (void*)data[i] << ", ";
	    std::cout << "))\n";

	    while(buff_tx(buff));
	    buff_msg_type(buff, Implementation::MSG_TYPE_RESP_DATA);

	    for (int i = 0; i < n_ret; ++i) {
	        buff_data_tx(buff, data[i]);
	        buff_tx(buff,true);
	        while(buff_tx(buff));
	    }
	}

	void receive_return_data(unsigned int buff, unsigned int n_ret, unsigned int *data){
	    sc_cout() << "\tComp_Manager::receive_return_data("
	            << (void*) buff << ", "
	            << (void*) n_ret << ")\n";

	    //unsigned int db_cnt = 0;
	    for (int i = 0; i < n_ret; ++i) {
	        while(!buff_rx(buff));// if(++db_cnt >= 25) sc_stop();

	        unsigned int msg_type = buff_msg_type(buff);
	        if(msg_type == Implementation::MSG_TYPE_RESP_DATA){
	            data[i] = buff_data_rx(buff);
	        }
	        else{
	            sc_cout() << "\tERR- Received msg of type " << (void*)msg_type << " is not return data - TSC=" << sc_simulation_time() << "\n";
	        }
	        buff_rx(buff, false);

	    }

	    sc_cout() << "\t=data(";
        for (unsigned int i = 0; i < n_ret; ++i)
            std::cout << (void*)data[i] << ", ";
        std::cout << ")\n";
	}

	unsigned int receive_call(unsigned int buff){
	    sc_cout() << "\tComp_Manager::receive_call("
                  << (void*) buff << ")\n";
	    unsigned int op_id = 0xFFFFFFFF;
	    if(buff_rx(buff)){
	        unsigned int msg_type = buff_msg_type(buff);
	        if(msg_type == Implementation::MSG_TYPE_CALL){
	            op_id = buff_data_rx(buff);
	        }
	        else{
	            sc_cout() << "\tERR- Received msg of type " << (void*)msg_type << " is not a call\n";
	        }
	        buff_rx(buff, false);
	    }
	    else{
	        sc_cout() << "\tERR- No received data on buffer " << (void*)buff << "\n";
	    }
	    sc_cout() << "\t= " << (void*)op_id << "\n";
	    return op_id;
	}

	unsigned int receive_call_data(unsigned int buff){
	    sc_cout() << "\tComp_Manager::receive_call_data("
	            << (void*) buff << ")\n";
	    unsigned int call_data = 0;
	    if(buff_rx(buff)){
	        unsigned int msg_type = buff_msg_type(buff);
	        if(msg_type == Implementation::MSG_TYPE_CALL_DATA){
	            call_data = buff_data_rx(buff);
	        }
	        else{
	            sc_cout() << "\tERR- Received msg of type " << (void*)msg_type << " is not call data\n";
	        }
	        buff_rx(buff, false);
	    }
	    else{
	        sc_cout() << "\tERR- No received data on buffer " << (void*)buff << "\n";
	    }
	    sc_cout() << "\t= " << (void*)call_data << "\n";
	    return call_data;
	}


	unsigned int agent_has_call(){
	    sc_cout() << "\tComp_Manager::agent_has_call()\n";
	    unsigned int ret = 0;
	    if(ctrl_status_agent_int()){
	        ret = buff_agent_disp_addr(ctrl_status_agent_buffer());
	    }
	    sc_cout() << "\t= " << (void*)ret << "\n";
	    return ret;
	}

	void agent_int_enable(sc_event *ev){
	    sc_cout() << "\tComp_Manager::agent_int_enable()\n";
        bus->int_enable(ev);
    }

    void agent_int_disable(){
        sc_cout() << "\tComp_Manager::agent_int_disable()\n";
        bus->int_disable();
    }


};

#endif /* SCHEDULER_TEST_H_ */

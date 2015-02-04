/*
 * Testbench.h
 *
 *  Created on: Nov 18, 2010
 *      Author: tiago
 */

#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc.h>
#include <iostream>
#include "bus_model.h"
#include "hw_mediator.h"



SC_MODULE(Testbench) {

	//Ports
	sc_in<bool> clk_in;
	sc_in<bool> rst_in;

	// write address channel
    sc_out<bool> awvalid;
    sc_in<bool> awready;
    sc_out<sc_lv<32> > awaddr;
    sc_out<sc_lv<3> > awprot;
    // write data channel
    sc_out<bool> wvalid;
    sc_in<bool> wready;
    sc_out<sc_lv<32> > wdata;
    sc_out<sc_lv<4> > wstrb;
    //write response channel
    sc_in<bool> bvalid;
    sc_out<bool> bready;
    sc_in<sc_lv<2> > bresp;
    //read address channel
    sc_out<bool> arvalid;
    sc_in<bool> arready;
    sc_out<sc_lv<32> > araddr;
    sc_out<sc_lv<3> > arprot;
    //read data channel
    sc_in<bool> rvalid;
    sc_out<bool> rready;
    sc_in<sc_lv<32> > rdata;
    sc_in<sc_lv<2> > rresp;
    sc_in<bool> irq_in;

	//Variables, modules & etc
	Bus_Model bus_model;
	Comp_Manager_Mediator manager;

	sc_event int_event;

	unsigned int tsc;

	bool on_int;
    sc_event int_end;


	SC_CTOR(Testbench) :bus_model("Bus_Model"), manager("Manager"){

		tsc = 0;
		on_int = false;

		bus_model.clk_in(clk_in);
		bus_model.rst_in(rst_in);

		bus_model.araddr(araddr);
		bus_model.arprot(arprot);
		bus_model.arready(arready);
		bus_model.arvalid(arvalid);

		bus_model.awaddr(awaddr);
		bus_model.awprot(awprot);
		bus_model.awready(awready);
		bus_model.awvalid(awvalid);

		bus_model.wdata(wdata);
		bus_model.wready(wready);
		bus_model.wstrb(wstrb);
		bus_model.wvalid(wvalid);

		bus_model.bready(bready);
		bus_model.bresp(bresp);
		bus_model.bvalid(bvalid);

		bus_model.rdata(rdata);
		bus_model.rready(rready);
		bus_model.rresp(rresp);
		bus_model.rvalid(rvalid);

		bus_model.irq_in(irq_in);

		manager.bus(bus_model);

		SC_THREAD(exec_testbench);

		SC_THREAD(int_handler);

		SC_METHOD(clk_counter);
		sensitive << clk_in.pos();

	}

	enum{
	    CALLEE_DISPATCHER = 0xABCDEFAB
	};

	unsigned int caller_proxy_buff;


	unsigned int callee_agent_buff;
	enum{
	    CALLE_STATE_RX_CALL,
	    CALLE_STATE_RX_CALL_DATA,
	    CALLE_STATE_DO_CALL,
	};
	unsigned int call_agent_state;

	void proxy_call_func_tsc(){
	    unsigned int arg = tsc;
        sc_cout() << "Sending call 'unsigned int func_tsc("<<arg<<")'\n";
        manager.send_call(caller_proxy_buff,
                          0//OP_FUNC_TSC
                          );
        manager.send_call_data(caller_proxy_buff,
                               1, &arg);

        sc_cout() << "Waiting return\n";
        unsigned int ret;
        manager.receive_return_data(caller_proxy_buff, 1, &ret);
        sc_cout() << "Call 'unsigned int func_tsc(arg)' returned " << ret << "\n";
        sc_cout() << "SW->HW->SW call return: " << tsc-ret << " cycles\n";
	}

	void proxy_call_func_tsc_and_data(){
        unsigned int arg[5];
	    unsigned char *data = reinterpret_cast<unsigned char*>(arg);
        for (int i = 0; i < 16; ++i) data[i] = i;
        arg[4] = tsc;

        sc_cout() << "Sending call 'unsigned int func_tsc_and_data(char[16])'\n";
        manager.send_call(caller_proxy_buff,
                          1//OP_FUNC_TSC_AND_DATA
                          );
        manager.send_call_data(caller_proxy_buff,
                               5, arg);

        sc_cout() << "Waiting return\n";
        unsigned int ret;
        manager.receive_return_data(caller_proxy_buff, 1, &ret);
        sc_cout() << "Call 'unsigned int func_tsc(char[16])' returned " << ret << "\n";
        sc_cout() << "SW->HW->SW call return: " << tsc-ret << " cycles\n";
    }

	void exec_testbench(){
	    wait(rst_in.posedge_event());
		bus_model.wait_cycles(5);

        sc_cout() << "Allocating caller proxy buffer" << std::endl;
        caller_proxy_buff = manager.alloc_proxy(0,0,2, //phy
                                                6,//type_id
                                                0 //iid
                                                );
        sc_cout() << "Using buffer " << caller_proxy_buff << "\n";

        sc_cout() << "Allocating a dummy proxy buffer\n";
        unsigned int val = manager.alloc_proxy(0,0,0,0,0);
        sc_cout() << "Returned " << val << "\n";

        sc_cout() << "Allocating a dummy agent buffer\n";
        val = manager.alloc_agent(1,0,0);
        sc_cout() << "Returned " << val << "\n";

        sc_cout() << "Allocating callee agent buffer\n";
        callee_agent_buff = manager.alloc_agent(5,//type_id
                                                0, //iid
                                                CALLEE_DISPATCHER //dispatcher_addr
                                                );
        sc_cout() << "Using buffer " << callee_agent_buff << "\n";

        sc_cout() << "Allocating another dummy proxy buffer. Should return an error\n";
        val = manager.alloc_proxy(0,0,0,0,0);
        sc_cout() << "Returned " << val << "\n";


        sc_cout() << "Setting up interrupts\n";
        call_agent_state = CALLE_STATE_RX_CALL;
        manager.agent_int_enable(&int_event);

        proxy_call_func_tsc();

        proxy_call_func_tsc_and_data();

        proxy_call_func_tsc();

        proxy_call_func_tsc_and_data();

        while(true){
            bus_model.wait_cycles(30);
            if(!on_int) break;
            wait(int_end);
        }


        sc_cout() << "Done" << std::endl;
        sc_stop();

	}


	void int_handler(){

	    unsigned int op_id = 25;//random
	    unsigned int data[5];
	    unsigned int idx = 0;
	    while(true){
	        wait(int_event);
	        on_int = true;
	        sc_cout() << "INT begin\n";
	        manager.agent_int_disable();
	        unsigned int dispatcher = manager.agent_has_call();
	        while(dispatcher){
	            if(dispatcher != CALLEE_DISPATCHER){
	                sc_cout() << "--ERR: Dispatcher doesn't match, dispatcher=" << dispatcher << ", it should be " << CALLEE_DISPATCHER << "\n";
	                break;
	            }

	            if(call_agent_state == CALLE_STATE_RX_CALL){
	                op_id = manager.receive_call(callee_agent_buff);
	                if((op_id == 0) || (op_id == 1)) { //op_func_tsc/op_func_tsc_and_data
	                    call_agent_state = CALLE_STATE_RX_CALL_DATA;
	                }
	                else{
	                    sc_cout() << "-- ERR: Wrong operation, op_id=" << op_id << ", it should be 0 or 1\n";
	                }
	            }
	            else if(call_agent_state == CALLE_STATE_RX_CALL_DATA){
	                data[idx] = manager.receive_call_data(callee_agent_buff);
	                sc_cout() << "Callee received call with data = " << (void*)(data[idx]) << "\n";
	                ++idx;

	                if( ((op_id == 0) && (idx == 1)) ||
	                    ((op_id == 1) && (idx == 5)) ){

	                    idx = 0;

	                    if(op_id == 0){
	                        unsigned int diff = tsc - data[0];
	                        sc_cout() << "SW->HW->SW call: " << diff << " cycles\n";

	                        sc_cout() << "Returning " << tsc << "\n";
	                        data[0] = tsc;
	                        manager.send_return_data(callee_agent_buff, 1, data);
	                    }
	                    else{
	                        unsigned int diff = tsc - data[4];
	                        sc_cout() << "SW->HW->SW call: " << diff << " cycles\n";

	                        sc_cout() << "Returning " << tsc << "\n";
	                        data[0] = tsc;
	                        manager.send_return_data(callee_agent_buff, 1, data);
	                    }

	                    call_agent_state = CALLE_STATE_RX_CALL;
	                }


	            }

	            dispatcher = manager.agent_has_call();
	        }
	        manager.agent_int_enable(&int_event);
	        sc_cout() << "INT end\n";
	        on_int = false;
	        int_end.notify();
	    }
	}


	void clk_counter(){
		//std::cout << "#CLK#" << std::endl;
		++tsc;
	}

};

#endif /* TESTBENCH_H_ */

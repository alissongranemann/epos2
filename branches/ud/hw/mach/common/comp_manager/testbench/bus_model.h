/*
 * Testbench.h
 *
 *  Created on: Nov 18, 2010
 *      Author: tiago
 */

#ifndef BUS_MODEL_H_
#define BUS_MODEL_H_

#include <systemc.h>
#include <iostream>

inline std::ostream& sc_cout(){

    const int MAX_SIZE = 20;
    const char * process_name = sc_get_current_process_handle().get_process_object()->basename();
    int cnt = 0;
    while(process_name[cnt] != 0) ++cnt;
    int diff = MAX_SIZE - cnt;

    std::cout << process_name;
    for (int i = 0; i < diff; ++i) std::cout << " ";
    std::cout << "| ";

    return std::cout;
}

class Bus_Model_IF : public sc_interface {

public:
    virtual unsigned int read(unsigned int address) = 0;
    virtual void write(unsigned int address, unsigned int data) = 0;
    virtual void int_enable(sc_event *ev) = 0;
    virtual void int_disable() = 0;
    virtual void wait_cycles(int cycles) = 0;
};

class Bus_Model : public sc_module, public Bus_Model_IF {
public:
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

    sc_event *int_event;

    bool op;
    bool op_read;
    unsigned int op_data;
    unsigned int op_address;
    sc_event op_done;

    sc_semaphore *sc_sem;


	SC_CTOR(Bus_Model){

        int_event = 0;

	    op = false;

	    sc_sem = new sc_semaphore(1);

	    SC_CTHREAD(decoder, clk_in.pos())
		reset_signal_is(rst_in, false);

	    SC_THREAD(irq_handler);
	}


	unsigned int read(unsigned int address){
	    sc_sem->wait();
	    sc_cout() << "\t\tBus_Model::read(" << (void*)address << ")\n";
	    op_read = true;
	    op_address = address;
	    op = true;
	    //std::cout << "BUS_MODEL_READ-wait_op\n";
	    wait(op_done);
	    unsigned int local_op_data = op_data;
	    sc_cout() << "\t\t= " << (void*)local_op_data << "\n";
	    sc_sem->post();
	    wait(0, SC_NS);
	    return local_op_data;
	}

	void write(unsigned int address, unsigned int data){
	    sc_sem->wait();
	    sc_cout() << "\t\tBus_Model::write(" << (void*)address << ", " << (void*)data << ")\n";
	    op_read = false;
        op_address = address;
        op_data = data;
        op = true;
        //std::cout << "BUS_MODEL_WRITE-wait_op\n";
        wait(op_done);
        sc_sem->post();
        wait(0, SC_NS);
    }

	void int_enable(sc_event *ev){
	    int_event = ev;
	}

	void int_disable(){
	    int_event = 0;
	}

	void wait_cycles(int cycles){
	    for (int i = 0; i < cycles; ++i) wait(clk_in.posedge_event());
	}




private:

	void irq_handler(){

	    while(true){
	        wait(irq_in.posedge_event());
	        //std::cout << "BUS_MODEL_INT\n";

	        while(op){
	            //std::cout << "BUS_MODEL_INT-wait_op\n";
	            wait(op_done);
	        }

	        //std::cout << "BUS_MODEL_INT-notify\n";
	        if(int_event) int_event->notify();
	    }

	}

	void decoder(){
		arvalid = false;
		awvalid = false;
		wvalid  = false;

		awprot = 0;
        arprot = 0;

        rready = true;
        bready = true;

        awaddr = 0;
        wstrb = 0;
        araddr = 0;
        wdata = 0;

		wait();
		while(true){

		    if (op){
		        //std::cout << "BUS_OP_BEGIN\n";
		        if (op_read) {
		            arvalid = true;
		            araddr = op_address;
		            wait(); while(!arready.read()) wait();
		            arvalid = false;
		            wait(); while(!rvalid.read()) wait();
		            op_data = rdata.read().to_uint();
		        }
		        else{
		            awvalid = true;
		            wstrb = 0xF;
		            wdata = op_data;
		            awaddr = op_address;
		            wait(); while(!awready.read()) wait();
                    awvalid = false;
                    wvalid = true;
                    wait(); while(!wready.read()) wait();
                    wvalid = false;
                    wstrb = 0;
		        }
		        op = false;
		        //std::cout << "BUS_OP_END\n";
		        op_done.notify();
		    }
			wait();
		}
	}

};

#endif /* TESTBENCH_H_ */

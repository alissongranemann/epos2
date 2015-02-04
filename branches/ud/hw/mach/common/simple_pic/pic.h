/*
 * uart.h
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#ifndef PIC_H_
#define PIC_H_

#include <systemc.h>
#include <arch/common/bus_slave_if.h>
#include <common/global.h>
#include <components/traits.h>

namespace Timed {
class pic : public sc_module,
	public bus_slave_if {

public:

	sc_in<bool>		rst;
	sc_in<bool>		irq_in[32];
	sc_out<bool>	irq_out;

	SC_HAS_PROCESS(pic);
	pic(sc_module_name nm_
			, unsigned int start_address_
			, unsigned int end_address_)
        : sc_module(nm_)
        , start_address(start_address_)
        , end_address(end_address_)
	{
		SC_THREAD(handle_int);

		SC_METHOD(inputs_changed);
		for (int i = 0; i < 32; ++i) sensitive << irq_in[i];



	}

	~pic() {

	}

	//interfaces
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

	unsigned int get_start_address() const;
	unsigned int get_end_address() const;

	slave_type get_type() const {return DEVICE;}


private:

	unsigned int	start_address;
	unsigned int	end_address;

	sc_event inputs_changed_evt;

	sc_uint<32> edge_enable;
	sc_uint<32> polarity;
public:
	sc_uint<32> mask;
	sc_uint<32> pending;
private:
	sc_event regs_changed;


    void inputs_changed(){
        inputs_changed_evt.notify();
    }

    void handle_int(){
        polarity = 0;
        edge_enable = 0;
        irq_out = false;
        mask = 0;
        sc_uint<32> irq = 0;

        while(rst.read() == true) wait(rst.negedge_event());

        while(true){
            wait(inputs_changed_evt | regs_changed);

            for (int i = 0; i < 32; ++i) {
                if(edge_enable[i]){
                    if(polarity[i]) pending[i] |= irq_in[i].posedge();
                    else            pending[i] |= irq_in[i].negedge();
                }
                else pending[i] |= polarity[i] ? irq_in[i].read() : !irq_in[i].read();
            }

            irq = pending & (~mask);
            irq_out = irq.or_reduce();

        }
    }

}; };

namespace Untimed {
class pic : public sc_module,
    public bus_slave_if {

public:

    sc_in<bool>     rst;
    sc_in<bool>     irq_in[32];
    sc_out<bool>    irq_out;

    SC_HAS_PROCESS(pic);
    pic(sc_module_name nm_
            , unsigned int start_address_
            , unsigned int end_address_)
        : sc_module(nm_)
        , start_address(start_address_)
        , end_address(end_address_)
    {
        SC_THREAD(handle_int);

        SC_METHOD(inputs_changed);
        for (int i = 0; i < 32; ++i) sensitive << irq_in[i];

        for (int i = 0; i < 32; ++i) irq_in_prev[i] = false;
    }

    ~pic() {

    }

    //interfaces
    void read(unsigned int *data, unsigned int address, int size);
    void write(unsigned int data, unsigned int address, int size);

    unsigned int get_start_address() const;
    unsigned int get_end_address() const;

    slave_type get_type() const {return DEVICE;}


private:

    unsigned int    start_address;
    unsigned int    end_address;

    sc_event inputs_changed_evt;

    bool irq_in_prev[32];

public:
    sc_uint<32> mask;
    sc_uint<32> pending;
private:
    sc_event regs_changed;


    void inputs_changed(){
        inputs_changed_evt.notify();
    }

    void handle_int(){
        irq_out = false;
        mask = 0;
        sc_uint<32> irq = 0;

        while(rst.read() == true) wait(rst.negedge_event());

        while(true){
            wait(inputs_changed_evt | regs_changed);

            for (int i = 0; i < 32; ++i) {
                if(irq_in[i].read() != irq_in_prev[i]) pending[i] = true;
                irq_in_prev[i] = irq_in[i].read();
            }
            irq = pending & (~mask);
            irq_out = irq.or_reduce();
        }
    }

}; };

DECLARE_SLAVE_DEVICE_MODEL(pic);

#endif /* GPIO_H_ */

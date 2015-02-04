/*
 * uart.h
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <systemc.h>
#include <arch/common/bus_slave_if.h>
#include <common/global.h>
#include <common/debugs.h>
#include <components/traits.h>

namespace Timed {
class timer : public sc_module,
	public bus_slave_if {

public:
	sc_in<bool>		rst;
	sc_out<bool>	irq;

	SC_HAS_PROCESS(timer);
	timer(sc_module_name nm_
			, unsigned int start_address_
			, unsigned int end_address_)
		: sc_module(nm_)
		, start_address(start_address_)
		, end_address(end_address_)
	{
		time = 0;
		int_time = 0;

		SC_THREAD(timer_proc);

	}

	~timer() {

	}

	//interfaces
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

	unsigned int get_start_address() const;
	unsigned int get_end_address() const;

	slave_type get_type() const {return DEVICE;}

	unsigned long int get_tsc(){ return time;}
	sc_signal<unsigned long int>& get_tsc_sig(){ return time;}

private:

	unsigned int	start_address;
	unsigned int	end_address;

	void timer_proc();

	sc_signal<unsigned long int> time;
	unsigned long int	int_time;
	unsigned long int	increment;


}; };

namespace Untimed {
class timer : public sc_module,
    public bus_slave_if {

public:
    sc_in<bool>     rst;
    sc_out<bool>    irq;

    SC_HAS_PROCESS(timer);
    timer(sc_module_name nm_
            , unsigned int start_address_
            , unsigned int end_address_)
        : sc_module(nm_)
        , start_address(start_address_)
        , end_address(end_address_)
    {
        SC_THREAD(timer_proc);
    }

    ~timer() {

    }

    //interfaces
    void read(unsigned int *data, unsigned int address, int size);
    void write(unsigned int data, unsigned int address, int size);

    unsigned int get_start_address() const;
    unsigned int get_end_address() const;

    slave_type get_type() const {return DEVICE;}

    unsigned long int get_tsc(){ return Global::get_sim_time_ticks<0>();}

private:

    unsigned int    start_address;
    unsigned int    end_address;

    void timer_proc();

    sc_event increment_written;
    unsigned long int   increment;


}; };

DECLARE_SLAVE_DEVICE_MODEL(timer);

#endif /* GPIO_H_ */

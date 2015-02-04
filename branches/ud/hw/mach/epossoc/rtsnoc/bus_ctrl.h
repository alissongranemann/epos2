/*
 * bus_ctrl.h
 *
 *  Created on: 29/03/2010
 *      Author: tiago
 */

#ifndef BUS_CTRL_H_
#define BUS_CTRL_H_

#include <systemc.h>
#include <arch/common/bus_slave_if.h>
#include <arch/common/bus_if.h>
#include <common/debugs.h>

/*
 * This guy here emulates the machine in which devices have wichbone interface
 * and are conected to the bus through a axi4->wishbone converter, and memories
 * are axi4 native. Therefore it aplies here an extra delay for slave of type DEVICE
 *
 */
class bus_ctrl : public sc_module,
	public bus_if {

public:

	// ports
	sc_port<bus_slave_if, 0>	module_port;

	// constructor
	bus_ctrl(sc_module_name nm_) : sc_module(nm_) {

		lock_ = 0;

		begin_ms = 0;
	}

	// destructor
	~bus_ctrl() {

	}

	//interfaces
	void lock();
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

private:

	void 			end_of_elaboration();
	bus_slave_if* 		get_module(unsigned int address);

	sc_event		free;

	int				lock_;

	long int begin_ms;

};

#endif /* BUS_CTRL_H_ */

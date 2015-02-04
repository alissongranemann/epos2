/*
 * fetch.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef FETCH_H_
#define FETCH_H_

#include <systemc.h>
#include <arch/common/bus_if.h>
#include "plasma_pack.h"


class Fetch {

public:
	sc_port<bus_if> mem_ctrl;

	// constructor
	Fetch(sc_module_name nm, pc_info *_pc_data, pc_new_info *_pc_new_data, cpu_info *_cpu_status) {
	    pc_data = _pc_data;
		pc_new_data = _pc_new_data;
		cpu_status = _cpu_status;

		pc_data->pc_next = 0x00;
		pc_data->pc_next_on_ds = false;
		pc_data->pc_next_plus4 = 0x04;
	}

	// destructor
	~Fetch() {
	}

	// process
	void do_fetch();

#ifdef dump_signals_vcd_internal
public:
#else
private:
#endif
	pc_info			*pc_data;
	pc_new_info		*pc_new_data;
	cpu_info        *cpu_status;
};

#endif /* FETCH_H_ */

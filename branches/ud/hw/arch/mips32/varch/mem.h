/*
 * mem.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef MEM_H_
#define MEM_H_

#include <systemc.h>
#include <arch/common/bus_if.h>
#include "plasma_pack.h"

//#include "memory.h"

class Mem {

public:

	// ports
	sc_port<bus_if> mem_ctrl;

	// constructor
	Mem(sc_module_name nm,
	    pc_info* _pc_data, dec_info *_dec_data, cpu_info *_cpu_status, mem_info *_mem_data, wb_info *_wb_data) {
		pc_data = _pc_data;
		dec_data = _dec_data;
		cpu_status = _cpu_status;
		mem_data = _mem_data;
		wb_data = _wb_data;
	}

	// destructor
	~Mem() {
	}

	// process
	void do_run();

private:

	void 			do_mem_access();
	void			do_read();
	void 			do_write();

	pc_info			*pc_data;
	dec_info		*dec_data;
	mem_info		*mem_data;
	wb_info			*wb_data;
	cpu_info		*cpu_status;

};

#endif /* MEM_H_ */

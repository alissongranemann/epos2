/*
 * exec.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef EXEC_H_
#define EXEC_H_

#include <systemc.h>

class Exec {

public:

	// ports
	sc_in<bool> irq;

	// constructor
	Exec(sc_module_name nm,
	     pc_info *_pc_data, pc_new_info *_pc_new_data, dec_info *_dec_data, cpu_info *_cpu_status, mem_info *_mem_data, wb_info *_wb_data) {

		pc_data = _pc_data;
		pc_new_data = _pc_new_data;
		dec_data = _dec_data;
		cpu_status = _cpu_status;
		mem_data = _mem_data;
		wb_data = _wb_data;

		pc_new_data->pc_source = FROM_INC4;
		pc_new_data->pc_new = 0x04;
		pc_new_data->pc_opcode = 0x04;

	}

	// destructor
	~Exec() {
	}

	// process
	int do_exec();

	// interfaces
	void write(int, int);

#ifdef dump_signals_vcd_internal
public:
#else
private:
#endif

	int			do_operate();
	void	    mult_big(int, int);
	void 		mult_big_signed(int, int);
	int         get_delay(unsigned int op, unsigned int func, unsigned int rt);

	int				A;
	int				B;

	unsigned int	uB;
	unsigned int	uA;

	int				hi;
	int				lo;

	int				exceptionId;
	bool 			branch;

    pc_info			*pc_data;
	pc_new_info		*pc_new_data;
	dec_info		*dec_data;
	mem_info		*mem_data;
	wb_info			*wb_data;
public:
	cpu_info		*cpu_status;

};

#endif /* EXEC_H_ */

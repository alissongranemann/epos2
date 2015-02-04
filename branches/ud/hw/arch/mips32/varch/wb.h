/*
 * wb.h
 *
 *  Created on: 04/03/2010
 *      Author: tiago
 */

#ifndef WB_H_
#define WB_H_

#include <systemc.h>
#include "plasma_pack.h"

class WB {

public:
	// constructor
	WB(sc_module_name nm,
	   int *r_, pc_info* _pc_data, dec_info *_dec_data, cpu_info *_cpu_status, wb_info *_wb_data) :
		r(r_){

		pc_data = _pc_data;
		dec_data = _dec_data;
		wb_data = _wb_data;
		cpu_status = _cpu_status;

		u = (unsigned int*) r;
	}

	// destructor
	~WB() {
	}

	// process
	void do_wb();

private:
	pc_info			*pc_data;
	dec_info		*dec_data;
	wb_info			*wb_data;
	cpu_info		*cpu_status;

	int				*r;
	unsigned int	*u;

};

#endif /* WB_H_ */

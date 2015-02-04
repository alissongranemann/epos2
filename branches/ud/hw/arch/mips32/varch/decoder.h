/*
 * Decoder.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <systemc.h>
#include "plasma_pack.h"
#include "exec.h"

class Decoder {

public:

	// ports
	Exec *exec;

	// constructor
	Decoder(sc_module_name nm, int *r_, pc_info *_pc_data, dec_info *_dec_data) :
		r(r_) {
		// process declaration
		pc_data = _pc_data;
		dec_data = _dec_data;

		u = (unsigned int*) r;
	}

	// destructor
	~Decoder() {
	}

	// process
	void do_decode();

private:
	int				*r;
	unsigned int	*u;

	pc_info			*pc_data;
	dec_info		*dec_data;

	void            show_dec();
};


#endif /* Decoder_H_ */

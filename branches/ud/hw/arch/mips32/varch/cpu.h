/*
 * cpu.h
 *
 *  Created on: Jan 13, 2012
 *      Author: tiago
 */

#ifndef CPU_H_
#define CPU_H_

#include <systemc.h>
#include <arch/common/bus_if.h>

SC_MODULE(cpu){
	sc_in<bool>		rst;
	sc_in<bool>		irq;
	sc_port<bus_if> bus;

	SC_CTOR(cpu){

	}
};

#endif /* CPU_H_ */

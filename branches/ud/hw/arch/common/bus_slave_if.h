/*
 * bus_slave_if.h
 *
 *  Created on: 29/03/2010
 *      Author: tiago
 */

#ifndef BUS_SLAVE_IF_H_
#define BUS_SLAVE_IF_H_

#include <systemc.h>

class bus_slave_if : public sc_interface {

public:

    typedef enum{
        DEVICE,
        MEMORY,
    }slave_type;

	virtual void read(unsigned int *data, unsigned int address, int size) = 0;
	virtual void write(unsigned int data, unsigned int address, int size) = 0;

	virtual unsigned int get_start_address() const = 0;
	virtual unsigned int get_end_address() const = 0;

	virtual slave_type get_type() const = 0;
};

#endif /* BUS_SLAVE_IF_H_ */

/*
 * mem_ctrl.cpp
 *
 *  Created on: 29/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include <iomanip>
#include "bus_ctrl.h"
#include <common/global.h>

/*
 *  process
 */

void bus_ctrl::end_of_elaboration() {
	// perform a static check for overlapping memory areas of the slaves

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::end_of_elaboration()";
#endif

	bool no_overlap;
	for(int i = 1; i < module_port.size(); ++i) {
		bus_slave_if *module_1 = module_port[i];
		for(int j = 0; j < i; ++j) {
			bus_slave_if *module_2 = module_port[j];
			no_overlap = (module_1->get_end_address() < module_2->get_start_address()) ||
					(module_1->get_start_address() > module_2->get_end_address());
			if(!no_overlap) {
				std::cout << "Error: overlapping address spaces of 2 slaves :" << endl;
				std::cout << "module " << i << " 0x" << std::hex << module_1->get_start_address() << " 0x" << std::hex << module_1->get_end_address() << endl;
				std::cout << "module " << i << " 0x" << std::hex << module_2->get_start_address() << " 0x" << std::hex << module_2->get_end_address() << endl;
				Global::safe_finish(this);
			}
		}
	}
}

bus_slave_if* bus_ctrl::get_module(unsigned int address) {

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::get_module(" << (void*)address << ")\n";
#endif

	for(int i = 0; i < module_port.size(); ++i) {
		bus_slave_if* module = module_port[i];
		if((module->get_start_address() <= address) &&
				(address <= module->get_end_address())) {

#ifdef mem_ctrl_debug
			std::cout << "mem_ctrl::get_module() - Module found\n";
#endif

			if(module->get_type() == bus_slave_if::DEVICE){
			    Global::delay_cycles<0>(this, 5);
			}

			return module;
		}
	}

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::get_module() - Module not found\n";
#endif

	return (bus_slave_if*) 0;
}

/*
 *  interfaces
 */

void bus_ctrl::lock() {
	if(lock_)
		wait(free);

	lock_ = 1;
}

void bus_ctrl::read(unsigned int *data, unsigned int address, int size) {

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::read(*data, " << (void*)address << ", " << size << ")\n";
#endif

	bus_slave_if  *module = 0;

	if(address == 0xFFFFFFFC) {
		std::cout << "## INFO Exit command detected\n";
		std::cout << "## INFO Real world time elapsed: " << Global::get_real_time_ms() - begin_ms << "ms\n";
		Global::safe_finish(this);
		goto mem_ctrl_read_end;
	}
	else if(address == 0xFFFFFFF8){
	    std::cout << "##INFO Begin command detected\n";
	    begin_ms = Global::get_real_time_ms();
	    goto mem_ctrl_read_end;
	}

	module = get_module(address);
	if(!module) {
		std::cout << "## ERROR: mem_ctrl::read(" << (void*)address<< ") - error: module no found\n";
		Global::safe_finish(this);
		goto mem_ctrl_read_end;
	}

	module->read(data, address, size);

	switch(size) {
	case 4:
		//assert((address &3) == 0);
		break;
	case 2:
		//assert((address & 1) == 0);
		break;
	case 1:
		break;

	default: std::cout << "## ERROR - READ MEMORY"; break;
	}

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::read():" << (void*)(*data) << "\n";
#endif
	mem_ctrl_read_end:
	lock_ = 0;
	free.notify();
}

void bus_ctrl::write(unsigned int data, unsigned int address, int size) {

	unsigned int data_;

	data_ = 0;

	bus_slave_if  *module = 0;

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::write("<< (void*)data << ", " << (void*)address << ", " << size << ")\n";
#endif

    if(address == 0xFFFFFFFC) {
        std::cout << "## INFO Exit command detected\n";
        std::cout << "## INFO Real world time elapsed: " << Global::get_real_time_ms() - begin_ms << "ms\n";
        Global::safe_finish(this);
        goto mem_ctrl_write_end;
    }
    else if(address == 0xFFFFFFF8){
        std::cout << "## INFO Begin command detected\n";
        begin_ms = Global::get_real_time_ms();
        goto mem_ctrl_write_end;
    }

	module = get_module(address);
	if(!module) {
	    std::cout << "## ERROR mem_ctrl::write("<< (void*)data << ", " << (void*)address << ", " << size << ") - target module not found\n";
	    Global::safe_finish(this);
	    goto mem_ctrl_write_end;
	}

	switch(size) {
	case 4:
		//assert((address & 3) == 0);
		data_ = data;
		break;
	case 2:
		//assert((address & 1) == 0);
		data_ = (unsigned short) data;
		break;
	case 1:
		data_ = (unsigned char) data;
		break;
	default:
		std::cout << "ERROR - WRITE MEMORY";
	}

	module->write(data_, address, size);

	mem_ctrl_write_end:
	lock_ = 0;
	free.notify();

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::write(): null\n";
#endif
}

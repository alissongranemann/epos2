/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include "pic.h"

namespace Timed {

void pic::read(unsigned int *data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case 0:
		*data = edge_enable;
		break;
	case 1:
		*data = polarity;
		break;
	case 2:
		*data = mask;
		break;
	case 3:
		*data = pending;
		break;
	default:
		*data = 0;
		break;
	}

	//std::cout << "##INFO: pic::read(data=" << (void*)(*data) << " address=" << (void*)address << ")\n"
	//			  <<"\tedge_enable=\t\t" << edge_enable.to_string(SC_BIN)<<"\n"
	//			  <<"\tpolarity=\t\t" << polarity.to_string(SC_BIN)<<"\n"
	//			  <<"\tmask=\t\t\t" << mask.to_string(SC_BIN)<<"\n"
	//			  <<"\tpending=\t\t" << pending.to_string(SC_BIN)
	//			  <<"\n";

	Global::delay_cycles<0>(this, 1);

}

void pic::write(unsigned int data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case 0:
		edge_enable = data;
		break;
	case 1:
		polarity = data;
		break;
	case 2:
		mask = data;
		break;
	case 3:{
		pending &= ~data;
		break;
	}
	default:
		break;
	}

	//std::cout << "##INFO: pic::write(data=" << (void*)data << " address=" << (void*)address << ")\n"
	//			  <<"\tedge_enable=\t\t" << edge_enable.to_string(SC_BIN)<<"\n"
	//			  <<"\tpolarity=\t\t" << polarity.to_string(SC_BIN)<<"\n"
	//			  <<"\tmask=\t\t\t" << mask.to_string(SC_BIN)<<"\n"
	//			  <<"\tpending=\t\t" << pending.to_string(SC_BIN)
	//			  <<"\n";

	Global::delay_cycles<0>(this, 1);

	regs_changed.notify();

}

unsigned int pic::get_start_address() const{
	return start_address;
}

unsigned int pic::get_end_address() const{
	return end_address;
}

};

namespace Untimed {

void pic::read(unsigned int *data, unsigned int address, int size) {

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
    case 0:
    case 1:
        *data = 0;
        break;
    case 2:
        *data = mask;
        break;
    case 3:
        *data = pending;
        break;
    default:
        *data = 0;
        break;
    }

    //std::cout << "##INFO: pic::read(data=" << (void*)(*data) << " address=" << (void*)address << ")\n"
    //            <<"\tedge_enable=\t\t" << edge_enable.to_string(SC_BIN)<<"\n"
    //            <<"\tpolarity=\t\t" << polarity.to_string(SC_BIN)<<"\n"
    //            <<"\tmask=\t\t\t" << mask.to_string(SC_BIN)<<"\n"
    //            <<"\tpending=\t\t" << pending.to_string(SC_BIN)
    //            <<"\n";

}

void pic::write(unsigned int data, unsigned int address, int size) {

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
    case 0:
    case 1:
        break;
    case 2:
        mask = data;
        break;
    case 3:{
        pending &= ~data;
        break;
    }
    default:
        break;
    }

    //std::cout << "##INFO: pic::write(data=" << (void*)data << " address=" << (void*)address << ")\n"
    //            <<"\tedge_enable=\t\t" << edge_enable.to_string(SC_BIN)<<"\n"
    //            <<"\tpolarity=\t\t" << polarity.to_string(SC_BIN)<<"\n"
    //            <<"\tmask=\t\t\t" << mask.to_string(SC_BIN)<<"\n"
    //            <<"\tpending=\t\t" << pending.to_string(SC_BIN)
    //            <<"\n";
    regs_changed.notify();

}

unsigned int pic::get_start_address() const{
    return start_address;
}

unsigned int pic::get_end_address() const{
    return end_address;
}

};


/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include "timer.h"
#include <system/types.h>

namespace Timed {

void timer::read(unsigned int *data, unsigned int address, int size) {

	if(address == start_address)
	    *data = time;
	else
	    *data = (unsigned int)Global::get_real_time_ms();

	//std::cout << "##INFO: timer::read(data=" << (void*)(*data) << " address=" << (void*)address << ") |"
	//		  <<" time=" << time
	//		  <<" int_time=" << int_time
	//		  <<"\n";

	Global::delay_cycles<0>(this, 1);
}

void timer::write(unsigned int data, unsigned int address, int size) {


    increment = data;
    int_time = (data != 0) ? (time + data) : 0;

	//std::cout << "##INFO: timer::write(data=" << (void*)data << " address=" << (void*)address << ") |"
	//			  <<" time=" << time
	//			  <<" int_time=" << int_time
	//			  <<"\n";

	Global::delay_cycles<0>(this, 1);

}

unsigned int timer::get_start_address() const{
	return start_address;
}

unsigned int timer::get_end_address() const{
	return end_address;
}

void timer::timer_proc() {
	time = 0;
	int_time = 0;
	irq = false;

	while(rst.read() == true) wait(rst.negedge_event());

	Global::delay_cycles<0>(this, 1);
	while(true){

		unsigned long int aux = time.read() + 1;
		Global::update_global_timer_ticks((unsigned int)aux);
	    time.write(aux);


		if((int_time != 0) && (int_time == time)){
			int_time = time + increment;
			irq = true;
			//std::cout << "##INFO: timer::timer_proc(irq=true) |"
			//				  <<" time=" << time
			//				  <<" int_time=" << int_time
			//				  <<"\n";
		}
		else{
			irq = false;
		}

		Global::delay_cycles<0>(this, 1);
	}
}

};

namespace Untimed {

void timer::read(unsigned int *data, unsigned int address, int size) {

    if(address == start_address)
        *data = Global::get_sim_time_ticks<0>();
    else
        *data = (unsigned int)Global::get_real_time_ms();

}

void timer::write(unsigned int data, unsigned int address, int size) {

    if(data != 0) {
        increment = data;
        increment_written.notify();
    }

}

unsigned int timer::get_start_address() const{
    return start_address;
}

unsigned int timer::get_end_address() const{
    return end_address;
}

void timer::timer_proc() {
    increment = 0;
    irq = false;

    while(rst.read() == true) wait(rst.negedge_event());

    while(true){

        if(increment == 0) wait(increment_written);

        if(increment != 0){
            Global::delay(this,Global::ticks_to_ps<0>(increment), SC_PS);
            irq = !irq.read();
        }

    }
}

};



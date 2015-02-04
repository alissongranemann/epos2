/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include "gpio.h"

namespace Timed {

void gpio::read(unsigned int *data, unsigned int address, int size) {

	//std::cout << "gpio::read(*data, " << address.to_string(SC_HEX) << ", " << size << ")\n";

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case 0:
		//*data = read(gpio_out);
		*data = gambi_gpio_out.read().to_uint();
		break;
	case 1:
		//*data = read(gpio_in);
		*data = gambi_gpio_in.read().to_uint();
		break;
	default:
		*data = 0;
		break;
	}

	//std::cout << "gpio::read():" << data->to_string(SC_HEX) << "\n";

	Global::delay_cycles<0>(this, 1);

}

void gpio::write(unsigned int data, unsigned int address, int size) {

	//std::cout << "gpio::write("<< data.to_string(SC_HEX) << ", " << address.to_string(SC_HEX) << ", " << size << ")\n";

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case 0:
		//write(gpio_out, data);
		gambi_gpio_out = data;
		break;
	default:
		break;
	}

	Global::delay_cycles<0>(this, 1);

}

unsigned int gpio::get_start_address() const{
	return start_address;
}

unsigned int gpio::get_end_address() const{
	return end_address;
}

unsigned int gpio::read(const char *filename){

	fstream gpio;

	gpio.open (filename, fstream::in | fstream::binary);

	unsigned int data = 0;

	char val[4];

	gpio.read(val, 4);

	data |= (static_cast<unsigned int>(val[0]) << 24);
	data |= (static_cast<unsigned int>(val[1]) << 16);
	data |= (static_cast<unsigned int>(val[2]) << 8);
	data |= static_cast<unsigned int>(val[3]);

	gpio.close();

	return data;

}

void gpio::write(const char *filename, unsigned int val){

	fstream gpio;

	gpio.open (filename, fstream::out | fstream::binary);

	char c[4];
	c[0] = static_cast<char>((val >> 24) & 0x000000FF);
	c[1] = static_cast<char>((val >> 16) & 0x000000FF);
	c[2] = static_cast<char>((val >> 8) & 0x000000FF);
	c[3] = static_cast<char>(val & 0x000000FF);

	gpio.write(c, 4);

	gpio.close();

}

};

namespace Untimed {

void gpio::read(unsigned int *data, unsigned int address, int size) {

    //std::cout << "gpio::read(*data, " << address.to_string(SC_HEX) << ", " << size << ")\n";

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
    case 0:
        //*data = read(gpio_out);
        *data = gambi_gpio_out.read().to_uint();
        break;
    case 1:
        //*data = read(gpio_in);
        *data = gambi_gpio_in.read().to_uint();
        break;
    default:
        *data = 0;
        break;
    }

    //std::cout << "gpio::read():" << data->to_string(SC_HEX) << "\n";

}

void gpio::write(unsigned int data, unsigned int address, int size) {

    //std::cout << "gpio::write("<< data.to_string(SC_HEX) << ", " << address.to_string(SC_HEX) << ", " << size << ")\n";

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
    case 0:
        //write(gpio_out, data);
        gambi_gpio_out = data;
        break;
    default:
        break;
    }

}

unsigned int gpio::get_start_address() const{
    return start_address;
}

unsigned int gpio::get_end_address() const{
    return end_address;
}

unsigned int gpio::read(const char *filename){

    fstream gpio;

    gpio.open (filename, fstream::in | fstream::binary);

    unsigned int data = 0;

    char val[4];

    gpio.read(val, 4);

    data |= (static_cast<unsigned int>(val[0]) << 24);
    data |= (static_cast<unsigned int>(val[1]) << 16);
    data |= (static_cast<unsigned int>(val[2]) << 8);
    data |= static_cast<unsigned int>(val[3]);

    gpio.close();

    return data;

}

void gpio::write(const char *filename, unsigned int val){

    fstream gpio;

    gpio.open (filename, fstream::out | fstream::binary);

    char c[4];
    c[0] = static_cast<char>((val >> 24) & 0x000000FF);
    c[1] = static_cast<char>((val >> 16) & 0x000000FF);
    c[2] = static_cast<char>((val >> 8) & 0x000000FF);
    c[3] = static_cast<char>(val & 0x000000FF);

    gpio.write(c, 4);

    gpio.close();

}

};



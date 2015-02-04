/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include "uart.h"
#include <arch/mips32/varch/plasma_pack.h>

//#define uart_debug

namespace Timed {

void uart::read(unsigned int *data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case SUART_TXLEVEL:
		*data = (unsigned int)ntohl((32 - tx_fifo.size()));
		break;
	case SUART_RXLEVEL:
		*data = (unsigned int)ntohl(rx_fifo.size());
		break;
	case SUART_RXCHAR:
		if(!rx_fifo.empty()){
			*data = (unsigned int)ntohl(rx_fifo.front());
			rx_fifo.pop();
			//std::cout << "uart::read():" << data->to_string(SC_HEX) << "\n";
			bootloader_check((char)*data);
		}
		else
			*data = 0;
		break;
	default:
		*data = 0;
		break;
	}

	Global::delay_cycles<0>(this, 1);

}

void uart::write(unsigned int data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case SUART_CLKDIV:
		//TODO
		break;
	case SUART_TXCHAR:
		tx_fifo.push((unsigned int)htonl(data));
		tx_fifo_add.notify();
		break;
	default:
		break;
	}

	Global::delay_cycles<0>(this, 1);

}

unsigned int uart::get_start_address() const{
	return start_address;
}

unsigned int uart::get_end_address() const{
	return end_address;
}

void uart::tx_thread(){
	while(true){
	    Global::safe_finish(this, true);

	    wait(tx_fifo_add);

		Global::safe_finish(this, false);

		while(!tx_fifo.empty()){
			unsigned int val = tx_fifo.front();
			tx_fifo.pop();

			//std::cout << std::setfill('0') << std::setw(8) << std::hex << data << "\n";

			//std::cout << (char) htonl(val);

			pty.terminal_write((char) htonl(val));
			delay_txrx();

		}
	}

}

void uart::rx_thread(){
	while(true){

	    if((bootloader_state==BOOTLOADER_END) && DISABLE_TX_AFTER_BOOT){
	        std::cout << "## INFO: UART - bootloader end detected, 'rx_thread' process exiting\n";
	        break;
	    }

		if(pty.input_available()){
			char c = pty.terminal_read();
			rx_fifo.push(htonl(0x000000FF & (unsigned int)c));
		}
		Global::delay_cycles<0>(this, 1);
	}
}

void uart::rxtx_int(){
	tx_int = (32 - tx_fifo.size()) != 0;
	rx_int = !rx_fifo.empty();

	if(USING_INTERRUPTS){

	    while(rst.read() == true) wait(rst.negedge_event());
	    Global::delay_cycles<0>(this, 1);

	    while(true){
	        tx_int = (32 - tx_fifo.size()) != 0;
	        rx_int = !rx_fifo.empty();
	        Global::delay_cycles<0>(this, 1);
	    }

	}
	std::cout << "## INFO: UART - interrupts hard disabled, 'rxtx_int' process exiting\n";
}

void uart::bootloader_check(char c){
    unsigned int tmp = 0;
    static unsigned int tmp_last = 0;
    switch (bootloader_state) {
    case BOOTLOADER_SYNC_0:
        if(c == BOOTLOADER_ENABLE) bootloader_state = BOOTLOADER_SYNC_1;
        break;
    case BOOTLOADER_SYNC_1:
        if(c == BOOTLOADER_ENABLE) bootloader_state = BOOTLOADER_SIZE;
        break;
    case BOOTLOADER_SIZE:
        reinterpret_cast<unsigned char*>(&bootloader_size)[bootloader_size_idx] = (unsigned char)c;
        if(bootloader_size_idx == 3) {
            bootloader_state = BOOTLOADER_RX;
            std::cout << "## INFO: Bootloader - receiving = " << bootloader_size << " bytes\n";
        }
        else ++bootloader_size_idx;
        break;
    case BOOTLOADER_RX:
        ++bootloader_cnt;
        tmp = (bootloader_cnt*100)/bootloader_size;
        if(((tmp % 25) == 0) && (tmp != tmp_last))
            std::cout << "## INFO: Bootloader - receiving " << tmp << " %\n";
        if(tmp == 100){
            bootloader_state = BOOTLOADER_END;
            std::cout << "## INFO: Bootloader - finished\n";
        }
        tmp_last = tmp;
        break;
    case BOOTLOADER_END:
    default:
        break;
    }
}

};

namespace Untimed {

void uart::read(unsigned int *data, unsigned int address, int size) {

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
    case SUART_TXLEVEL:
        *data = (unsigned int)ntohl((32 - tx_fifo.size()));
        break;
    case SUART_RXLEVEL:
        *data = (unsigned int)ntohl(rx_fifo.size());
        break;
    case SUART_RXCHAR:
        if(!rx_fifo.empty()){
            *data = (unsigned int)ntohl(rx_fifo.front());
            rx_fifo.pop();
            //std::cout << "uart::read():" << data->to_string(SC_HEX) << "\n";
            bootloader_check((char)*data);
        }
        else
            *data = 0;
        break;
    default:
        *data = 0;
        break;
    }
}

void uart::write(unsigned int data, unsigned int address, int size) {

    unsigned int addr = (address & 0x000000FF) >> 2;

    switch (addr) {
    case SUART_CLKDIV:
        //TODO
        break;
    case SUART_TXCHAR:
        tx_fifo.push((unsigned int)htonl(data));
        tx_fifo_add.notify();
        break;
    default:
        break;
    }
}

unsigned int uart::get_start_address() const{
    return start_address;
}

unsigned int uart::get_end_address() const{
    return end_address;
}

void uart::tx_thread(){
    while(true){
        Global::safe_finish(this, true);

        wait(tx_fifo_add);

        Global::safe_finish(this, false);

        while(!tx_fifo.empty()){
            unsigned int val = tx_fifo.front();
            tx_fifo.pop();

            //std::cout << std::setfill('0') << std::setw(8) << std::hex << data << "\n";

            //std::cout << (char) htonl(val);

            pty.terminal_write((char) htonl(val));
        }
    }

}

void uart::rx_thread(){
    while(true){

        if(bootloader_state==BOOTLOADER_END){
            std::cout << "## INFO: UART - bootloader end detected, 'rx_thread' process exiting\n";
            break;
        }

        if(pty.input_available()){
            char c = pty.terminal_read();
            rx_fifo.push(htonl(0x000000FF & (unsigned int)c));
        }
        Global::delay_cycles<0>(this, 1);
    }
}

void uart::rxtx_int(){
    tx_int = (32 - tx_fifo.size()) != 0;
    rx_int = !rx_fifo.empty();

    std::cout << "## INFO: UART - interrupts hard disabled, 'rxtx_int' process exiting\n";
}

void uart::bootloader_check(char c){
    unsigned int tmp = 0;
    static unsigned int tmp_last = 0;
    switch (bootloader_state) {
    case BOOTLOADER_SYNC_0:
        if(c == BOOTLOADER_ENABLE) bootloader_state = BOOTLOADER_SYNC_1;
        break;
    case BOOTLOADER_SYNC_1:
        if(c == BOOTLOADER_ENABLE) bootloader_state = BOOTLOADER_SIZE;
        break;
    case BOOTLOADER_SIZE:
        reinterpret_cast<unsigned char*>(&bootloader_size)[bootloader_size_idx] = (unsigned char)c;
        if(bootloader_size_idx == 3) {
            bootloader_state = BOOTLOADER_RX;
            std::cout << "## INFO: Bootloader - receiving = " << bootloader_size << " bytes\n";
        }
        else ++bootloader_size_idx;
        break;
    case BOOTLOADER_RX:
        ++bootloader_cnt;
        tmp = (bootloader_cnt*100)/bootloader_size;
        if(((tmp % 25) == 0) && (tmp != tmp_last))
            std::cout << "## INFO: Bootloader - receiving " << tmp << " %\n";
        if(tmp == 100){
            bootloader_state = BOOTLOADER_END;
            std::cout << "## INFO: Bootloader - finished\n";
        }
        tmp_last = tmp;
        break;
    case BOOTLOADER_END:
    default:
        break;
    }
}

};



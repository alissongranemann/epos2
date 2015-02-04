/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include "simple_uart.h"

#define ntohs(A) ( ((A)>>8) | (((A)&0xff)<<8) )
#define htons(A) ntohs(A)
#define ntohl(A) ( ((A)>>24) | (((A)&0xff0000)>>8) | (((A)&0xff00)<<8) | ((A)<<24) )
#define htonl(A) ntohl(A)

void simple_uart::read(unsigned int *data, unsigned int addr, int size) {

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
		}
		else
			*data = 0;
		break;
	default:
		*data = 0;
		break;
	}

}

void simple_uart::write(unsigned int data, unsigned int addr, int size) {

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

void simple_uart::tx_thread(){
	while(true){
		wait(tx_fifo_add);

		while(!tx_fifo.empty()){
			unsigned int val = tx_fifo.front();
			tx_fifo.pop();

			//std::cout << std::setfill('0') << std::setw(8) << std::hex << data << "\n";

			//std::cout << (char) htonl(val);

			pty.terminal_write((char) htonl(val));
		}
	}

}

void simple_uart::rx_thread(){
	while(true){
		if(pty.input_available()){
			char c = pty.terminal_read();
			rx_fifo.push(htonl(0x000000FF & (unsigned int)c));
		}
		wait(clk_i.posedge_event());
	}
}

void simple_uart::rxtx_int(){
	tx_int_o = (32 - tx_fifo.size()) != 0;
	rx_int_o = !rx_fifo.empty();
	wait();
	while(true){
		tx_int_o = (32 - tx_fifo.size()) != 0;
		rx_int_o = !rx_fifo.empty();
		wait();
	}
}

void simple_uart::rtl_proc(){
    ack_o = false;
    tx_o = false;
    baud_o = false;
    wait();
    while(true){
        if(cyc_i.read() && stb_i.read()){
            if(we_i.read()){
                write(dat_i.read().to_uint(), adr_i.read().to_uint(), 4);
            }
            else{
                unsigned int aux = 0;
                read(&aux, adr_i.read().to_uint(), 4);
                dat_o = aux;
            }
            ack_o = true;
            wait();
            ack_o = false;
        }
        wait();
    }
}

void simple_uart::print_time(){
    while(true){
        sc_time wait_time(100,SC_MS);
        wait(wait_time);

        std::cout << "## INFO: Simulation time: " << sc_time_stamp().to_seconds()
                  << "s | Real time: " << (get_real_time_ms() - real_time_start)/1000.0 << "s\n";
    }
}

SC_MODULE_EXPORT(simple_uart);



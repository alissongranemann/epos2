/*
 * proc_io_node.h
 *
 *  Created on: Jan 13, 2012
 *      Author: tiago
 */

#ifndef PROC_IO_NODE_H_
#define PROC_IO_NODE_H_

#include <systemc.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <arch/mips32/varch/mips32.h>
#include <arch/common/bus_if.h>
#include <mach/common/simple_uart/uart.h>
#include <mach/common/gpio/gpio.h>
#include <mach/common/simple_timer/timer.h>
#include <mach/common/simple_pic/pic.h>
#include <mach/common/rtsnoc_router/rtsnoc_bus_proxy.h>
#include <mach/common/comp_manager/comp_manager.h>
#include "memory.h"
#include "bus_ctrl.h"

template<
    unsigned int X,
	unsigned int Y,
	unsigned int NOC_PROXY_LOCAL_ADDR,
	unsigned int COMP_MANAGER_LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
SC_MODULE(cpu_io_node){

	enum{
		BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,

		PROC_NOC_BUS_RATE = 2,

		COMP_MANAGER_BUFFER_SIZE = 4
	};

	static const unsigned int MEM_ADDR_START	= 0x00000000;
	static const unsigned int MEM_ADDR_END		= 0x0FFFFFFF;
	static const unsigned int MEM_SIZE			= 0x10000000;

	static const unsigned int SRAM_ADDR_START	= 0x10000000;
	static const unsigned int SRAM_ADDR_END		= 0x1FFFFFFF;
	static const unsigned int SRAM_SIZE			= 0x10000000;

	static const unsigned int UART_ADDR_START	= 0x80000000;
	static const unsigned int UART_ADDR_STOP	= 0x800003FF;

	static const unsigned int GPIO_ADDR_START	= 0x80000400;
	static const unsigned int GPIO_ADDR_STOP	= 0x800007FF;

	static const unsigned int TIMER_ADDR_START	= 0x80000800;
	static const unsigned int TIMER_ADDR_STOP	= 0x80000BFF;

	static const unsigned int PIC_ADDR_START	= 0x80000C00;
	static const unsigned int PIC_ADDR_STOP		= 0x80000FFF;

	static const unsigned int NOC_ADDR_START	= 0x80001000;
	static const unsigned int NOC_ADDR_STOP		= 0x800013FF;

	static const unsigned int COMP_MANAGER_ADDR_START    = 0x80001400;
    static const unsigned int COMP_MANAGER_ADDR_STOP     = 0x800017FF;

	typedef rtsnoc_bus_proxy<X,Y,
							NOC_PROXY_LOCAL_ADDR,
							SIZE_DATA,SIZE_X,SIZE_Y,PROC_NOC_BUS_RATE> rtsnoc_bus_proxy_t;

	typedef comp_manager<X,Y,
                         COMP_MANAGER_LOCAL_ADDR,
                         SIZE_DATA,SIZE_X,SIZE_Y,COMP_MANAGER_BUFFER_SIZE> comp_manager_t;

	sc_in<bool> rst;

	sc_in<sc_uint<32> > gpio_in;
	sc_out<sc_uint<32> > gpio_out;
	sc_in<bool>	ext_int[8];

	sc_fifo_out<sc_biguint<BUS_SIZE> > noc_din;
	sc_fifo_in<sc_biguint<BUS_SIZE> > noc_dout;

	sc_fifo_out<sc_biguint<BUS_SIZE> > cmp_manager_din;
    sc_fifo_in<sc_biguint<BUS_SIZE> > cmp_manager_dout;

	mips32		cpu;
	bus_ctrl	bus;

	memory<memory_types::ON_CHIP>	    m_ram;
	memory<memory_types::EXTERNAL>      m_sram;
	uart		m_uart;
	gpio		m_gpio;
	timer		m_timer;
	pic			m_pic;
	rtsnoc_bus_proxy_t	m_noc_proxy;
	comp_manager_t m_comp_manager;

	sc_signal<bool> irq;

	sc_signal<bool> irqs[32];

	SC_CTOR(cpu_io_node)
		:cpu("cpu"),
		 bus("m_ctrl"),
		 m_ram("m_ram", MEM_ADDR_START, MEM_ADDR_END, MEM_SIZE),
		 m_sram("m_sram", SRAM_ADDR_START, SRAM_ADDR_END, SRAM_SIZE),
		 m_uart("m_uart", UART_ADDR_START, UART_ADDR_STOP),
		 m_gpio("m_gpio", GPIO_ADDR_START, GPIO_ADDR_STOP, "gpio_in.io", 0x0, "gpio_out.io", 0x0),
		 m_timer("m_timer", TIMER_ADDR_START, TIMER_ADDR_STOP),
		 m_pic("m_pic", PIC_ADDR_START, PIC_ADDR_STOP),
		 m_noc_proxy("m_noc_proxy", NOC_ADDR_START, NOC_ADDR_STOP),
		 m_comp_manager("m_comp_manager",COMP_MANAGER_ADDR_START,COMP_MANAGER_ADDR_STOP){

		cpu.rst(rst);
		cpu.irq(irq);
		cpu.bus(bus);

		bus.module_port(m_ram);
		bus.module_port(m_sram);
		bus.module_port(m_uart);
		bus.module_port(m_gpio);
		bus.module_port(m_timer);
		bus.module_port(m_pic);
		bus.module_port(m_noc_proxy);
		bus.module_port(m_comp_manager);

		m_uart.rst(rst);

		m_gpio.gambi_gpio_in(gpio_in);
		m_gpio.gambi_gpio_out(gpio_out);

		m_timer.rst(rst);

		m_pic.rst(rst);
		m_pic.irq_out(irq);
		for (int i = 0; i < 32; ++i) m_pic.irq_in[i](irqs[i]);

		m_timer.irq(irqs[0]);
		m_uart.rx_int(irqs[1]);
		m_uart.tx_int(irqs[2]);
		SC_METHOD(ext_int_p); for (int i = 0; i < 8; ++i) sensitive << ext_int[i];
		m_noc_proxy.irq(irqs[11]);
		m_comp_manager.irq(irqs[12]);
		for (int i = 13; i <= 31; ++i) irqs[i] = false;

		m_noc_proxy.rst(rst);
		m_noc_proxy.din(noc_din);
		m_noc_proxy.dout(noc_dout);

		m_comp_manager.rst(rst);
		m_comp_manager.din(cmp_manager_din);
        m_comp_manager.dout(cmp_manager_dout);

		std::cout << "## INFO: Elaborating node " << this->name() << "\n";
		std::cout << "\tLoading bootloader\n";
		bool boot_ok = load_bootloader();
		if(boot_ok) {
		    std::cout << "\tBootloader loaded\n";
		} else {
		    std::cout << "\tERROR: CAN'T LOAD BOOT LOADER" << std::endl;
		    sc_stop();
		}

	}

	void ext_int_p(){
		for (int i = 0; i < 8; ++i) irqs[i+3] = ext_int[i].read();
	}

	unsigned int StrToInt(const std::string& s, std::ios_base& (*f)(std::ios_base&)) {

		std::istringstream data(s);

		unsigned int t;
		data >> f >> t;
		return t;
	}

	unsigned long int get_tsc(){ return m_timer.get_tsc();}

private:
	bool load_bootloader(){
	    ifstream in_file ("../../../mach/common/coregen/ram_amba_128k.txt",
	                      ios::in);

	    if(in_file) {

	        std::cout << "Reading memory file \n";

	        std::string line;
	        unsigned int ptr;

	        ptr = 0;

	        while (getline(in_file, line)) {
	            //cout << "line: " << line;
	            unsigned int instruction = StrToInt(line, hex);
	            //cout << "instruction: " << hex << instruction << "\n";
	            m_ram.write_during_elaboration(instruction, ptr, 4);
	            //cout << hex << *(unsigned int*) ptr << "\n";
	            ptr += 4;
	        }

	        return true;

	    } else {
	        return false;
	    }

	}

};

#endif /* PROC_IO_NODE_H_ */

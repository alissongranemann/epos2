/*
 * mem.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include "mem.h"

/*
 * 	process
 */

void Mem::do_run() {

#ifdef cpu_debug1
    std::cout << "DEBUG - mem::do_run() - executando acesso a memoria\n";
#endif

    if(!cpu_status->skip && mem_data->is_valid)
        do_mem_access();

}

/*
 *  internal functions
 */

void Mem::do_mem_access() {

	if(mem_data->mem_r_w == 1)
		do_read();
	else
		do_write();
}

void Mem::do_write() {

	mem_ctrl->lock();
#ifdef dec_verbose
	if(mem_data->address == 0x10080308){
		ofstream outfile;
		outfile.open("mem_dump.txt",ios::app);
		outfile << "WB: " << std::setfill('0') << std::setw(8) << std::hex << mem_data->data << " PC: " << std::setw(8) << std::hex << pc_data->pc_fetch << "\n";
		outfile.close();
	}
#endif
	mem_ctrl->write(mem_data->data, mem_data->address, mem_data->size);
}

void Mem::do_read() {

	unsigned int data;

	mem_ctrl->lock();
	mem_ctrl->read(&data, mem_data->address, mem_data->size);
#ifdef dec_verbose
	if(mem_data->address == 0x10080308){
		ofstream outfile;
		outfile.open("mem_dump.txt",ios::app);
		outfile << "RB: " << std::setfill('0') << std::setw(8) << std::hex << data << " PC: " << std::setw(8) << std::hex << pc_data->pc_fetch << "\n";
		outfile.close();
	}
#endif
	wb_data->is_valid = true;

	switch(mem_data->size) {
	case 4:
		wb_data->result = data;
		break;
	case 2:
		if(mem_data->un_sig == 1)
			wb_data->result = (unsigned short)data;
		else
			wb_data->result = (signed short)data;
		break;
	case 1:
		if(mem_data->un_sig == 1)
			wb_data->result = (unsigned char)data;
		else
			wb_data->result = (signed char)data;
		break;
	}
	wb_data->reg_dest = mem_data->reg_dest;
	wb_data->un_sig = mem_data->un_sig;
}

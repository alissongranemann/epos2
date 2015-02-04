/*
 * fetch.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include "fetch.h"


/*
 *  process
 */

void Fetch::do_fetch() {

#ifdef cpu_debug1
    std::cout << "DEBUG - fetch::do_fetch() - executando fetch\n";
#endif
    mem_ctrl->lock();
    mem_ctrl->read(&pc_data->opcode, pc_data->pc_next, 4);
    pc_data->pc_current = pc_data->pc_next;
    pc_data->pc_current_on_ds = pc_data->pc_next_on_ds;

    cpu_status->skip = cpu_status->skip_next;
    cpu_status->skip_next = false;

    //std::cout << "OP_CODE: " << (void*)pc_data->opcode << " fetched from " << (void*)(pc_data->pc_fetch) << "\n";

    switch(pc_new_data->pc_source) {
    case FROM_INC4:
        pc_data->pc_next = pc_data->pc_next_plus4;
        break;
    case FROM_BRANCH:
    case FROM_LBRANCH:
        pc_data->pc_next = pc_new_data->pc_opcode;
        break;
    case FROM_EXCEPTION:
        pc_data->pc_next = 0x3c;
        break;
    case FROM_REG:
        pc_data->pc_next = pc_new_data->pc_new;
        break;
    default:
        pc_data->pc_next = pc_data->pc_next_plus4;
    }

    pc_data->pc_next &= ~3; // byte align
    pc_data->pc_next_plus4 = pc_data->pc_next + 4;

    pc_data->epc = pc_data->pc_next; //TODO check this
    //if(pc_next != pc + 4) {
    //	pc_data->epc |= 2; // branch delay slot
    //}

    //if(stop_count == 1300)
    //	return;
    //else
    //	stop_count++;
}

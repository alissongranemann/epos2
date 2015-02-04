/*
 * wb.cpp
 *
 *  Created on: 04/03/2010
 *      Author: tiago
 */

#include "wb.h"

//#define dec_verbose
//#define r_verbose


/*
 *  process
 */

void WB::do_wb() {

#ifdef cpu_debug1
    std::cout << "DEBUG - WB::do_wb() - executando write back\n";
#endif

    if(!cpu_status->skip && wb_data->is_valid) {
        if(wb_data->un_sig == 1)
            u[wb_data->reg_dest] = wb_data->result;
        else
            r[wb_data->reg_dest] = wb_data->result;
    }
}


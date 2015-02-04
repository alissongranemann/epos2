/*
 * Decoder.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include "plasma_pack.h"
#include "decoder.h"


/*
 * 	process
 */

void Decoder::do_decode() {

#ifdef cpu_debug1
    std::cout << "DEBUG - Decoder::do_decode() - executando decodificacao\n";
#endif


    dec_data->op = (pc_data->opcode >> 26) & 0x3f;
    dec_data->rs = (pc_data->opcode >> 21) & 0x1f;
    dec_data->rt = (pc_data->opcode >> 16) & 0x1f;
    dec_data->rd = (pc_data->opcode >> 11) & 0x1f;
    dec_data->re = (pc_data->opcode >> 6) & 0x1f;
    dec_data->func = (pc_data->opcode & 0x3f);
    dec_data->imm = pc_data->opcode & 0xffff;
    dec_data->imm_shift = ((long int)(short int)dec_data->imm << 2) - 4;
    dec_data->target = (pc_data->opcode << 6) >> 4;

    r[0] = 0;

#ifdef cpu_debug3
    std::cout << "DEBUG - Decoder::do_decode() - ";
    std::cout << "op=" << opcode_string[dec_data->op];
    if(dec_data->op == SPECIAL){
        std::cout << "/" << special_string[dec_data->func];
    }
    else if (dec_data->op == REGIMM){
        std::cout << "/" << regimm_string[dec_data->func];
    }
    std::cout << " rs=" << dec_data->rs;
    std::cout << " rt=" << dec_data->rt;
    std::cout << " rd=" << dec_data->rd;
    std::cout << " re=" << dec_data->re;
    std::cout << " imm=" << dec_data->imm;
    std::cout << " target=" << dec_data->target << "\n";


    std::cout << "DEBUG - Decoder::do_decode() - chamando exec->write()\n";
#endif

    exec->write(r[dec_data->rs], r[dec_data->rt]);

#ifdef cpu_dec_verbose
    show_dec();
#endif
}

void Decoder::show_dec() {

    ofstream outfile;
    outfile.open("dec_dump.txt",ios::app);

    /*
     *  show pc and opcode
     */

    outfile << "*" << std::setfill('0') << std::setw(8) << std::hex << pc_data->pc_current << " " << std::setw(8) << pc_data->opcode;

    /*
     *  show decoded data
     */

    outfile << std::setfill(' ') << std::setw(9);
    if(dec_data->op == 0) outfile << special_string[dec_data->func];
    else if (dec_data->op == 1) outfile << regimm_string[dec_data->rt];
    else outfile << opcode_string[dec_data->op];

    outfile << " " << std::dec << "$" << std::setfill('0') << std::setw(2) << dec_data->rs;
    outfile << " " << std::dec << "$" << std::setfill('0') << std::setw(2) << dec_data->rt;
    outfile << " " << std::dec << "$" << std::setfill('0') << std::setw(2) << dec_data->rd;
    outfile << " " << std::dec << "$" << std::setfill('0') << std::setw(2) << dec_data->re;
    outfile << " " << std::setw(4) << std::hex << dec_data->imm;

    outfile << " r[" << std::dec << std::setfill('0') << std::setw(2) << dec_data->rs << "]=" << std::setw(8) << std::hex << r[dec_data->rs];
    outfile << " r[" << std::dec << std::setfill('0') << std::setw(2) << dec_data->rt << "]=" << std::setw(8) << std::hex << r[dec_data->rt];

#ifdef cpu_dec_verbose
//  outfile << " | EXEC_PC = " << std::setfill('0') << std::setw(8) << std::hex << exec_gambi->pc_data->pc_fetch << " EXEC_STATUS: " << exec_gambi->irq.read() << "/" << exec_gambi->exceptionId;
#endif

    outfile << "\n";

    outfile.close();

}

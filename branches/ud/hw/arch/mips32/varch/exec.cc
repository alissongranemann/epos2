/*
 * exec.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include "plasma_pack.h"
#include "exec.h"

/*
 *  interfaces
 */

void Exec::write(int A_, int B_) {

#ifdef cpu_debug3
	std::cout << "DEBUG - Exec::write - recebendo dados do decoder\n";
#endif

	A = A_;
	B = B_;

	uB = (unsigned int) B;
	uA = (unsigned int) A;

#ifdef cpu_debug3
	std::cout << "DEBUG - Exec::write - dados copiados\n";
#endif
}

/*
 *  process
 */

int Exec::do_exec() {

#ifdef cpu_debug1
    std::cout << "DEBUG - Exec::do_exec() - executando operacao\n";
#endif

    int i_delay = 0;

    branch = false;

    pc_new_data->pc_source = FROM_INC4;
    pc_new_data->pc_opcode = dec_data->imm_shift + pc_data->pc_next_plus4;

    pc_data->pc_next_on_ds = false;

    mem_data->reset();
    wb_data->reset();

    if(!cpu_status->skip) {

        i_delay = do_operate();

        if(branch)
            pc_new_data->pc_source = FROM_BRANCH;

        if(exceptionId) {
            // restaura o valor de RT - necessário?
            wb_data->is_valid = true;
            wb_data->result = B;
            wb_data->reg_dest = dec_data->rt;


            pc_new_data->pc_source = FROM_EXCEPTION;
            cpu_status->skip_next = true;
            exceptionId = false;
        }
    }

    return i_delay;
}

/*
 *  internal functions
 */

int Exec::do_operate() {

    static bool skipLWR = false;

	bool donot_handle_irq_now = false;

	switch(dec_data->op) {
	case SPECIAL:
		switch(dec_data->func) {
		case SLL:
			wb_data->is_valid = true;
			wb_data->result = B << dec_data->re;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SRL:
			wb_data->is_valid = true;
			wb_data->result = uB >> dec_data->re;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SRA:
			wb_data->is_valid = true;
			wb_data->result = B >> dec_data->re;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SLLV:
			wb_data->is_valid = true;
			wb_data->result = B << A;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SRLV:
			wb_data->is_valid = true;
			wb_data->result = uB >> A;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SRAV:
			wb_data->is_valid = true;
			wb_data->result = B >> A;
			wb_data->reg_dest = dec_data->rd;
			break;
		case JR:
			//std::cout << "JR: " << std::setfill('0') << std::setw(8) << std::hex << A << "\n";
			pc_new_data->pc_new = A;
			pc_new_data->pc_source = FROM_REG;
			pc_data->pc_next_on_ds = true;
			//cpu_status->skip = 1;
			break;
		case JALR:
			wb_data->is_valid = true;
			wb_data->result = pc_data->pc_next_plus4;
			wb_data->reg_dest = dec_data->rd;
			pc_new_data->pc_new = A;
			pc_new_data->pc_source = FROM_REG;
			pc_data->pc_next_on_ds = true;
			//cpu_status->skip = 1;
			break;
		case MOVZ: // não consta nos opcodes
			if(!B) {
				wb_data->is_valid = true;
				wb_data->result = A;
				wb_data->reg_dest = dec_data->rd;
			}
			break;  /*IV*/
		case MOVN: // não consta nos opcodes
			if(B) {
				wb_data->is_valid = true;
				wb_data->result = A;
				wb_data->reg_dest = dec_data->rd;
			}
			break;  /*IV*/
		case SYSCALL:
			if(cpu_status->status & 1){
				cpu_status->epc = pc_data->epc;
				exceptionId = true;
			}
			break;
		case BREAK:
			if(cpu_status->status & 1){
			    cpu_status->epc = pc_data->epc;
				exceptionId = true;
			}
			break;
		case SYNC: break; // não consta nos opcodes
		case MFHI:
			wb_data->is_valid = true;
			wb_data->result = hi;
			wb_data->reg_dest = dec_data->rd;
			break;
		case MTHI:
			hi = A;
			break;
		case MFLO:
			wb_data->is_valid = true;
			wb_data->result = lo;
			wb_data->reg_dest = dec_data->rd;
			break;
		case MTLO:
			lo = A;
			break;
		case MULT:
			mult_big_signed(A, B);
			break;
		case MULTU:
			mult_big(A, B);
			break;
		case DIV:
			lo = A / B;
			hi = A % B;
			break;
		case DIVU:
			if(uB == 0){
			    std::cout << "Division by zero\n";
			    sc_stop();
			}else{
		    lo = uA / uB;
			hi = uA % uB;}
			break;
		case ADD:
			wb_data->is_valid = true;
			wb_data->result = A + B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case ADDU:
			wb_data->is_valid = true;
			wb_data->result = A + B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SUB:
			wb_data->is_valid = true;
			wb_data->result = A - B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SUBU: // não consta nos opcodes
			wb_data->is_valid = true;
			wb_data->result = A - B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case AND:
			wb_data->is_valid = true;
			wb_data->result = A & B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case OR:
			wb_data->is_valid = true;
			wb_data->result = A | B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case XOR:
			wb_data->is_valid = true;
			wb_data->result = A ^ B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case NOR:
			wb_data->is_valid = true;
			wb_data->result = ~(A | B);
			wb_data->reg_dest = dec_data->rd;
			break;
		case SLT:
			wb_data->is_valid = true;
			wb_data->result = A < B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SLTU: // não consta nos opcodes
			wb_data->is_valid = true;
			wb_data->result = uA < uB;
			wb_data->reg_dest = dec_data->rd;
			break;
		case DADDU: // não consta nos opcodes
			wb_data->is_valid = true;
			wb_data->result = A + uB;
			wb_data->reg_dest = dec_data->rd;
			break;
		case TGEU:	std::cout << "##ERROR - Instruction TGEU not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; sc_stop();
		case TLT:	std::cout << "##ERROR - Instruction TLT not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; sc_stop();
		case TLTU:  std::cout << "##ERROR - Instruction TLTU not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; sc_stop();
		case TEQ:	std::cout << "##ERROR - Instruction TEQ not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; sc_stop();
		case TNE:	std::cout << "##ERROR - Instruction TNE not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; sc_stop();
		default:	std::cout << "##ERROR - Unknown instruction opcode: " << dec_data->op << " func: " << dec_data->func << " pc: " << (void*)pc_data->opcode << " pc_addr: " << (void*)pc_data->pc_current << "\n"; sc_stop();
		}
		break;
	case REGIMM:
		switch(dec_data->rt) {
		case BLTZAL:
			wb_data->is_valid = true;
			wb_data->result = pc_data->pc_next_plus4;
			wb_data->reg_dest = 31;
		case BLTZ:
		    branch = A < 0;
		    pc_data->pc_next_on_ds = true;
		    break;
		case BGEZAL:
			wb_data->is_valid = true;
			wb_data->result = pc_data->pc_next_plus4;
			wb_data->reg_dest = 31;
		case BGEZ:
		    branch = A >= 0;
		    pc_data->pc_next_on_ds = true;
		    break;
		case BLTZALL:
			wb_data->is_valid = true;
			wb_data->result = pc_data->pc_next_plus4;
			wb_data->reg_dest = 31;
		case BLTZL:
		    branch = A < 0;
		    cpu_status->skip_next = !branch;
		    pc_data->pc_next_on_ds = true;
		    break;
		case BGEZALL:
			wb_data->is_valid = true;
			wb_data->result = pc_data->pc_next_plus4;
			wb_data->reg_dest = 31;
		case BGEZL:
		    branch = A >= 0;
		    cpu_status->skip_next = !branch;
		    pc_data->pc_next_on_ds = true;
		    break;
		default: 	std::cout << "##ERROR - Unknown instruction opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; sc_stop();
		}
		break;
	case JAL:
		wb_data->is_valid = true;
		wb_data->result = pc_data->pc_next_plus4;
		wb_data->reg_dest = 31;
	case J:
		pc_new_data->pc_new = ((pc_data->pc_next_plus4 - 4) & 0xf0000000) | dec_data->target;
		pc_new_data->pc_source = FROM_REG;
		pc_data->pc_next_on_ds = true;
		//cpu_status->skip = 1;
		break;
	case BEQ:
	    branch = A == B;
	    pc_data->pc_next_on_ds = true;
	    break;
	case BNE:
	    branch = A != B;
	    pc_data->pc_next_on_ds = true;
	    break;
	case BLEZ:
	    branch = A <= 0;
	    pc_data->pc_next_on_ds = true;
	    break;
	case BGTZ:
	    branch = A > 0;
	    pc_data->pc_next_on_ds = true;
	    break;
	case ADDI:
		wb_data->is_valid = true;
		wb_data->result = A + (short)dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case ADDIU:
		wb_data->is_valid = true;
		wb_data->result = uA + (short)dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		wb_data->un_sig = 1;
		break;
	case SLTI:
		wb_data->is_valid = true;
		wb_data->result = A < (short)dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case SLTIU:
		wb_data->is_valid = true;
		wb_data->result = uA < (unsigned int)(short)dec_data->imm;
		wb_data->un_sig = 1;
		wb_data->reg_dest = dec_data->rt;
		break;
	case ANDI:
		wb_data->is_valid = true;
		wb_data->result = A & dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case ORI:
		wb_data->is_valid = true;
		wb_data->result = A | dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case XORI:
		wb_data->is_valid = true;
		wb_data->result = A ^ dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case LUI:
		wb_data->is_valid = true;
		wb_data->result = (dec_data->imm << 16);
		wb_data->reg_dest = dec_data->rt;
		break;
	case COP0:
		if((pc_data->opcode & (1 << 23)) == 0) {// move from CP0
			wb_data->is_valid = true;
			if(dec_data->rd == 12){
				wb_data->result = cpu_status->status; //std::cout << "##INFO move from CP0 - status = " << status << "\n";
			}
			else if(dec_data->rd == 13){
				std::cout << "##ERROR move from CP0 - cause reg not implemented " << dec_data->rd << "\n"; sc_stop();
			}
			else if(dec_data->rd == 14)
				wb_data->result = cpu_status->epc;
			else{
				std::cout << "##ERROR move from CP0 - unknown cp0 reg " << dec_data->rd << "\n"; sc_stop();
			}
			wb_data->reg_dest = dec_data->rt;
		} else {
			if(dec_data->rd == 12){
			    cpu_status->status = B; //std::cout << "##INFO move to CP0 - status = " << status << "\n";
				if(cpu_status->processId && (B & 2)) {
					cpu_status->userMode |= B & 2;
				}
			}
			else if(dec_data->rd == 13){
				std::cout << "##ERROR move to CP0 - cause reg not implemented " << dec_data->rd << "\n"; sc_stop();
			}
			else if(dec_data->rd == 14)
				cpu_status->epc = B;
			else{
				std::cout << "##ERROR move to CP0 - unknown cp0 reg " << dec_data->rd << "\n"; sc_stop();
			}

		}
		donot_handle_irq_now = true;
		break;
	case BEQL:
	    branch = A == B;
	    cpu_status->skip_next = !branch;
	    pc_data->pc_next_on_ds = true;
	    break;
	case BNEL:
	    branch = A != B;
	    cpu_status->skip_next = !branch;
	    pc_data->pc_next_on_ds = true;
	    break;
	case BLEZL:
	    branch = A <= 0;
	    cpu_status->skip_next = !branch;
	    pc_data->pc_next_on_ds = true;
	    break;
	case BGTZL:
	    branch = A > 0;
	    cpu_status->skip_next = !branch;
	    pc_data->pc_next_on_ds = true;
	    break;
	case LB:
		mem_data->is_valid = true;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 1;
		mem_data->reg_dest = dec_data->rt;
		break;
	case LH:
		mem_data->is_valid = true;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 2;
		mem_data->reg_dest = dec_data->rt;
		break;
	case LWL:	//break;
	case LW:
		mem_data->is_valid = true;
		mem_data->address = (short)dec_data->imm + A;
		//std::cout << "Instrução LW - endereço para leitura:" << std::setfill(' ') << std::setw(8) << std::hex << mem_data->address << "\n";
		mem_data->size = 4;
		mem_data->reg_dest = dec_data->rt;
		break;
	case LBU: // não consta nos opcodes
		mem_data->is_valid = true;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 1;
		mem_data->reg_dest = dec_data->rt;
		mem_data->un_sig = 1;
		break;
	case LHU: // não consta nos opcodes
		mem_data->is_valid = true;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 2;
		mem_data->reg_dest = dec_data->rt;
		mem_data->un_sig = 1;
		break;
	case LWR:
	    if(!skipLWR){
	        std::cout << "##WARNING - Skipping instruction LWR which is not implemented, pc: " << (void*)pc_data->pc_current << ", opcode: " << dec_data->op << ", func: " << dec_data->func << "\n";
	        std::cout << "          - next LWR will be skipped without notification\n";
	        skipLWR = true;
	    }
	    break;
	case SB:
		mem_data->is_valid  = true;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 1;
		mem_data->mem_r_w = 0;
		break;
	case SH:
		mem_data->is_valid = true;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 2;
		mem_data->mem_r_w = 0;
		break;
	case SWL:	//break;
	case SW:
		mem_data->is_valid = true;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 4;
		mem_data->mem_r_w = 0;
		break;
	case SWR:	std::cout << "##ERROR - Instruction SWR not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; sc_stop();
	case CACHE: break; // não consta nos opcodes
	case LL: // não consta nos opcodes
		mem_data->is_valid = true;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 4;
		mem_data->reg_dest = dec_data->rt;
		break;
	case SC: // não consta nos opcodes
		mem_data->is_valid = true;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 4;
		mem_data->mem_r_w = 0;
		wb_data->is_valid = true;
		wb_data->reg_dest = dec_data->rt;
		wb_data->result = 1;
		break;
	default:	std::cout << "##ERROR - Unknown instruction opcode: " << dec_data->op << " func: " << dec_data->func << " pc: " << (void*)pc_data->opcode << " pc_addr: " << (void*)pc_data->pc_current << "\n"; sc_stop();
	}

//	donot_handle_irq_now = donot_handle_irq_now || branch;

	if(irq.read() && !donot_handle_irq_now && !(pc_data->pc_next_on_ds) && !(pc_data->pc_current_on_ds)){
		if(cpu_status->status & 1){
		    cpu_status->epc = pc_data->epc;
			exceptionId = true;
			//std::cout << "##INFO: interrupt triggered\n";
			cpu_status->status = cpu_status->status & ~1;
		}
		//else{
		//	std::cout << "##INFO: interrupt triggered and ignored\n";
		//}
	}

	return get_delay(dec_data->op, dec_data->func, dec_data->rt);
}

void Exec::mult_big(int a, int b) {

	unsigned int ahi, alo, bhi, blo;
	unsigned int c0, c1, c2;
	unsigned int c1_a, c1_b;

	ahi = (unsigned int)a >> 16;
	alo = a & 0xffff;
	bhi = (unsigned int)b >> 16;
	blo = b & 0xffff;

	c0 = alo * blo;
	c1_a = ahi * blo;
	c1_b = alo * bhi;
	c2 = ahi * bhi;

	c2 += (c1_a >> 16) + (c1_b >> 16);
	c1 = (c1_a & 0xffff) + (c1_b & 0xffff) + (c0 >> 16);
	c2 += (c1 >> 16);
	c0 = (c1 << 16) + (c0 & 0xffff);

	hi = c2;
	lo = c0;
}

void Exec::mult_big_signed(int a, int b) {

	unsigned int ahi, alo, bhi, blo;
	unsigned int c0, c1, c2;
	unsigned int c1_a, c1_b;

	ahi = a >> 16;
	alo = a & 0xffff;
	bhi = b >> 16;
	blo = b & 0xffff;

	c0 = alo * blo;
	c1_a = ahi * blo;
	c1_b = alo * bhi;
	c2 = ahi * bhi;

	c2 += (c1_a >> 16) + (c1_b >> 16);
	c1 = (c1_a & 0xffff) + (c1_b & 0xffff) + (c0 >> 16);
	c2 += (c1 >> 16);
	c0 = (c1 << 16) + (c0 & 0xffff);
	hi = c2;
	lo = c0;
}

int Exec::get_delay(unsigned int op, unsigned int func, unsigned int rt)
{
    switch(op) {
    case SPECIAL:
        switch(func) {
        case MULT:
            return 30;
        case MULTU:
            return 30;
        case DIV:
            return 27;
        case DIVU:
            return 27;
        case SLL:
        case SRL:
        case SRA:
        case SLLV:
        case SRLV:
        case SRAV:
        case JR:
        case JALR:
        case MOVZ:
        case MOVN:
        case SYSCALL:
        case BREAK:
        case SYNC:
        case MFHI:
        case MTHI:
        case MFLO:
        case MTLO:
        case ADD:
        case ADDU:
        case SUB:
        case SUBU:
        case AND:
        case OR:
        case XOR:
        case NOR:
        case SLT:
        case SLTU:
        case DADDU:
        case TGEU:
        case TLT:
        case TLTU:
        case TEQ:
        case TNE:
        default:
            return 0;
        }
    break;
    case REGIMM:
        switch(rt) {
        case BLTZAL:
        case BLTZ:
        case BGEZAL:
        case BGEZ:
        case BLTZALL:
        case BLTZL:
        case BGEZALL:
        case BGEZL:
        default:
            return 0;
        }
    break;
    case JAL:
    case J:
    case BEQ:
    case BNE:
    case BLEZ:
    case BGTZ:
    case ADDI:
    case ADDIU:
    case SLTI:
    case SLTIU:
    case ANDI:
    case ORI:
    case XORI:
    case LUI:
    case COP0:
    case BEQL:
    case BNEL:
    case BLEZL:
    case BGTZL:
    case LB:
    case LH:
    case LWL:
    case LW:
    case LBU:
    case LHU:
    case LWR:
    case SB:
    case SH:
    case SWL:
    case SW:
    case SWR:
    case CACHE:
    case LL:
    case SC:
    default:
        return 0;
    }

}

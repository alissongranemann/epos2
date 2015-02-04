/*
 * plasma_pack.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef PLASMA_PACK_H_
#define PLASMA_PACK_H_

#include <string>
#include <iostream>
#include <iomanip>
#include <common/debugs.h>

#define ntohs(A) ( ((A)>>8) | (((A)&0xff)<<8) )
#define htons(A) ntohs(A)
#define ntohl(A) ( ((A)>>24) | (((A)&0xff0000)>>8) | (((A)&0xff00)<<8) | ((A)<<24) )
#define htonl(A) ntohl(A)

enum OPCODE {
	SPECIAL = 0x00, REGIMM = 0x01, J = 0x02, JAL = 0x03, BEQ = 0x04, BNE = 0x05,
	BLEZ = 0x06, BGTZ = 0x07, ADDI = 0x08, ADDIU = 0x09, SLTI = 0x0a,
	SLTIU = 0x0b, ANDI = 0x0c, ORI = 0x0d, XORI = 0x0e, LUI = 0x0f, COP0 = 0x10,
	BEQL = 0x14, BNEL = 0x15, BLEZL = 0x16, BGTZL = 0x17, LB = 0x20,
	LH = 0x21, LWL = 0x22, LW = 0x23, LBU = 0x24, LHU = 0x25, LWR = 0x26,
	SB = 0x28, SH = 0x29, SWL = 0x2a, SW = 0x2b, SWR = 0x2e, CACHE = 0x2f,
	LL = 0x30, SC = 0x38
};

enum FUNC {
	SLL = 0x00, SRL = 0x02, SRA = 0x03, SLLV = 0x04, SRLV = 0x06, SRAV = 0x07,
	JR = 0x08, JALR = 0x09, MOVZ = 0x0a, MOVN = 0x0b, SYSCALL = 0x0c, BREAK = 0x0d,
	SYNC = 0x0f, MFHI = 0x10, MTHI = 0x11, MFLO = 0x12, MTLO = 0x13, MULT = 0x18,
	MULTU = 0x19, DIV = 0x1a, DIVU = 0x1b, ADD = 0x20, ADDU = 0x21, SUB = 0x22,
	SUBU = 0x23, AND = 0x24, OR = 0x25, XOR = 0x26, NOR = 0x27, SLT = 0x2a,
	SLTU = 0x2b, DADDU = 0x2d, TGEU = 0x31, TLT = 0x32, TLTU = 0x33, TEQ = 0x34, TNE = 0x36

};

enum REGIMM {
	BLTZAL = 0x10, BLTZ = 0x00, BGEZAL = 0x11, BGEZ = 0x01,
	BLTZALL = 0x12, BLTZL = 0x02, BGEZALL = 0x13, BGEZL = 0x03,
};

static std::string opcode_string[] = {
	"SPECIAL", "REGIMM", "J", "JAL", "BEQ", "BNE", "BLEZ", "BGTZ",
	"ADDI", "ADDIU", "SLTI", "SLTIU", "ANDI", "ORI", "XORI", "LUI",
	"COP0", "COP1", "COP2", "COP3", "BEQL", "BNEL", "BLEZL", "BGTZL",
	"?", "?", "?", "?", "?", "?", "?", "?",
	"LB", "LH", "LWL", "LW", "LBU", "LHU", "LWR", "?",
	"SB", "SH", "SWL", "SW", "?", "?", "SWR", "CACHE",
	"LL", "LWC1", "LWC2", "LWC3", "?", "LDC1", "LDC2", "LDC3",
	"SC", "SWC1", "SWC2", "SWC3", "?", "SDC1", "SDC2", "SDC3",
};

static std::string special_string[] = {
	"SLL", "?", "SRL", "SRA", "SLLV", "?", "SRLV", "SRAV",
	"JR", "JALR", "MOVZ", "MOVN", "SYSCALL", "BREAK", "?", "SYNC",
	"MFHI", "MTHI", "MFLO", "MTLO", "?", "?", "?", "?",
	"MULT", "MULTU", "DIV", "DIVU", "?", "?", "?", "?",
	"ADD", "ADDU", "SUB", "SUBU", "AND", "OR", "XOR", "NOR",
	"?", "?", "SLT", "SLTU", "?", "DADDU", "?", "?",
	"TGE", "TGEU", "TLT", "TLTU", "TEQ", "?", "TNE", "?",
	"?", "?", "?", "?", "?", "?", "?", "?"
};

static std::string regimm_string[] = {
	"BLTZ","BGEZ","BLTZL","BGEZL","?","?","?","?",
	"TGEI","TGEIU","TLTI","TLTIU","TEQI","?","TNEI","?",
	"BLTZAL","BEGZAL","BLTZALL","BGEZALL","?","?","?","?",
	"?","?","?","?","?","?","?","?"
};

enum pc_source_type {
	FROM_INC4, FROM_BRANCH, FROM_LBRANCH,
	FROM_EXCEPTION, FROM_REG
};

struct pc_info {
	unsigned int	opcode;
	unsigned int	epc;
	int				pc_current;
	bool            pc_current_on_ds;
	int             pc_next;
	bool            pc_next_on_ds;
	int             pc_next_plus4;
};

struct pc_new_info {
	int				pc_new;
	int				pc_opcode;
	pc_source_type	pc_source;
};

struct dec_info {
	unsigned int	op, rs, rt, rd, re, func, imm, target;
	int				imm_shift;
};

struct mem_info {
	int				data;		// for write only
	unsigned int	address;	// for read/write
	int				size;		// for read/write
	unsigned int	reg_dest;	// for read only
	int				un_sig;		// for read/write
	int				mem_r_w;	// if 1 => read, if 0 => write
	bool            is_valid;

	mem_info() {
		reset();
	}

	void reset(){
	    un_sig = 0;
	    mem_r_w = 1;
	    is_valid = false;
	}
};

struct wb_info {
	int 			result;
	unsigned int 	reg_dest;
	int 			un_sig;
	bool            is_valid;

	wb_info() {
		reset();
	}

	void reset(){
	   un_sig = 0;
	   is_valid = false;
	}

};

struct cpu_info {
	int 			processId;
	int 			userMode;
	unsigned int 	epc;
	unsigned int    status;
	bool     		skip;
	bool            skip_next;

	cpu_info() {
		processId = 0;
		userMode = 0;
		epc = 0;
		status = 0;
		skip = false;
		skip_next = false;
	}
};

#endif /* PLASMA_PACK_H_ */

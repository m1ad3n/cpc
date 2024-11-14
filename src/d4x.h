#ifndef D4X_HEADER
#define D4X_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "d4x_mem.h"

#define REG_COUNT 6

typedef struct {
	/**
	 * program counter
	 * stack pointer
	 */
	u16 PC;
	u16 SP;
	
	/**
	 * instruction reg
	 * program status word
	 * accumulator
	 */
	u8  IR;
	u8  PSW;
	u8  AC;

	/* general purpose regs */
	u8  regs[REG_COUNT];

	/* cpu memory */
	u8  *mem;
} D4X;

enum GPR {
	REG_B = 0,
	REG_C,
	REG_D,
	REG_E,
	REG_H,
	REG_L,
};

enum PSW_FLAGS {
	P_ZF = 1,
	P_SF = 2,
	P_CF = 4,
	P_OF = 8,
	P_EF = 16,
	P_GF = 32,
	P_LF = 64,
};

enum CPU_INSTRUCTION {
	IR_NOP = 0,
	IR_MOV,
	IR_ADD,
	IR_SUB,
	IR_INC,
	IR_DEC,
	IR_CMP,
	IR_AND,
	IR_OR,
	IR_XOR,
	IR_PUSH,
	IR_POP,
	IR_JMP,
	IR_JE,
	IR_JZ,
	IR_JNE,
	IR_JNZ,
	IR_JGE,
	IR_JG,
	IR_JLE,
	IR_JL,

	IR_OP1_REG = 64,
	IR_OP2_REG = 128,
};

D4X* cpu_create(memory_t mem) {
	D4X* ncpu = (D4X*)malloc(sizeof(D4X));
	ncpu->mem = mem;
	return ncpu;
}

void cpu_reset(D4X* cpu) {
	cpu->PC = 0x1000;
	cpu->SP = 0x00FF;
	cpu->AC = 0;
	cpu->PSW = 0;
	cpu->IR = 0;

	for (u8 i = 0; i < REG_COUNT; i++) {
		cpu->regs[i] = 0;
	}
}

void cpu_panic(D4X* cpu, const char* panic_msg) {
	printf("=== cpu panicked\n");
	printf("=== %s\n", panic_msg);
	free(cpu);
	exit(1);
}

void cpu_fetch_ir(D4X* cpu) {
	cpu->IR = cpu->mem[cpu->PC++];
}

void LD_set_flags(D4X* cpu, u8* reg) {
	cpu->PSW |= (*reg == 0) ? P_ZF : 0;
	cpu->PSW |= (*reg & 0x80) ? P_SF : 0;
}

void CMP_set_flags(D4X* cpu, u8 reg, u8 value) {
	cpu->PSW = 0;

	if (reg == value) cpu->PSW |= P_EF;
	else cpu->PSW &= ~(P_EF);

	if (reg > value) cpu->PSW |= P_GF;
	else cpu->PSW &= ~(P_GF);

	if (reg < value) cpu->PSW |= P_LF;
	else cpu->PSW &= ~(P_LF);
}

void cpu_exec_ir(D4X* cpu) {

	u16 op1_addr;
	u8 op1_offset = 0;
	u8 op2_offset = 0;
	u8* op1;
	u8* op2;

	if (cpu->IR & IR_OP1_REG) {
		op1 = &cpu->regs[cpu->mem[cpu->PC]];
		op1_offset += 1;
	}
	else {
		op1_addr = mem_fetch_word(cpu->mem, cpu->PC);
		op1 = &cpu->mem[op1_addr];
		op1_offset += 2;
	}
	
	if (cpu->IR & IR_OP2_REG) {
		op2 = &cpu->regs[cpu->mem[cpu->PC + op1_offset]];
		op2_offset += 1;
	}
	else {
		op2 = &cpu->mem[mem_fetch_word(cpu->mem, cpu->PC + op1_offset)];
		op2_offset += 2;
	}

	switch (cpu->IR & 0x1F) {
	case IR_NOP:
		return;

	case IR_MOV:
		*op1 = *op2;
		cpu->PC += op1_offset + op2_offset;
		break;

	case IR_ADD:
		*op1 += *op2;
		cpu->PC += op1_offset + op2_offset;
		break;

	case IR_SUB:
		*op1 -= *op2;
		cpu->PC += op1_offset + op2_offset;
		break;

	case IR_INC:
		*op1 += 1;
		cpu->PC += op1_offset;
		break;

	case IR_DEC:
		*op1 -= 1;
		cpu->PC += op1_offset;
		break;

	case IR_AND:
		*op1 &= *op2;
		cpu->PC += op1_offset + op2_offset;
		break;

	case IR_OR:
		*op1 |= *op2;
		cpu->PC += op1_offset + op2_offset;
		break;

	case IR_XOR:
		*op1 ^= *op2;
		cpu->PC += op1_offset + op2_offset;
		break;

	case IR_PUSH:
		if (cpu->SP == STACK_START) cpu_panic(cpu, "stack overflow");
		cpu->mem[cpu->SP--] = *op1;
		cpu->PC += op1_offset;
		break;

	case IR_POP:
		if (cpu->SP == 0xFF) cpu_panic(cpu, "stack underflow");
		*op1 = cpu->mem[++cpu->SP];
		cpu->PC += op1_offset;
		break;

	case IR_CMP:
		CMP_set_flags(cpu, *op1, *op2);
		cpu->PC += op1_offset + op2_offset;
		break;

	case IR_JMP:
		cpu->PC = op1_addr;
		break;

	case IR_JE:
	case IR_JZ:
		if (cpu->PSW & P_EF)
			cpu->PC = op1_addr;
		else
			cpu->PC += op1_offset;
		break;

	case IR_JNE:
	case IR_JNZ:
		if (!(cpu->PSW & P_EF))
			cpu->PC = op1_addr;
		else
			cpu->PC += op1_offset;
		break;

	case IR_JLE:
		if ((cpu->PSW & P_EF) || (cpu->PSW & P_LF))
			cpu->PC = op1_addr;
		else
			cpu->PC += op1_offset;
		break;

	case IR_JL:
		if (cpu->PSW & P_LF)
			cpu->PC = op1_addr;
		else
			cpu->PC += op1_offset;
		break;

	case IR_JGE:
		if ((cpu->PSW & P_EF) || (cpu->PSW & P_GF))
			cpu->PC = op1_addr;
		else
			cpu->PC += op1_offset;
		break;

	case IR_JG:
		if (cpu->PSW & P_GF)
			cpu->PC = op1_addr;
		else
			cpu->PC += op1_offset;
		break;

	}
}

void cpu_run(D4X* cpu) {
	while (cpu->PC < VIDEO_START) {
		cpu_fetch_ir(cpu);
		cpu_exec_ir(cpu);
	}
}

void cpu_destroy(D4X* cpu) {
	free(cpu);
}

#endif // D4X_HEADER

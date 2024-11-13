#ifndef CPU_HEADER
#define CPU_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mem.h"

#define ERROR(msg) fprintf(stderr, msg)
#define ERR_FMT(msg, ...) fprintf(stderr, msg, __VA_ARGS__)
#define REGINFO(reg, value, should) fprintf(stdout, "%s is 0x%hX\t%s\n", #reg, value, (should) ? should : "")

typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
	u16 PC; // program counter
	u16 SP; // stack pointer

	u8 IR;  // instruction register
	u8 A;   // ACCUMULATOR
	u8 X;   // index register
	u8 Y;   // index register

	u8 PSW; // status flags

	u8  *mem;
} cpu_t;

typedef enum {
	PSW_C = 1,  // carry
	PSW_Z = 2,  // zero
	PSW_I = 4,  // interrupt
	PSW_D = 8,  // decimal
	PSW_B = 16,  // break
	PSW_O = 32,  // overflow
	PSW_N = 64,  // negative
} PSW_FLAGS;

typedef enum {
	IR_NOP = 0x0,
	IR_LDA_IM = 0xA9,
	IR_LDA_ZP = 0xA5,
	IR_LDX_IM = 0xA2,
	IR_LDX_ZP = 0xA6,
	IR_LDY_IM = 0xA0,
	IR_LDY_ZP = 0xA4,
	IR_JSR_ABS = 0x4C,
	IR_INX = 0xE8,
	IR_INY = 0xC8,
	IR_DECX = 0xCA,
	IR_DECY = 0x88,
	IR_CMP_IM = 0xC9,	// compare two addresses
	IR_CMPX_IM = 0xE0,	// compare reg x with a value
	IR_CMPY_IM = 0xC0,	// compare reg y with a value
	IR_BCC = 0x90,		// if carry is clear
	IR_BCS = 0xB0,		// if carry is set
	IR_BVS = 0x70,		// if overflow is set
	IR_PHA = 0x48,		// push accumulator to the stack
	IR_PLA = 0x68		// pops the value from stack into accumulator
} CPU_INSTRUCTION_SET;

cpu_t* cpu_create(memory_t mem) {
	cpu_t* ncpu = (cpu_t*)malloc(sizeof(cpu_t));
	ncpu->mem = mem;
	return ncpu;
}

void cpu_reset(cpu_t* cpu) {
	cpu->PC = mem_fetch_word(cpu->mem, CPU_RESET_MEM);
	cpu->SP = 0x01FF;
	cpu->A = cpu->X = cpu->Y = 0;
	cpu->PSW = 0;
	cpu->IR = 0;
}

void cpu_panic(cpu_t* cpu, const char* panic_msg) {
	ERROR("=== CPU PANICKED ===\n");
	ERR_FMT("=== %s ===\n", panic_msg);
	REGINFO(PC, cpu->PC, NULL);
	REGINFO(SP, cpu->SP, "[0x100 - 0x1FF]");
	free(cpu);
	exit(1);
}

void cpu_fetch_ir(cpu_t* cpu) {
	cpu->IR = cpu->mem[cpu->PC++];
}

void cpu_write_mem(cpu_t* cpu, u16 offset, u8 value) {
	cpu->mem[offset] = value;
}

void LD_set_flags(cpu_t* cpu, u8* reg) {
	cpu->PSW |= (*reg == 0) ? PSW_Z : 0;
	cpu->PSW |= (*reg & 0x80) ? PSW_N : 0;
}

void CMP_set_flags(cpu_t* cpu, u8 reg, u8 value) {
	if (reg == value) cpu->PSW |= PSW_Z;
	else cpu->PSW &= ~(PSW_Z);

	if (reg > value) cpu->PSW |= PSW_C;
	else cpu->PSW &= ~(PSW_C);

	if (reg < value) cpu->PSW |= PSW_O;
	else cpu->PSW &= ~(PSW_O);
}

void cpu_exec_ir(cpu_t* cpu) {
	u16 addr;
	switch (cpu->IR) {
	case IR_LDA_IM:
		cpu->A = cpu->mem[cpu->PC++];
		LD_set_flags(cpu, &cpu->A);
		break;

	case IR_LDA_ZP:
		addr = cpu->mem[cpu->PC++];
		cpu->A = cpu->mem[addr % (CPU_ZERO_PAGE_END + 1)];
		LD_set_flags(cpu, &cpu->A);
		break;

	case IR_LDX_IM:
		cpu->X = cpu->mem[cpu->PC++];
		LD_set_flags(cpu, &cpu->X);
		break;

	case IR_LDX_ZP:
		addr = cpu->mem[cpu->PC++];
		cpu->X = cpu->mem[addr % (CPU_ZERO_PAGE_END + 1)];
		LD_set_flags(cpu, &cpu->X);
		break;

	case IR_LDY_IM:
		cpu->X = cpu->mem[cpu->PC++];
		LD_set_flags(cpu, &cpu->X);
		break;

	case IR_LDY_ZP:
		addr = cpu->mem[cpu->PC++];
		cpu->Y = cpu->mem[addr % (CPU_ZERO_PAGE_END + 1)];
		LD_set_flags(cpu, &cpu->Y);
		break;

	case IR_JSR_ABS:
		cpu->PC = mem_fetch_word(cpu->mem, cpu->PC);
		break;

	case IR_INX:
		cpu->X++;
		LD_set_flags(cpu, &cpu->X);
		break;

	case IR_INY:
		cpu->Y++;
		LD_set_flags(cpu, &cpu->Y);
		break;

	case IR_DECX:
		cpu->X--;
		LD_set_flags(cpu, &cpu->X);
		break;

	case IR_DECY:
		cpu->Y--;
		LD_set_flags(cpu, &cpu->Y);
		break;

	case IR_CMP_IM:
		CMP_set_flags(cpu, cpu->mem[cpu->mem[cpu->PC + 1]], cpu->mem[cpu->mem[cpu->PC + 2]]);
		cpu->PC += 2;
		break;

	case IR_CMPX_IM:
		CMP_set_flags(cpu, cpu->X, cpu->mem[cpu->PC++]);
		break;

	case IR_CMPY_IM:
		CMP_set_flags(cpu, cpu->Y, cpu->mem[cpu->PC++]);
		break;

	case IR_BCC:
		if (!(cpu->PSW & PSW_C))
			cpu->PC = mem_fetch_word(cpu->mem, cpu->PC);
		break;

	case IR_BCS:
		if (cpu->PSW & PSW_C)
			cpu->PC = mem_fetch_word(cpu->mem, cpu->PC);
		break;

	case IR_BVS:
		if (cpu->PSW & PSW_O)
			cpu->PC = mem_fetch_word(cpu->mem, cpu->PC);
		break;

	case IR_PHA:
		if (cpu->SP == CPU_STACK_START) cpu_panic(cpu, "STACK OVERFLOW");
		cpu->mem[cpu->SP--] = cpu->A;
		break;

	case IR_PLA:
		if (++cpu->SP > (CPU_STACK_START + 0xFF)) cpu_panic(cpu, "STACK UNDERFLOW");
		cpu->A = cpu->mem[cpu->SP];
		break;
	}
}

void cpu_run(cpu_t* cpu) {
	while (cpu->PC != MEMORY_CAP) {
		cpu_fetch_ir(cpu);
		cpu_exec_ir(cpu);
	}
}

void cpu_destroy(cpu_t* cpu) {
	free(cpu);
}

#endif // CPU_HEADER

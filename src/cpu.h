#ifndef CPU_HEADER
#define CPU_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEBUG(msg, ...) fprintf(stdout, msg, __VA_ARGS__)
#define ERROR(msg, ...) fprintf(stderr, msg, __VA_ARGS__)

#define CPU_WIDTH unsigned char
#define MEMORY_CAP 255
#define REG_COUNT 10

typedef CPU_WIDTH memory_t[MEMORY_CAP];
typedef CPU_WIDTH cregister;

typedef struct {
	cregister* PC;
	cregister reg[10];
	CPU_WIDTH* mem;
} cpu_t;

typedef enum {
	R_MAR,
	R_MDR,
	R_IR,
	R_AC,
	R_PSW,
	R_A,
	R_B,
} CPU_REGISTERS;

typedef enum {
	PSW_N = 1,  // is result negative
	PSW_Z = 2,  // is result zero
	PSW_C = 4,  // carry
	PSW_V = 8,  // overflow 
} PSW_FLAGS;

typedef enum {
	I_NOP, 	// do nothing
	I_INC, 	// ++
	I_DEC, 	// --
	
	I_ADD, 	// +
	I_SUB, 	// -
	
	I_SHL, 	// shift left
	I_SHR, 	// shift right

	I_AND, 	// &
	I_OR,  	// |
	I_XOR, 	// ^
	I_CMP, 	// compare

	I_PUSH,	// push value or addres to the stack
	I_POP,	// pop the value from the stack

	I_MOV,
	I_JMP,

} CPU_INSTRUCTION_SET;

typedef enum {
	IR_NO_OP = 32,		// [OC]
	IR_ONE_OP = 64,		// [OC] OP
	IR_OP2_ADDR = 128,	// [OC] OP (OP)(ADDRESS)
				// false = second operand is literal
} IR_FLAGS;

cpu_t* cpu_create(memory_t mem) {
	cpu_t* ncpu = (cpu_t*)malloc(sizeof(cpu_t));
	ncpu->mem = mem;
	for (uint8_t i = 0; i < REG_COUNT; i++)
		ncpu->reg[i] = 0;
	ncpu->PC = &ncpu->reg[0];
	return ncpu;
}

void cpu_fetch_ir(cpu_t* cpu) {
	// 8 bit mode
	cpu->reg[R_IR] = (cregister)(cpu->mem[*cpu->PC]);
}

void cpu_write_mem(cpu_t* cpu, CPU_WIDTH offset, CPU_WIDTH value) {
	cpu->mem[offset] = value;
}

CPU_WIDTH cpu_decode_exec(cpu_t* cpu) {
	cregister* RA = &cpu->reg[R_A];
	cregister* RB = &cpu->reg[R_B];

	*RA = (cregister)(cpu->mem[*cpu->PC + 1]);
	*RB = (cregister)(cpu->mem[*cpu->PC + 2]);

	CPU_WIDTH pc_offset = 1;

	switch (cpu->reg[R_IR] & 31) {
	case I_NOP: break;
	case I_INC:
		cpu->mem[*RA]++;
		pc_offset++;
		break;

	case I_DEC:
		cpu->mem[*RA]--;
		pc_offset++;
		break;

	case I_SHL:
		if (cpu->reg[R_IR] & IR_ONE_OP) {
			cpu->reg[R_AC] <<= *RA;
			pc_offset++;
		}
		else {
			cpu->mem[*RA] <<= *RB;
			pc_offset += 2;
		}

		break;

	case I_ADD:
		cpu->mem[*RA] += (cpu->reg[R_IR] & IR_OP2_ADDR) ? cpu->mem[*RB] : *RB;
		pc_offset += 2;
		break;

	case I_SUB:
		cpu->mem[*RA] -= (cpu->reg[R_IR] & IR_OP2_ADDR) ? cpu->mem[*RB] : *RB;
		pc_offset += 2;
		break;

	case I_MOV:
		cpu->mem[*RA] = (cpu->reg[R_IR] & IR_OP2_ADDR) ? cpu->mem[*RB] : *RB;
		pc_offset += 2;
		break;

	case I_JMP:
		*cpu->PC = *RA;
		pc_offset = 0;
		break;

	default:
		ERROR("segfault: %hhX\t%hhX\n", *cpu->PC, cpu->reg[R_IR]);
		exit(1);
	}

	return pc_offset;
}

void cpu_run(cpu_t* cpu) {
	int i = 0;
	while (*cpu->PC != MEMORY_CAP) {
		cpu_fetch_ir(cpu);
		*cpu->PC += cpu_decode_exec(cpu);
		printf("%hhX\t%hhX\n", *cpu->PC, cpu->reg[R_IR]);
		i++;
		if (i > 5) break;
	}
}

void cpu_destroy(cpu_t* cpu) {
	free(cpu);
}

#endif // CPU_HEADER

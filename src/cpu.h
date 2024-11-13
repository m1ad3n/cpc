#ifndef CPU_HEADER
#define CPU_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEBUG(msg, ...) fprintf(stdout, msg, __VA_ARGS__)
#define ERROR(msg, ...) fprintf(stderr, msg, __VA_ARGS__)

typedef unsigned char u8;
typedef unsigned short u16;

#define CPU_PROGRAM_MEM 0x8000
#define CPU_RESET_MEM   0xFFFC
#define CPU_STACK_START 0x0100
#define CPU_ZERO_PAGE_END 0xFF

#define MEMORY_CAP 0xFFFF

typedef u8 memory_t[MEMORY_CAP];

typedef struct {
	u16 PC; // program counter
	u16 SP; // stack pointer

	u8 IR; // instruction register
	u8 A;  // ACCUMULATOR
	u8 X;  // index register
	u8 Y;  // index register
	
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
	IR_NOP,
	IR_LDA_IM = 0xA9,
	IR_LDA_ZP = 0xA5
} CPU_INSTRUCTION_SET;

cpu_t* cpu_create(memory_t mem) {
	cpu_t* ncpu = (cpu_t*)malloc(sizeof(cpu_t));
	ncpu->mem = mem;
	return ncpu;
}

void cpu_init_mem(memory_t mem) {
	// zero out memory
	for (u16 i = 0; i < MEMORY_CAP; i++)
		mem[i] = 0;

	mem[CPU_RESET_MEM] = 0x80;
	mem[CPU_RESET_MEM + 1] = 0x00;
}

void cpu_reset(cpu_t* cpu) {
	cpu->PC = (cpu->mem[0xFFFC] << 8) | (cpu->mem[0xFFFD]);
	cpu->SP = 0x01FF;
	cpu->A = cpu->X = cpu->Y = 0;
	cpu->PSW = 0;
	cpu->IR = 0;
}

void cpu_fetch_ir(cpu_t* cpu) {
	cpu->IR = cpu->mem[cpu->PC++];
}

void cpu_write_mem(cpu_t* cpu, u16 offset, u8 value) {
	cpu->mem[offset] = value;
}

void LDA_set_flags(cpu_t* cpu) {
	cpu->PSW |= (cpu->A == 0) ? PSW_Z : 0;
	cpu->PSW |= (cpu->A & 0x80) ? PSW_N : 0;
}

void cpu_exec_ir(cpu_t* cpu) {
	u8 addr;
	switch (cpu->IR) {
		case IR_LDA_IM:
			cpu->A = cpu->mem[cpu->PC++];
			LDA_set_flags(cpu);
			break;
		
		case IR_LDA_ZP:
			addr = cpu->mem[cpu->PC++];
			cpu->A = cpu->mem[addr % (CPU_ZERO_PAGE_END + 1)];
			LDA_set_flags(cpu);
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

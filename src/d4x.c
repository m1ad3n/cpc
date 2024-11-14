#include "d4x.h"

int main(void) {
	memory_t mem;
	cpu_init_mem(mem);

	// PROGRAM STARTS

	/* PUSH 0xCA */
	mem[0x1000] = IR_PUSH;
	mem[0x1001] = 0xFB;
	mem[0x1002] = 0xFF;

	/* POP %B */
	mem[0x1003] = IR_POP | IR_OP1_REG;
	mem[0x1004] = REG_B;

	/* some data constants */
	mem[0xFFFA] = 0x11;
	mem[0xFFFB] = 0xCA;

	// PROGRAM ENDS

	D4X* cpu = cpu_create(mem);

	cpu_reset(cpu);
	cpu_run(cpu);
	
	printf("REG B = %hhX\n", cpu->regs[REG_B]);
	cpu_destroy(cpu);

	return 0;
}

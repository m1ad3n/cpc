#include "d4x.h"

int main(void) {
	memory_t mem;
	cpu_init_mem(mem);

	// PROGRAM STARTS

	/* MOV %E, 0xFFFA (17) */
	mem[0x1000] = IR_MOV | IR_OP1_REG;
	mem[0x1001] = REG_E;
	mem[0x1002] = 0xFA;
	mem[0x1003] = 0xFF;

	/* INC %E */
	mem[0x1004] = IR_INC | IR_OP1_REG;
	mem[0x1005] = REG_E;

	/* CMP %E, 0xFFFB (202) */
	mem[0x1006] = IR_CMP | IR_OP1_REG;
	mem[0x1007] = REG_E;
	mem[0x1008] = 0xFB;
	mem[0x1009] = 0xFF;

	/* JL 0x1004 */
	mem[0x100A] = IR_JL;
	mem[0x100B] = 0x04;
	mem[0x100C] = 0x10;

	/* some data constants */
	mem[0xFFFA] = 0x11;
	mem[0xFFFB] = 0xCA;

	// PROGRAM ENDS

	D4X* cpu = cpu_create(mem);

	cpu_reset(cpu);
	cpu_run(cpu);
	
	printf("REG E = %d\n", cpu->regs[REG_E]);
	cpu_destroy(cpu);

	return 0;
}

#include "cpu.h"

int main(void) {
	memory_t mem;
	cpu_init_mem(mem);

	// PROGRAM STARTS
	mem[0xF1] = 0x88;
	mem[0x8000] = IR_PLA;
	mem[0x8001] = 0xA;
	mem[0x8003] = IR_PHA;
	mem[0x8004] = IR_LDA_ZP;
	mem[0x8005] = 0xF1;
	mem[0x8006] = IR_PLA;
	// mem[0x8007] = 0x70;
	
	// mem[0x7000] = IR_INX;
	// mem[0x7001] = IR_JSR_ABS;
	// mem[0x7002] = 0x05;
	// mem[0x7003] = 0x80;
	// PROGRAM ENDS

	cpu_t* cpc = cpu_create(mem);

	cpu_reset(cpc);
	cpu_run(cpc);
	
	printf("AC = %hhX\n", cpc->A);
	cpu_destroy(cpc);

	return 0;
}

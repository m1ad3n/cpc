#include "cpu.h"

int main(void) {
	memory_t mem;
	cpu_init_mem(mem);

	// PROGRAM STARTS
	mem[0x8000] = IR_LDA_ZP;
	mem[0x8001] = 0x39;
	mem[0x0039] = 0xFA;
	// PROGRAM ENDS

	cpu_t* cpc = cpu_create(mem);

	cpu_reset(cpc);
	cpu_run(cpc);
	
	DEBUG("AC = %hhX\n", cpc->A);
	cpu_destroy(cpc);

	return 0;
}

#include "cpu.h"

int main(void) {
	memory_t ram = {
		I_INC, 0xaa,
		I_SHL, 0xaa, 0x1,
		I_JMP, 0x0
	};

	cpu_t* cpc = cpu_create(ram);
	cpu_run(cpc);

	DEBUG("0xAA = %d\n", cpc->mem[0xaa]);
	cpu_destroy(cpc);
	return 0;
}

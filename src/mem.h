#ifndef MEMORY_HEADER
#define MEMORY_HEADER

#define CPU_PROGRAM_MEM   0x8000
#define CPU_RESET_MEM     0xFFFC
#define CPU_STACK_START   0x0100
#define CPU_ZERO_PAGE_END 0xFF

// #define MEMORY_LAYOUT_BIGENDIAN
#define MEMORY_LAYOUT_LITTLENDIAN

#define MEMORY_CAP 0xFFFF

typedef unsigned char u8;
typedef unsigned short u16;
typedef u8 memory_t[MEMORY_CAP];

void cpu_init_mem(memory_t mem) {
	// zero out memory
	for (u16 i = 0; i < MEMORY_CAP; i++)
		mem[i] = 0;

	mem[CPU_RESET_MEM] = 0x00;
	mem[CPU_RESET_MEM + 1] = 0x80;
}

u16 mem_fetch_word(memory_t mem, u16 addr) {
	u16 val = ((u16)mem[addr]) | ((u16)mem[addr + 1] << (u16)8);
	#ifdef MEMORY_LAYOUT_BIGENDIAN
	u16 lsb = (val & 0xFF) << 8;
	val >>= 8;
	val |= lsb;
	#endif
	return val;
}

void mem_write_word(memory_t mem, u16 addr, u16 value) {
	mem[addr] = (value & 0xFF);
	mem[addr + 1] = (value >> 8);
}

#endif

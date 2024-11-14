#ifndef MEMORY_HEADER
#define MEMORY_HEADER

#define STACK_START 	0x0000
#define RAM_START		0x0100
#define PROGRAM_START	0x1000
#define VIDEO_START		0x8889
#define MEMORY_CAP 		0xFFFF

// #define MEMORY_LAYOUT_BIGENDIAN
#define MEMORY_LAYOUT_LITTLENDIAN

typedef unsigned char u8;
typedef unsigned short u16;
typedef u8 memory_t[MEMORY_CAP];

void cpu_init_mem(memory_t mem) {
	// zero out memory
	for (u16 i = 0; i < MEMORY_CAP; i++)
		mem[i] = 0;
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

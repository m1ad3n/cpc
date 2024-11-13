# 8-bit Processor Emulator: Modified 6502
### Overview

This project is an emulator of an 8-bit processor, inspired by the 6502 architecture but with some modifications. The purpose of this project is to deepen understanding of computer architecture, specifically the inner workings of processors, memory, and the execution of instructions at a low level.

### Goal

The main goal of this project is to:

* Emulate an 8-bit processor that behaves similarly to the 6502 CPU, with custom modifications.
* Learn about computer architecture by simulating the operation of a processor, including fetching, decoding, and executing instructions.
* Provide insight into the way memory, registers, and the stack work, and how instructions interact with these components to execute a program.

### Features

The emulator is based on the following key components:

* Modified 6502 Processor: The project uses a custom version of the 6502 processor with a few modifications to fit specific goals for learning.
* Instruction Set: A set of assembly instructions will be implemented to simulate the behavior of the processor.
* Memory and Stack Management: Emulation of memory operations (read/write) and stack-based operations (push/pop).
* Processor Flags: Handling of processor status flags, including Zero (Z), Carry (C), Negative (N), and others.
* Custom Modifications: Certain instructions or features of the standard 6502 processor have been modified for educational purposes.

### How It Works

The core of the emulator follows these steps to simulate the processor’s operation:

* Memory: Memory is initialized and organized in the same way as it would be on a real 6502-based system. Each address corresponds to a byte of data.
* Registers: The CPU has registers such as the Accumulator (A), X and Y indices, Program Counter (PC), Stack Pointer (SP), and Processor Status Register (P), which track the state of the CPU.
* Instruction Fetch/Decode/Execute: The CPU fetches an instruction from memory at the address pointed to by the Program Counter (PC), decodes it, and executes it by updating registers, memory, and flags.
* Stack Operations: The emulator includes basic operations for pushing to and popping from the stack, which is critical for function calls, returns, and context switching.

### Error Handling

The emulator checks for conditions such as stack underflow (popping from an empty stack) and other potential errors during execution.

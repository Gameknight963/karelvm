#pragma once
#include <cstdint>

enum class instruction : uint32_t
{
	// exit
	EXIT = 0x00,

	// treat the next byte as a pointer register, and write the value in the following register to it
	WRITE = 0x01,

	// treat the next byte as a destination register, and dereference the following pointer register into it
	READ = 0x02,

	// treat the next byte as a register, and move the following word into it
    MOV = 0x03,

	// treat the next byte as a register, and push its value to the stack
	PUSH = 0x04,

	// treat the next byte as a register, and pop the top of the stack into it
	POP = 0x05,

	// treat the next two bytes as registers, add them, and put the result in the first one
	ADD = 0x06,

	// treat the next two bytes as registers, subtract them, and put the result in the first one
	SUB = 0x07,

	// treat the next two bytes as registers, multiply their values, and store the low 24 bits in
	// the first register and the high 24 bits in the second
	MULT = 0x08,

	// treat the next two bytes as registers, integer divide their values, and store the low 24
	// bits in the first register and the high 24 bits in the second
	// faults on division by 0
	DIV = 0x09,
};

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
};

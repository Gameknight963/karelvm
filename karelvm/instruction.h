#pragma once
#include <cstdint>

enum class instruction : uint32_t
{
	// exit
	EXIT = 0x00,

	// write the value in a1 to the pointer in a0
	WRITE = 0x01,

	// dereference a0 into a0
	READ = 0x02,

	// treat the next word as a register, and move the following word into it
    MOV = 0x03,

	// treat the next word as a register, and push its value to the stack
	PUSH = 0x04,

	// treat the next word as a register, and pop the top of the stack into it
	POP = 0x05,
};

#pragma once

enum class instruction : char
{
	// exit
	EXIT = 0x00,

	// write the value in a1 to the pointer in a0
	WRITE = 0x01,

	// dereference a0 into a0
	READ = 0x02,

	// treat the next byte as a register, and move the following byte into it
    MOV = 0x03,

	// treat the next byte as a register, and push it's value to the stack
	PUSH = 0x04,

	// treat the next byte as a register, and pop the top of the stack into it
	POP = 0x05,
};
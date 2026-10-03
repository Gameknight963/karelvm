#pragma once

enum class instruction : char
{
	// exit
	EXIT = 0x00,

	// write the value in r1 to the pointer in r0
	WRITE = 0x01,

	// dereference r0 into r0
	READ = 0x02,

	// treat the next value as a register, and move the value after that into it
	MOV = 0x03,
};
#pragma once
#include "instruction.h"
#include "reg.h"

// who cares
uint32_t program[100] =
{
	//// mov a0, 5
	//(uint32_t)instruction::MOV,
	//(uint32_t)reg::a0,
	//5,

	//// mov a1, 44
	//(uint32_t)instruction::MOV,
	//(uint32_t)reg::a1,
	//44,

	//(uint32_t)instruction::WRITE,

	// mov s0, 24
	(uint32_t)instruction::MOV,
	(uint32_t)reg::s0,
	0xAAB440,

	// push s0
	(uint32_t)instruction::PUSH,
	(uint32_t)reg::s0,

	(uint32_t)instruction::EXIT
};

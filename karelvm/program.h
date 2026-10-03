#pragma once
#include "instruction.h"
#include "reg.h"

// who cares
uint8_t program[100] =
{
	//// mov a0, 5
	//(uint8_t)instruction::MOV,
	//(uint8_t)reg::a0,
	//5,

	//// mov a1, 44
	//(uint8_t)instruction::MOV,
	//(uint8_t)reg::a1,
	//44,

	//(uint8_t)instruction::WRITE,

	// mov s0, 24
	(uint8_t)instruction::MOV,
	(uint8_t)reg::s0,
	24,

	// push s0
	(uint8_t)instruction::PUSH,
	(uint8_t)reg::s0,
	(uint8_t)instruction::PUSH,
	(uint8_t)instruction::EXIT
};
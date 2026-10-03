#pragma once
#include "instruction.h"
#include "reg.h"

uint8_t program[10] = 
{
	(uint8_t)instruction::MOV,
	(uint8_t)reg::r0,
	5,

	(uint8_t)instruction::MOV,
	(uint8_t)reg::r1,
	44,

	(uint8_t)instruction::WRITE,

	(uint8_t)instruction::EXIT
};
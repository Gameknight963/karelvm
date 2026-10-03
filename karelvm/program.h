#pragma once
#include "instruction.h"
#include "reg.h"

#define U24_BYTES(value) \
    static_cast<uint8_t>(uint32_t(value)), \
    static_cast<uint8_t>(uint32_t(value) >> 8), \
    static_cast<uint8_t>(uint32_t(value) >> 16)
#define MOV(destination, value) static_cast<uint8_t>(instruction::MOV), static_cast<uint8_t>(reg::destination), U24_BYTES(value),
#define PUSH(source) static_cast<uint8_t>(instruction::PUSH), static_cast<uint8_t>(reg::source),
#define POP(destination) static_cast<uint8_t>(instruction::POP), static_cast<uint8_t>(reg::destination),
#define READ(destination, pointer) static_cast<uint8_t>(instruction::READ), static_cast<uint8_t>(reg::destination), static_cast<uint8_t>(reg::pointer),
#define WRITE(pointer, source) static_cast<uint8_t>(instruction::WRITE), static_cast<uint8_t>(reg::pointer), static_cast<uint8_t>(reg::source),
#define EXIT() static_cast<uint8_t>(instruction::EXIT),
#define ADD(ra, rb) static_cast<uint8_t>(instruction::ADD), static_cast<uint8_t>(reg::ra), static_cast<uint8_t>(reg::rb),
#define SUB(ra, rb) static_cast<uint8_t>(instruction::SUB), static_cast<uint8_t>(reg::ra), static_cast<uint8_t>(reg::rb),
#define MULT(ra, rb) static_cast<uint8_t>(instruction::MULT), static_cast<uint8_t>(reg::ra), static_cast<uint8_t>(reg::rb),
#define DIV(ra, rb) static_cast<uint8_t>(instruction::DIV), static_cast<uint8_t>(reg::ra), static_cast<uint8_t>(reg::rb),
#define JMP(target) static_cast<uint8_t>(instruction::JMP), U24_BYTES(target),
#define CALL(target) static_cast<uint8_t>(instruction::CALL), U24_BYTES(target),
#define RET() static_cast<uint8_t>(instruction::RET),

constexpr uint8_t program[] = {
    MOV(s0, 2112)
    MOV(s1, 5297)
    // this will work
    ADD(s0, s1)

    MOV(s1, 0)
    // this will fault
    DIV(s0, s1)
    PUSH(s0)
    EXIT()
};

#undef MOV
#undef PUSH
#undef POP
#undef READ
#undef WRITE
#undef EXIT
#undef U24_BYTES

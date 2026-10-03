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
#define JZ(condition, target) static_cast<uint8_t>(instruction::JZ), static_cast<uint8_t>(reg::condition), U24_BYTES(target),
#define JNZ(condition, target) static_cast<uint8_t>(instruction::JNZ), static_cast<uint8_t>(reg::condition), U24_BYTES(target),
#define CMP(destination, ra, rb) static_cast<uint8_t>(instruction::CMP), static_cast<uint8_t>(reg::destination), static_cast<uint8_t>(reg::ra), static_cast<uint8_t>(reg::rb),

// Fill the bottom 28 rows of the 32x32 grid with a blue/pink/green/yellow gradient.
// Memory address 111 is grid cell 128, after the 17 register cells.
constexpr uint32_t next_row = 30;
constexpr uint32_t next_pixel = 43;

constexpr uint8_t program[] = {
    MOV(a0, 111) // destination cell
    MOV(s0, 0xD05028) // first row starts at RGB(40, 80, 208)
    MOV(s1, 0xFE0006) // each column adds 6 red and subtracts 2 blue
    MOV(s2, 0xFC0600) // each row adds 6 green and subtracts 4 blue
    MOV(s3, 1)  // counter step
    MOV(s5, 28) // rows remaining

    // next_row:
    MOV(a1, 0)
    ADD(a1, s0) // copy the row's starting color
    MOV(a2, 32) // columns remaining

    // next_pixel:
    WRITE(a0, a1)
    ADD(a0, s3) // advance to the next cell
    ADD(a1, s1) // advance the color
    SUB(a2, s3)
    JNZ(a2, next_pixel)

    ADD(s0, s2) // advance the next row's starting color
    SUB(s5, s3)
    JNZ(s5, next_row)
    EXIT()
};

#undef MOV
#undef PUSH
#undef POP
#undef READ
#undef WRITE
#undef EXIT
#undef ADD
#undef SUB
#undef MULT
#undef DIV
#undef JMP
#undef CALL
#undef RET
#undef JZ
#undef JNZ
#undef CMP
#undef U24_BYTES

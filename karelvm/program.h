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

constexpr uint8_t program[] = {
    MOV(s0, 0xAAB440)
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

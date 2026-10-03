#pragma once

// represents karel "memory addresses" which are "registers"
enum class reg : char
{
    r0 = 0x00,
    r1 = 0x01,
    r2 = 0x02,
    r3 = 0x03,
    r4 = 0x04,
    r5 = 0x05,
    r6 = 0x06,
    r7 = 0x07,
    r8 = 0x08,
    r9 = 0x09,
    r10 = 0x0A,
    r11 = 0x0B,
    r12 = 0x0C,
    r13 = 0x0D,
    r14 = 0x0E,
    r15 = 0x0F,
    r16 = 0x10,
};
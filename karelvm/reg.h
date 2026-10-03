#pragma once

// represents karel "memory addresses" which are "registers"
enum class reg : char
{
    // argument 0
    a0 = 0x00,
    // argument 1
    a1 = 0x01,
    // argument 2
    a2 = 0x02,
    // argument 3
    a3 = 0x03,

    // (calle-saved)
    s0 = 0x04,
    // (calle-saved)
    s1 = 0x05,
    // (calle-saved)
    s2 = 0x06,
    // (calle-saved)
    s3 = 0x07,
    // (calle-saved)
    s4 = 0x08,
    // (calle-saved)
    s5 = 0x09,

    // (caller-saved)
    tmp0 = 0x0A,
    // (caller-saved)
    tmp1 = 0x0B,
    // (caller-saved)
    tmp2 = 0x0C,
    // (caller-saved)
    tmp3 = 0x0D,
    // (caller-saved)
    tmp4 = 0x0E,

    // stack pointer
    sp = 0x0F,

    // base pointer
    bp = 0x10,
};
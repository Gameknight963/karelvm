#pragma once
#include <cstdint>

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    std::uint8_t& operator[](size_t index) {
        return (&r)[index];
    }

    const std::uint8_t& operator[](size_t index) const {
        return (&r)[index];
    }
};
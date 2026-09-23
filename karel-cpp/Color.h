#pragma once
#include <cstdint>
#include <Windows.h>

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    COLORREF ToColorRef() const
    {
        return RGB(this->r, this->g, this->b);
    }
};
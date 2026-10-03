#pragma once
#include <Windows.h>
#include "../karel-cpp/Color.h"

COLORREF ColorToColorRef(Color c)
{
    return RGB(c.r, c.g, c.b);
}
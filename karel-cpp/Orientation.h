#pragma once
#include <string>
#include <cstdint>
#include "Point.h"

class Orientation
{
private:
    uint8_t orientation;

    static const uint8_t north = 1;
    static const uint8_t east = 2;
    static const uint8_t south = 3;
    static const uint8_t west = 4;

public:

    static Orientation North()
    {
        return Orientation(north);
    }

    static Orientation East()
    {
        return Orientation(east);
    }

    static Orientation South()
    {
        return Orientation(south);
    }

    static Orientation West()
    {
        return Orientation(west);
    }

    Orientation(uint8_t orientation);

    uint8_t GetOrientation() const;
    Point AsPoint() const;

    Orientation Rotate(int8_t amount) const;
    Orientation RotateLeft() const;
    Orientation RotateRight() const;
    const char* ToString() const;
};
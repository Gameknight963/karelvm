#include "Orientation.h"
#include <stdexcept>

Orientation::Orientation(uint8_t orientation)
{
    if (orientation < north || orientation > west)
    {
        throw std::invalid_argument("Invalid orientation");
    }
    this->orientation = orientation - 1;
}

uint8_t Orientation::GetOrientation() const
{
    return this->orientation + 1;
}

Orientation Orientation::Rotate(int8_t amount) const
{
    uint8_t result = ((this->orientation + amount) % 4 + 4) % 4 + 1;
    return Orientation(result);
}

Orientation Orientation::RotateLeft() const
{
    return this->Rotate(-1);
}

Orientation Orientation::RotateRight() const
{
    return this->Rotate(1);
}

Point Orientation::AsPoint() const
{
    switch (GetOrientation())
    {
    case north:
        return Point(0, -1);

    case east:
        return Point(1, 0);

    case south:
        return Point(0, 1);

    case west:
        return Point(-1, 0);

    default:
        return Point(0, 0);
    }
}

const char* Orientation::ToString() const
{
    switch (GetOrientation())
    {
    case north:
        return "North";

    case east:
        return "East";

    case south:
        return "South";

    case west:
        return "West";

    default:
        return "Unknown";
    }
}

#include "Orientation.h"

Orientation::Orientation(Byte orientation)
{
    if (orientation < north || orientation > west)
    {
        throw gcnew ArgumentException("Invalid orientation.", "orientation");
    }

    // Store zero-based so a default-initialized value is also North.
    this->orientation = orientation - 1;
}

Byte Orientation::GetOrientation()
{
    return this->orientation + 1;
}

Orientation Orientation::Rotate(SByte amount)
{
    Byte result = ((this->orientation + amount) % 4 + 4) % 4 + 1;

    return Orientation(result);
}

Orientation Orientation::RotateLeft()
{
    return this->Rotate(-1);
}

Orientation Orientation::RotateRight()
{
    return this->Rotate(1);
}

Point Orientation::AsPoint()
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

String^ Orientation::ToString()
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

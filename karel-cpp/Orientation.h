#pragma once

using namespace System;
using namespace System::Numerics;
using namespace System::Drawing;

public value class Orientation
{
private:
    Byte orientation;

    literal Byte north = 1;
    literal Byte east = 2;
    literal Byte south = 3;
    literal Byte west = 4;

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

    Orientation(Byte orientation);

    Byte GetOrientation();
    Point AsPoint();

    Orientation Rotate(SByte amount);
    Orientation RotateLeft();
    Orientation RotateRight();
    virtual String^ ToString() override;
};
using namespace System;
using namespace System::Numerics;

#include "Karel.h"

namespace karel_cpp
{

    Karel::Karel(int x, int y)
    {
        if (x <= 0)
            throw gcnew ArgumentOutOfRangeException("x", "Grid width must be positive.");
        if (y <= 0 || static_cast<Int64>(x) * y > Int32::MaxValue)
            throw gcnew ArgumentOutOfRangeException("y", "Grid height must be positive and the grid must fit in an array.");
        this->size = Size(x, y);
        this->position = Point(0, 0);
        this->orientation = Orientation::East();
        this->canvas_vector = gcnew array<Color>(x * y);
        for (int i = 0; i < canvas_vector->Length; i++)
            canvas_vector[i] = Color::White;
    }

    int Karel::getVectorIndex(int x, int y)
    {
        if (x < 0 || x >= size.Width)
            throw gcnew ArgumentOutOfRangeException("x");
        if (y < 0 || y >= size.Height)
            throw gcnew ArgumentOutOfRangeException("y");
        return x + y * size.Width;
    }

    Color Karel::GetColorBeneath()
    {
        int index = getVectorIndex(this->position.X, this->position.Y);
        return canvas_vector[index];
    }

    Color Karel::GetColorAt(int x, int y)
    {
        int index = getVectorIndex(x, y);
        return canvas_vector[index];
    }

    void Karel::Paint(Color color)
    {
        int index = getVectorIndex(this->position.X, this->position.Y);
        canvas_vector[index] = color;
        Changed(this, EventArgs::Empty);
    }

    Size Karel::GetGridSize()
    {
        return size;
    }

    Point Karel::GetPosition()
    {
        return this->position;
    }

    Orientation Karel::GetOrientation()
    {
        return orientation;
    }

    // We need to use an overload since managed classes
    // don't allow default arguments.

    void Karel::Move()
    {
        Karel::Move(1);
    }

    void Karel::Move(int steps)
    {
        Point direction = orientation.AsPoint();
        Int64 x = position.X + static_cast<Int64>(direction.X) * steps;
        Int64 y = position.Y + static_cast<Int64>(direction.Y) * steps;
        if (x < 0 || x >= size.Width || y < 0 || y >= size.Height)
            throw gcnew InvalidOperationException("Karel cannot move outside the grid.");
        position = Point(static_cast<int>(x), static_cast<int>(y));
        Changed(this, EventArgs::Empty);
    }

    void Karel::Rotate(SByte amount)
    {
        this->orientation = this->orientation.Rotate(amount);
        Changed(this, EventArgs::Empty);
    }

    void Karel::RotateRight()
    {
        Rotate(1);
    }

    void Karel::RotateLeft()
    {
        Rotate(-1);
    }
}

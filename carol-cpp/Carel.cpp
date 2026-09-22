using namespace System;
using namespace System::Numerics;

#include "Carel.h"
#include <cstdlib>

namespace carel_cpp
{
    Carel::Carel(Point size)
    {
        this->size = size;
        this->position = Point(0, 0);
        this->vector_length = size.X * size.Y;
        this->canvas_vector = (Color*)malloc(sizeof(int) * vector_length);
    }

    Carel::~Carel()
    {
        free(this->canvas_vector);
    }

    int Carel::getVectorIndex(Point point)
    {
        return point.X + point.Y * this->size.X;
    }

    Color Carel::GetColorBeneath()
    {
        int index = getVectorIndex(this->position);

        // todo: improve error
        if (index > vector_length)
            throw gcnew Exception("out of range");

        return canvas_vector[index];
    }

    void Carel::Paint(Color color)
    {
        throw gcnew NotImplementedException();
    }

    Point Carel::GetGridSize()
    {
        return size;
    }

    Point Carel::GetPosition()
    {
        return position;
    }

    Orientation Carel::GetOrientation()
    {
        return orientation;
    }

    Point add(Point a, Point b)
    {
        return Point((a.X + b.X), (a.Y + b.Y));
    }

    Point multiply(Point a, Point b)
    {
        return Point((a.X * b.X), (a.Y * b.Y));
    }

    Point scale(Point a, int amount)
    {
        return Point((a.X * amount), (a.Y * amount));
    }

    void Carel::Move(int steps = 0)
    {
        position = add(this->position, scale(orientation.AsPoint(), steps));
    }

    void Carel::Rotate(SByte amount)
    {
        this->orientation.Rotate(amount);
    }

    void Carel::RotateRight()
    {
        orientation.RotateLeft();
    }

    void Carel::RotateLeft()
    {
        orientation.RotateLeft();
    }
}
using namespace System;
using namespace System::Numerics;

#include "Karel.h"
#include <cstdlib>

namespace karel_cpp
{
    Karel::Karel(Point size)
    {
        this->size = size;
        this->position = Point(0, 0);
        this->vector_length = size.X * size.Y;
        this->canvas_vector = (Color*)malloc(sizeof(int) * vector_length);
    }

    Karel::~Karel()
    {
        free(this->canvas_vector);
    }

    int Karel::getVectorIndex(Point point)
    {
        return point.X + point.Y * this->size.X;
    }

    Color Karel::GetColorBeneath()
    {
        int index = getVectorIndex(this->position);
        if (index > vector_length)
            throw gcnew Exception(String::Format("out of range: {0}", index));
        return canvas_vector[index];
    }

    void Karel::Paint(Color color)
    {
        int index = getVectorIndex(this->position);
        if (index > vector_length)
            throw gcnew Exception(String::Format("out of range: {0}", index));
        canvas_vector[index] = color;
    }

    Point Karel::GetGridSize()
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

    void Karel::Move(int steps = 0)
    {
        position = add(this->position, scale(orientation.AsPoint(), steps));
    }

    void Karel::Rotate(SByte amount)
    {
        this->orientation.Rotate(amount);
    }

    void Karel::RotateRight()
    {
        orientation.RotateLeft();
    }

    void Karel::RotateLeft()
    {
        this->orientation.RotateLeft();
    }
}
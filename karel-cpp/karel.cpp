#include "Color.h"
#include "Karel.h"
#include <cstdint>
#include <stdexcept>
#include <cstdlib>

namespace karel_cpp
{
    Karel::Karel(uint32_t x, uint32_t y) :
        position(0, 0),
        size(x, y),
        orientation(Orientation::East())
    {
        canvas_vector_length = x * y;
        this->canvas_vector = 
            static_cast<Color*>(
                std::malloc(canvas_vector_length * sizeof(Color)));

        if (!this->canvas_vector) throw std::bad_alloc{};

        for (int i = 0; i < canvas_vector_length; ++i)
            canvas_vector[i] = Color{ 255, 255, 255 };
    }

    Karel::~Karel()
    {
        std::free(canvas_vector);
    }

    int Karel::getVectorIndex(int x, int y) const
    {
        if (x < 0 || x >= size.x)
            throw std::out_of_range("x");
        if (y < 0 || y >= size.y)
            throw std::out_of_range("y");
        return x + y * size.x;
    }

    Color Karel::GetColorBeneath()
    {
        int index = getVectorIndex(this->position.x, this->position.y);
        return canvas_vector[index];
    }

    Color Karel::GetColorAt(int x, int y)
    {
        int index = getVectorIndex(x, y);
        return canvas_vector[index];
    }

    void Karel::checkAndCallChanged() const
    {
        if (Changed) Changed();
    }

    void Karel::Paint(Color color)
    {
        int index = getVectorIndex(this->position.x, this->position.y);
        canvas_vector[index] = color;
        checkAndCallChanged();
    }

    Point Karel::GetGridSize() const
    {
        return size;
    }

    Point Karel::GetPosition() const
    {
        return this->position;
    }

    Orientation Karel::GetOrientation() const
    {
        return orientation;
    }

    void Karel::Move(int steps)
    {
        Point direction = orientation.AsPoint();
        int x = position.x + direction.x * steps;
        int y = position.y + direction.y * steps;
        if (x < 0 || x >= size.x || y < 0 || y >= size.y)
            throw std::logic_error("Karel doesn't know how to move outside the grid");
        position = Point(x, y);
        checkAndCallChanged();
    }

    void Karel::Rotate(int8_t amount)
    {
        this->orientation = this->orientation.Rotate(amount);
        checkAndCallChanged();
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

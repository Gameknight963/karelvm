#pragma once
#include "Orientation.h"
#include "Color.h"
#include <cstdint>
#include <functional>

namespace karel_cpp
{
	class Karel
	{
	private:
		Point position;
		Point size;
		Orientation orientation;
		Color* canvas_vector;
		size_t canvas_vector_length;
		int getVectorIndex(int x, int y) const;
		void checkAndCallChanged() const;

	public:
		std::function<void()> Changed;
		Karel(uint32_t x, uint32_t y);
		~Karel();
		void Move(int steps = 1);
		void Rotate(int8_t amount);
		void RotateLeft();
		void RotateRight();

		Point GetPosition() const;
		Point GetGridSize() const;
		Orientation GetOrientation() const;

		Color GetColorBeneath();
		Color GetColorAt(int x, int y);
		void Paint(Color color);
	};
}

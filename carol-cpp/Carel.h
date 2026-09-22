#pragma once
#include "Orientation.h"

using namespace System;
using namespace System::Numerics;
using namespace System::Drawing;

namespace carel_cpp
{
	public ref class Carel
	{
	private:
		Point position;
		Point size;
		Orientation orientation;
		Color* canvas_vector;
		int vector_length;
		int getVectorIndex(Point point);

	public:
		Carel(Point size);
		~Carel();
		void Move(int steps);
		void Rotate(SByte amount);
		void RotateLeft();
		void RotateRight();

		Point GetPosition();
		Point GetGridSize();
		Orientation GetOrientation();

		Color GetColorBeneath();
		void Paint(Color color);
	};
}
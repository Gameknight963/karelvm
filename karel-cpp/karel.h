#pragma once
#include "Orientation.h"

using namespace System;
using namespace System::Numerics;
using namespace System::Drawing;

namespace karel_cpp
{
	public ref class Karel
	{
	private:
		Point position;
		Size size;
		Orientation orientation;
		array<Color>^ canvas_vector;
		int getVectorIndex(int x, int y);

	public:
		event EventHandler^ Changed;
		Karel(int x, int y);
		void Move();
		void Move(int steps);
		void Rotate(SByte amount);
		void RotateLeft();
		void RotateRight();

		Point GetPosition();
		Size GetGridSize();
		Orientation GetOrientation();

		Color GetColorBeneath();
		Color GetColorAt(int x, int y);
		void Paint(Color color);
	};
}

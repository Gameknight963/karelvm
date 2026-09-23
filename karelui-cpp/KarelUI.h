#pragma once
#include <windows.h>
#include "Karel.h"
#include <cstdint>

class KarelUI
{
private:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Draw(HDC dc) const;
	karel_cpp::Karel* karel;
	uint32_t squareSize;
	inline static bool isInitialized = false;
	std::function<bool()> onTick;
	int tickRateMs = 50;
public:
	KarelUI(
		int x, int y,
		std::function<bool()> onTick,
		int squareSize,
		int tickRateMs);
	~KarelUI();

	int Show();
	karel_cpp::Karel* GetKarel() const;
};

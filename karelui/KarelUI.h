#pragma once
#include <windows.h>
#include "../karel-cpp/karel.h"
#include <cstdint>
#include <chrono>

class KarelUI
{
private:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Draw(HDC dc) const;
	karel_cpp::Karel* karel;
	uint32_t squareSize;
	inline static bool isInitialized = false;
	std::function<bool()> onTick;
	std::function<bool()> initialize = nullptr;
	bool ticking = false;
	bool timingInterrupted = false;
	std::chrono::steady_clock::time_point lastTickTime{};
	double tickDebtMs = 0;
	// Requested interval between instructions, independent of the UI timer.
	int tickRateMs = 50;
	HWND hwnd = nullptr;
public:
	KarelUI(
		int x, int y,
		std::function<bool()> onTick,
		int squareSize,
		int tickRateMs);
	KarelUI(
		int x, int y,
		std::function<bool()> onTick,
		std::function<bool()> initialize,
		int squareSize,
		int tickRateMs);
	~KarelUI();

	int Show();
	karel_cpp::Karel* GetKarel() const;
	HWND GetHwnd() const;
};

#include "KarelUI.h"
#include <exception>
#include <iostream>

using namespace karel_cpp;

static KarelUI* karelui = nullptr;
static int ticks = 0;
static Karel* karel;

static bool Tick()
{
    karel->Paint(Color{ 255, 180, 80 });

    if (ticks % 10 == 9)
        karel->RotateRight();
    else
        karel->Move();

    ticks++;
    return true;
}

std::wstring ToWide(const char* str)
{
    return std::wstring(str, str + strlen(str)).c_str();
}

int main()
{
    //HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    //DWORD mode = 0;
    //GetConsoleMode(handle, &mode);
    //SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    try
    {
        karelui = new KarelUI(
            10, 10, Tick, 20, 200);
        karel = karelui->GetKarel();
        return karelui->Show();
    }
    catch (const std::exception& error)
    {
        MessageBoxA(
            nullptr, 
            error.what(),
            "error",
            0);

        return 1;
    }
}

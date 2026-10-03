#include "../karelui/KarelUI.h"
#include "reg.h"
#include "instruction.h"
#include "program.h"
#include <exception>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <sstream>

using namespace karel_cpp;

static KarelUI* karelui = nullptr;
static int ip = 0;
static Karel* karel;

constexpr int RESERVED_ADDRESSES = 16;

static void karel_goto(int x, int y)
{
    Point pos = karel->GetPosition();

    karel->Face(Orientation::East());
    karel->Move(x - pos.x);
    karel->Face(Orientation::South());
    karel->Move(y - pos.y);
}

static void karel_goto_address(int karelptr)
{
    int index = (karelptr + RESERVED_ADDRESSES) / 3;
    int x = index % karel->GetGridSize().x;
    int y = index / karel->GetGridSize().y;
    karel_goto(x, y);
}

static uint8_t read_address(int karelptr)
{
    karel_goto_address(karelptr);
    return karel->GetColorBeneath()[(karelptr + RESERVED_ADDRESSES) % 3];
}

static void write_address(int karelptr, uint8_t value)
{
    karel_goto_address(karelptr);
    Color c = karel->GetColorBeneath();
    c[(karelptr + RESERVED_ADDRESSES) % 3] = value;
    karel->Paint(c);
}

static uint8_t read_register(reg reg)
{
    int address = (int)reg / 3;
    karel_goto(address, 0);
    return karel->GetColorBeneath()[(int)reg % 3];
}

static void write_register(reg reg, uint8_t value)
{
    int address = (int)reg / 3;
    karel_goto(address, 0);
    Color c = karel->GetColorBeneath();
    c[(int)reg % 3] = value;
    karel->Paint(c);
}

static bool Tick()
{
    uint8_t current = program[ip];
    switch (current)
    {
        case (uint8_t)instruction::EXIT:
            MessageBoxA(karelui->GetHwnd(), "program completed", "", MB_OK);
            return false;
        case (uint8_t)instruction::READ:
        {
            uint8_t karelptr = read_register(reg::r0);
            write_register(reg::r0, read_address(karelptr));
            break;
        }
        case (uint8_t)instruction::WRITE:
        {
            uint8_t karelptr = read_register(reg::r0);
            uint8_t value = read_register(reg::r1);
            write_address(karelptr, value);
            break;
        }
        case (uint8_t)instruction::MOV:
        {
            reg r = (reg)program[++ip];
            uint8_t value = program[++ip];
            write_register(r, value);
            break;
        }
        default:
        {
            std::ostringstream message;
            message << "unknown instruction: 0x"
                << std::hex << std::uppercase << std::setfill('0')
                << std::setw(2) << static_cast<unsigned int>(current);
            MessageBoxA(
                nullptr,
                message.str().c_str(),
                "error",
                0);
            return false;
        }
    }
    ip++;
    return true;
}

int main()
{
    // to enable virtual processing, uncomment:
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
        MessageBoxA(karelui->GetHwnd(), error.what(), "error", MB_OK);
        return 1;
    }
}

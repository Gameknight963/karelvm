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
static uint32_t ip = 0;
static Karel* karel;

constexpr uint32_t REGISTER_ADDRESSES = 17;
constexpr int GRID_SIZE_X = 32;
constexpr int GRID_SIZE_Y = 32;
constexpr int MAX_ADDRESS = GRID_SIZE_X * GRID_SIZE_Y - REGISTER_ADDRESSES;

static void karel_goto(int x, int y)
{
    Point pos = karel->GetPosition();

    karel->Face(Orientation::East());
    karel->Move(x - pos.x);
    karel->Face(Orientation::South());
    karel->Move(y - pos.y);
}

static uint32_t read_cell()
{
    Color c = karel->GetColorBeneath();
    return uint32_t(c.r) | (uint32_t(c.g) << 8) | (uint32_t(c.b) << 16);
}

static void write_cell(uint32_t value)
{
    karel->Paint(Color{
        static_cast<uint8_t>(value),
        static_cast<uint8_t>(value >> 8),
        static_cast<uint8_t>(value >> 16)
    });
}

static void karel_goto_address(uint32_t karelptr)
{
    if (karelptr >= MAX_ADDRESS)
        throw std::out_of_range("memory address outside the grid");
    uint32_t index = karelptr + REGISTER_ADDRESSES;
    int x = index % karel->GetGridSize().x;
    int y = index / karel->GetGridSize().x;
    karel_goto(x, y);
}

static uint32_t read_address(uint32_t karelptr)
{
    karel_goto_address(karelptr);
    return read_cell();
}

static void write_address(uint32_t karelptr, uint32_t value)
{
    karel_goto_address(karelptr);
    write_cell(value);
}

static uint32_t read_register(reg r)
{
    uint32_t index = static_cast<uint32_t>(r);
    if (index >= REGISTER_ADDRESSES)
        throw std::out_of_range("invalid register");
    karel_goto(index % GRID_SIZE_X, index / GRID_SIZE_X);
    return read_cell();
}

static void write_register(reg r, uint32_t value)
{
    uint32_t index = static_cast<uint32_t>(r);
    if (index >= REGISTER_ADDRESSES)
        throw std::out_of_range("invalid register");
    karel_goto(index % GRID_SIZE_X, index / GRID_SIZE_X);
    write_cell(value);
}

static void push_stack(reg r)
{
    uint32_t sp = read_register(reg::sp);
    if (sp == 0 || sp > MAX_ADDRESS)
        throw std::out_of_range("stack overflow");
    uint32_t value = read_register(r);
    write_address(--sp, value);
    // stack grows downwards
    write_register(reg::sp, sp);
}

static void pop_stack(reg r)
{
    uint32_t sp = read_register(reg::sp);
    if (sp >= MAX_ADDRESS)
        throw std::out_of_range("stack underflow");
    uint32_t value = read_address(sp);
    write_register(reg::sp, sp + 1);
    write_register(r, value);
}

static bool initialize()
{
    write_register(reg::sp, MAX_ADDRESS);
    return true;
}

static bool Tick()
{
    uint32_t current = program[ip];
    switch ((instruction)current)
    {
        case instruction::EXIT:
            MessageBoxA(karelui->GetHwnd(), "program completed", "", MB_OK);
            return false;
        case instruction::READ:
        {
            uint32_t karelptr = read_register(reg::a0);
            write_register(reg::a0, read_address(karelptr));
            break;
        }
        case instruction::WRITE:
        {
            uint32_t karelptr = read_register(reg::a0);
            uint32_t value = read_register(reg::a1);
            write_address(karelptr, value);
            break;
        }
        case instruction::MOV:
        {
            reg r = (reg)program[++ip];
            uint32_t value = program[++ip];
            write_register(r, value);
            break;
        }
        case instruction::PUSH:
        {
            reg r = (reg)program[++ip];
            push_stack(r);
            break;
        }
        case instruction::POP:
        {
            reg r = (reg)program[++ip];
            pop_stack(r);
            break;
        }
        default:
        {
            std::ostringstream message;
            message << "unknown instruction: 0x"
                << std::hex << std::uppercase << std::setfill('0')
                << std::setw(6) << current;
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
        karelui = new KarelUI(GRID_SIZE_X, GRID_SIZE_Y, Tick, initialize, 20, 200);
        karel = karelui->GetKarel();
        return karelui->Show();
    }
    catch (const std::exception& error)
    {
        MessageBoxA(karelui->GetHwnd(), error.what(), "error", MB_OK);
        return 1;
    }
}

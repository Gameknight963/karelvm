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
constexpr uint32_t PROGRAM_BYTES = sizeof(program);
constexpr uint32_t PROGRAM_CELLS = (PROGRAM_BYTES + 2) / 3;
static_assert(PROGRAM_CELLS <= MAX_ADDRESS, "program does not fit in Karel memory");

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
    if (sp <= PROGRAM_CELLS || sp > MAX_ADDRESS)
        throw std::out_of_range("stack overflow");
    uint32_t value = read_register(r);
    write_address(--sp, value);
    // stack grows downwards
    write_register(reg::sp, sp);
}

static void pop_stack(reg r)
{
    uint32_t sp = read_register(reg::sp);
    if (sp < PROGRAM_CELLS || sp >= MAX_ADDRESS)
        throw std::out_of_range("stack underflow");
    uint32_t value = read_address(sp);
    write_register(reg::sp, sp + 1);
    write_register(r, value);
}

static uint8_t fetch_byte()
{
    if (ip >= PROGRAM_BYTES)
        throw std::out_of_range("unexpected end of bytecode");
    uint32_t cell = read_address(ip / 3);
    uint8_t byte = static_cast<uint8_t>(cell >> ((ip % 3) * 8));
    ++ip;
    return byte;
}

static uint32_t fetch_word()
{
    uint32_t low = fetch_byte();
    uint32_t middle = fetch_byte();
    uint32_t high = fetch_byte();
    return low | (middle << 8) | (high << 16);
}

static bool initialize()
{
    // Memory layout: register cells, packed program cells, free memory/stack.
    // Pack bytes into red, green, then blue; zero-pad the final cell.
    for (uint32_t cell = 0; cell < PROGRAM_CELLS; ++cell)
    {
        uint32_t value = 0;
        for (uint32_t channel = 0; channel < 3; ++channel)
        {
            uint32_t offset = cell * 3 + channel;
            if (offset < PROGRAM_BYTES)
                value |= uint32_t(program[offset]) << (channel * 8);
        }
        write_address(cell, value);
    }
    ip = 0;
    write_register(reg::sp, MAX_ADDRESS);
    return true;
}

static bool Tick()
{
    uint8_t current = fetch_byte();
    switch ((instruction)current)
    {
        case instruction::EXIT:
            MessageBoxA(karelui->GetHwnd(), "program completed", "", MB_OK);
            return false;
        case instruction::READ:
        {
            reg destination = static_cast<reg>(fetch_byte());
            reg pointer = static_cast<reg>(fetch_byte());
            uint32_t karelptr = read_register(pointer);
            write_register(destination, read_address(karelptr));
            break;
        }
        case instruction::WRITE:
        {
            reg pointer = static_cast<reg>(fetch_byte());
            reg source = static_cast<reg>(fetch_byte());
            uint32_t karelptr = read_register(pointer);
            uint32_t value = read_register(source);
            write_address(karelptr, value);
            break;
        }
        case instruction::MOV:
        {
            reg r = static_cast<reg>(fetch_byte());
            uint32_t value = fetch_word();
            write_register(r, value);
            break;
        }
        case instruction::PUSH:
        {
            reg r = static_cast<reg>(fetch_byte());
            push_stack(r);
            break;
        }
        case instruction::POP:
        {
            reg r = static_cast<reg>(fetch_byte());
            pop_stack(r);
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

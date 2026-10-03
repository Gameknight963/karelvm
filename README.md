# karelvm
vm that runs on Karel

![picture of running program](https://i.imgur.com/BWoCpn8.png)

## Implemented now
 - A custom C++ reimplementation of karel. (I couldn't find one lol.)
 - A custom C++ implementation of a UI for karel (for Windows only)
 - A register-based bytecode that lets you write a wide variety of programs

## _Maybe_ in the future
 - Signed int operations, currently all arethmtetic operations assume unsigned
 - Syscalls defined by the host runtime, to allow for things like i/o
 - More ways to manipluate numbers such as bit shifts
 - Proper assembler. Currently there's a header file "program.h" with a few macros that make it _fine_ to edit but pretty annoying to make functions and such since it's not possible to make labels this way.
 - Maybe LLVM target cause why not (would allow programming karelvm in C)

### 24 bit quirk due to the constraints

karelvm natively uses 24 bit integers. This is because colors have three 8-bit channels (R, G, and B), and that can be abused to hold a 24-bit integer. 

An 8 bit integer would also work nicely if you split it across all 3 channels. karelvm actually used to do that, problem is that it just gives you so little addressable memory to work with. 

All 24-bit integers need to be represented as uint32_t in C++ code as MSVC has no way to use a native 24 bit type. This will not be an issue in C with Clang, since, if you use C23, it lets you define your own arbitrary-size integers:

```c
typedef unsigned _BitInt(24) uint24_t;
```

 > If you're waiting for Microsoft to finally leave C17 and implement C23, wait five minutes

So, each square corresponds with one memory address. However, the program itself is not loaded into memory the same way. It's split across color channels like earlier to save memory, since most instructions are a single byte.

When you pass a register as an operand, that's also a single byte.

The 24 bit integer limit is 1,6777,215 (16777215 without quotes) (0xFFFFFF as hex)

## Basic runtime structure

 - The first 16 phsyical addresses are reserved for registers. Your '0' starts after these, you never see them.
 - The program itself is loaded in the bytes after that. (Hmm, I wonder how hard it would be to make a hooking library targetting this runtime lol)
 - After that, normal addressing begins.
 - There's no `malloc` or `free`, you get what you get. And yes this means you have to use hardcoded addresses

## Get started

As stated earlier, this project only works on Windows (although I imagine it should work fine under Wine.)

karelvm has no dependencies, so you can just clone it and build it normally with the desktop development for c++ visual studio workload.


## Project Structure

 - `karel-cpp`: (Probably) cross-platform reimplementation of Karel. Pure C++, no Windows APIs
 - `karelui`: Win32 UI implementation wrapping a Karel instance.
 - `karelvm`: Uses `karelui` to implement the vm itself

## Current instruction set
- `0x00 EXIT()` - Displays “program completed” and stops execution
- `0x01 WRITE(pointer, source)` - Stores `source` register in the memory cell addressed by `pointer`
- `0x02 READ(destination, pointer)` - Loads the memory cell addressed by `pointer` into register `destination`
- `0x03 MOV(destination, value)` - Loads a 24-bit immediate into register `destination`
- `0x04 PUSH(source)` - Decrements `sp` and stores register `source` at the new stack top`
- `0x05 POP(destination)` - Loads the stack top into register `destination` and increments `sp`
- `0x06 ADD(a, b)` - Stores `a + b` in `a`, where both are registers, wrapping to 24 bits. Leaves `b` unchanged
- `0x07 SUB(a, b)` - Stores `a - b` in `a`, where both are registers, wrapping to 24 bits. Leaves `b` unchanged
- `0x08 MULT(a, b)` - Multiplies their original values. Stores the product’s low 24 bits in `a` and high 24 bits in `b`
- `0x09 DIV(a, b)` - Integer division: stores the quotient’s low 24 bits in `a` and high 24 bits in `b`. Faults on division by zero. Currently `b` becomes zero because I did not think about how big the result would be.
- `0x0A JMP(target)` - Sets `ip` to `target`
- `0x0B CALL(target)` - Pushes the next instruction’s address, then jumps to `target`
- `0x0C RET()` — Pops an address into `ip`
- `0x0D JZ(condition, target)` - Jumps if `condition` is zero
- `0x0E JNZ(condition, target)` - Jumps if `condition` is nonzero
- `0x0F CMP(destination, a, b)` - Compares unsigned values. Stores `0` if equal, `1` if `a > b`, or `0xFFFFFF` if `a < b`

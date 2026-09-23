#include "KarelUI.h"
#include <exception>
#include <iostream>

int main()
{
    try
    {
        KarelUI* window = nullptr;
        int ticks = 0;

        KarelUI ui(
            10, 10,
            [&]() -> bool
            {
                auto* karel = window->GetKarel();

                karel->Paint(Color{ 255, 180, 80 });

                if (ticks % 10 == 9)
                    karel->RotateRight();
                else
                    karel->Move();

                ++ticks;
                return true;
            },
            20,  // Cell size in pixels
            200  // Milliseconds between steps
        );

        window = &ui;
        return ui.Show();
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

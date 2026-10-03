#include <windows.h>
#include "KarelUI.h"
#include "../karel-cpp/karel.h"
#include "../karel-cpp/Color.h"
#include "ColorToColorRef.h"
#include <cstdint>
#include <system_error>
#include <functional>

#pragma warning(disable: 28251) // Inconsistent annotation

using namespace karel_cpp;

KarelUI::KarelUI(
    int x, int y,
    std::function<bool()> onTick,
    std::function<bool()> initialize,
    int squareSize,
    int tickRateMs)
{
    if (tickRateMs <= 0)
        throw std::invalid_argument("tick interval must be positive");
    this->karel = new Karel(x, y);
    this->squareSize = squareSize;
    this->tickRateMs = tickRateMs;
    this->onTick = onTick;
    this->initialize = initialize;

    if (isInitialized) return;
    isInitialized = true;

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"KarelWindow";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);

    if (!RegisterClassW(&wc))
    {
        DWORD error = GetLastError();
        throw std::system_error(
            error, std::system_category(), "RegisterClassW failed"
        );
    }
}

KarelUI::KarelUI(
    int x, int y,
    std::function<bool()> onTick,
    int squareSize = 20,
    int tickRateMs = 50) : 
    KarelUI(x, y, onTick, nullptr, squareSize, tickRateMs) {}

KarelUI::~KarelUI()
{
    delete this->karel;
}

int KarelUI::Show()
{
    const Point grid = karel->GetGridSize();
    RECT bounds{ 0, 0, grid.x * static_cast<int>(squareSize),
        grid.y * static_cast<int>(squareSize) };
    if (!AdjustWindowRectEx(&bounds, WS_OVERLAPPEDWINDOW, FALSE, 0))
        throw std::system_error(GetLastError(), std::system_category(), "AdjustWindowRectEx failed");

    HWND hwnd = CreateWindowExW(
        0,
        L"KarelWindow", // Our registered window class
        L"Karel", // Title bar text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        bounds.right - bounds.left, bounds.bottom - bounds.top,
        nullptr, // No parent window
        nullptr,
        GetModuleHandleW(nullptr),
        this
    );

    this->hwnd = hwnd;

    if (!hwnd)
    {
        DWORD error = GetLastError();
        throw std::system_error(
            error, std::system_category(), 
            "CreateWindowExW failed"
        );
    }

    karel->Changed = [hwnd] { InvalidateRect(hwnd, nullptr, FALSE); };

    if (initialize)
    {
        if (!initialize()) return 0;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);

    lastTickTime = std::chrono::steady_clock::now();
    tickDebtMs = 0;
    if (!SetTimer(hwnd, 1, 10, nullptr))
        throw std::runtime_error("SetTimer failed");

    MSG msg{};

    while (true)
    {
        BOOL result = GetMessageW(&msg, nullptr, 0, 0);
        if (result == -1) return 1; // Error
        if (result == 0) break; // Quit requested
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

Karel* KarelUI::GetKarel() const
{
    return karel;
}

HWND KarelUI::GetHwnd() const
{
    return this->hwnd;
}

void KarelUI::Draw(HDC dc) const
{
    const Point grid = karel->GetGridSize();
    const int cellSize = static_cast<int>(squareSize);
    // Stock drawing objects belong to Windows; restore them, but do not delete them.
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(DC_BRUSH));
    HGDIOBJ oldPen = SelectObject(dc, GetStockObject(DC_PEN));
    SetDCPenColor(dc, RGB(211, 211, 211));

    for (int y = 0; y < grid.y; ++y)
    {
        for (int x = 0; x < grid.x; ++x)
        {
            SetDCBrushColor(dc, ColorToColorRef(karel->GetColorAt(x, y)));
            Rectangle(dc, x * cellSize, y * cellSize,
                (x + 1) * cellSize + 1, (y + 1) * cellSize + 1);
        }
    }

    const Point position = karel->GetPosition();
    const Point direction = karel->GetOrientation().AsPoint();
    const int centerX = position.x * cellSize + cellSize / 2;
    const int centerY = position.y * cellSize + cellSize / 2;
    const int tip = cellSize * 8 / 20;
    const int tail = cellSize * 5 / 20;
    POINT arrow[] = {
        { centerX + direction.x * tip, centerY + direction.y * tip },
        { centerX - direction.x * tail - direction.y * tail,
          centerY - direction.y * tail + direction.x * tail },
        { centerX - direction.x * tail + direction.y * tail,
          centerY - direction.y * tail - direction.x * tail }
    };
    SetDCBrushColor(dc, RGB(0, 0, 0));
    SelectObject(dc, GetStockObject(NULL_PEN));
    Polygon(dc, arrow, 3);

    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
}

LRESULT CALLBACK KarelUI::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE)
    {
        auto* info = reinterpret_cast<CREATESTRUCTW*>(lParam);
        auto* This = static_cast<KarelUI*>(info->lpCreateParams);

        SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(This)
        );
    }
    if (message == WM_DESTROY)
    {
        auto* This = reinterpret_cast<KarelUI*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (This)
            This->karel->Changed = {};
        PostQuitMessage(0);
        return 0;
    }

    if (message == WM_ERASEBKGND)
        return 1; // WM_PAINT fills the entire background in the back buffer.

    if (message == WM_PAINT)
    {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd, &ps);

        auto* This = reinterpret_cast<KarelUI*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        RECT client{};
        GetClientRect(hwnd, &client);
        if (This && client.right > 0 && client.bottom > 0)
        {
            HDC buffer = CreateCompatibleDC(dc);
            HBITMAP bitmap = buffer ? CreateCompatibleBitmap(dc, client.right, client.bottom) : nullptr;
            if (bitmap)
            {
                HGDIOBJ oldBitmap = SelectObject(buffer, bitmap);
                FillRect(buffer, &client, GetSysColorBrush(COLOR_WINDOW));
                This->Draw(buffer);
                BitBlt(dc, 0, 0, client.right, client.bottom, buffer, 0, 0, SRCCOPY);
                SelectObject(buffer, oldBitmap);
                DeleteObject(bitmap);
            }
            else
            {
                FillRect(dc, &client, GetSysColorBrush(COLOR_WINDOW));
                This->Draw(dc);
            }
            if (buffer)
                DeleteDC(buffer);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }

    // messageboxes are modal dialogs and disable their owner, discard time spent paused
    if (message == WM_ENABLE)
    {
        auto* This = reinterpret_cast<KarelUI*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (This)
        {
            This->lastTickTime = std::chrono::steady_clock::now();
            This->tickDebtMs = 0;
            This->timingInterrupted = This->ticking;
        }
    }

    if (message == WM_TIMER && wParam == 1)
    {
        auto* This = reinterpret_cast<KarelUI*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (This && This->onTick)
        {
            using Clock = std::chrono::steady_clock;
            const auto now = Clock::now();
            if (This->ticking || !IsWindowEnabled(hwnd))
            {
                // a nested message loop must not execute ticks or accumulate debt
                This->lastTickTime = now;
                This->tickDebtMs = 0;
                This->timingInterrupted = This->ticking;
                return 0;
            }

            This->tickDebtMs += std::chrono::duration<double, std::milli>(
                now - This->lastTickTime).count();
            This->lastTickTime = now;
            This->timingInterrupted = false;
            This->ticking = true;
            struct TickGuard
            {
                bool& ticking;
                ~TickGuard() { ticking = false; }
            } guard{ This->ticking };

            // leave the message loop time to paint and respond to input
            const auto deadline = now + std::chrono::milliseconds(4);
            while (This->tickDebtMs >= This->tickRateMs)
            {
                This->tickDebtMs -= This->tickRateMs;
                if (!This->onTick())
                {
                    DestroyWindow(hwnd);
                    break;
                }
                if (!IsWindow(hwnd))
                    break;
                if (This->timingInterrupted)
                {
                    This->lastTickTime = Clock::now();
                    This->tickDebtMs = 0;
                    break;
                }
                if (Clock::now() >= deadline)
                    break;
            }
        }
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

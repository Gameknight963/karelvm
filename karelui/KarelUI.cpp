#include <windows.h>
#include "KarelUI.h"
#include "Karel.h"
#include <cstdint>
#include <system_error>
#include <functional>

#pragma warning(disable: 28251) // Inconsistent annotation

using namespace karel_cpp;

KarelUI::KarelUI(
    int x, int y, 
    std::function<bool()> onTick, 
    int squareSize = 20,
    int tickRateMs = 50)
{
    this->karel = new Karel(x, y);
    this->squareSize = squareSize;
    this->tickRateMs = tickRateMs;
    this->onTick = onTick;

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
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        bounds.right - bounds.left, bounds.bottom - bounds.top,
        nullptr, // No parent window
        nullptr,
        GetModuleHandleW(nullptr),
        this
    );

    if (!hwnd)
    {
        DWORD error = GetLastError();
        throw std::system_error(
            error, std::system_category(), 
            "CreateWindowExW failed"
        );
    }

    if (SetTimer(hwnd, 1, tickRateMs, nullptr) == 0)
    {
        throw std::runtime_error("SetTimer failed");
    }

    karel->Changed = [hwnd] { InvalidateRect(hwnd, nullptr, FALSE); };
    ShowWindow(hwnd, SW_SHOWNORMAL);

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
            SetDCBrushColor(dc, karel->GetColorAt(x, y).ToColorRef());
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

    if (message == WM_TIMER && wParam == 1)
    {
        auto* This = reinterpret_cast<KarelUI*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (This && This->onTick)
        {
            if (!This->onTick())
                DestroyWindow(hwnd);
        }
        return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

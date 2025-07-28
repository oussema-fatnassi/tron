#include <windows.h>
#include <string>
#include <iostream>
#include "../../Include/EngineExports.hpp"

LRESULT CALLBACK TronWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        std::cout << "[Window] Close button clicked\n";
        RequestEngineShutdown(); // Signal engine to stop
        return 0;

    case WM_DESTROY:
        std::cout << "[Window] Window destroyed\n";
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // Let D3D handle painting, just validate
        EndPaint(hwnd, &ps);
        return 0;
    }

    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

HWND CreateSimpleWindow(HINSTANCE hInstance, int width, int height, const std::wstring& title) {
    // 1. Define the window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = TronWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TronEngineWindowClass";
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        std::wcerr << L"RegisterClass failed: " << GetLastError() << std::endl;
        return nullptr;
    }

    // 2. Create the window
    HWND hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd) {
        std::cerr << "CreateWindowExW failed: " << GetLastError() << std::endl;
    }

    return hwnd;
}
#include <windows.h>
#include <string>
#include <iostream>

HWND CreateSimpleWindow(HINSTANCE hInstance, int width, int height, const std::wstring& title) {
    // 1. Define the window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = DefWindowProcW;
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
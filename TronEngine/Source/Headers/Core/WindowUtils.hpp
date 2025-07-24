#pragma once
#include <windows.h>
#include <string>

HWND CreateSimpleWindow(HINSTANCE hInstance, int width, int height, const std::wstring& title);
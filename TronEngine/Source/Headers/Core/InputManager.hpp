#pragma once

#include <Windows.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <unordered_map>

// Enum for key state
enum class KeyState {
    Up,
    Down,
    Pressed,   // Just pressed this frame
    Released   // Just released this frame
};

class InputManager {
public:
    InputManager();
    ~InputManager();

    // Call once per frame to update input states
    void Update();

    // Keyboard
    bool IsKeyDown(int vkey) const;
    bool IsKeyPressed(int vkey) const;
    bool IsKeyReleased(int vkey) const;

    // Mouse
    bool IsMouseButtonDown(int button) const; // 0: left, 1: right, 2: middle
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonReleased(int button) const;
    POINT GetMousePosition() const;
    int GetMouseWheelDelta() const;

    // Mouse lock functionality
    void SetMouseLock(bool lock);
    bool IsMouseLocked() const { return _mouseLocked; }
    void SetWindowHandle(HWND hwnd) { _hwnd = hwnd; }

    // Internal: called from window proc
    void OnKeyEvent(WPARAM wParam, bool down);
    void OnMouseButtonEvent(int button, bool down);
    void OnMouseMove(LPARAM lParam);
    void OnMouseWheel(WPARAM wParam);

private:
    std::unordered_map<int, KeyState> _keyStates;
    std::unordered_map<int, KeyState> _mouseButtonStates;
    POINT _mousePos;
    int _mouseWheelDelta;
    std::mutex _mutex;

    bool _mouseLocked = false;
    HWND _hwnd = nullptr;
    RECT _windowRect;
    int _centerX = 0;
    int _centerY = 0;
};
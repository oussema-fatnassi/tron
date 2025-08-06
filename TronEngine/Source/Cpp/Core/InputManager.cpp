#include "../../Headers/Core/InputManager.hpp"

InputManager::InputManager()
    : _mouseWheelDelta(0)
{
    _mousePos = {0, 0};
}

InputManager::~InputManager() {}

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

void InputManager::Update()
{
    std::lock_guard<std::mutex> lock(_mutex);

    // Update key states
    for (auto &[key, state] : _keyStates)
    {
        if (state == KeyState::Pressed)
            state = KeyState::Down;
        else if (state == KeyState::Released)
            state = KeyState::Up;
    }

    // Update mouse button states
    for (auto &[btn, state] : _mouseButtonStates)
    {
        if (state == KeyState::Pressed)
            state = KeyState::Down;
        else if (state == KeyState::Released)
            state = KeyState::Up;
    }

    _mouseWheelDelta = 0; // Reset wheel delta each frame
}

bool InputManager::IsKeyDown(int vkey) const
{
    auto it = _keyStates.find(vkey);
    return it != _keyStates.end() && (it->second == KeyState::Down || it->second == KeyState::Pressed);
}

bool InputManager::IsKeyPressed(int vkey) const
{
    auto it = _keyStates.find(vkey);
    return it != _keyStates.end() && it->second == KeyState::Pressed;
}

bool InputManager::IsKeyReleased(int vkey) const
{
    auto it = _keyStates.find(vkey);
    return it != _keyStates.end() && it->second == KeyState::Released;
}

bool InputManager::IsMouseButtonDown(int button) const
{
    auto it = _mouseButtonStates.find(button);
    return it != _mouseButtonStates.end() && (it->second == KeyState::Down || it->second == KeyState::Pressed);
}

bool InputManager::IsMouseButtonPressed(int button) const
{
    auto it = _mouseButtonStates.find(button);
    return it != _mouseButtonStates.end() && it->second == KeyState::Pressed;
}

bool InputManager::IsMouseButtonReleased(int button) const
{
    auto it = _mouseButtonStates.find(button);
    return it != _mouseButtonStates.end() && it->second == KeyState::Released;
}

POINT InputManager::GetMousePosition() const
{
    return _mousePos;
}

int InputManager::GetMouseWheelDelta() const
{
    return _mouseWheelDelta;
}

void InputManager::SetMouseLock(bool lock) {
    if (!_hwnd) return;

    _mouseLocked = lock;

    if (lock) {
        // Get window rectangle for clipping
        GetClientRect(_hwnd, &_windowRect);
        ClientToScreen(_hwnd, (LPPOINT)&_windowRect.left);
        ClientToScreen(_hwnd, (LPPOINT)&_windowRect.right);

        // Calculate center of window
        _centerX = (_windowRect.left + _windowRect.right) / 2;
        _centerY = (_windowRect.top + _windowRect.bottom) / 2;

        // Hide cursor and clip it to window
        ShowCursor(FALSE);
        ClipCursor(&_windowRect);

        // Reset mouse to center
        SetCursorPos(_centerX, _centerY);
        _mousePos.x = _centerX;
        _mousePos.y = _centerY;
    }
    else {
        // Show cursor and remove clipping
        ShowCursor(TRUE);
        ClipCursor(nullptr);
    }
}

void InputManager::OnKeyEvent(WPARAM wParam, bool down)
{
    std::lock_guard<std::mutex> lock(_mutex);
    int vkey = static_cast<int>(wParam);
    auto &state = _keyStates[vkey];

    if (down)
    {
        // Only set to Pressed if we were previously Up or Released
        // This prevents Windows key repeat from retriggering Pressed state
        if (state == KeyState::Up || state == KeyState::Released)
        {
            state = KeyState::Pressed;
        }
        // If already Pressed or Down, ignore (Windows key repeat)
        // Do nothing for key repeat
    }
    else
    {
        // Key released - only set to Released if we were Down or Pressed
        if (state == KeyState::Down || state == KeyState::Pressed)
        {
            state = KeyState::Released;
        }
    }
}

void InputManager::OnMouseButtonEvent(int button, bool down)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto &state = _mouseButtonStates[button];
    if (down)
    {
        if (state == KeyState::Up || state == KeyState::Released)
            state = KeyState::Pressed;
        else
            state = KeyState::Down;
    }
    else
    {
        if (state == KeyState::Down || state == KeyState::Pressed)
            state = KeyState::Released;
        else
            state = KeyState::Up;
    }
}

void InputManager::OnMouseMove(LPARAM lParam)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_mouseLocked && _hwnd) {
        // Get current cursor position
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        // Calculate delta from center
        int deltaX = cursorPos.x - _centerX;
        int deltaY = cursorPos.y - _centerY;

        // Store the delta as the "position" for mouse look
        _mousePos.x = deltaX;
        _mousePos.y = deltaY;

        // Reset cursor to center
        if (deltaX != 0 || deltaY != 0) {
            SetCursorPos(_centerX, _centerY);
        }
    }
    else {
        // Normal mouse behavior
        _mousePos.x = GET_X_LPARAM(lParam);
        _mousePos.y = GET_Y_LPARAM(lParam);
    }
}

void InputManager::OnMouseWheel(WPARAM wParam)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _mouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
}
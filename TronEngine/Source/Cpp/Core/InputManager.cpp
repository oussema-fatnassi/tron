#include "../../Headers/Core/InputManager.hpp"

InputManager::InputManager()
    : _mouseWheelDelta(0)
{
    _mousePos = { 0, 0 };
}

InputManager::~InputManager() {}

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

void InputManager::Update() {
    std::lock_guard<std::mutex> lock(_mutex);

    // Update key states
    for (auto& [key, state] : _keyStates) {
        if (state == KeyState::Pressed)
            state = KeyState::Down;
        else if (state == KeyState::Released)
            state = KeyState::Up;
    }
    // Update mouse button states
    for (auto& [btn, state] : _mouseButtonStates) {
        if (state == KeyState::Pressed)
            state = KeyState::Down;
        else if (state == KeyState::Released)
            state = KeyState::Up;
    }
    _mouseWheelDelta = 0; // Reset wheel delta each frame
}

bool InputManager::IsKeyDown(int vkey) const {
    auto it = _keyStates.find(vkey);
    return it != _keyStates.end() && (it->second == KeyState::Down || it->second == KeyState::Pressed);
}

bool InputManager::IsKeyPressed(int vkey) const {
    auto it = _keyStates.find(vkey);
    return it != _keyStates.end() && it->second == KeyState::Pressed;
}

bool InputManager::IsKeyReleased(int vkey) const {
    auto it = _keyStates.find(vkey);
    return it != _keyStates.end() && it->second == KeyState::Released;
}

bool InputManager::IsMouseButtonDown(int button) const {
    auto it = _mouseButtonStates.find(button);
    return it != _mouseButtonStates.end() && (it->second == KeyState::Down || it->second == KeyState::Pressed);
}

bool InputManager::IsMouseButtonPressed(int button) const {
    auto it = _mouseButtonStates.find(button);
    return it != _mouseButtonStates.end() && it->second == KeyState::Pressed;
}

bool InputManager::IsMouseButtonReleased(int button) const {
    auto it = _mouseButtonStates.find(button);
    return it != _mouseButtonStates.end() && it->second == KeyState::Released;
}

POINT InputManager::GetMousePosition() const {
    return _mousePos;
}

int InputManager::GetMouseWheelDelta() const {
    return _mouseWheelDelta;
}

void InputManager::OnKeyEvent(WPARAM wParam, bool down) {
    std::lock_guard<std::mutex> lock(_mutex);
    int vkey = static_cast<int>(wParam);
    auto& state = _keyStates[vkey];
    if (down) {
        if (state == KeyState::Up || state == KeyState::Released)
            state = KeyState::Pressed;
        else
            state = KeyState::Down;
    } else {
        if (state == KeyState::Down || state == KeyState::Pressed)
            state = KeyState::Released;
        else
            state = KeyState::Up;
    }
}

void InputManager::OnMouseButtonEvent(int button, bool down) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto& state = _mouseButtonStates[button];
    if (down) {
        if (state == KeyState::Up || state == KeyState::Released)
            state = KeyState::Pressed;
        else
            state = KeyState::Down;
    } else {
        if (state == KeyState::Down || state == KeyState::Pressed)
            state = KeyState::Released;
        else
            state = KeyState::Up;
    }
}

void InputManager::OnMouseMove(LPARAM lParam) {
    std::lock_guard<std::mutex> lock(_mutex);
    _mousePos.x = GET_X_LPARAM(lParam);
    _mousePos.y = GET_Y_LPARAM(lParam);
}

void InputManager::OnMouseWheel(WPARAM wParam) {
    std::lock_guard<std::mutex> lock(_mutex);
    _mouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
}
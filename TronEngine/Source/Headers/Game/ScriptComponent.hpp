#pragma once
#include "Component.hpp"
#include "../../Include/ScriptAPI.hpp"

// Internal Script component that holds a pointer to user's script
struct Script : Component {
    ScriptBase* userScript = nullptr;  // Store pointer to user's script

    // Forward the Update call to the user's script
    void Update(float deltaTime) {
        if (userScript) {
            userScript->Update(deltaTime);
        }
    }
};
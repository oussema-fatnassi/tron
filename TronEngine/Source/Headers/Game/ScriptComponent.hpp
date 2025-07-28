// Headers/Game/ScriptComponent.hpp - Simple lifecycle version
#pragma once
#include "Component.hpp"
#include "../../Include/ScriptAPI.hpp"
#include <iostream>

struct Script : public Component {
    ScriptBase* userScript = nullptr;
    bool started = false;

    // Constructor
    Script() = default;

    // Destructor - automatic OnDestroy() call
    ~Script() {
        if (userScript) {
            try {
                userScript->OnDestroy();
                std::cout << "[Script] OnDestroy() called for entity " << userScript->entity << std::endl;
            }
            catch (const std::exception& e) {
                std::cout << "[Script] Error in OnDestroy(): " << e.what() << std::endl;
            }

            delete userScript;
            userScript = nullptr;
        }
    }

    // Disable copy (we manage raw pointers)
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;

    // Enable move for efficiency
    Script(Script&& other) noexcept
        : userScript(other.userScript), started(other.started) {
        other.userScript = nullptr;
        other.started = false;
    }

    Script& operator=(Script&& other) noexcept {
        if (this != &other) {
            if (userScript) {
                userScript->OnDestroy();
                delete userScript;
            }
            userScript = other.userScript;
            started = other.started;
            other.userScript = nullptr;
            other.started = false;
        }
        return *this;
    }

    // Call Start() once
    void EnsureStarted() {
        if (!started && userScript) {
            try {
                userScript->Start();
                started = true;
                std::cout << "[Script] Start() called for entity " << userScript->entity << std::endl;
            }
            catch (const std::exception& e) {
                std::cout << "[Script] Error in Start(): " << e.what() << std::endl;
            }
        }
    }

    // Update script (your existing logic + lifecycle)
    void Update(float deltaTime) {
        if (userScript) {
            // Ensure Start() is called before first Update()
            EnsureStarted();

            // Call user's Update() method (your existing system)
            try {
                userScript->Update(deltaTime);
            }
            catch (const std::exception& e) {
                std::cout << "[Script] Error in Update(): " << e.what() << std::endl;
            }
        }
    }
};
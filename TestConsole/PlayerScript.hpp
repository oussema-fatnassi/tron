// PlayerScript.hpp - Enhanced with Camera FPS controls
#pragma once
#include "TronEngine.hpp"
#include <iostream>
#include <string>
#include <iomanip>

class PlayerScript : public ScriptBase {
private:
    std::string playerName;
    bool hasStoppedAtTarget = false;
    float targetX = 250.0f;
    int updateCount = 0;

    // Camera and movement settings
    float movementSpeed = 5.0f;
    float mouseSensitivity = 0.1f;
    
    // Mouse look state
    bool firstMouseInput = true;
    int lastMouseX = 640; // Screen center
    int lastMouseY = 360;
    
    // Camera rotation (stored separately for smooth control)
    float cameraPitch = 0.0f;
    float cameraYaw = 0.0f;

public:
    PlayerScript(const std::string& name = "Player")
        : playerName(name), hasStoppedAtTarget(false) {
        std::cout << "[" << playerName << "] FPS Player Script instance created" << std::endl;
    }

    void Start() override {
        std::cout << "\n[" << playerName << "] START() - Setting up FPS player on entity " << entity << std::endl;

        // Set initial position (spawn point)
        float spawnX = 0.0f, spawnY = 2.0f, spawnZ = 0.0f; // Slightly above ground
        
        if (AddTransformComponent(entity, spawnX, spawnY, spawnZ)) {
            std::cout << "[" << playerName << "] Transform component added at spawn point ("
                << spawnX << ", " << spawnY << ", " << spawnZ << ")" << std::endl;
        }

        // Add a visual cube for the player (optional - for debugging)
        if (AddMeshRendererComponent(entity, PRIMITIVE_CUBE, "RainbowShader")) {
            SetMeshRendererColor(entity, 0.0f, 1.0f, 0.0f, 1.0f); // Green player
            std::cout << "[" << playerName << "] Visual representation added" << std::endl;
        }

        std::cout << "[" << playerName << "] FPS Player initialization complete!" << std::endl;
        std::cout << "[" << playerName << "] Controls: WASD to move, Mouse to look around, Space/Shift for up/down" << std::endl;
    }

    void Update(float deltaTime) override {
        updateCount++;

        // Process FPS camera and movement controls
        ProcessFPSControls(deltaTime);

        // Optional: Your original logic can still be here
        // CheckTargetReached();

        // Debug info every few seconds
        if (updateCount % 240 == 0) { // Every ~2 seconds at 120fps
            PrintPlayerStatus();
        }
    }

    void OnDestroy() override {
        std::cout << "\n[" << playerName << "] OnDestroy() called on entity " << entity << std::endl;
        std::cout << "[" << playerName << "] Final stats:" << std::endl;
        std::cout << "[" << playerName << "]   - Total updates: " << updateCount << std::endl;
        std::cout << "[" << playerName << "] FPS Player cleanup complete!\n" << std::endl;
    }

private:
    void ProcessFPSControls(float deltaTime) {
        // This is a simplified FPS control system
        // In a real implementation, you'd want to integrate with the Camera class
        
        // For now, we'll just demonstrate basic movement
        // The actual camera integration would be done in the Engine class
        
        // Get current position
        float currentX, currentY, currentZ;
        if (!GetTransformComponent(entity, &currentX, &currentY, &currentZ)) {
            return; // No transform component
        }

        // Calculate movement (this is a placeholder - real FPS controls would use camera direction)
        float moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
        float speed = movementSpeed * deltaTime;

        // Simple movement for demonstration
        // In real implementation, this would use camera forward/right vectors
        if (IsKeyDown('W')) moveZ += speed;   // Forward
        if (IsKeyDown('S')) moveZ -= speed;   // Backward  
        if (IsKeyDown('A')) moveX -= speed;   // Left
        if (IsKeyDown('D')) moveX += speed;   // Right
        if (IsKeyDown(VK_SPACE)) moveY += speed;   // Up
        if (IsKeyDown(VK_SHIFT)) moveY -= speed;   // Down

        // Apply movement if any key was pressed
        if (moveX != 0.0f || moveY != 0.0f || moveZ != 0.0f) {
            // Update transform component with new position
            // Note: We need to add a function to update existing transform
            RemoveTransformComponent(entity);
            AddTransformComponent(entity, currentX + moveX, currentY + moveY, currentZ + moveZ);
        }
    }

    void CheckTargetReached() {
        float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z)) {
            if (x >= targetX && !hasStoppedAtTarget) {
                std::cout << "[" << playerName << "] TARGET REACHED at x="
                    << x << " after " << updateCount << " updates!" << std::endl;
                hasStoppedAtTarget = true;
            }
        }
    }

    void PrintPlayerStatus() {
        float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z)) {
            std::cout << "[" << playerName << "] Position: (" 
                << std::fixed << std::setprecision(2)
                << x << ", " << y << ", " << z << ")" << std::endl;
        }
    }

    // Placeholder input functions (these would be provided by the engine)
    bool IsKeyDown(int vkey) {
        // TODO: This should call the engine's input system
        // For now, return false as placeholder
        return false;
    }

public:
    // Public interface
    const std::string& GetName() const { return playerName; }
    bool HasReachedTarget() const { return hasStoppedAtTarget; }
    int GetUpdateCount() const { return updateCount; }
    
    void SetMovementSpeed(float speed) { movementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
};
// TestConsole/FirstPersonCameraScript.hpp - FIXED MOVEMENT DIRECTION
#pragma once
#include "TronEngine.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>

class FirstPersonCameraScript : public ScriptBase {
private:
    std::string playerName;
    int updateCount = 0;
    
    // Movement settings
    float movementSpeed = 5.0f;
    float mouseSensitivity = 0.002f;  // Slightly increased for easier testing
    
    // Camera rotation state
    float cameraYaw = 0.0f;    // Left/right rotation (Y-axis)
    float cameraPitch = 0.0f;  // Up/down rotation (X-axis)
    
    // Mouse state tracking
    bool firstMouse = true;
    int lastMouseX = 0;
    int lastMouseY = 0;
    
    // Camera limits
    const float maxPitch = 1.5f;   // ~85 degrees
    const float minPitch = -1.5f;  // ~-85 degrees

    bool mouseLocked = false;

public:
    FirstPersonCameraScript(const std::string& name = "FirstPersonPlayer")
        : playerName(name) {
        std::cout << "[" << playerName << "] Camera Script created\n";
    }

    void Start() override {
        std::cout << "\n[" << playerName << "] START() - Setting up camera on entity " << entity << "\n";
        
        // Set initial camera position (start back from origin, looking forward)
        float spawnX = 0.0f, spawnY = 2.0f, spawnZ = 5.0f;
        
        // Add transform component
        if (AddTransformComponent(entity, spawnX, spawnY, spawnZ)) {
            std::cout << "[" << playerName << "] ✓ Transform added at (" 
                      << spawnX << ", " << spawnY << ", " << spawnZ << ")\n";
        } else {
            std::cout << "[" << playerName << "] ✗ Failed to add Transform component\n";
            return;
        }
        
        // IMPORTANT: Register this entity as the camera
        if (SetCameraEntity(entity)) {
            std::cout << "[" << playerName << "] ✓ Entity registered as camera\n";
        } else {
            std::cout << "[" << playerName << "] ✗ Failed to register as camera entity\n";
        }
        
        // Initialize camera rotation - LOOK TOWARD ORIGIN (negative Z direction)
        cameraYaw = -1.0f;     // Face forward (toward negative Z)
        cameraPitch = 0.0f;   // Level horizon
        SetTransformRotation(entity, cameraPitch, cameraYaw, 0.0f);
        
        std::cout << "[" << playerName << "] ✓ Camera setup - should see red box ahead!\n";
        std::cout << "[" << playerName << "] Controls: ZQSD=move, Mouse=look, P=debug\n";

        std::cout << "[" << playerName << "] Mouse locked to center for 360° look\n";
        std::cout << "[" << playerName << "] Press TAB to toggle mouse lock\n";
    }

    void Update(float deltaTime) override {
        updateCount++;
        
        // Process input
        ProcessMouseLook();
        ProcessMovement(deltaTime);
        
        // Debug output every few seconds
        if (updateCount % 300 == 0) { // Every ~2.5 seconds at 120fps
            PrintStatus();
        }
        
        // Manual debug
        if (IsKeyPressed(VK_TAB)) {
            mouseLocked = !mouseLocked;
            EnableMouseLock(mouseLocked);
            std::cout << "[" << playerName << "] Mouse lock: "
                << (mouseLocked ? "ON" : "OFF") << "\n";
        }
    }

    void OnDestroy() override {
        std::cout << "\n[" << playerName << "] OnDestroy() - Camera destroyed\n";
    }

private:
    void ProcessMouseLook() {
        if (!mouseLocked) {
            // Store position before rotation
            float posX, posY, posZ;
            GetTransformComponent(entity, &posX, &posY, &posZ);

            int mouseX, mouseY;
            GetMousePosition(&mouseX, &mouseY);

            if (firstMouse) {
                lastMouseX = mouseX;
                lastMouseY = mouseY;
                firstMouse = false;
                return;
            }

            int deltaX = mouseX - lastMouseX;
            int deltaY = mouseY - lastMouseY;

            lastMouseX = mouseX;
            lastMouseY = mouseY;

            if (deltaX == 0 && deltaY == 0) return;

            // Update rotation ONLY
            cameraYaw -= deltaX * mouseSensitivity;
            cameraPitch += deltaY * mouseSensitivity;
            cameraPitch = std::clamp(cameraPitch, minPitch, maxPitch);

            // Set ONLY rotation, not position!
            SetTransformRotation(entity, cameraPitch, cameraYaw, 0.0f);

            // Verify position didn't change
            float newX, newY, newZ;
            GetTransformComponent(entity, &newX, &newY, &newZ);
            if (newX != posX || newY != posY || newZ != posZ) {
                std::cout << "[ERROR] Position changed during rotation! Was ("
                    << posX << "," << posY << "," << posZ << ") now ("
                    << newX << "," << newY << "," << newZ << ")\n";
            }
            return;
        }
        int deltaX, deltaY;
        GetMousePosition(&deltaX, &deltaY);

        if (deltaX == 0 && deltaY == 0) return;

        // Apply rotation based on delta
        cameraYaw -= deltaX * mouseSensitivity;
        cameraPitch += deltaY * mouseSensitivity;
        cameraPitch = std::clamp(cameraPitch, minPitch, maxPitch);

        SetTransformRotation(entity, cameraPitch, cameraYaw, 0.0f);
    }
    
    void ProcessMovement(float deltaTime) {
        float currentX, currentY, currentZ;
        if (!GetTransformComponent(entity, &currentX, &currentY, &currentZ)) return;

        float frameSpeed = movementSpeed * deltaTime;
        float moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
        bool moved = false;

        // Forward/Backward (Z/S keys)
        if (IsKeyDown('Z') || IsKeyDown('z')) {
            // Move forward in camera direction
            moveX += sinf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moveY += -sinf(cameraPitch) * frameSpeed;
            moveZ += -cosf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Moving FORWARD\n";
        }
        if (IsKeyDown('S') || IsKeyDown('s')) {
            // Move backward (opposite of forward)
            moveX -= sinf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moveY -= -sinf(cameraPitch) * frameSpeed;
            moveZ -= -cosf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Moving BACKWARD\n";
        }

        // Left/Right strafing (Q/D keys) - FIXED: Inverted the direction
        if (IsKeyDown('Q') || IsKeyDown('q')) {
            // Strafe left - use correct right vector calculation
            float rightX = cosf(cameraYaw);
            float rightZ = sinf(cameraYaw);
            moveX += rightX * frameSpeed;  // Changed from -= to +=
            moveZ += rightZ * frameSpeed;  // Changed from -= to +=
            moved = true;
            std::cout << "[" << playerName << "] Strafing LEFT\n";
        }
        if (IsKeyDown('D') || IsKeyDown('d')) {
            // Strafe right
            float rightX = cosf(cameraYaw);
            float rightZ = sinf(cameraYaw);
            moveX -= rightX * frameSpeed;  // Changed from += to -=
            moveZ -= rightZ * frameSpeed;  // Changed from += to -=
            moved = true;
            std::cout << "[" << playerName << "] Strafing RIGHT\n";
        }

        // Vertical movement (Space/Shift) - ADDED
        if (IsKeyDown(VK_SPACE)) {
            // Move up (positive Y in world space)
            moveY += frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Moving UP\n";
        }
        if (IsKeyDown(VK_SHIFT)) {
            // Move down (negative Y in world space)
            moveY -= frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Moving DOWN\n";
        }

        // Apply movement
        if (moved) {
            float newX = currentX + moveX;
            float newY = currentY + moveY;
            float newZ = currentZ + moveZ;
            SetTransformPosition(entity, newX, newY, newZ);
            std::cout << "[" << playerName << "] New position: ("
                << std::fixed << std::setprecision(2)
                << newX << ", " << newY << ", " << newZ << ")\n";
        }
    }
    
    void PrintStatus() {
        float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z)) {
            std::cout << "\n[" << playerName << "] === CAMERA STATUS ===\n";
            std::cout << "  Position: (" << std::fixed << std::setprecision(2)
                      << x << ", " << y << ", " << z << ")\n";
            std::cout << "  Yaw: " << RadiansToDegrees(cameraYaw) << "°\n";
            std::cout << "  Pitch: " << RadiansToDegrees(cameraPitch) << "°\n";
            
            // Calculate and show where we're looking
            float forwardX = sinf(cameraYaw) * cosf(cameraPitch);
            float forwardY = -sinf(cameraPitch);
            float forwardZ = -cosf(cameraYaw) * cosf(cameraPitch);
            
            std::cout << "  Looking at: (" << (x + forwardX) << ", " << (y + forwardY) << ", " << (z + forwardZ) << ")\n";
            std::cout << "  Forward vector: (" << forwardX << ", " << forwardY << ", " << forwardZ << ")\n";
            std::cout << "==============================\n\n";
        }
    }
    
    float RadiansToDegrees(float radians) const {
        return radians * 180.0f / 3.14159265358979323846f;
    }

public:
    void SetMovementSpeed(float speed) { movementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
};
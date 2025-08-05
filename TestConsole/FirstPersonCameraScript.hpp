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
        cameraYaw = 0.0f;     // Face forward (toward negative Z)
        cameraPitch = 0.0f;   // Level horizon
        SetTransformRotation(entity, cameraPitch, cameraYaw, 0.0f);
        
        std::cout << "[" << playerName << "] ✓ Camera setup - should see red box ahead!\n";
        std::cout << "[" << playerName << "] Controls: ZQSD=move, Mouse=look, P=debug\n";
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
        if (IsKeyPressed('P') || IsKeyPressed('p')) {
            PrintStatus();
            PrintCameraMatrices(); // Print matrix debug info
        }
    }

    void OnDestroy() override {
        std::cout << "\n[" << playerName << "] OnDestroy() - Camera destroyed\n";
    }

private:
    void ProcessMouseLook() {
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
        
        // Apply mouse sensitivity
        cameraYaw += deltaX * mouseSensitivity;
        cameraPitch += deltaY * mouseSensitivity; // Invert Y
        
        // Clamp pitch
        //cameraPitch = std::clamp(cameraPitch, minPitch, maxPitch);
        cameraPitch = max(-1.5f, min(1.5f, cameraPitch));
        
        // Update transform rotation
        SetTransformRotation(entity, cameraPitch, cameraYaw, 0.0f);
        
        // Debug mouse movement occasionally
        static int mouseDebugCount = 0;
        if ((++mouseDebugCount % 120) == 0) { // Every 1 second
            std::cout << "[" << playerName << "] Mouse: yaw=" << RadiansToDegrees(cameraYaw) 
                      << "° pitch=" << RadiansToDegrees(cameraPitch) << "°\n";
        }
    }
    
    void ProcessMovement(float deltaTime) {
        float currentX, currentY, currentZ;
        if (!GetTransformComponent(entity, &currentX, &currentY, &currentZ)) return;
        
        float frameSpeed = movementSpeed * deltaTime;
        float moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
        bool moved = false;
        
        // FIXED: Forward/Backward movement (match the camera matrix calculation)
        if (IsKeyDown('Z') || IsKeyDown('z')) {
            // Move forward - same calculation as camera matrix
            moveX += sinf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moveY += -sinf(cameraPitch) * frameSpeed;
            moveZ += -cosf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Moving FORWARD\n";
        }
        if (IsKeyDown('S') || IsKeyDown('s')) {
            // Move backward - opposite of forward
            moveX -= sinf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moveY -= -sinf(cameraPitch) * frameSpeed;
            moveZ -= -cosf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Moving BACKWARD\n";
        }
        
        // FIXED: Left/Right strafing (perpendicular to forward)
        if (IsKeyDown('Q') || IsKeyDown('q')) {
            // Strafe left (90 degrees left from forward direction)
            moveX += sinf(cameraYaw - 1.5708f) * frameSpeed; // -90 degrees
            moveZ += -cosf(cameraYaw - 1.5708f) * frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Strafing LEFT\n";
        }
        if (IsKeyDown('D') || IsKeyDown('d')) {
            // Strafe right (90 degrees right from forward direction)
            moveX += sinf(cameraYaw + 1.5708f) * frameSpeed; // +90 degrees
            moveZ += -cosf(cameraYaw + 1.5708f) * frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Strafing RIGHT\n";
        }
        
        // Vertical movement (world-space up/down)
        if (IsKeyDown(VK_SPACE)) {
            moveY += frameSpeed;
            moved = true;
            std::cout << "[" << playerName << "] Moving UP\n";
        }
        if (IsKeyDown(VK_SHIFT)) {
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
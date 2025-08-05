// Create new file: TestConsole/FirstPersonCameraScript.hpp
// FIXED VERSION - Cleaner and more robust

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
    float mouseSensitivity = 0.001f;  // Reduced for better control
    
    // Camera rotation state
    float cameraYaw = 0.0f;    // Left/right rotation (Y-axis)
    float cameraPitch = 0.0f;  // Up/down rotation (X-axis)
    
    // Mouse state tracking
    bool firstMouse = true;
    int lastMouseX = 0;
    int lastMouseY = 0;
    
    // Camera limits (prevent camera flip)
    const float maxPitch = 1.5f;   // ~85 degrees in radians
    const float minPitch = -1.5f;  // ~-85 degrees in radians

public:
    FirstPersonCameraScript(const std::string& name = "FirstPersonPlayer")
        : playerName(name) {
        std::cout << "[" << playerName << "] First Person Camera Script created\n";
    }

    void Start() override {
        std::cout << "\n[" << playerName << "] START() - Setting up first person camera on entity " << entity << "\n";
        
        // Set initial spawn position (start above and back from origin)
        float spawnX = 0.0f, spawnY = 2.0f, spawnZ = 5.0f;
        
        // Add transform component
        if (AddTransformComponent(entity, spawnX, spawnY, spawnZ)) {
            std::cout << "[" << playerName << "] ✓ Transform added at spawn point (" 
                      << spawnX << ", " << spawnY << ", " << spawnZ << ")\n";
        } else {
            std::cout << "[" << playerName << "] ✗ Failed to add Transform component\n";
            return;
        }
        
        // Optional: Add visual representation (small cube to see where camera is)
        if (AddMeshRendererComponent(entity, PRIMITIVE_CUBE, "blue")) {
            SetMeshRendererColor(entity, 0.0f, 1.0f, 0.0f, 0.3f); // Semi-transparent green
            SetTransformUniformScale(entity, 0.1f); // Very small cube
            std::cout << "[" << playerName << "] ✓ Visual representation added (tiny green cube)\n";
        }
        
        // Add collision detection for player
        if (AddBoxColliderComponent(entity, 0.6f, 1.8f, 0.6f, false)) {
            std::cout << "[" << playerName << "] ✓ Player collider added (0.6 x 1.8 x 0.6)\n";
        }
        
        // Initialize camera rotation (look toward origin)
        cameraYaw = 0.0f;     // Face forward (negative Z)
        cameraPitch = 0.0f;   // Level horizon
        SetTransformRotation(entity, cameraPitch, cameraYaw, 0.0f);
        
        std::cout << "[" << playerName << "] ✓ First person camera setup complete!\n";
        PrintControls();
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
        
        // Manual status print
        if (IsKeyPressed('P') || IsKeyPressed('p')) {
            PrintStatus();
        }
    }

    void OnDestroy() override {
        std::cout << "\n[" << playerName << "] OnDestroy() - First person camera entity destroyed\n";
        std::cout << "[" << playerName << "] Total updates: " << updateCount << "\n";
        std::cout << "[" << playerName << "] Camera cleanup complete!\n\n";
    }

    void OnTriggerEnter(uint32_t otherEntity) override {
        std::cout << "[" << playerName << "] Player collided with entity " << otherEntity << "\n";
        // Player collision - could add health system, damage, etc. here
    }

private:
    void ProcessMouseLook() {
        int mouseX, mouseY;
        GetMousePosition(&mouseX, &mouseY);
        
        // Initialize mouse position on first frame
        if (firstMouse) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
            return;
        }
        
        // Calculate mouse movement delta
        int deltaX = mouseX - lastMouseX;
        int deltaY = mouseY - lastMouseY;
        
        // Update last position
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        
        // Only process if mouse actually moved
        if (deltaX == 0 && deltaY == 0) {
            return;
        }
        
        // Apply mouse sensitivity and update camera rotation
        cameraYaw += deltaX * mouseSensitivity;      // Horizontal rotation
        cameraPitch -= deltaY * mouseSensitivity;    // Vertical rotation (inverted)
        
        // Clamp pitch to prevent camera flipping
        cameraPitch = std::clamp(cameraPitch, minPitch, maxPitch);
        
        // Apply rotation to entity transform
        SetTransformRotation(entity, cameraPitch, cameraYaw, 0.0f);
        
        // Debug output for mouse look (reduced frequency)
        static int mouseDebugCount = 0;
        if ((++mouseDebugCount % 60) == 0) { // Every ~0.5 seconds
            std::cout << "[" << playerName << "] Mouse look: yaw=" 
                      << RadiansToDegrees(cameraYaw) << "°, pitch=" 
                      << RadiansToDegrees(cameraPitch) << "° (delta: " 
                      << deltaX << "," << deltaY << ")\n";
        }
    }
    
    void ProcessMovement(float deltaTime) {
        // Get current position
        float currentX, currentY, currentZ;
        if (!GetTransformComponent(entity, &currentX, &currentY, &currentZ)) {
            return; // No transform component available
        }
        
        // Calculate movement speed for this frame
        float frameSpeed = movementSpeed * deltaTime;
        
        // Movement accumulator
        float moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
        bool moved = false;
        
        // Forward/Backward movement (relative to camera direction)
        if (IsKeyDown('Z') || IsKeyDown('z')) {
            // Move forward in the direction the camera is facing
            moveX += -sinf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moveY += -sinf(cameraPitch) * frameSpeed;
            moveZ += -cosf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moved = true;
        }
        if (IsKeyDown('S') || IsKeyDown('s')) {
            // Move backward (opposite of forward)
            moveX -= -sinf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moveY -= -sinf(cameraPitch) * frameSpeed;
            moveZ -= -cosf(cameraYaw) * cosf(cameraPitch) * frameSpeed;
            moved = true;
        }
        
        // Left/Right strafing (perpendicular to forward direction)
        if (IsKeyDown('Q') || IsKeyDown('q')) {
            // Strafe left (90 degrees left from forward direction)
            moveX += -cosf(cameraYaw + 1.5708f) * frameSpeed; // +90 degrees
            moveZ += -sinf(cameraYaw + 1.5708f) * frameSpeed;
            moved = true;
        }
        if (IsKeyDown('D') || IsKeyDown('d')) {
            // Strafe right (90 degrees right from forward direction)
            moveX += -cosf(cameraYaw - 1.5708f) * frameSpeed; // -90 degrees
            moveZ += -sinf(cameraYaw - 1.5708f) * frameSpeed;
            moved = true;
        }
        
        // Vertical movement (world-space up/down)
        if (IsKeyDown(VK_SPACE)) {
            moveY += frameSpeed; // Move up
            moved = true;
        }
        if (IsKeyDown(VK_SHIFT)) {
            moveY -= frameSpeed; // Move down
            moved = true;
        }
        
        // Apply movement if any keys were pressed
        if (moved) {
            float newX = currentX + moveX;
            float newY = currentY + moveY;
            float newZ = currentZ + moveZ;
            
            // Update position
            SetTransformPosition(entity, newX, newY, newZ);
            
            // Debug output for movement (reduced frequency)
            static int moveDebugCount = 0;
            if ((++moveDebugCount % 60) == 0) { // Every ~0.5 seconds
                std::cout << "[" << playerName << "] Moved to: (" 
                          << std::fixed << std::setprecision(2)
                          << newX << ", " << newY << ", " << newZ << ")\n";
            }
        }
    }
    
    void PrintStatus() {
        float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z)) {
            std::cout << "\n[" << playerName << "] === CAMERA STATUS ===\n";
            std::cout << "  Position: (" << std::fixed << std::setprecision(2)
                      << x << ", " << y << ", " << z << ")\n";
            std::cout << "  Yaw (left/right): " << RadiansToDegrees(cameraYaw) << "°\n";
            std::cout << "  Pitch (up/down): " << RadiansToDegrees(cameraPitch) << "°\n";
            std::cout << "  Movement Speed: " << movementSpeed << " units/sec\n";
            std::cout << "  Mouse Sensitivity: " << mouseSensitivity << "\n";
            std::cout << "  Updates: " << updateCount << "\n";
            std::cout << "==============================\n\n";
        }
    }
    
    void PrintControls() {
        std::cout << "\n[" << playerName << "] === FIRST PERSON CONTROLS ===\n";
        std::cout << "  Z/S      - Move forward/backward\n";
        std::cout << "  Q/D      - Strafe left/right\n";
        std::cout << "  Mouse    - Look around (first person)\n";
        std::cout << "  Space    - Move up (fly)\n";
        std::cout << "  Shift    - Move down (fly)\n";
        std::cout << "  P        - Print status\n";
        std::cout << "  ESC      - Exit game\n";
        std::cout << "==============================================\n\n";
    }
    
    // Helper function for angle conversion
    float RadiansToDegrees(float radians) const {
        return radians * 180.0f / 3.14159265358979323846f;
    }

public:
    // Public interface for customization
    void SetMovementSpeed(float speed) { 
        movementSpeed = speed; 
        std::cout << "[" << playerName << "] Movement speed set to " << speed << "\n";
    }
    
    void SetMouseSensitivity(float sensitivity) { 
        mouseSensitivity = sensitivity; 
        std::cout << "[" << playerName << "] Mouse sensitivity set to " << sensitivity << "\n";
    }
    
    // Getters
    float GetMovementSpeed() const { return movementSpeed; }
    float GetMouseSensitivity() const { return mouseSensitivity; }
    
    void GetCameraRotation(float& pitch, float& yaw) const {
        pitch = cameraPitch;
        yaw = cameraYaw;
    }
    
    void GetCameraRotationDegrees(float& pitchDeg, float& yawDeg) const {
        pitchDeg = RadiansToDegrees(cameraPitch);
        yawDeg = RadiansToDegrees(cameraYaw);
    }
};
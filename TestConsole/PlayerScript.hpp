// PlayerScript.hpp - With Proper Camera Integration
#pragma once
#include "TronEngine.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

class PlayerScript : public ScriptBase
{
private:
    std::string playerName;
    int updateCount = 0;

    // Movement settings
    float movementSpeed = 5.0f;
    bool cameraSetup = false;

public:
    PlayerScript(const std::string &name = "Player")
        : playerName(name)
    {
        std::cout << "[" << playerName << "] Player Script instance created" << std::endl;
    }

    void Start() override
    {
        std::cout << "\n[" << playerName << "] START() - Setting up first-person player on entity " << entity << std::endl;

        // Set initial position
        float spawnX = 0.0f, spawnY = 2.0f, spawnZ = 0.0f;

        if (AddTransformComponent(entity, spawnX, spawnY, spawnZ))
        {
            std::cout << "[" << playerName << "] Transform component added at ("
                      << spawnX << ", " << spawnY << ", " << spawnZ << ")" << std::endl;
        }

        // *** MAKE PLAYER INVISIBLE FOR TESTING ***
        if (AddMeshRendererComponent(entity, PRIMITIVE_CUBE, "blue"))
        {
            SetMeshRendererColor(entity, 0.0f, 1.0f, 1.0f, 1.0f);
            SetTransformUniformScale(entity, 0.1f);
            // Make player invisible to test camera
            SetMeshRendererVisible(entity, false);
            std::cout << "[" << playerName << "] Player made INVISIBLE for camera testing" << std::endl;
        }

        // Add collider
        AddBoxColliderComponent(entity, 1.5f, 2.0f, 1.5f, false);

        // Setup first-person camera
        SetupFirstPersonCamera();

        std::cout << "[" << playerName << "] First-person player initialization complete!" << std::endl;
        std::cout << "[" << playerName << "] Controls: ZQSD to move, Mouse to look around" << std::endl;
        std::cout << "[" << playerName << "] Player is INVISIBLE - you should see the world from camera view" << std::endl;
    }

    void Update(float deltaTime) override
    {
        updateCount++;

        // Update camera first
        if (cameraSetup) {
            UpdatePlayerCamera(deltaTime);
        }

        // Process movement
        ProcessFirstPersonMovement(deltaTime);

        // Debug info
        if (updateCount % 360 == 0) {
            PrintPlayerStatus();
        }
    }

    void OnDestroy() override
    {
        std::cout << "\n[" << playerName << "] OnDestroy() called on entity " << entity << std::endl;
        std::cout << "[" << playerName << "] Player cleanup complete!\n" << std::endl;
    }

private:
    void SetupFirstPersonCamera()
    {
        // Create first-person camera
        if (CreatePlayerCamera(75.0f, 16.0f/9.0f, 0.1f, 1000.0f)) {
            std::cout << "[" << playerName << "] Player camera created\n";
            
            // Attach to this entity
            if (AttachPlayerCameraToEntity(entity)) {
                std::cout << "[" << playerName << "] Camera attached to player entity\n";
                
                // Set camera settings
                if (SetPlayerCameraSettings(movementSpeed, 0.002f)) {
                    std::cout << "[" << playerName << "] Camera settings configured\n";
                    cameraSetup = true;
                } else {
                    std::cout << "[" << playerName << "] ERROR: Failed to set camera settings\n";
                }
            } else {
                std::cout << "[" << playerName << "] ERROR: Failed to attach camera\n";
            }
        } else {
            std::cout << "[" << playerName << "] ERROR: Failed to create camera\n";
        }
    }

    void ProcessFirstPersonMovement(float deltaTime)
    {
        // Get current position
        float currentX, currentY, currentZ;
        if (!GetTransformComponent(entity, &currentX, &currentY, &currentZ))
        {
            return;
        }

        // Get camera rotation to determine movement direction
        float pitch, yaw, roll;
        bool hasRotation = GetPlayerCameraRotation(&pitch, &yaw, &roll);
        
        if (!hasRotation) {
            // Fallback to transform rotation
            GetTransformRotation(entity, &pitch, &yaw, &roll);
        }

        // Calculate movement vector based on camera orientation
        float moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
        float speed = movementSpeed * deltaTime;
        bool moved = false;

        // Calculate forward and right vectors from yaw
        float forwardX = cosf(yaw);
        float forwardZ = sinf(yaw);
        float rightX = cosf(yaw + 1.5708f); // yaw + 90 degrees  
        float rightZ = sinf(yaw + 1.5708f);

        // ZQSD movement relative to camera look direction
        if (IsKeyDown('Z') || IsKeyDown('z'))
        {
            moveX += forwardX * speed;
            moveZ += forwardZ * speed;
            moved = true;
        }
        if (IsKeyDown('S') || IsKeyDown('s'))
        {
            moveX -= forwardX * speed;
            moveZ -= forwardZ * speed;
            moved = true;
        }
        if (IsKeyDown('Q') || IsKeyDown('q'))
        {
            moveX -= rightX * speed;
            moveZ -= rightZ * speed;
            moved = true;
        }
        if (IsKeyDown('D') || IsKeyDown('d'))
        {
            moveX += rightX * speed;
            moveZ += rightZ * speed;
            moved = true;
        }

        // Vertical movement
        if (IsKeyDown(VK_SPACE))
        {
            moveY += speed;
            moved = true;
        }
        if (IsKeyDown(VK_SHIFT))
        {
            moveY -= speed;
            moved = true;
        }

        // Apply movement
        if (moved)
        {
            float newX = currentX + moveX;
            float newY = currentY + moveY;
            float newZ = currentZ + moveZ;

            SetTransformPosition(entity, newX, newY, newZ);

            if (updateCount % 120 == 0) {
                std::cout << "[" << playerName << "] Position: ("
                          << std::fixed << std::setprecision(2)
                          << newX << ", " << newY << ", " << newZ << ")\n";
            }
        }

        // Debug keys
        if (IsKeyPressed('P') || IsKeyPressed('p'))
        {
            PrintPlayerStatus();
        }

        if (IsKeyPressed('C') || IsKeyPressed('c'))
        {
            PrintPlayerCameraInfo();
        }

        // Toggle visibility
        if (IsKeyPressed('V') || IsKeyPressed('v'))
        {
            static bool visible = true;
            visible = !visible;
            SetMeshRendererVisible(entity, visible);
            std::cout << "[" << playerName << "] Player visibility: " << (visible ? "ON" : "OFF") << std::endl;
        }
    }

    void PrintPlayerStatus()
    {
        float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z))
        {
            std::cout << "[" << playerName << "] === PLAYER STATUS ===" << std::endl;
            std::cout << "[" << playerName << "] Entity Position: ("
                      << std::fixed << std::setprecision(2)
                      << x << ", " << y << ", " << z << ")" << std::endl;
            
            if (cameraSetup) {
                float camX, camY, camZ;
                if (GetPlayerCameraPosition(&camX, &camY, &camZ)) {
                    std::cout << "[" << playerName << "] Camera Position: ("
                              << camX << ", " << camY << ", " << camZ << ")" << std::endl;
                }
                
                float pitch, yaw, roll;
                if (GetPlayerCameraRotation(&pitch, &yaw, &roll)) {
                    std::cout << "[" << playerName << "] Camera Rotation: pitch="
                              << (pitch * 180.0f / 3.14159f) << "°, yaw="
                              << (yaw * 180.0f / 3.14159f) << "°" << std::endl;
                }
            }
            
            std::cout << "[" << playerName << "] Speed: " << movementSpeed << std::endl;
            std::cout << "[" << playerName << "] Camera Setup: " << (cameraSetup ? "YES" : "NO") << std::endl;
        }
    }

public:
    const std::string &GetName() const { return playerName; }
    int GetUpdateCount() const { return updateCount; }

    void SetMovementSpeed(float speed) 
    { 
        movementSpeed = speed;
        if (cameraSetup) {
            SetPlayerCameraSettings(speed, 0.002f);
        }
    }
    
    void SetMouseSensitivity(float sensitivity) 
    { 
        if (cameraSetup) {
            SetPlayerCameraSettings(movementSpeed, sensitivity);
        }
    }
};
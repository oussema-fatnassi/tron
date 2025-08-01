// PlayerScript.hpp - Working version with real input
#pragma once
#include "TronEngine.hpp"
#include <iostream>
#include <iomanip>
#include <string>

class PlayerScript : public ScriptBase
{
private:
    std::string playerName;
    int updateCount = 0;

    // Movement settings
    float movementSpeed = 2.0f;

    // Simple camera rotation state
    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;
    float mouseSensitivity = 0.002f;

    // Mouse state
    bool firstMouse = true;
    int lastMouseX = 640;
    int lastMouseY = 360;

public:
    PlayerScript(const std::string &name = "Player")
        : playerName(name)
    {
        std::cout << "[" << playerName << "] Player Script instance created" << std::endl;
    }

    void Start() override
    {
        std::cout << "\n[" << playerName << "] START() - Setting up player on entity " << entity << std::endl;

        // Set initial position
        float spawnX = 0.0f, spawnY = 0.0f, spawnZ = 0.0f;

        if (AddTransformComponent(entity, spawnX, spawnY, spawnZ))
        {
            std::cout << "[" << playerName << "] Transform component added at ("
                      << spawnX << ", " << spawnY << ", " << spawnZ << ")" << std::endl;
        }

        // Add visual representation (optional)
        if (AddMeshRendererComponent(entity, PRIMITIVE_CUBE, "blue"))
        {
            SetMeshRendererColor(entity, 0.0f, 1.0f, 1.0f, 1.0f); // Cyan player
            std::cout << "[" << playerName << "] Visual representation added (cyan cube)" << std::endl;
        }

        std::cout << "[" << playerName << "] Player initialization complete!" << std::endl;
        std::cout << "[" << playerName << "] Controls: WASD to move, P to print position" << std::endl;
    }

    void Update(float deltaTime) override
    {
        updateCount++;
		if (IsKeyPressed(90))
		{
			std::cout << "[" << playerName << "] Escape key pressed - quitting game!" << std::endl;
			return;
		}
        if (IsKeyReleased(90))
		{
			std::cout << "[" << playerName << "] Escape key released - quitting game!" << std::endl;
			return;
		}

        // Process movement controls
        //ProcessMovement(deltaTime);

        //// Process mouse look (simple version)
        //ProcessMouseLook();

        //// Debug info every few seconds
        //if (updateCount % 300 == 0)
        //{ // Every ~2.5 seconds at 120fps
        //    PrintPlayerStatus();
        //}
    }

    void OnDestroy() override
    {
        std::cout << "\n[" << playerName << "] OnDestroy() called on entity " << entity << std::endl;
        std::cout << "[" << playerName << "] Total updates: " << updateCount << std::endl;
        std::cout << "[" << playerName << "] Player cleanup complete!\n"
                  << std::endl;
    }

private:
    void ProcessMovement(float deltaTime)
    {
        // Get current position
        float currentX, currentY, currentZ;
        if (!GetTransformComponent(entity, &currentX, &currentY, &currentZ))
        {
            return; // No transform component
        }

        // Calculate movement
        float moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
        float speed = movementSpeed * deltaTime;
        bool moved = false;

        // WASD movement (world-relative for now)
        if (IsKeyDown('W') || IsKeyDown('w'))
        {
            moveZ -= speed; // Forward (negative Z)
            moved = true;
            std::cout << "[" << playerName << "] Moving forward\n";
        }
        if (IsKeyDown('S') || IsKeyDown('s'))
        {
            moveZ += speed; // Backward (positive Z)
            moved = true;
            std::cout << "[" << playerName << "] Moving backward\n";
        }
        if (IsKeyDown('A') || IsKeyDown('a'))
        {
            moveX -= speed; // Left (negative X)
            moved = true;
            std::cout << "[" << playerName << "] Moving left\n";
        }
        if (IsKeyDown('D') || IsKeyDown('d'))
        {
            moveX += speed; // Right (positive X)
            moved = true;
            std::cout << "[" << playerName << "] Moving right\n";
        }

        // Vertical movement
        if (IsKeyDown(VK_SPACE))
        {
            moveY += speed; // Up
            moved = true;
            std::cout << "[" << playerName << "] Moving up\n";
        }
        if (IsKeyDown(VK_SHIFT))
        {
            moveY -= speed; // Down
            moved = true;
            std::cout << "[" << playerName << "] Moving down\n";
        }

        // Apply movement if any key was pressed
        if (moved)
        {
            float newX = currentX + moveX;
            float newY = currentY + moveY;
            float newZ = currentZ + moveZ;

            // Update position using the enhanced transform API
            SetTransformPosition(entity, newX, newY, newZ);

            std::cout << "[" << playerName << "] New position: ("
                      << std::fixed << std::setprecision(2)
                      << newX << ", " << newY << ", " << newZ << ")\n";
        }

        // Debug key
        if (IsKeyPressed('P') || IsKeyPressed('p'))
        {
            PrintPlayerStatus();
        }
    }

    void ProcessMouseLook()
    {
        int mouseX, mouseY;
        GetMousePosition(&mouseX, &mouseY);

        if (firstMouse)
        {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
            return;
        }

        // Calculate mouse delta
        int deltaX = mouseX - lastMouseX;
        int deltaY = mouseY - lastMouseY;

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        // Only process if there's actual movement
        if (deltaX != 0 || deltaY != 0)
        {
            // Update camera rotation
            cameraYaw += deltaX * mouseSensitivity;
            cameraPitch -= deltaY * mouseSensitivity; // Invert Y

            // Clamp pitch
            if (cameraPitch > 1.57f)
                cameraPitch = 1.57f; // ~90 degrees
            if (cameraPitch < -1.57f)
                cameraPitch = -1.57f; // ~-90 degrees

            // Apply rotation to transform (if we had rotation support)
            // For now, just log it
            std::cout << "[" << playerName << "] Camera rotation: yaw="
                      << (cameraYaw * 180.0f / 3.14159f) << "°, pitch="
                      << (cameraPitch * 180.0f / 3.14159f) << "°\n";

            // TODO: Apply rotation to entity transform when rotation is supported
            // SetTransformRotation(entity, cameraPitch, cameraYaw, 0.0f);
        }
    }

    void PrintPlayerStatus()
    {
        float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z))
        {
            std::cout << "[" << playerName << "] Position: ("
                      << std::fixed << std::setprecision(2)
                      << x << ", " << y << ", " << z << ")" << std::endl;
            std::cout << "[" << playerName << "] Camera: yaw="
                      << (cameraYaw * 180.0f / 3.14159f) << "°, pitch="
                      << (cameraPitch * 180.0f / 3.14159f) << "°" << std::endl;
        }
    }

public:
    // Public interface
    const std::string &GetName() const { return playerName; }
    int GetUpdateCount() const { return updateCount; }

    void SetMovementSpeed(float speed) { movementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
};
#pragma once
#include "../Game/System.hpp"
#include "../Core/InputManager.hpp"

// Forward declarations
struct Transform;

// <summary>
// CameraSystem - Basic first-person camera controls
// Works with the current  rendering pipeline (no matrices yet)
// </summary>
// <remarks>
// This system updates Transform components based on input to simulate camera movement.
// Since we don't have a proper camera matrix pipeline yet, this system moves the WORLD
// around the camera instead of moving the camera through the world.
// 
// Controls:
// - WASD: Move forward/back/left/right
// - Mouse: Look around (rotates the world opposite to mouse movement)
// - Space/Shift: Move up/down
// </remarks>
class CameraSystem : public System {
private:
    InputManager* inputManager;
    Entity cameraEntity;  // The entity representing the camera/player
    
    // Camera state
    float cameraX, cameraY, cameraZ;
    float cameraPitch, cameraYaw;
    
    // Camera settings
    float movementSpeed;
    float mouseSensitivity;
    bool firstMouseInput;
    int lastMouseX, lastMouseY;
    
    // Camera limits
    float maxPitch, minPitch;

public:
    CameraSystem(InputManager* inputMgr);
    ~CameraSystem() = default;

    void Update(float deltaTime) override;
    
    // Camera control methods
    void AttachToEntity(Entity entity);
    void DetachFromEntity();
    
    // Camera settings
    void SetMovementSpeed(float speed) { movementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    
    // Camera state
    void SetPosition(float x, float y, float z);
    void GetPosition(float& x, float& y, float& z) const;
    void SetRotation(float pitch, float yaw);
    void GetRotation(float& pitch, float& yaw) const;
    
    // Debug
    void PrintCameraInfo() const;

private:
    void ProcessKeyboardInput(float deltaTime);
    void ProcessMouseInput();
    void UpdateWorldTransforms(); // Moves world objects based on camera position
    
    // Helper methods
    void GetForwardVector(float& x, float& y, float& z) const;
    void GetRightVector(float& x, float& y, float& z) const;
};
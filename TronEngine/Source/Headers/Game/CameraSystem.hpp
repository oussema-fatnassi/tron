// TronEngine/Source/Headers/Game/CameraSystem.hpp - Simplified version
#pragma once
#include "../Game/System.hpp"
#include "../Core/InputManager.hpp"

// Forward declarations
struct Transform;

// <summary>
// CameraSystem - Simplified First-Person Camera Control
// Handles mouse look and applies rotation directly to entity transforms
// </summary>
// <remarks>
// This system processes mouse input and applies camera rotation directly to entities
// with Transform components. It works by:
// - Processing mouse movement for look control
// - Applying rotation to entity transforms
// - Providing camera state information
// 
// Movement is handled by individual entity scripts (like PlayerScript),
// while this system focuses purely on camera rotation control.
// </remarks>
class CameraSystem : public System {
private:
    InputManager* inputManager;
    Entity cameraEntity;  // Currently attached entity
    
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
    // Input processing
    void ProcessInput(float deltaTime);
    void ProcessMouseInput();
    void ProcessKeyboardInput(float deltaTime);
    void ApplyRotationToEntities(float pitchDelta, float yawDelta);
    
    // Helper methods
    void GetForwardVector(float& x, float& y, float& z) const;
    void GetRightVector(float& x, float& y, float& z) const;
};
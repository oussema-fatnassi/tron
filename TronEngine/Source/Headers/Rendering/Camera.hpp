#pragma once
#include "../Math/Matrix.hpp"
#include "../Game/Types.hpp"
#include <iomanip>

// Forward declarations
struct Transform;
class InputManager;
class World;

// <summary>
// Camera class for 3D rendering and first-person controls
// Handles view and projection matrices, and provides FPS-style camera controls
// </summary>
// <remarks>
// This camera system provides:
// - View matrix generation from position and rotation
// - Projection matrix with configurable FOV and aspect ratio
// - First-person mouse look controls
// - WASD movement controls
// - Integration with ECS Transform component
// - Smooth camera movement and rotation
// 
// The camera can be attached to an entity or used independently.
// When attached, it uses the entity's Transform component for positioning.
// </remarks>
class Camera {
public:
    // Camera parameters
    float fovDegrees;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    // Movement settings
    float movementSpeed;     // Units per second
    float mouseSensitivity;  // Degrees per pixel
    float smoothingFactor;   // For smooth camera movement (0 = instant, 1 = no movement)

    // Mouse look state
    bool mouseLookEnabled;
    int lastMouseX, lastMouseY;
    bool firstMouseInput;

    // Camera limits
    float maxPitchDegrees;   // Prevent camera from flipping
    float minPitchDegrees;

private:
    // Cached matrices
    mutable Matrix viewMatrix;
    mutable Matrix projectionMatrix;
    mutable bool viewMatrixDirty;
    mutable bool projectionMatrixDirty;

    // ECS integration
    World* world;
    Entity attachedEntity;  // Entity this camera is attached to (0 = not attached)

    // Current camera state (used when not attached to entity)
    float cameraX, cameraY, cameraZ;
    float cameraPitch, cameraYaw, cameraRoll;

public:
    // Constructors
    Camera();
    Camera(float fov, float aspect, float nearPlane, float farPlane);

    // ECS Integration
    void AttachToEntity(World* worldPtr, Entity entity);
    void DetachFromEntity();
    bool IsAttachedToEntity() const { return attachedEntity != 0 && world != nullptr; }

    // Matrix getters
    const Matrix& GetViewMatrix() const;
    const Matrix& GetProjectionMatrix() const;
    Matrix GetViewProjectionMatrix() const;

    // Camera state
    void GetPosition(float& x, float& y, float& z) const;
    void GetRotation(float& pitch, float& yaw, float& roll) const;
    void SetPosition(float x, float y, float z);
    void SetRotation(float pitch, float yaw, float roll);

    // Camera controls
    void Update(float deltaTime, InputManager* inputManager);
    void ProcessMouseLook(int mouseX, int mouseY);
    void ProcessMovement(float deltaTime, InputManager* inputManager);

    // Camera configuration
    void SetProjection(float fov, float aspect, float nearPlane, float farPlane);
    void SetAspectRatio(float aspect);
    void SetFieldOfView(float fovDeg);

    // Utility methods
    void GetForwardVector(float& x, float& y, float& z) const;
    void GetRightVector(float& x, float& y, float& z) const;
    void GetUpVector(float& x, float& y, float& z) const;

    // Debug
    void PrintCameraInfo() const;

private:
    // Internal matrix updates
    void UpdateViewMatrix() const;
    void UpdateProjectionMatrix() const;
    void MarkViewDirty() { viewMatrixDirty = true; }
    void MarkProjectionDirty() { projectionMatrixDirty = true; }

    // Get transform component if attached to entity
    Transform* GetAttachedTransform() const;

    // Clamp pitch to prevent camera flip
    void ClampPitch();

    // TODO: Future enhancements
    // - Different camera modes (orbit, follow, etc.)
    // - Camera shake effects
    // - Smooth follow/chase camera
    // - Camera interpolation between positions
    // - Frustum culling calculations
    // - Ray casting from camera for mouse picking
};

// Utility functions for common camera operations
namespace CameraUtils {
    // TODO: Implement utility functions
    // - Screen to world ray casting
    // - World to screen projection
    // - Camera frustum calculations
    // - Smooth camera transitions
}
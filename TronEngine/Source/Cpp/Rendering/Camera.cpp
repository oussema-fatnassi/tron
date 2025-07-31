#include "../../Headers/Rendering/Camera.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Core/InputManager.hpp"
#include <iostream>
#include <algorithm>

Camera::Camera()
    : fovDegrees(75.0f)
    , aspectRatio(16.0f / 9.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , movementSpeed(10.0f)
    , mouseSensitivity(0.1f)
    , smoothingFactor(0.0f)
    , mouseLookEnabled(true)
    , lastMouseX(0)
    , lastMouseY(0)
    , firstMouseInput(true)
    , maxPitchDegrees(89.0f)
    , minPitchDegrees(-89.0f)
    , viewMatrixDirty(true)
    , projectionMatrixDirty(true)
    , world(nullptr)
    , attachedEntity(0)
    , cameraX(0), cameraY(0), cameraZ(0)
    , cameraPitch(0), cameraYaw(0), cameraRoll(0)
{
    std::cout << "[Camera] Camera created with default settings\n";
}

Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
    : Camera() // Delegate to default constructor
{
    SetProjection(fov, aspect, nearPlane, farPlane);
}

void Camera::AttachToEntity(World* worldPtr, Entity entity) {
    if (!worldPtr || !worldPtr->IsValidEntity(entity)) {
        std::cout << "[Camera] Error: Cannot attach to invalid entity\n";
        return;
    }

    world = worldPtr;
    attachedEntity = entity;
    MarkViewDirty();

    std::cout << "[Camera] Attached to entity " << entity << "\n";
}

void Camera::DetachFromEntity() {
    if (IsAttachedToEntity()) {
        // Store current transform state before detaching
        Transform* transform = GetAttachedTransform();
        if (transform) {
            cameraX = transform->x;
            cameraY = transform->y;
            cameraZ = transform->z;
            cameraPitch = transform->pitch;
            cameraYaw = transform->yaw;
            cameraRoll = transform->roll;
        }

        std::cout << "[Camera] Detached from entity " << attachedEntity << "\n";
        world = nullptr;
        attachedEntity = 0;
        MarkViewDirty();
    }
}

const Matrix& Camera::GetViewMatrix() const {
    if (viewMatrixDirty) {
        UpdateViewMatrix();
    }
    return viewMatrix;
}

const Matrix& Camera::GetProjectionMatrix() const {
    if (projectionMatrixDirty) {
        UpdateProjectionMatrix();
    }
    return projectionMatrix;
}

Matrix Camera::GetViewProjectionMatrix() const {
    return GetProjectionMatrix() * GetViewMatrix();
}

void Camera::GetPosition(float& x, float& y, float& z) const {
    if (IsAttachedToEntity()) {
        Transform* transform = GetAttachedTransform();
        if (transform) {
            x = transform->x;
            y = transform->y;
            z = transform->z;
            return;
        }
    }
    
    x = cameraX;
    y = cameraY;
    z = cameraZ;
}

void Camera::GetRotation(float& pitch, float& yaw, float& roll) const {
    if (IsAttachedToEntity()) {
        Transform* transform = GetAttachedTransform();
        if (transform) {
            pitch = transform->pitch;
            yaw = transform->yaw;
            roll = transform->roll;
            return;
        }
    }
    
    pitch = cameraPitch;
    yaw = cameraYaw;
    roll = cameraRoll;
}

void Camera::SetPosition(float x, float y, float z) {
    if (IsAttachedToEntity()) {
        Transform* transform = GetAttachedTransform();
        if (transform) {
            transform->SetPosition(x, y, z);
        }
    } else {
        cameraX = x;
        cameraY = y;
        cameraZ = z;
    }
    MarkViewDirty();
}

void Camera::SetRotation(float pitch, float yaw, float roll) {
    if (IsAttachedToEntity()) {
        Transform* transform = GetAttachedTransform();
        if (transform) {
            transform->SetRotation(pitch, yaw, roll);
        }
    } else {
        cameraPitch = pitch;
        cameraYaw = yaw;
        cameraRoll = roll;
    }
    MarkViewDirty();
}

void Camera::Update(float deltaTime, InputManager* inputManager) {
    if (!inputManager) return;

    // Process mouse look
    if (mouseLookEnabled) {
        POINT mousePos = inputManager->GetMousePosition();
        ProcessMouseLook(mousePos.x, mousePos.y);
    }

    // Process movement
    ProcessMovement(deltaTime, inputManager);

    // Update input manager (important for key state transitions)
    // This should be called by the engine, but we ensure it here
    // inputManager->Update(); // Don't call this here, engine handles it
}

void Camera::ProcessMouseLook(int mouseX, int mouseY) {
    if (firstMouseInput) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouseInput = false;
        return;
    }

    // Calculate mouse delta
    int deltaX = mouseX - lastMouseX;
    int deltaY = mouseY - lastMouseY;
    
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    // Convert to rotation
    float yawDelta = deltaX * mouseSensitivity * DegreesToRadians(1.0f);
    float pitchDelta = -deltaY * mouseSensitivity * DegreesToRadians(1.0f); // Invert Y

    // Apply rotation
    float currentPitch, currentYaw, currentRoll;
    GetRotation(currentPitch, currentYaw, currentRoll);
    
    currentYaw += yawDelta;
    currentPitch += pitchDelta;

    // Clamp pitch to prevent camera flip
    float pitchDegrees = RadiansToDegrees(currentPitch);
    pitchDegrees = std::clamp(pitchDegrees, minPitchDegrees, maxPitchDegrees);
    currentPitch = DegreesToRadians(pitchDegrees);

    SetRotation(currentPitch, currentYaw, currentRoll);
}

void Camera::ProcessMovement(float deltaTime, InputManager* inputManager) {
    float moveSpeed = movementSpeed * deltaTime;
    
    // Get current position and vectors
    float currentX, currentY, currentZ;
    GetPosition(currentX, currentY, currentZ);
    
    float forwardX, forwardY, forwardZ;
    float rightX, rightY, rightZ;
    GetForwardVector(forwardX, forwardY, forwardZ);
    GetRightVector(rightX, rightY, rightZ);

    // Calculate movement vector
    float moveX = 0, moveY = 0, moveZ = 0;

    // WASD movement
    if (inputManager->IsKeyDown('W') || inputManager->IsKeyDown('w')) {
        moveX += forwardX * moveSpeed;
        moveY += forwardY * moveSpeed;
        moveZ += forwardZ * moveSpeed;
    }
    if (inputManager->IsKeyDown('S') || inputManager->IsKeyDown('s')) {
        moveX -= forwardX * moveSpeed;
        moveY -= forwardY * moveSpeed;
        moveZ -= forwardZ * moveSpeed;
    }
    if (inputManager->IsKeyDown('A') || inputManager->IsKeyDown('a')) {
        moveX -= rightX * moveSpeed;
        moveY -= rightY * moveSpeed;
        moveZ -= rightZ * moveSpeed;
    }
    if (inputManager->IsKeyDown('D') || inputManager->IsKeyDown('d')) {
        moveX += rightX * moveSpeed;
        moveY += rightY * moveSpeed;
        moveZ += rightZ * moveSpeed;
    }

    // Vertical movement (Space/Shift)
    if (inputManager->IsKeyDown(VK_SPACE)) {
        moveY += moveSpeed; // Up
    }
    if (inputManager->IsKeyDown(VK_SHIFT)) {
        moveY -= moveSpeed; // Down
    }

    // Apply movement
    if (moveX != 0 || moveY != 0 || moveZ != 0) {
        SetPosition(currentX + moveX, currentY + moveY, currentZ + moveZ);
    }
}

void Camera::SetProjection(float fov, float aspect, float nearPlane, float farPlane) {
    fovDegrees = fov;
    aspectRatio = aspect;
    nearPlane = nearPlane;
    farPlane = farPlane;
    MarkProjectionDirty();

    std::cout << "[Camera] Projection updated: FOV=" << fov << "°, Aspect="
        << aspect << ", Near=" << nearPlane << ", Far=" << farPlane << "\n";
}

void Camera::SetAspectRatio(float aspect) {
    aspectRatio = aspect;
    MarkProjectionDirty();
}

void Camera::SetFieldOfView(float fovDeg) {
    fovDegrees = fovDeg;
    MarkProjectionDirty();
}

void Camera::GetForwardVector(float& x, float& y, float& z) const {
    if (IsAttachedToEntity()) {
        Transform* transform = GetAttachedTransform();
        if (transform) {
            transform->GetForward(x, y, z);
            return;
        }
    }
    
    // Calculate forward vector from rotation
    Matrix rotationMatrix = Matrix::RotationEuler(cameraPitch, cameraYaw, cameraRoll);
    x = -rotationMatrix.At(0, 2);
    y = -rotationMatrix.At(1, 2);
    z = -rotationMatrix.At(2, 2);
}

void Camera::GetRightVector(float& x, float& y, float& z) const {
    if (IsAttachedToEntity()) {
        Transform* transform = GetAttachedTransform();
        if (transform) {
            transform->GetRight(x, y, z);
            return;
        }
    }
    
    // Calculate right vector from rotation
    Matrix rotationMatrix = Matrix::RotationEuler(cameraPitch, cameraYaw, cameraRoll);
    x = rotationMatrix.At(0, 0);
    y = rotationMatrix.At(1, 0);
    z = rotationMatrix.At(2, 0);
}

void Camera::GetUpVector(float& x, float& y, float& z) const {
    if (IsAttachedToEntity()) {
        Transform* transform = GetAttachedTransform();
        if (transform) {
            transform->GetUp(x, y, z);
            return;
        }
    }
    
    // Calculate up vector from rotation
    Matrix rotationMatrix = Matrix::RotationEuler(cameraPitch, cameraYaw, cameraRoll);
    x = rotationMatrix.At(0, 1);
    y = rotationMatrix.At(1, 1);
    z = rotationMatrix.At(2, 1);
}

void Camera::PrintCameraInfo() const {
    float x, y, z, pitch, yaw, roll;
    GetPosition(x, y, z);
    GetRotation(pitch, yaw, roll);
    
    std::cout << "[Camera] Position(" << x << ", " << y << ", " << z << ") "
              << "Rotation(" << RadiansToDegrees(pitch) << "°, " 
              << RadiansToDegrees(yaw) << "°, " << RadiansToDegrees(roll) << "°)\n";
    std::cout << "[Camera] FOV=" << fovDegrees << "° Aspect=" << aspectRatio 
              << " Near=" << nearPlane << " Far=" << farPlane << "\n";
    
    if (IsAttachedToEntity()) {
        std::cout << "[Camera] Attached to entity " << attachedEntity << "\n";
    } else {
        std::cout << "[Camera] Not attached to entity\n";
    }
}

void Camera::UpdateViewMatrix() const {
    float eyeX, eyeY, eyeZ;
    GetPosition(eyeX, eyeY, eyeZ);
    
    float forwardX, forwardY, forwardZ;
    GetForwardVector(forwardX, forwardY, forwardZ);
    
    float upX, upY, upZ;
    GetUpVector(upX, upY, upZ);
    
    // Target is position + forward
    float targetX = eyeX + forwardX;
    float targetY = eyeY + forwardY;
    float targetZ = eyeZ + forwardZ;
    
    viewMatrix = Matrix::LookAt(eyeX, eyeY, eyeZ,
                                targetX, targetY, targetZ,
                                upX, upY, upZ);
    
    viewMatrixDirty = false;
}

void Camera::UpdateProjectionMatrix() const {
    projectionMatrix = Matrix::Perspective(
        DegreesToRadians(fovDegrees),
        aspectRatio,
        nearPlane,
        farPlane
    );
    
    projectionMatrixDirty = false;
}

Transform* Camera::GetAttachedTransform() const {
    if (!IsAttachedToEntity()) {
        return nullptr;
    }
    
    return world->GetComponent<Transform>(attachedEntity);
}

void Camera::ClampPitch() {
    float currentPitch, currentYaw, currentRoll;
    GetRotation(currentPitch, currentYaw, currentRoll);
    
    float pitchDegrees = RadiansToDegrees(currentPitch);
    pitchDegrees = std::clamp(pitchDegrees, minPitchDegrees, maxPitchDegrees);
    currentPitch = DegreesToRadians(pitchDegrees);
    
    SetRotation(currentPitch, currentYaw, currentRoll);
}
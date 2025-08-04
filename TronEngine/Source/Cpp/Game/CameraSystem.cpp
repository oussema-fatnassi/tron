// TronEngine/Source/Cpp/Game/CameraSystem.cpp - Complete Fixed version
#include "../../Headers/Game/CameraSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

CameraSystem::CameraSystem(InputManager* inputMgr)
    : inputManager(inputMgr)
    , cameraEntity(0)
    , cameraX(0), cameraY(0), cameraZ(0)
    , cameraPitch(0), cameraYaw(0)
    , movementSpeed(5.0f)
    , mouseSensitivity(0.002f)
    , firstMouseInput(true)
    , lastMouseX(0), lastMouseY(0)
    , maxPitch(1.57f), minPitch(-1.57f)
{
    std::cout << "[CameraSystem] Initialized for first-person control\n";
}

void CameraSystem::Update(float deltaTime) {
    if (!inputManager) return;
    
    // Process input for entities with Transform components
    ProcessInput(deltaTime);
}

void CameraSystem::ProcessInput(float deltaTime) {
    // Process mouse look
    ProcessMouseInput();
    
    // Process keyboard input for camera-controlled entities
    ProcessKeyboardInput(deltaTime);
}

void CameraSystem::ProcessMouseInput() {
    POINT mousePos = inputManager->GetMousePosition();
    
    if (firstMouseInput) {
        lastMouseX = mousePos.x;
        lastMouseY = mousePos.y;
        firstMouseInput = false;
        return;
    }
    
    // Calculate mouse delta
    int deltaX = mousePos.x - lastMouseX;
    int deltaY = mousePos.y - lastMouseY;
    
    lastMouseX = mousePos.x;
    lastMouseY = mousePos.y;
    
    // Only process if there's actual movement
    if (deltaX != 0 || deltaY != 0) {
        // Convert to radians
        float yawDelta = deltaX * mouseSensitivity;
        float pitchDelta = -deltaY * mouseSensitivity; // Invert Y axis
        
        // Apply rotation to all entities in this system (usually just the player)
        ApplyRotationToEntities(pitchDelta, yawDelta);
        
        if (abs(deltaX) > 2 || abs(deltaY) > 2) { // Only log significant movement
            std::cout << "[CameraSystem] Mouse look applied to " << entities.size() << " entities\n";
        }
    }
}

void CameraSystem::ApplyRotationToEntities(float pitchDelta, float yawDelta) {
    // Apply rotation to all entities managed by this system
    for (Entity entity : entities) {
        auto* transform = world->GetComponent<Transform>(entity);
        if (!transform) continue;
        
        // Update rotation
        float newPitch = transform->pitch + pitchDelta;
        float newYaw = transform->yaw + yawDelta;
        
        // Clamp pitch to prevent camera flip
        newPitch = std::clamp(newPitch, minPitch, maxPitch);
        
        // Apply the rotation
        transform->SetRotation(newPitch, newYaw, transform->roll);
        
        // Update our internal state for the first entity (main camera)
        if (entity == entities[0]) {
            cameraPitch = newPitch;
            cameraYaw = newYaw;
        }
    }
}

void CameraSystem::ProcessKeyboardInput(float deltaTime) {
    // This system now focuses on camera rotation only
    // Movement is handled by individual entity scripts (like PlayerScript)
    
    // Debug keys
    if (inputManager->IsKeyPressed('C') || inputManager->IsKeyPressed('c')) {
        PrintCameraInfo();
    }
    
    // Camera sensitivity adjustment
    if (inputManager->IsKeyPressed(VK_OEM_PLUS)) { // '+' key
        mouseSensitivity *= 1.1f;
        std::cout << "[CameraSystem] Mouse sensitivity: " << mouseSensitivity << "\n";
    }
    if (inputManager->IsKeyPressed(VK_OEM_MINUS)) { // '-' key
        mouseSensitivity *= 0.9f;
        std::cout << "[CameraSystem] Mouse sensitivity: " << mouseSensitivity << "\n";
    }
}

void CameraSystem::AttachToEntity(Entity entity) {
    cameraEntity = entity;
    
    // Initialize camera position from entity transform
    if (world) {
        auto* transform = world->GetComponent<Transform>(entity);
        if (transform) {
            cameraX = transform->x;
            cameraY = transform->y;
            cameraZ = transform->z;
            cameraPitch = transform->pitch;
            cameraYaw = transform->yaw;
        }
    }
    
    std::cout << "[CameraSystem] Attached to entity " << entity << "\n";
}

void CameraSystem::DetachFromEntity() {
    cameraEntity = 0;
    std::cout << "[CameraSystem] Detached from entity\n";
}

void CameraSystem::SetPosition(float x, float y, float z) {
    cameraX = x;
    cameraY = y;
    cameraZ = z;
    
    // Apply to attached entity if exists
    if (cameraEntity != 0 && world) {
        auto* transform = world->GetComponent<Transform>(cameraEntity);
        if (transform) {
            transform->SetPosition(x, y, z);
        }
    }
}

void CameraSystem::GetPosition(float& x, float& y, float& z) const {
    if (cameraEntity != 0 && world) {
        auto* transform = world->GetComponent<Transform>(cameraEntity);
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

void CameraSystem::SetRotation(float pitch, float yaw) {
    cameraPitch = std::clamp(pitch, minPitch, maxPitch);
    cameraYaw = yaw;
    
    // Apply to attached entity if exists
    if (cameraEntity != 0 && world) {
        auto* transform = world->GetComponent<Transform>(cameraEntity);
        if (transform) {
            transform->SetRotation(cameraPitch, cameraYaw, transform->roll);
        }
    }
}

void CameraSystem::GetRotation(float& pitch, float& yaw) const {
    if (cameraEntity != 0 && world) {
        auto* transform = world->GetComponent<Transform>(cameraEntity);
        if (transform) {
            pitch = transform->pitch;
            yaw = transform->yaw;
            return;
        }
    }
    
    pitch = cameraPitch;
    yaw = cameraYaw;
}

void CameraSystem::GetForwardVector(float& x, float& y, float& z) const {
    // Calculate forward vector from current rotation
    float currentPitch = cameraPitch;
    float currentYaw = cameraYaw;
    
    // Get rotation from first entity if available
    if (!entities.empty() && world) {
        auto* transform = world->GetComponent<Transform>(entities[0]);
        if (transform) {
            currentPitch = transform->pitch;
            currentYaw = transform->yaw;
        }
    }
    
    float cosPitch = cosf(currentPitch);
    float sinPitch = sinf(currentPitch);
    float cosYaw = cosf(currentYaw);
    float sinYaw = sinf(currentYaw);
    
    x = cosYaw * cosPitch;
    y = -sinPitch;
    z = sinYaw * cosPitch;
}

void CameraSystem::GetRightVector(float& x, float& y, float& z) const {
    float currentYaw = cameraYaw;
    
    // Get rotation from first entity if available
    if (!entities.empty() && world) {
        auto* transform = world->GetComponent<Transform>(entities[0]);
        if (transform) {
            currentYaw = transform->yaw;
        }
    }
    
    // Right vector is perpendicular to forward
    x = cosf(currentYaw + 1.5708f); // yaw + 90 degrees
    y = 0.0f;
    z = sinf(currentYaw + 1.5708f);
}

void CameraSystem::PrintCameraInfo() const {
    std::cout << "[CameraSystem] === CAMERA INFO ===" << std::endl;
    std::cout << "[CameraSystem] Entities managed: " << entities.size() << std::endl;
    
    if (!entities.empty() && world) {
        Entity firstEntity = entities[0];
        auto* transform = world->GetComponent<Transform>(firstEntity);
        if (transform) {
            std::cout << "[CameraSystem] Primary Entity " << firstEntity << ":" << std::endl;
            std::cout << "  Position: (" << transform->x << ", " << transform->y << ", " << transform->z << ")" << std::endl;
            std::cout << "  Rotation: pitch=" << (transform->pitch * 180.0f / 3.14159f) 
                      << "°, yaw=" << (transform->yaw * 180.0f / 3.14159f) 
                      << "°, roll=" << (transform->roll * 180.0f / 3.14159f) << "°" << std::endl;
        }
    }
    
    std::cout << "[CameraSystem] Settings:" << std::endl;
    std::cout << "  Speed: " << movementSpeed << std::endl;
    std::cout << "  Mouse Sensitivity: " << mouseSensitivity << std::endl;
    std::cout << "  Pitch Limits: " << (minPitch * 180.0f / 3.14159f) 
              << "° to " << (maxPitch * 180.0f / 3.14159f) << "°" << std::endl;
    
    if (cameraEntity != 0) {
        std::cout << "[CameraSystem] Legacy attached entity: " << cameraEntity << std::endl;
    }
    
    // Print forward and right vectors
    float fx, fy, fz, rx, ry, rz;
    GetForwardVector(fx, fy, fz);
    GetRightVector(rx, ry, rz);
    std::cout << "[CameraSystem] Forward: (" << fx << ", " << fy << ", " << fz << ")" << std::endl;
    std::cout << "[CameraSystem] Right: (" << rx << ", " << ry << ", " << rz << ")" << std::endl;
}
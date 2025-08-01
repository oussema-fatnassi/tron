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
    , mouseSensitivity(0.002f)  // Radians per pixel
    , firstMouseInput(true)
    , lastMouseX(0), lastMouseY(0)
    , maxPitch(1.57f), minPitch(-1.57f)  // ~90 degrees in radians
{
    std::cout << "[CameraSystem] Initialized with basic FPS controls\n";
}

void CameraSystem::Update(float deltaTime) {
    if (!inputManager) return;
    
    // Process input
    ProcessKeyboardInput(deltaTime);
    ProcessMouseInput();
    
    // Update world transforms based on camera position
    // (This is a hack - in proper implementation, we'd update view matrix)
    // UpdateWorldTransforms();
}

void CameraSystem::ProcessKeyboardInput(float deltaTime) {
    if (!world) return;
    
    float moveSpeed = movementSpeed * deltaTime;
    bool moved = false;
    
    // DIRECT OBJECT MOVEMENT TEST - Move the first object directly
    auto allEntities = world->GetAllEntities();
    if (allEntities.empty()) return;
    
    Entity firstEntity = allEntities[0];
    auto* transform = world->GetComponent<Transform>(firstEntity);
    if (!transform) return;
    
    // WASD directly moves the first object (for testing)
    if (inputManager->IsKeyDown('W') || inputManager->IsKeyDown('w')) {
        transform->z -= moveSpeed * 10.0f; // Move forward (negative Z)
        moved = true;
        std::cout << "[CameraTest] Moving object FORWARD to Z=" << transform->z << "\n";
    }
    if (inputManager->IsKeyDown('S') || inputManager->IsKeyDown('s')) {
        transform->z += moveSpeed * 10.0f; // Move backward (positive Z)
        moved = true;  
        std::cout << "[CameraTest] Moving object BACKWARD to Z=" << transform->z << "\n";
    }
    if (inputManager->IsKeyDown('A') || inputManager->IsKeyDown('a')) {
        transform->x -= moveSpeed * 10.0f; // Move left (negative X)
        moved = true;
        std::cout << "[CameraTest] Moving object LEFT to X=" << transform->x << "\n";
    }
    if (inputManager->IsKeyDown('D') || inputManager->IsKeyDown('d')) {
        transform->x += moveSpeed * 10.0f; // Move right (positive X)
        moved = true;
        std::cout << "[CameraTest] Moving object RIGHT to X=" << transform->x << "\n";
    }
    
    // Vertical movement
    if (inputManager->IsKeyDown(VK_SPACE)) {
        transform->y += moveSpeed * 10.0f; // Move up
        moved = true;
        std::cout << "[CameraTest] Moving object UP to Y=" << transform->y << "\n";
    }
    if (inputManager->IsKeyDown(VK_SHIFT)) {
        transform->y -= moveSpeed * 10.0f; // Move down
        moved = true;
        std::cout << "[CameraTest] Moving object DOWN to Y=" << transform->y << "\n";
    }
    
    if (moved) {
        std::cout << "[CameraTest] Object position: (" << transform->x 
                  << ", " << transform->y << ", " << transform->z << ")\n";
    }
    
    // Debug key
    if (inputManager->IsKeyPressed('P') || inputManager->IsKeyPressed('p')) {
        PrintCameraInfo();
        std::cout << "[CameraTest] First object position: (" << transform->x 
                  << ", " << transform->y << ", " << transform->z << ")\n";
    }
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
        
        // Apply rotation
        cameraYaw += yawDelta;
        cameraPitch += pitchDelta;
        
        // Clamp pitch to prevent camera flip
        cameraPitch = std::clamp(cameraPitch, minPitch, maxPitch);
        
        std::cout << "[Camera] Mouse look: pitch=" << (cameraPitch * 180.0f / 3.14159f) 
                  << "°, yaw=" << (cameraYaw * 180.0f / 3.14159f) << "°\n";
    }
}

void CameraSystem::UpdateWorldTransforms() {
    // HACK: Since we don't have proper camera matrices yet,
    // we move the world objects relative to camera position
    
    if (!world) return;
    
    // Get all entities and update their transforms relative to camera
    auto allEntities = world->GetAllEntities();
    
    for (Entity entity : allEntities) {
        // Skip the camera entity itself
        if (entity == cameraEntity) continue;
        
        auto* transform = world->GetComponent<Transform>(entity);
        if (!transform) continue;
        
        // This is a simplified transformation - in real implementation,
        // we'd use proper view matrices
        
        // For now, just offset objects by negative camera position
        // This creates the illusion that the camera is moving
        transform->x -= cameraX * 0.01f; // Scale factor to prevent too much movement
        transform->y -= cameraY * 0.01f;
        transform->z -= cameraZ * 0.01f;
        
        // TODO: Apply rotation transformations based on camera pitch/yaw
        
        // Reset camera position to prevent accumulation
        // (This is part of the hack - not how real cameras work)
    }
    
    // Reset camera position after applying to world
    // (In real implementation, camera position would persist)
    cameraX *= 0.99f; // Gradually reduce to prevent drift
    cameraY *= 0.99f;
    cameraZ *= 0.99f;
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
}

void CameraSystem::GetPosition(float& x, float& y, float& z) const {
    x = cameraX;
    y = cameraY;
    z = cameraZ;
}

void CameraSystem::SetRotation(float pitch, float yaw) {
    cameraPitch = std::clamp(pitch, minPitch, maxPitch);
    cameraYaw = yaw;
}

void CameraSystem::GetRotation(float& pitch, float& yaw) const {
    pitch = cameraPitch;
    yaw = cameraYaw;
}

void CameraSystem::GetForwardVector(float& x, float& y, float& z) const {
    // Calculate forward vector from yaw and pitch
    float cosPitch = cosf(cameraPitch);
    float sinPitch = sinf(cameraPitch);
    float cosYaw = cosf(cameraYaw);
    float sinYaw = sinf(cameraYaw);
    
    x = cosYaw * cosPitch;
    y = -sinPitch;
    z = sinYaw * cosPitch;
}

void CameraSystem::GetRightVector(float& x, float& y, float& z) const {
    // Right vector is perpendicular to forward
    x = cosf(cameraYaw + 1.5708f); // yaw + 90 degrees
    y = 0.0f;
    z = sinf(cameraYaw + 1.5708f);
}

void CameraSystem::PrintCameraInfo() const {
    std::cout << "[CameraSystem] Camera Info:\n";
    std::cout << "  Position: (" << cameraX << ", " << cameraY << ", " << cameraZ << ")\n";
    std::cout << "  Rotation: pitch=" << (cameraPitch * 180.0f / 3.14159f) 
              << "°, yaw=" << (cameraYaw * 180.0f / 3.14159f) << "°\n";
    std::cout << "  Speed: " << movementSpeed << ", Sensitivity: " << mouseSensitivity << "\n";
    
    if (cameraEntity != 0) {
        std::cout << "  Attached to entity: " << cameraEntity << "\n";
    } else {
        std::cout << "  Not attached to any entity\n";
    }
}
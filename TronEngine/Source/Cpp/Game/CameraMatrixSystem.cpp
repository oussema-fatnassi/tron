// TronEngine/Source/Cpp/Game/CameraMatrixSystem.cpp - FIXED DIRECTION
#include "../../Headers/Game/CameraMatrixSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/ConstantBuffers.hpp"
#include "../../Headers/Rendering/D3D/BufferedCommandQueue.hpp"
#include "../../Headers/Communication/RenderCommand.hpp"
#include <iostream>

CameraMatrixSystem::CameraMatrixSystem(BufferedCommandQueue* cmdQueue)
    : activeCamera(nullptr)
    , commandQueue(cmdQueue)
    , matricesDirty(true)
    , cameraEntity(0) {
    
    // Initialize with sensible default matrices - FIXED INITIAL DIRECTION
    viewMatrix = Matrix::LookAt(
        0.0f, 2.0f, 5.0f,    // Camera position (back and up)
        0.0f, 2.0f, 0.0f,    // Look at origin (where the boxes are)
        0.0f, 1.0f, 0.0f     // Up vector
    );
    
    projectionMatrix = Matrix::Perspective(
        DegreesToRadians(75.0f),  // 75-degree FOV
        16.0f / 9.0f,             // 16:9 aspect ratio
        0.1f,                     // Near plane
        1000.0f                   // Far plane
    );
    
    std::cout << "[CameraMatrixSystem] Initialized - camera at (0,2,5) looking at (0,2,0)\n";
}

void CameraMatrixSystem::Update(float deltaTime) {
    // Update camera from entity transform
    if (cameraEntity != 0 && world) {
        UpdateCameraFromEntity();
    } else if (activeCamera) {
        // Fallback: Use Camera object if available
        activeCamera->Update(deltaTime, nullptr);
        UpdateMatricesFromCamera();
    }
    
    matricesDirty = false;
}

void CameraMatrixSystem::UpdateCameraFromEntity() {
    if (!world || cameraEntity == 0) return;
    
    auto* transform = world->GetComponent<Transform>(cameraEntity);
    if (!transform) return;
    
    // Camera position
    float eyeX = transform->x;
    float eyeY = transform->y; 
    float eyeZ = transform->z;
    
    // FIXED: Calculate forward vector correctly
    float pitch = transform->pitch;
    float yaw = transform->yaw;
    
    // Standard FPS camera forward calculation
    float forwardX = sinf(yaw) * cosf(pitch);      // FIXED: positive sin for yaw
    float forwardY = -sinf(pitch);                 // Look up/down
    float forwardZ = -cosf(yaw) * cosf(pitch);     // FIXED: negative cos for forward (toward negative Z)
    
    // Target position (camera position + forward direction)
    float targetX = eyeX + forwardX;
    float targetY = eyeY + forwardY;
    float targetZ = eyeZ + forwardZ;
    
    // Create view matrix - STANDARD LOOKAT
    viewMatrix = Matrix::LookAt(
        eyeX, eyeY, eyeZ,        // Camera position
        targetX, targetY, targetZ, // Look at target
        0.0f, 1.0f, 0.0f         // Up vector (world Y-up)
    );
    
    // Debug output with better frequency and more detail
    static int debugCount = 0;
    if ((++debugCount % 120) == 0) { // Every 1 second at 120fps
        std::cout << "[CameraMatrix] Camera(" << eyeX << "," << eyeY << "," << eyeZ 
                  << ") -> Target(" << targetX << "," << targetY << "," << targetZ << ")\n";
        std::cout << "[CameraMatrix] Forward vector: (" << forwardX << "," << forwardY << "," << forwardZ << ")\n";
        std::cout << "[CameraMatrix] Yaw=" << (yaw * 180.0f / 3.14159f) << "° Pitch=" << (pitch * 180.0f / 3.14159f) << "°\n";
    }
}

void CameraMatrixSystem::UpdateMatricesFromCamera() {
    if (!activeCamera) return;
    
    viewMatrix = activeCamera->GetViewMatrix();
    projectionMatrix = activeCamera->GetProjectionMatrix();
    matricesDirty = false;
}

void CameraMatrixSystem::SetCameraEntity(Entity entity) {
    if (cameraEntity != entity) {
        cameraEntity = entity;
        MarkMatricesDirty();
        
        if (entity != 0) {
            std::cout << "[CameraMatrixSystem] Camera entity set to " << entity << "\n";
        } else {
            std::cout << "[CameraMatrixSystem] Camera entity cleared\n";
        }
    }
}

void CameraMatrixSystem::SetActiveCamera(Camera* camera) {
    if (activeCamera != camera) {
        activeCamera = camera;
        MarkMatricesDirty();
        
        if (camera) {
            std::cout << "[CameraMatrixSystem] Active camera object set\n";
        } else {
            std::cout << "[CameraMatrixSystem] Active camera object cleared\n";
        }
    }
}

const Matrix& CameraMatrixSystem::GetViewMatrix() const {
    return viewMatrix;
}

const Matrix& CameraMatrixSystem::GetProjectionMatrix() const {
    return projectionMatrix;
}

Matrix CameraMatrixSystem::GetViewProjectionMatrix() const {
    return projectionMatrix * viewMatrix;
}

void CameraMatrixSystem::CreateWorldViewProjectionMatrix(const Matrix& worldMatrix, Matrix& outWVP) const {
    // Correct order: Projection * View * World
    outWVP = projectionMatrix * viewMatrix * worldMatrix;
}

void CameraMatrixSystem::PrintCameraMatrices() const {
    std::cout << "\n[CameraMatrixSystem] === MATRIX DEBUG ===\n";
    
    if (cameraEntity != 0 && world) {
        auto* transform = world->GetComponent<Transform>(cameraEntity);
        if (transform) {
            std::cout << "  Entity " << cameraEntity << " Transform:\n";
            std::cout << "    Position: (" << transform->x << ", " << transform->y << ", " << transform->z << ")\n";
            std::cout << "    Rotation: (" << (transform->pitch * 180.0f / 3.14159f) << "°, " 
                      << (transform->yaw * 180.0f / 3.14159f) << "°, " << (transform->roll * 180.0f / 3.14159f) << "°)\n";
                      
            // Calculate and show forward vector
            float pitch = transform->pitch;
            float yaw = transform->yaw;
            float forwardX = sinf(yaw) * cosf(pitch);
            float forwardY = -sinf(pitch);
            float forwardZ = -cosf(yaw) * cosf(pitch);
            
            std::cout << "    Forward Vector: (" << forwardX << ", " << forwardY << ", " << forwardZ << ")\n";
            std::cout << "    Looking At: (" << (transform->x + forwardX) << ", " << (transform->y + forwardY) << ", " << (transform->z + forwardZ) << ")\n";
        }
    }
    
    std::cout << "  View Matrix (first row): [" << viewMatrix.At(0,0) << ", " << viewMatrix.At(0,1) << ", " << viewMatrix.At(0,2) << ", " << viewMatrix.At(0,3) << "]\n";
    std::cout << "  Projection Matrix (diagonal): [" << projectionMatrix.At(0,0) << ", " << projectionMatrix.At(1,1) << ", " << projectionMatrix.At(2,2) << ", " << projectionMatrix.At(3,3) << "]\n";
    std::cout << "==========================================\n\n";
}
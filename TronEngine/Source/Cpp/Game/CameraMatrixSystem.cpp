#include "../../Headers/Game/CameraMatrixSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/ConstantBuffers.hpp"
#include "../../Headers/Rendering/D3D/BufferedCommandQueue.hpp"
#include "../../Headers/Communication/RenderCommand.hpp"
#include <iostream>

CameraMatrixSystem::CameraMatrixSystem(BufferedCommandQueue* cmdQueue)
    : activeCamera(nullptr)
    , commandQueue(cmdQueue)
    , matricesDirty(true) {
    
    // Initialize with identity matrices
    viewMatrix = Matrix::Identity();
    projectionMatrix = Matrix::Perspective(
        DegreesToRadians(75.0f),  // 75-degree FOV
        16.0f / 9.0f,             // 16:9 aspect ratio
        0.1f,                     // Near plane
        1000.0f                   // Far plane
    );
    
    std::cout << "[CameraMatrixSystem] Initialized with default projection matrix\n";
}

void CameraMatrixSystem::Update(float deltaTime) {
    if (!activeCamera) {
        return; // No camera to update
    }
    
    // Update camera (handles input, movement, etc.)
    // Note: You'll need to pass InputManager to camera
    // activeCamera->Update(deltaTime, inputManager);
    
    // Update matrices if camera changed
    UpdateMatrices();
    
    // Send updated matrices to render thread
    if (commandQueue) {
        SendCameraMatricesToRenderer();
    }
}

void CameraMatrixSystem::SetActiveCamera(Camera* camera) {
    if (activeCamera != camera) {
        activeCamera = camera;
        MarkMatricesDirty();
        
        if (camera) {
            std::cout << "[CameraMatrixSystem] Active camera set\n";
            camera->PrintCameraInfo();
        } else {
            std::cout << "[CameraMatrixSystem] No active camera\n";
        }
    }
}

const Matrix& CameraMatrixSystem::GetViewMatrix() const {
    if (matricesDirty) {
        const_cast<CameraMatrixSystem*>(this)->UpdateMatrices();
    }
    return viewMatrix;
}

const Matrix& CameraMatrixSystem::GetProjectionMatrix() const {
    if (matricesDirty) {
        const_cast<CameraMatrixSystem*>(this)->UpdateMatrices();
    }
    return projectionMatrix;
}

Matrix CameraMatrixSystem::GetViewProjectionMatrix() const {
    return GetProjectionMatrix() * GetViewMatrix();
}

void CameraMatrixSystem::CreateWorldViewProjectionMatrix(const Matrix& worldMatrix, Matrix& outWVP) const {
    // World * View * Projection (order matters!)
    outWVP = GetProjectionMatrix() * GetViewMatrix() * worldMatrix;
}

void CameraMatrixSystem::UpdateMatrices() {
    if (!activeCamera || !matricesDirty) {
        return;
    }
    
    // Get updated matrices from camera
    viewMatrix = activeCamera->GetViewMatrix();
    projectionMatrix = activeCamera->GetProjectionMatrix();
    
    matricesDirty = false;
    
    // Debug output (remove in production)
    static int frameCount = 0;
    if ((++frameCount % 300) == 0) { // Every ~2.5 seconds at 120fps
        std::cout << "[CameraMatrixSystem] Matrices updated (frame " << frameCount << ")\n";
    }
}

void CameraMatrixSystem::SendCameraMatricesToRenderer() {
    // Create a special render command for camera matrices
    // This will be processed by RenderExecutor to update constant buffers
    
    RenderCommand cameraCommand;
    cameraCommand.type = RenderCommandType::SET_VIEWPORT; // Reuse existing type
    
    // For now, we'll integrate this into the MeshRenderSystem
    // The matrices will be sent with each mesh command
    
    // TODO: Implement camera-specific render command if needed
}

void CameraMatrixSystem::PrintCameraMatrices() const {
    std::cout << "[CameraMatrixSystem] === Camera Matrices ===\n";
    
    if (activeCamera) {
        float x, y, z, pitch, yaw, roll;
        activeCamera->GetPosition(x, y, z);
        activeCamera->GetRotation(pitch, yaw, roll);
        
        std::cout << "  Camera Position: (" << x << ", " << y << ", " << z << ")\n";
        std::cout << "  Camera Rotation: (" << RadiansToDegrees(pitch) << "°, " 
                  << RadiansToDegrees(yaw) << "°, " << RadiansToDegrees(roll) << "°)\n";
        std::cout << "  FOV: " << activeCamera->fovDegrees << "°\n";
        std::cout << "  Aspect: " << activeCamera->aspectRatio << "\n";
    } else {
        std::cout << "  No active camera\n";
    }
    
    std::cout << "  View Matrix:\n";
    viewMatrix.Print();
    
    std::cout << "  Projection Matrix:\n";
    projectionMatrix.Print();
    
    std::cout << "========================================\n";
}
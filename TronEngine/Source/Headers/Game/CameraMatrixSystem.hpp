// TronEngine/Source/Headers/Game/CameraMatrixSystem.hpp - FIXED VERSION
#pragma once
#include "System.hpp"
#include "../Math/Matrix.hpp"
#include "../Rendering/Camera.hpp"
#include "Types.hpp"

// Forward declarations
class BufferedCommandQueue;
struct CameraMatricesBuffer;

// <summary>
// CameraMatrixSystem - FIXED VERSION with Entity support
// This system manages camera matrices from either Entity transforms or Camera objects
// </summary>
class CameraMatrixSystem : public System {
private:
    // Camera sources (use either entity OR camera object)
    Entity cameraEntity;                     // Entity with Transform component (preferred)
    Camera* activeCamera;                    // Camera object (fallback)
    
    BufferedCommandQueue* commandQueue;     // Communication with render thread
    
    // Cached matrices (updated when camera changes)
    mutable Matrix viewMatrix;
    mutable Matrix projectionMatrix;
    mutable bool matricesDirty;

public:
    CameraMatrixSystem(BufferedCommandQueue* cmdQueue);
    ~CameraMatrixSystem() = default;

    // System interface
    void Update(float deltaTime) override;

    // Camera management - NEW: Support for Entity-based cameras
    void SetCameraEntity(Entity entity);     // Use entity transform as camera
    Entity GetCameraEntity() const { return cameraEntity; }
    
    void SetActiveCamera(Camera* camera);    // Use camera object (fallback)
    Camera* GetActiveCamera() const { return activeCamera; }
    
    // Matrix access
    const Matrix& GetViewMatrix() const;
    const Matrix& GetProjectionMatrix() const;
    Matrix GetViewProjectionMatrix() const;
    
    // For render commands
    void CreateWorldViewProjectionMatrix(const Matrix& worldMatrix, Matrix& outWVP) const;
    
    // Debug
    void PrintCameraMatrices() const;
    void DebugPrintMatrices() const;

private:
    // Update methods
    void UpdateCameraFromEntity();           // NEW: Update from entity transform
    void UpdateMatricesFromCamera();         // Update from camera object
    void MarkMatricesDirty() { matricesDirty = true; }
};
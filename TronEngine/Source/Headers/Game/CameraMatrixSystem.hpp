// Create new file: TronEngine/Source/Headers/Game/CameraMatrixSystem.hpp

#pragma once
#include "System.hpp"
#include "../Math/Matrix.hpp"
#include "../Rendering/Camera.hpp"

// Forward declarations
class BufferedCommandQueue;
struct CameraMatricesBuffer;

// <summary>
// CameraMatrixSystem - Manages camera matrices and integrates with rendering
// This system updates camera matrices and sends them to the render thread
// </summary>
class CameraMatrixSystem : public System {
private:
    Camera* activeCamera;                    // Currently active camera
    BufferedCommandQueue* commandQueue;     // Communication with render thread
    
    // Cached matrices (updated when camera changes)
    mutable Matrix viewMatrix;
    mutable Matrix projectionMatrix;
    mutable bool matricesDirty;
    
    // Rendering integration
    void SendCameraMatricesToRenderer();

public:
    CameraMatrixSystem(BufferedCommandQueue* cmdQueue);
    ~CameraMatrixSystem() = default;

    // System interface
    void Update(float deltaTime) override;

    // Camera management
    void SetActiveCamera(Camera* camera);
    Camera* GetActiveCamera() const { return activeCamera; }
    
    // Matrix access
    const Matrix& GetViewMatrix() const;
    const Matrix& GetProjectionMatrix() const;
    Matrix GetViewProjectionMatrix() const;
    
    // For render commands
    void CreateWorldViewProjectionMatrix(const Matrix& worldMatrix, Matrix& outWVP) const;
    
    // Debug
    void PrintCameraMatrices() const;

private:
    void UpdateMatrices();
    void MarkMatricesDirty() { matricesDirty = true; }
};
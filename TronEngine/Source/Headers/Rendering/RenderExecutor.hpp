#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include "../Communication/RenderCommand.hpp"

// Forward declarations
class RenderEngine;
class MeshManager;
class MaterialManager;
struct MeshData;
struct Shader;
struct Material;

// <summary>
// RenderExecutor - Pure rendering class with NO ECS knowledge
// This class executes RenderCommands from the game thread using D3D11
// </summary>
// <remarks>
// CLEAN SEPARATION: This class operates in the render thread and handles only D3D11 API calls.
// It processes RenderCommands without any knowledge of entities, components, or systems.
// All game logic has been abstracted away into RenderCommands.
// 
// KEY POINT: No naming collision with MeshRenderer component!
// </remarks>
class RenderExecutor {

    struct ObjectTransformBuffer {
        float position[3];    // x, y, z
        float padding1;       // Padding for 16-byte alignment
        float scale[3];       // scaleX, scaleY, scaleZ
        float padding2;       // Padding for 16-byte alignment  
        float rotation[3];    // pitch, yaw, roll
        float padding3;       // Padding for 16-byte alignment
    };

    // Camera matrices buffer structure
    struct CameraMatricesBuffer {
        float viewMatrix[16];        // Camera view matrix (4x4)
        float projectionMatrix[16];  // Camera projection matrix (4x4)  
        float viewProjectionMatrix[16]; // Combined matrix (4x4)
        float cameraPosition[3];     // Camera world position
        float padding;               // Padding for 16-byte alignment
    };

private:
    RenderEngine* renderEngine;
    MeshManager* meshManager;
    MaterialManager* materialManager;

    // Current frame statistics
    uint32_t drawCallsThisFrame;
    uint32_t verticesRenderedThisFrame;

public:
    RenderExecutor(RenderEngine* engine, MeshManager* meshMgr, MaterialManager* materialMgr);
    ~RenderExecutor() = default;

    // Main rendering methods
    void ExecuteRenderCommands(const std::vector<RenderCommand>& commands);  // Batch processing
    void ExecuteRenderCommand(const RenderCommand& command);                 // Single command

    // Frame statistics
    uint32_t GetDrawCallsThisFrame() const { return drawCallsThisFrame; }
    uint32_t GetVerticesRenderedThisFrame() const { return verticesRenderedThisFrame; }
    void ResetFrameStats();

private:
    // Core rendering operations
    void RenderSingleMesh(const RenderCommand& command);
    void RenderMeshBatch(const std::vector<RenderCommand>& meshCommands);
    void ClearScreen(const RenderCommand& command);

    // Pipeline setup helpers
    bool SetupShaderPipeline(ID3D11DeviceContext* context, const std::string& shaderName);
    void UpdateRenderConstants(ID3D11DeviceContext* context, const RenderCommand& command);

    // Resource validation
    bool ValidateRenderResources(const RenderCommand& command,
        MeshData*& outMesh,
        Shader*& outShader,
        Material*& outMaterial);

    // Camera matrix helpers
    bool GetCameraMatrices(float viewMatrix[16], float projectionMatrix[16], 
                          float viewProjectionMatrix[16], float cameraPosition[3]);
    void CreateFallbackCameraMatrices(CameraMatricesBuffer& cameraData);
    
    // Constant buffer helpers
    void CreateConstantBuffer(size_t size, ID3D11Buffer** buffer, const char* debugName);
    void UpdateConstantBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, 
                             const void* data, size_t size);

    // Matrix/Transform helpers
    void CreateWorldMatrix(const RenderTransform& transform, float worldMatrix[16]);
    void CreateViewProjectionMatrix(float viewProjMatrix[16]);
    
    // Matrix math helpers
    void CreateViewMatrix(const float cameraPos[3], float pitch, float yaw, float roll, float viewMatrix[16]);
    void CreateProjectionMatrix(float fovDegrees, float aspectRatio, float nearPlane, float farPlane, float projMatrix[16]);
    void MultiplyMatrices(const float matrixA[16], const float matrixB[16], float result[16]);
    void CreateViewMatrixLookAt(const float cameraPos[3], float pitch, float yaw, float roll, float viewMatrix[16]);
    void CreateLookAtMatrix(float eyeX, float eyeY, float eyeZ,
                           float targetX, float targetY, float targetZ,
                           float upX, float upY, float upZ,
                           float viewMatrix[16]);
};
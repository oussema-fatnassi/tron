#include "../../Headers/Rendering/RenderExecutor.hpp"
#include "../../Headers/Rendering/RenderEngine.hpp"
#include "../../Headers/Rendering/Resources/MeshManager.hpp"
#include "../../Headers/Rendering/Resources/MaterialManager.hpp"
#include "../../Headers/Rendering/Resources/ShaderManager.hpp"
#include "../../Include/EngineExports.hpp"
#include <iostream>
#include <cmath>
#include <cstring>

RenderExecutor::RenderExecutor(RenderEngine* engine, MeshManager* meshMgr, MaterialManager* materialMgr)
    : renderEngine(engine)
    , meshManager(meshMgr)
    , materialManager(materialMgr)
    , drawCallsThisFrame(0)
    , verticesRenderedThisFrame(0) {

    //std::cout << "[RenderExecutor] Pure rendering layer initialized (NO ECS dependencies)\n";
}

// BATCH PROCESSING - Handles multiple commands efficiently
void RenderExecutor::ExecuteRenderCommands(const std::vector<RenderCommand>& commands) {
    if (!renderEngine || commands.empty()) {
        return;
    }

    // Reset frame statistics
    ResetFrameStats();

    //std::cout << "[RenderExecutor] Executing " << commands.size() << " render commands (batch)\n";

    // Separate commands by type for optimization
    std::vector<RenderCommand> meshCommands;

    for (const auto& command : commands) {
        switch (command.type) {
        case RenderCommandType::CLEAR_SCREEN:
            ClearScreen(command);
            break;

        case RenderCommandType::DRAW_MESH:
            if (command.visible) {
                meshCommands.push_back(command);
            }
            break;

        case RenderCommandType::BEGIN_FRAME:
        case RenderCommandType::END_FRAME:
        case RenderCommandType::SET_VIEWPORT:
            // These are handled by RenderEngine itself
            break;
        }
    }

    // Render all meshes in optimized batch
    if (!meshCommands.empty()) {
        RenderMeshBatch(meshCommands);
    }

    //std::cout << "[RenderExecutor] Batch complete - " << drawCallsThisFrame
    //    << " draw calls, " << verticesRenderedThisFrame << " vertices\n";
}

// SINGLE COMMAND PROCESSING - Handles one command immediately
void RenderExecutor::ExecuteRenderCommand(const RenderCommand& command) {
    if (!renderEngine) {
        return;
    }

    // Handle single command immediately (no batching optimization)
    switch (command.type) {
    case RenderCommandType::CLEAR_SCREEN:
        ClearScreen(command);
        break;

    case RenderCommandType::DRAW_MESH:
        if (command.visible) {
            // Set up shader pipeline for single command
            ID3D11DeviceContext* context = renderEngine->GetDeviceContext();
            if (context && SetupShaderPipeline(context, command.shaderName)) {
                RenderSingleMesh(command);
            }
        }
        break;

    case RenderCommandType::BEGIN_FRAME:
    case RenderCommandType::END_FRAME:
    case RenderCommandType::SET_VIEWPORT:
        break;
    }
}

void RenderExecutor::RenderMeshBatch(const std::vector<RenderCommand>& meshCommands) {
    ID3D11DeviceContext* context = renderEngine->GetDeviceContext();
    if (!context) {
        return;
    }

    std::string currentShader = "";

    for (const auto& command : meshCommands) {
        // Optimization: Only change shader if different from current
        if (command.shaderName != currentShader) {
            if (!SetupShaderPipeline(context, command.shaderName)) {
                continue;
            }
            currentShader = command.shaderName;
        }

        RenderSingleMesh(command);
    }
}

void RenderExecutor::RenderSingleMesh(const RenderCommand& command) {
    ID3D11DeviceContext* context = renderEngine->GetDeviceContext();

    // Validate and get resources
    MeshData* meshData = nullptr;
    Shader* shader = nullptr;
    Material* material = nullptr;

    if (!ValidateRenderResources(command, meshData, shader, material)) {
        return; // Resource validation failed
    }

    // Update constants (transform, color, etc.)
    UpdateRenderConstants(context, command);

    // Set vertex and index buffers  
    UINT stride = sizeof(float) * 6; // pos(3) + color(3) - matches your vertex format
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &meshData->vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(meshData->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Execute draw call
    context->DrawIndexed(meshData->indexCount, 0, 0);

    // Update statistics
    drawCallsThisFrame++;
    verticesRenderedThisFrame += meshData->indexCount; // Approximation
}

void RenderExecutor::ClearScreen(const RenderCommand& command) {
    // Note: In your architecture, screen clearing is handled by RenderEngine::BeginFrame()
    // This is here for completeness if you want command-based clearing
}

bool RenderExecutor::SetupShaderPipeline(ID3D11DeviceContext* context, const std::string& shaderName) {
    if (!renderEngine) {
        return false;
    }

    // Get shader from render engine
    Shader* shader = renderEngine->GetShader(shaderName);
    if (!shader || !shader->vertexShader || !shader->pixelShader || !shader->inputLayout) {
        return false;
    }

    // Set shader pipeline
    context->IASetInputLayout(shader->inputLayout);
    context->VSSetShader(shader->vertexShader, nullptr, 0);
    context->PSSetShader(shader->pixelShader, nullptr, 0);

    return true;
}

void RenderExecutor::UpdateRenderConstants(ID3D11DeviceContext* context, const RenderCommand& command) {
    // === OBJECT TRANSFORM BUFFER (b0) ===
    ObjectTransformBuffer transformData = {};
    
    // Copy position from render command
    transformData.position[0] = command.transform.position[0];
    transformData.position[1] = command.transform.position[1]; 
    transformData.position[2] = command.transform.position[2];
    
    // Copy scale (default to 1.0 if not provided)
    transformData.scale[0] = (command.transform.scale[0] != 0.0f) ? command.transform.scale[0] : 1.0f;
    transformData.scale[1] = (command.transform.scale[1] != 0.0f) ? command.transform.scale[1] : 1.0f;
    transformData.scale[2] = (command.transform.scale[2] != 0.0f) ? command.transform.scale[2] : 1.0f;
    
    // Copy rotation
    transformData.rotation[0] = command.transform.rotation[0];
    transformData.rotation[1] = command.transform.rotation[1];
    transformData.rotation[2] = command.transform.rotation[2];
    
    // === CAMERA MATRICES BUFFER (b1) ===
    CameraMatricesBuffer cameraData = {};
    
    // Get camera matrices from the player camera
    bool cameraMatricesValid = GetCameraMatrices(
        cameraData.viewMatrix, 
        cameraData.projectionMatrix, 
        cameraData.viewProjectionMatrix,
        cameraData.cameraPosition
    );
    
    if (!cameraMatricesValid) {
        // Fallback: Create simple identity/orthographic matrices
        CreateFallbackCameraMatrices(cameraData);
    }
    
    // === UPDATE CONSTANT BUFFERS ===
    
    // Update object transform buffer (b0)
    static ID3D11Buffer* s_transformBuffer = nullptr;
    if (!s_transformBuffer) {
        CreateConstantBuffer(sizeof(ObjectTransformBuffer), &s_transformBuffer, "ObjectTransform");
    }
    UpdateConstantBuffer(context, s_transformBuffer, &transformData, sizeof(ObjectTransformBuffer));
    context->VSSetConstantBuffers(0, 1, &s_transformBuffer);
    
    // Update camera matrices buffer (b1) 
    static ID3D11Buffer* s_cameraBuffer = nullptr;
    if (!s_cameraBuffer) {
        CreateConstantBuffer(sizeof(CameraMatricesBuffer), &s_cameraBuffer, "CameraMatrices");
    }
    UpdateConstantBuffer(context, s_cameraBuffer, &cameraData, sizeof(CameraMatricesBuffer));
    context->VSSetConstantBuffers(1, 1, &s_cameraBuffer);
    
    // Keep existing color buffer for pixel shader (b1 in pixel shader, different from vertex shader b1)
    ID3D11Buffer* colorBuffer = renderEngine->GetColorConstantBuffer();
    if (colorBuffer) {
        context->PSSetConstantBuffers(1, 1, &colorBuffer);
    }
}

// Helper method to get camera matrices from the player camera
bool RenderExecutor::GetCameraMatrices(float viewMatrix[16], float projectionMatrix[16], 
                                      float viewProjectionMatrix[16], float cameraPosition[3]) {
    // Try to get camera position
    if (!GetPlayerCameraPosition(&cameraPosition[0], &cameraPosition[1], &cameraPosition[2])) {
        return false;
    }
    
    // Get camera rotation
    float pitch, yaw, roll;
    if (!GetPlayerCameraRotation(&pitch, &yaw, &roll)) {
        return false;
    }
    
    // Create view matrix using proper look-at calculation
    CreateViewMatrixLookAt(cameraPosition, pitch, yaw, roll, viewMatrix);
    
    // Create projection matrix 
    CreateProjectionMatrix(75.0f, 16.0f/9.0f, 0.1f, 1000.0f, projectionMatrix);
    
    // Multiply projection * view for combined matrix (note the order!)
    MultiplyMatrices(projectionMatrix, viewMatrix, viewProjectionMatrix);
    
    return true;
}

// Helper method to create fallback matrices when camera is not available
void RenderExecutor::CreateFallbackCameraMatrices(CameraMatricesBuffer& cameraData) {
    // Create identity view matrix
    std::memset(cameraData.viewMatrix, 0, sizeof(float) * 16);
    cameraData.viewMatrix[0] = cameraData.viewMatrix[5] = cameraData.viewMatrix[10] = cameraData.viewMatrix[15] = 1.0f;
    
    // Create simple orthographic projection (your original approach)
    std::memset(cameraData.projectionMatrix, 0, sizeof(float) * 16);
    cameraData.projectionMatrix[0] = 0.1f;  // Scale X
    cameraData.projectionMatrix[5] = 0.1f;  // Scale Y  
    cameraData.projectionMatrix[10] = 1.0f; // Z
    cameraData.projectionMatrix[15] = 1.0f; // W
    
    // Copy projection as view-projection (since view is identity)
    std::memcpy(cameraData.viewProjectionMatrix, cameraData.projectionMatrix, sizeof(float) * 16);
    
    // Default camera position
    cameraData.cameraPosition[0] = 0.0f;
    cameraData.cameraPosition[1] = 0.0f;
    cameraData.cameraPosition[2] = 0.0f;
}

// Helper methods for constant buffer management
void RenderExecutor::CreateConstantBuffer(size_t size, ID3D11Buffer** buffer, const char* debugName) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = static_cast<UINT>(size);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    HRESULT hr = renderEngine->GetDevice()->CreateBuffer(&bufferDesc, nullptr, buffer);
    if (FAILED(hr)) {
        std::cout << "[RenderExecutor] Failed to create " << debugName << " constant buffer: " 
                  << std::hex << hr << std::dec << "\n";
        *buffer = nullptr;
    }
}

void RenderExecutor::UpdateConstantBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, 
                                         const void* data, size_t size) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    if (SUCCEEDED(hr)) {
        std::memcpy(mappedResource.pData, data, size);
        context->Unmap(buffer, 0);
    } else {
        std::cout << "[RenderExecutor] Failed to map constant buffer: " 
                  << std::hex << hr << std::dec << "\n";
    }
}

bool RenderExecutor::ValidateRenderResources(const RenderCommand& command,
    MeshData*& outMesh,
    Shader*& outShader,
    Material*& outMaterial) {
    // Validate mesh
    outMesh = meshManager->GetMesh(command.meshName);
    if (!outMesh || !outMesh->vertexBuffer || !outMesh->indexBuffer) {
        return false;
    }

    // Validate shader
    outShader = renderEngine->GetShader(command.shaderName);
    if (!outShader) {
        return false;
    }

    // Validate material (optional)
    outMaterial = nullptr;
    if (!command.materialName.empty() && materialManager) {
        outMaterial = materialManager->GetMaterial(command.materialName);
    }

    return true;
}

void RenderExecutor::CreateWorldMatrix(const RenderTransform& transform, float worldMatrix[16]) {
    // TODO: Implement proper matrix math
    // For now, create identity matrix
    for (int i = 0; i < 16; ++i) {
        worldMatrix[i] = 0.0f;
    }
    worldMatrix[0] = worldMatrix[5] = worldMatrix[10] = worldMatrix[15] = 1.0f;

    // Apply translation
    worldMatrix[12] = transform.position[0];
    worldMatrix[13] = transform.position[1];
    worldMatrix[14] = transform.position[2];
}

void RenderExecutor::CreateViewProjectionMatrix(float viewProjMatrix[16]) {
    // TODO: Implement proper camera/projection matrix
    // For now, create identity matrix
    for (int i = 0; i < 16; ++i) {
        viewProjMatrix[i] = 0.0f;
    }
    viewProjMatrix[0] = viewProjMatrix[5] = viewProjMatrix[10] = viewProjMatrix[15] = 1.0f;
}

// Matrix math helper implementations
void RenderExecutor::CreateViewMatrix(const float cameraPos[3], float pitch, float yaw, float roll, float viewMatrix[16]) {
    // Simplified view matrix creation
    // For now, create a basic look-at matrix
    
    // Initialize as identity
    std::memset(viewMatrix, 0, sizeof(float) * 16);
    viewMatrix[0] = viewMatrix[5] = viewMatrix[10] = viewMatrix[15] = 1.0f;
    
    // Apply camera translation (inverse of camera position)
    viewMatrix[12] = -cameraPos[0];
    viewMatrix[13] = -cameraPos[1];
    viewMatrix[14] = -cameraPos[2];
    
    // TODO: Apply rotation matrices for pitch/yaw/roll
    // For now, just use translation
}

void RenderExecutor::CreateProjectionMatrix(float fovDegrees, float aspectRatio, float nearPlane, float farPlane, float projMatrix[16]) {
    // Convert FOV to radians
    float fovRadians = fovDegrees * 3.14159265f / 180.0f;
    float tanHalfFov = tanf(fovRadians * 0.5f);
    
    // Clear matrix
    std::memset(projMatrix, 0, sizeof(float) * 16);
    
    // Create perspective projection matrix (column-major for HLSL)
    projMatrix[0]  = 1.0f / (aspectRatio * tanHalfFov);  // [0][0]
    projMatrix[5]  = 1.0f / tanHalfFov;                  // [1][1]
    projMatrix[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);  // [2][2]
    projMatrix[11] = -1.0f;                              // [2][3]
    projMatrix[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);  // [3][2]
    // projMatrix[15] = 0.0f (already set by memset)
}

void RenderExecutor::MultiplyMatrices(const float matrixA[16], const float matrixB[16], float result[16]) {
    // Temporary result to handle in-place multiplication
    float temp[16];
    
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            temp[col * 4 + row] = 0.0f;
            for (int k = 0; k < 4; k++) {
                temp[col * 4 + row] += matrixA[k * 4 + row] * matrixB[col * 4 + k];
            }
        }
    }
    
    // Copy result back
    std::memcpy(result, temp, sizeof(float) * 16);
}

void RenderExecutor::ResetFrameStats() {
    drawCallsThisFrame = 0;
    verticesRenderedThisFrame = 0;
}

void RenderExecutor::CreateViewMatrixLookAt(const float cameraPos[3], float pitch, float yaw, float roll, float viewMatrix[16]) {
    // Calculate forward vector from pitch and yaw
    float cosPitch = cosf(pitch);
    float sinPitch = sinf(pitch);
    float cosYaw = cosf(yaw);
    float sinYaw = sinf(yaw);
    
    float forwardX = cosYaw * cosPitch;
    float forwardY = -sinPitch;  // Negative because Y points up
    float forwardZ = sinYaw * cosPitch;
    
    // Target point = camera position + forward vector
    float targetX = cameraPos[0] + forwardX;
    float targetY = cameraPos[1] + forwardY;
    float targetZ = cameraPos[2] + forwardZ;
    
    // Up vector (world up)
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    
    // Create look-at view matrix
    CreateLookAtMatrix(cameraPos[0], cameraPos[1], cameraPos[2],
                       targetX, targetY, targetZ,
                       upX, upY, upZ,
                       viewMatrix);
}

void RenderExecutor::CreateLookAtMatrix(float eyeX, float eyeY, float eyeZ,
                                       float targetX, float targetY, float targetZ,
                                       float upX, float upY, float upZ,
                                       float viewMatrix[16]) {
    // Calculate forward vector (normalized)
    float forwardX = targetX - eyeX;
    float forwardY = targetY - eyeY;
    float forwardZ = targetZ - eyeZ;
    float forwardLength = sqrtf(forwardX*forwardX + forwardY*forwardY + forwardZ*forwardZ);
    forwardX /= forwardLength;
    forwardY /= forwardLength;
    forwardZ /= forwardLength;
    
    // Calculate right vector (forward x up, normalized)
    float rightX = forwardY * upZ - forwardZ * upY;
    float rightY = forwardZ * upX - forwardX * upZ;
    float rightZ = forwardX * upY - forwardY * upX;
    float rightLength = sqrtf(rightX*rightX + rightY*rightY + rightZ*rightZ);
    rightX /= rightLength;
    rightY /= rightLength;
    rightZ /= rightLength;
    
    // Calculate true up vector (right x forward)
    float trueUpX = rightY * forwardZ - rightZ * forwardY;
    float trueUpY = rightZ * forwardX - rightX * forwardZ;
    float trueUpZ = rightX * forwardY - rightY * forwardX;
    
    // Build view matrix (column-major for HLSL)
    viewMatrix[0]  = rightX;   viewMatrix[1]  = trueUpX;   viewMatrix[2]  = -forwardX;  viewMatrix[3]  = 0.0f;
    viewMatrix[4]  = rightY;   viewMatrix[5]  = trueUpY;   viewMatrix[6]  = -forwardY;  viewMatrix[7]  = 0.0f;
    viewMatrix[8]  = rightZ;   viewMatrix[9]  = trueUpZ;   viewMatrix[10] = -forwardZ;  viewMatrix[11] = 0.0f;
    viewMatrix[12] = -(rightX * eyeX + rightY * eyeY + rightZ * eyeZ);
    viewMatrix[13] = -(trueUpX * eyeX + trueUpY * eyeY + trueUpZ * eyeZ);
    viewMatrix[14] = forwardX * eyeX + forwardY * eyeY + forwardZ * eyeZ;
    viewMatrix[15] = 1.0f;
}

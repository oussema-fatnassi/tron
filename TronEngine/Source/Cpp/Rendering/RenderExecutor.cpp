#include "../../Headers/Rendering/RenderExecutor.hpp"
#include "../../Headers/Rendering/RenderEngine.hpp"
#include "../../Headers/Rendering/Resources/MeshManager.hpp"
#include "../../Headers/Rendering/Resources/MaterialManager.hpp"
#include "../../Headers/Rendering/Resources/ShaderManager.hpp"
#include "../../Headers/Game/ConstantBuffers.hpp"
#include <iostream>
#include <cmath>

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

    //std::cout << "[RenderExecutor] Executing single render command: ";

    // Handle single command immediately (no batching optimization)
    switch (command.type) {
    case RenderCommandType::CLEAR_SCREEN:
        //std::cout << "CLEAR_SCREEN\n";
        ClearScreen(command);
        break;

    case RenderCommandType::DRAW_MESH:
        //std::cout << "DRAW_MESH (" << command.meshName << ")\n";
        if (command.visible) {
            // Set up shader pipeline for single command
            ID3D11DeviceContext* context = renderEngine->GetDeviceContext();
            if (context && SetupShaderPipeline(context, command.shaderName)) {
                RenderSingleMesh(command);
            }
        }
        break;

    case RenderCommandType::BEGIN_FRAME:
        //std::cout << "BEGIN_FRAME\n";
        break;
    case RenderCommandType::END_FRAME:
        //std::cout << "END_FRAME\n";
        break;
    case RenderCommandType::SET_VIEWPORT:
        //std::cout << "SET_VIEWPORT\n";
        break;
    }
}

void RenderExecutor::RenderMeshBatch(const std::vector<RenderCommand>& meshCommands) {
    ID3D11DeviceContext* context = renderEngine->GetDeviceContext();
    if (!context) {
        //std::cout << "[RenderExecutor] Error: No D3D11 context available\n";
        return;
    }

    std::string currentShader = "";

    for (const auto& command : meshCommands) {
        // Optimization: Only change shader if different from current
        if (command.shaderName != currentShader) {
            if (!SetupShaderPipeline(context, command.shaderName)) {
                //std::cout << "[RenderExecutor] Warning: Failed to setup shader '"
                //    << command.shaderName << "' for mesh '" << command.meshName << "'\n";
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
   // std::cout << "[RenderExecutor] Clear screen command (handled by RenderEngine)\n";
}

bool RenderExecutor::SetupShaderPipeline(ID3D11DeviceContext* context, const std::string& shaderName) {
    if (!renderEngine) {
        return false;
    }

    // Get shader from render engine
    Shader* shader = renderEngine->GetShader(shaderName);
    if (!shader || !shader->vertexShader || !shader->pixelShader || !shader->inputLayout) {
        //std::cout << "[RenderExecutor] Error: Shader '" << shaderName << "' not found or incomplete\n";
        return false;
    }

    // Set shader pipeline
    context->IASetInputLayout(shader->inputLayout);
    context->VSSetShader(shader->vertexShader, nullptr, 0);
    context->PSSetShader(shader->pixelShader, nullptr, 0);

    return true;
}

void RenderExecutor::UpdateRenderConstants(ID3D11DeviceContext* context, const RenderCommand& command) {
    // Create camera matrices constant buffer (NEW APPROACH)
    static ID3D11Buffer* s_cameraMatricesBuffer = nullptr;
    
    if (!s_cameraMatricesBuffer) {
        // Create the camera matrices constant buffer once
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = sizeof(CameraMatricesBuffer);
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        HRESULT hr = renderEngine->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &s_cameraMatricesBuffer);
        
        if (FAILED(hr)) {
            std::cout << "[RenderExecutor] Failed to create camera matrices constant buffer: " 
                      << std::hex << hr << std::dec << "\n";
            return;
        }
        
        std::cout << "[RenderExecutor] Created camera matrices constant buffer\n";
    }
    
    // Prepare camera matrices data
    CameraMatricesBuffer cameraData;
    
    if (command.cameraMatrices.hasValidMatrices) {
        // Use matrices from the command
        memcpy(cameraData.worldMatrix.Data(), command.cameraMatrices.worldMatrix.data, sizeof(float) * 16);
        memcpy(cameraData.viewMatrix.Data(), command.cameraMatrices.viewMatrix.data, sizeof(float) * 16);
        memcpy(cameraData.projectionMatrix.Data(), command.cameraMatrices.projectionMatrix.data, sizeof(float) * 16);
        memcpy(cameraData.worldViewProjMatrix.Data(), command.cameraMatrices.worldViewProjMatrix.data, sizeof(float) * 16);
    } else {
        // Create matrices from transform data (fallback)
        Matrix worldMatrix = CreateWorldMatrixFromTransform(command.transform);
        Matrix viewMatrix = Matrix::Identity(); // Default view
        Matrix projMatrix = Matrix::Perspective(DegreesToRadians(75.0f), 16.0f/9.0f, 0.1f, 1000.0f);
        
        cameraData.worldMatrix = worldMatrix;
        cameraData.viewMatrix = viewMatrix;
        cameraData.projectionMatrix = projMatrix;
        cameraData.worldViewProjMatrix = projMatrix * viewMatrix * worldMatrix;
    }
    
    // Map and update the camera matrices buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(s_cameraMatricesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    if (SUCCEEDED(hr)) {
        memcpy(mappedResource.pData, &cameraData, sizeof(CameraMatricesBuffer));
        context->Unmap(s_cameraMatricesBuffer, 0);
        
        // Bind to vertex shader constant buffer slot 0
        context->VSSetConstantBuffers(0, 1, &s_cameraMatricesBuffer);
        
        // Keep existing pixel shader constant buffer (slot 1)
        ID3D11Buffer* colorBuffer = renderEngine->GetColorConstantBuffer();
        if (colorBuffer) {
            context->PSSetConstantBuffers(1, 1, &colorBuffer);
        }
        
        // Debug output (remove in production)
        static int debugCount = 0;
        if ((++debugCount % 300) == 0) {
            //std::cout << "[RenderExecutor] Camera matrices updated (frame " << debugCount << ")\n";
        }
    } else {
        std::cout << "[RenderExecutor] Failed to map camera matrices buffer: " 
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
        //std::cout << "[RenderExecutor] Error: Mesh '" << command.meshName << "' not found or invalid\n";
        return false;
    }

    // Validate shader
    outShader = renderEngine->GetShader(command.shaderName);
    if (!outShader) {
        //std::cout << "[RenderExecutor] Error: Shader '" << command.shaderName << "' not found\n";
        return false;
    }

    // Validate material (optional)
    outMaterial = nullptr;
    if (!command.materialName.empty() && materialManager) {
        outMaterial = materialManager->GetMaterial(command.materialName);
        // Material is optional, so don't fail if not found
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

void RenderExecutor::ResetFrameStats() {
    drawCallsThisFrame = 0;
    verticesRenderedThisFrame = 0;
}

// Helper method to create world matrix from transform
Matrix RenderExecutor::CreateWorldMatrixFromTransform(const RenderTransform& transform) {
    // Create TRS matrix: Translation * Rotation * Scale
    Matrix translation = Matrix::Translation(transform.position[0], transform.position[1], transform.position[2]);
    Matrix rotation = Matrix::RotationEuler(transform.rotation[0], transform.rotation[1], transform.rotation[2]);
    Matrix scale = Matrix::Scale(transform.scale[0], transform.scale[1], transform.scale[2]);
    
    return translation * rotation * scale;
}
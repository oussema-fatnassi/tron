#include "../../Headers/Rendering/RenderExecutor.hpp"
#include "../../Headers/Rendering/RenderEngine.hpp"
#include "../../Headers/Rendering/Resources/MeshManager.hpp"
#include "../../Headers/Rendering/Resources/MaterialManager.hpp"
#include "../../Headers/Rendering/Resources/ShaderManager.hpp"
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
        std::cout << "BEGIN_FRAME\n";
        break;
    case RenderCommandType::END_FRAME:
        std::cout << "END_FRAME\n";
        break;
    case RenderCommandType::SET_VIEWPORT:
        std::cout << "SET_VIEWPORT\n";
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
    // Create transform data from the render command
    ObjectTransformBuffer transformData = {};
    
    // Copy position from render command
    transformData.position[0] = command.transform.position[0];
    transformData.position[1] = command.transform.position[1]; 
    transformData.position[2] = command.transform.position[2];
    
    // Copy scale (default to 1.0 if not provided)
    transformData.scale[0] = (command.transform.scale[0] != 0.0f) ? command.transform.scale[0] : 1.0f;
    transformData.scale[1] = (command.transform.scale[1] != 0.0f) ? command.transform.scale[1] : 1.0f;
    transformData.scale[2] = (command.transform.scale[2] != 0.0f) ? command.transform.scale[2] : 1.0f;
    
    // Copy rotation (default to 0.0 if not provided)
    transformData.rotation[0] = command.transform.rotation[0];
    transformData.rotation[1] = command.transform.rotation[1];
    transformData.rotation[2] = command.transform.rotation[2];
    
    std::cout << "[RenderExecutor] Transform data: pos(" 
              << transformData.position[0] << ", " << transformData.position[1] 
              << ", " << transformData.position[2] << ") scale(" 
              << transformData.scale[0] << ", " << transformData.scale[1] 
              << ", " << transformData.scale[2] << ")\n";
    
    // Map the existing constant buffer instead of creating a new one each frame
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    
    // Get or create a persistent constant buffer
    static ID3D11Buffer* s_transformBuffer = nullptr;
    
    if (!s_transformBuffer) {
        // Create the constant buffer once
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = sizeof(ObjectTransformBuffer);
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        HRESULT hr = renderEngine->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &s_transformBuffer);
        
        if (FAILED(hr)) {
            std::cout << "[RenderExecutor] Failed to create transform constant buffer: " 
                      << std::hex << hr << std::dec << "\n";
            return;
        }
        
        std::cout << "[RenderExecutor] Created persistent transform constant buffer\n";
    }
    
    // Map and update the buffer
    HRESULT hr = context->Map(s_transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    if (SUCCEEDED(hr)) {
        // Copy the transform data to the mapped buffer
        memcpy(mappedResource.pData, &transformData, sizeof(ObjectTransformBuffer));
        context->Unmap(s_transformBuffer, 0);
        
        // Bind to vertex shader constant buffer slot 0
        context->VSSetConstantBuffers(0, 1, &s_transformBuffer);
        
        // Also keep the existing color buffer for pixel shader
        ID3D11Buffer* colorBuffer = renderEngine->GetColorConstantBuffer();
        if (colorBuffer) {
            context->PSSetConstantBuffers(1, 1, &colorBuffer);
        }
        
        // Debug: Verify the constant buffer is bound
        std::cout << "[RenderExecutor] Transform constant buffer updated and bound successfully\n";
    } else {
        std::cout << "[RenderExecutor] Failed to map transform constant buffer: " 
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
#include "../../Headers/Game/MeshRenderSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/MeshRendererComponent.hpp"
#include <iostream>

void MeshRenderSystem::Update(float deltaTime) {
    // MeshRenderSystem doesn't do per-frame updates
    // All rendering happens in the Render() method called from the render thread
    (void)deltaTime; // Suppress unused parameter warning
}

void MeshRenderSystem::Render() {
    if (!renderEngine || !meshManager) {
        return;
    }

    ID3D11DeviceContext* context = renderEngine->GetDeviceContext();
    if (!context) {
        return;
    }

    // Iterate through all entities with MeshRenderer components
    for (Entity entity : entities) {
        auto* transform = world->GetComponent<Transform>(entity);
        auto* meshRenderer = world->GetComponent<MeshRenderer>(entity);

        if (!transform || !meshRenderer || !meshRenderer->isVisible) {
            continue; // Skip invisible or incomplete entities
        }

        // Get mesh data
        MeshData* meshData = meshManager->GetMesh(meshRenderer->GetMeshName());
        if (!meshData || !meshData->vertexBuffer || !meshData->indexBuffer) {
            // Mesh not found or not loaded properly
            continue;
        }

        // Set up shader pipeline
        if (!SetupShaderPipeline(context, meshRenderer->shaderName)) {
            continue; // Failed to set up shader
        }

        // Update entity-specific constants (transform, color, etc.)
        UpdateEntityConstants(context, transform, meshRenderer);

        // Set vertex and index buffers
        UINT stride = sizeof(float) * 6; // pos(3) + color(3) - matches your vertex format
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &meshData->vertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(meshData->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Draw the mesh
        context->DrawIndexed(meshData->indexCount, 0, 0);
    }
}

bool MeshRenderSystem::SetupShaderPipeline(ID3D11DeviceContext* context, const std::string& shaderName) {
    if (!renderEngine) {
        return false;
    }

    // Get shader from render engine
    Shader* shader = renderEngine->GetShader(shaderName);
    if (!shader || !shader->vertexShader || !shader->pixelShader || !shader->inputLayout) {
        std::cout << "[MeshRenderSystem] Warning: Shader '" << shaderName << "' not found or incomplete\n";
        return false;
    }

    // Set shader pipeline
    context->IASetInputLayout(shader->inputLayout);
    context->VSSetShader(shader->vertexShader, nullptr, 0);
    context->PSSetShader(shader->pixelShader, nullptr, 0);

    return true;
}

void MeshRenderSystem::UpdateEntityConstants(ID3D11DeviceContext* context,
    const Transform* transform,
    const MeshRenderer* meshRenderer) {
    // TODO: In a full implementation, you would create and update constant buffers here
    // For now, we'll use the existing color constant buffer from RenderEngine

    // FIXED: Get the buffer pointer and pass it correctly
    if (renderEngine) {
        ID3D11Buffer* colorBuffer = renderEngine->GetColorConstantBuffer();
        if (colorBuffer) {
            context->PSSetConstantBuffers(1, 1, &colorBuffer);
        }
    }

    // In a production system, you would:
    // 1. Create a world transformation matrix from the Transform component
    // 2. Update vertex shader constant buffer with the world matrix
    // 3. Update pixel shader constant buffer with color and material properties
    // 4. Handle transparency/alpha blending if needed
}
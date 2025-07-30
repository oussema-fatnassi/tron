#pragma once
#include "System.hpp"
#include "../Rendering/RenderEngine.hpp"
#include "../Rendering/Resources/MeshManager.hpp"
#include "../Rendering/Resources/MaterialManager.hpp"
#include <iostream>

// <summary>
// MeshRenderSystem is a System that handles rendering of entities with MeshRenderer components.
// It integrates with the Transform component to position meshes in world space.
// </summary>
// <remarks>
// This system processes all entities that have both Transform and MeshRenderer components.
// It uses the rendering engine to draw meshes with their associated materials and shaders.
// The system supports both primitive meshes and custom loaded meshes, handling visibility,
// transparency, and color overrides. It works closely with the existing RenderEngine
// architecture and D3D11 rendering pipeline.
// </remarks>
class MeshRenderSystem : public System {
private:
    RenderEngine* renderEngine;
    MeshManager* meshManager;
    MaterialManager* materialManager;

public:
    MeshRenderSystem(RenderEngine* engine, MeshManager* meshMgr, MaterialManager* materialMgr)
        : renderEngine(engine), meshManager(meshMgr), materialManager(materialMgr) {
    }

    void Update(float deltaTime) override;

    // Called during render phase instead of Update for better separation
    void Render();

private:
    // Helper method to set up shader pipeline for an entity
    bool SetupShaderPipeline(ID3D11DeviceContext* context, const std::string& shaderName);

    // Helper method to update constant buffers with entity data
    void UpdateEntityConstants(ID3D11DeviceContext* context,
        const class Transform* transform,
        const struct MeshRenderer* meshRenderer);
};

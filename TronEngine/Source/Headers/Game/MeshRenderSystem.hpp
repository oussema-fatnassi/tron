#pragma once
#include "System.hpp"
#include <iostream>

// Forward declarations - NO rendering dependencies!
class CommandQueue;
struct Transform;
struct MeshRenderer;  // This is the COMPONENT, not our rendering class
struct RenderCommand;

// <summary>
// MeshRenderSystem - CLEAN ECS System with NO Direct3D dependencies
// This system processes entities with Transform + MeshRenderer components
// and generates RenderCommands for the rendering thread to execute.
// </summary>
// <remarks>
// CLEAN SEPARATION: This system operates in the game thread and handles only ECS logic:
// - Iterating over entities with MeshRenderer components
// - Converting ECS data (Transform, MeshRenderer) to RenderCommands
// - Sending commands to the render thread via CommandQueue
// 
// It has NO knowledge of:
// - Direct3D API calls
// - Shaders, buffers, or rendering resources
// - Render engine internals
// 
// All rendering work is abstracted into RenderCommands that are processed
// by the RenderExecutor class in the rendering thread.
// </remarks>
class MeshRenderSystem : public System {
private:
    CommandQueue* commandQueue;          // Communication with render thread
    uint32_t entitiesProcessed;          // Statistics for debugging

public:
    // Constructor takes only CommandQueue - no rendering dependencies
    MeshRenderSystem(CommandQueue* cmdQueue);
    ~MeshRenderSystem() = default;

    // System interface
    void Update(float deltaTime) override;

    // Main method - generates RenderCommands from ECS data
    void GenerateRenderCommands();

    // Entity management callbacks
    void OnEntityAdded(Entity entity) override;
    void OnEntityRemoved(Entity entity) override;

    // Utility methods for game logic
    uint32_t GetVisibleEntityCount() const;
    void SetAllEntitiesVisible(bool visible);
    void PrintSystemStats() const;

    // Getters
    uint32_t GetEntitiesProcessedLastFrame() const { return entitiesProcessed; }

private:
    // Helper method to convert ECS entity data to RenderCommand
    RenderCommand CreateRenderCommandFromEntity(Entity entity,
        const Transform* transform,
        const MeshRenderer* meshRenderer);
};
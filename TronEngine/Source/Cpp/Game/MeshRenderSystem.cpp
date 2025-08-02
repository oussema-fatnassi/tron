#include "../../Headers/Game/MeshRenderSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/MeshRendererComponent.hpp"
#include "../../Headers/Communication/RenderCommand.hpp"
#include "../../Headers/Rendering/D3D/BufferedCommandQueue.hpp"
#include <iostream>

// Constructor now only takes CommandQueue - no direct rendering dependencies
MeshRenderSystem::MeshRenderSystem(BufferedCommandQueue* cmdQueue)
    : commandQueue(cmdQueue)
    , entitiesProcessed(0) {

    //std::cout << "[MeshRenderSystem] Clean ECS rendering system initialized (NO D3D dependencies)\n";
}

void MeshRenderSystem::Update(float deltaTime) {
    // MeshRenderSystem focuses on ECS logic
    // The main work happens in GenerateRenderCommands()

    // Optional: Update any rendering-related logic here
    // (e.g., animation, visibility culling based on distance, etc.)
    (void)deltaTime; // Suppress unused parameter warning
}

void MeshRenderSystem::GenerateRenderCommands() {
    if (!commandQueue) {
        //std::cout << "[MeshRenderSystem] Error: No command queue available\n";
        return;
    }

    std::vector<RenderCommand> renderCommands;
    entitiesProcessed = 0;

    //std::cout << "[MeshRenderSystem] Generating render commands for " << entities.size() << " entities\n";

    // Iterate through all entities with MeshRenderer components - PURE ECS LOGIC
    for (Entity entity : entities) {
        auto* transform = world->GetComponent<Transform>(entity);
        auto* meshRenderer = world->GetComponent<MeshRenderer>(entity);  // This is the COMPONENT

        // Validate components
        if (!transform || !meshRenderer) {
            //std::cout << "[MeshRenderSystem] Warning: Entity " << entity
            //    << " missing required components\n";
            continue;
        }

        // Skip invisible entities
        if (!meshRenderer->isVisible) {
            continue;
        }

        // Convert ECS data to RenderCommand - NO D3D KNOWLEDGE!
        RenderCommand command = CreateRenderCommandFromEntity(entity, transform, meshRenderer);
        renderCommands.push_back(command);

        entitiesProcessed++;
    }

    // Send all commands to render thread at once (more efficient)
    if (!renderCommands.empty()) {
        commandQueue->PushCommands(renderCommands);
        //std::cout << "[MeshRenderSystem] Sent " << renderCommands.size()
        //    << " render commands to render thread\n";
    }
}

RenderCommand MeshRenderSystem::CreateRenderCommandFromEntity(Entity entity,
    const Transform* transform,
    const MeshRenderer* meshRenderer) {
    // Convert ECS Transform to RenderTransform
    RenderTransform renderTransform;
    renderTransform.position[0] = transform->x;
    renderTransform.position[1] = transform->y;
    renderTransform.position[2] = transform->z;

    // FIXED: Use proper rotation and scale from Transform component
    renderTransform.rotation[0] = transform->rotationX;
    renderTransform.rotation[1] = transform->rotationY;
    renderTransform.rotation[2] = transform->rotationZ;

    // FIXED: Use proper scale values from Transform component
    renderTransform.scale[0] = transform->scaleX;
    renderTransform.scale[1] = transform->scaleY;
    renderTransform.scale[2] = transform->scaleZ;

    // Convert MeshRenderer color to RenderColor
    RenderColor renderColor(
        meshRenderer->color[0],
        meshRenderer->color[1],
        meshRenderer->color[2],
        meshRenderer->color[3]
    );

    // Create render command with all necessary data
    RenderCommand command = RenderCommand::CreateDrawMesh(
        meshRenderer->GetMeshName(),
        meshRenderer->shaderName,
        renderTransform,
        renderColor,
        meshRenderer->materialName
    );

    // Set additional properties
    command.visible = meshRenderer->isVisible;
    command.alpha = meshRenderer->alpha;

    return command;
}

void MeshRenderSystem::OnEntityAdded(Entity entity) {
    //std::cout << "[MeshRenderSystem] Entity " << entity << " added to render system\n";

    // Optional: Validate that entity has required components
    if (world) {
        auto* transform = world->GetComponent<Transform>(entity);
        auto* meshRenderer = world->GetComponent<MeshRenderer>(entity);

        if (!transform) {
            //std::cout << "[MeshRenderSystem] Warning: Entity " << entity << " has no Transform component\n";
        }
        if (!meshRenderer) {
            //std::cout << "[MeshRenderSystem] Warning: Entity " << entity << " has no MeshRenderer component\n";
        }
    }
}

void MeshRenderSystem::OnEntityRemoved(Entity entity) {
   //std::cout << "[MeshRenderSystem] Entity " << entity << " removed from render system\n";

    // Optional: Send command to remove any cached rendering data
    // (not needed for current implementation)
}

// Utility methods for debugging and optimization
uint32_t MeshRenderSystem::GetVisibleEntityCount() const {
    uint32_t visibleCount = 0;

    for (Entity entity : entities) {
        auto* meshRenderer = world->GetComponent<MeshRenderer>(entity);
        if (meshRenderer && meshRenderer->isVisible) {
            visibleCount++;
        }
    }

    return visibleCount;
}

void MeshRenderSystem::SetAllEntitiesVisible(bool visible) {
    //std::cout << "[MeshRenderSystem] Setting visibility of all entities to " << visible << "\n";

    for (Entity entity : entities) {
        auto* meshRenderer = world->GetComponent<MeshRenderer>(entity);
        if (meshRenderer) {
            meshRenderer->isVisible = visible;
        }
    }
}

void MeshRenderSystem::PrintSystemStats() const {
    /*uint32_t totalEntities = entities.size();
    uint32_t visibleEntities = GetVisibleEntityCount();

    std::cout << "[MeshRenderSystem] Stats:\n";
    std::cout << "  Total entities: " << totalEntities << "\n";
    std::cout << "  Visible entities: " << visibleEntities << "\n";
    std::cout << "  Entities processed last frame: " << entitiesProcessed << "\n";*/
}

// TronEngine/Source/Cpp/Game/PhysicsSystem.cpp - UPDATED WITH SPATIAL GRID
#include "../../Headers/Game/PhysicsSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/BoxColliderComponent.hpp"
#include "../../Headers/Game/ScriptComponent.hpp"
#include <chrono>
#include <algorithm>

PhysicsSystem::PhysicsSystem(float cellSize)
    : gridCellSize(cellSize)
    , debugOutput(false)
    , collisionChecksLastFrame(0)
    , triggerEventsLastFrame(0)
    , totalEntitiesProcessed(0)
    , broadPhaseTimeMs(0.0f)
    , narrowPhaseTimeMs(0.0f)
    , updateTimeMs(0.0f)
{
    // Initialize spatial grid for broad phase optimization
    spatialGrid = std::make_unique<SpatialGrid>(cellSize);

    if (debugOutput) {
        std::cout << "[PhysicsSystem] Initialized with spatial grid (cell size: "
            << cellSize << ")\n";
    }
}

void PhysicsSystem::Update(float deltaTime) {
    auto updateStart = std::chrono::high_resolution_clock::now();

    // Reset frame statistics
    collisionChecksLastFrame = 0;
    triggerEventsLastFrame = 0;
    totalEntitiesProcessed = entities.size();

    // Phase 1: Broad Phase - Update spatial grid
    auto broadStart = std::chrono::high_resolution_clock::now();
    BroadPhaseUpdate();
    auto broadEnd = std::chrono::high_resolution_clock::now();

    // Phase 2: Narrow Phase - Precise collision detection
    auto narrowStart = std::chrono::high_resolution_clock::now();
    NarrowPhaseCollisionDetection();
    auto narrowEnd = std::chrono::high_resolution_clock::now();

    // Phase 3: Process trigger events (OnTriggerExit for entities no longer colliding)
    ProcessTriggerEvents();

    auto updateEnd = std::chrono::high_resolution_clock::now();

    // Update timing statistics
    broadPhaseTimeMs = std::chrono::duration<float, std::milli>(broadEnd - broadStart).count();
    narrowPhaseTimeMs = std::chrono::duration<float, std::milli>(narrowEnd - narrowStart).count();
    updateTimeMs = std::chrono::duration<float, std::milli>(updateEnd - updateStart).count();

    // Debug output every few seconds
    static int frameCount = 0;
    if (debugOutput && (++frameCount % 300 == 0)) { // Every ~2.5 seconds at 120fps
        PrintPhysicsStats();
    }
}

void PhysicsSystem::BroadPhaseUpdate() {
    // Update all entities in the spatial grid
    for (Entity entity : entities) {
        UpdateEntityInGrid(entity);
    }
}

void PhysicsSystem::UpdateEntityInGrid(Entity entity) {
    if (!world) return;

    auto* transform = world->GetComponent<Transform>(entity);
    auto* collider = world->GetComponent<BoxCollider>(entity);

    if (!transform || !collider || !collider->isEnabled) {
        // Remove from grid if no longer has required components or disabled
        RemoveEntityFromGrid(entity);
        return;
    }

    // Calculate AABB for this entity
    AABB aabb = CalculateAABB(transform, collider);

    // Update entity position in spatial grid
    spatialGrid->UpdateEntity(entity, aabb);
}

void PhysicsSystem::RemoveEntityFromGrid(Entity entity) {
    spatialGrid->RemoveEntity(entity);
}

void PhysicsSystem::NarrowPhaseCollisionDetection() {
    if (!world) return;

    // Get potential collision pairs from broad phase
    auto potentialPairs = spatialGrid->GetPotentialCollisions();
    collisionChecksLastFrame = spatialGrid->GetBroadPhaseChecks();

    // Process each potential collision pair
    for (const auto& pair : potentialPairs) {
        Entity entityA = pair.first;
        Entity entityB = pair.second;

        // Get components for both entities
        auto* transformA = world->GetComponent<Transform>(entityA);
        auto* colliderA = world->GetComponent<BoxCollider>(entityA);
        auto* transformB = world->GetComponent<Transform>(entityB);
        auto* colliderB = world->GetComponent<BoxCollider>(entityB);

        // Validate components
        if (!transformA || !colliderA || !transformB || !colliderB) {
            continue;
        }

        if (!colliderA->isEnabled || !colliderB->isEnabled) {
            continue;
        }

        // Calculate AABBs for precise collision detection
        AABB aabbA = CalculateAABB(transformA, colliderA);
        AABB aabbB = CalculateAABB(transformB, colliderB);

        // Narrow Phase: Precise AABB collision test
        bool isColliding = AABBOverlap(aabbA, aabbB);

        if (isColliding) {
            // Handle trigger events for trigger colliders
            if (colliderA->isTrigger || colliderB->isTrigger) {
                ProcessTriggerEnter(entityA, entityB, colliderA, colliderB);
            }

            // TODO: Handle solid collision response here
            // For now, we only handle triggers
        }
    }
}

void PhysicsSystem::ProcessTriggerEvents() {
    // Process OnTriggerExit for entities that are no longer colliding
    for (Entity entity : entities) {
        auto* collider = world->GetComponent<BoxCollider>(entity);
        if (!collider || !collider->isTrigger) continue;

        // Find entities that were in contact last frame but not this frame
        for (Entity previousEntity : collider->previousTriggers) {
            if (collider->currentTriggers.find(previousEntity) == collider->currentTriggers.end()) {
                // Entity exited trigger
                ProcessTriggerExit(entity, previousEntity, collider, nullptr);
            }
        }

        // Update previous triggers for next frame
        collider->previousTriggers = collider->currentTriggers;
        collider->currentTriggers.clear();
    }
}

void PhysicsSystem::ProcessTriggerEnter(Entity entityA, Entity entityB,
    BoxCollider* colliderA, BoxCollider* colliderB) {
    // Check if this is a new collision (not from previous frame)
    bool newCollisionA = colliderA->isTrigger &&
        colliderA->previousTriggers.find(entityB) == colliderA->previousTriggers.end();
    bool newCollisionB = colliderB->isTrigger &&
        colliderB->previousTriggers.find(entityA) == colliderB->previousTriggers.end();

    if (colliderA->isTrigger) {
        colliderA->currentTriggers.insert(entityB);
        if (newCollisionA) {
            SendTriggerEventToEntity(entityA, entityB, true); // OnTriggerEnter
            triggerEventsLastFrame++;
        }
    }

    if (colliderB->isTrigger) {
        colliderB->currentTriggers.insert(entityA);
        if (newCollisionB) {
            SendTriggerEventToEntity(entityB, entityA, true); // OnTriggerEnter
            triggerEventsLastFrame++;
        }
    }
}

void PhysicsSystem::ProcessTriggerExit(Entity entityA, Entity entityB,
    BoxCollider* colliderA, BoxCollider* colliderB) {
    SendTriggerEventToEntity(entityA, entityB, false); // OnTriggerExit
    triggerEventsLastFrame++;

    if (debugOutput) {
        std::cout << "[PhysicsSystem] OnTriggerExit: Entity " << entityA
            << " and Entity " << entityB << "\n";
    }
}

void PhysicsSystem::SendTriggerEventToEntity(Entity entity, Entity otherEntity, bool isEnter) {
    if (!world) return;

    // Get the script component to send trigger events
    auto* script = world->GetComponent<Script>(entity);
    if (script && script->userScript) {
        try {
            if (isEnter) {
                script->userScript->OnTriggerEnter(otherEntity);
            }
            else {
                script->userScript->OnTriggerExit(otherEntity);
            }
        }
        catch (const std::exception& e) {
            std::cout << "[PhysicsSystem] Error in trigger event for entity "
                << entity << ": " << e.what() << "\n";
        }
    }
}

AABB PhysicsSystem::CalculateAABB(const Transform* transform, const BoxCollider* collider) const {
    // Calculate world-space AABB from transform and collider
    float halfWidth = collider->width * transform->scaleX * 0.5f;
    float halfHeight = collider->height * transform->scaleY * 0.5f;
    float halfDepth = collider->depth * transform->scaleZ * 0.5f;

    return AABB(
        transform->x - halfWidth,  // minX
        transform->y - halfHeight, // minY
        transform->z - halfDepth,  // minZ
        transform->x + halfWidth,  // maxX
        transform->y + halfHeight, // maxY
        transform->z + halfDepth   // maxZ
    );
}

bool PhysicsSystem::AABBOverlap(const AABB& a, const AABB& b) const {
    return a.Overlaps(b);
}

void PhysicsSystem::OnEntityAdded(Entity entity) {
    if (debugOutput) {
        std::cout << "[PhysicsSystem] Entity " << entity << " added to physics system\n";
    }

    // Entity will be added to spatial grid on next update
    UpdateEntityInGrid(entity);
}

void PhysicsSystem::OnEntityRemoved(Entity entity) {
    if (debugOutput) {
        std::cout << "[PhysicsSystem] Entity " << entity << " removed from physics system\n";
    }

    // Remove entity from spatial grid
    RemoveEntityFromGrid(entity);
}

void PhysicsSystem::SetGridCellSize(float cellSize) {
    if (cellSize <= 0.0f) {
        std::cout << "[PhysicsSystem] Warning: Invalid cell size " << cellSize << "\n";
        return;
    }

    gridCellSize = cellSize;
    spatialGrid->SetCellSize(cellSize);

    std::cout << "[PhysicsSystem] Grid cell size updated to " << cellSize << "\n";

    // Rebuild spatial grid with new cell size
    BroadPhaseUpdate();
}

void PhysicsSystem::PrintPhysicsStats() const {
    std::cout << "\n[PhysicsSystem] === Physics Performance Stats ===\n";
    std::cout << "  Entities processed: " << totalEntitiesProcessed << "\n";
    std::cout << "  Collision checks: " << collisionChecksLastFrame << "\n";
    std::cout << "  Trigger events: " << triggerEventsLastFrame << "\n";
    std::cout << "  Update time: " << updateTimeMs << "ms\n";
    std::cout << "    Broad phase: " << broadPhaseTimeMs << "ms\n";
    std::cout << "    Narrow phase: " << narrowPhaseTimeMs << "ms\n";

    // Calculate efficiency
    if (totalEntitiesProcessed > 1) {
        uint32_t bruteForceChecks = (totalEntitiesProcessed * (totalEntitiesProcessed - 1)) / 2;
        float efficiency = bruteForceChecks > 0 ?
            (static_cast<float>(collisionChecksLastFrame) / bruteForceChecks) * 100.0f : 0.0f;

        std::cout << "  Spatial grid efficiency: " << efficiency << "% of brute force\n";
        std::cout << "  Brute force would be: " << bruteForceChecks << " checks\n";
    }

    // Print spatial grid statistics
    spatialGrid->PrintGridStats();
    std::cout << "================================================\n\n";
}
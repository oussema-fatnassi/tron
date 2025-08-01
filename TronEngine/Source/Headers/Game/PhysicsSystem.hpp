// TronEngine/Source/Headers/Game/PhysicsSystem.hpp - UPDATED WITH SPATIAL GRID
#pragma once
#include "System.hpp"
#include "SpatialGrid.hpp"
#include <set>
#include <iostream>

// Forward declarations
struct Transform;
struct BoxCollider;
class World;

// <summary>
// PhysicsSystem - UPDATED with Broad Phase Optimization
// Now uses SpatialGrid for efficient collision detection
// Implements both Broad Phase (spatial partitioning) and Narrow Phase (precise collision)
// </summary>
// <remarks>
// ARCHITECTURE:
// 1. Broad Phase: SpatialGrid eliminates distant objects
// 2. Narrow Phase: AABB-vs-AABB precise collision detection
// 3. Trigger Events: OnTriggerEnter/OnTriggerExit for scripts
// 
// PERFORMANCE:
// - Without spatial grid: O(n²) collision checks
// - With spatial grid: O(n) average case, much better for sparse worlds
// 
// INTEGRATION:
// - Works seamlessly with existing BoxCollider components
// - Maintains compatibility with trigger events
// - Adds debugging and statistics
// </remarks>
class PhysicsSystem : public System {
private:
    // Broad Phase: Spatial partitioning for optimization
    std::unique_ptr<SpatialGrid> spatialGrid;
    float gridCellSize;

    // Debugging and statistics
    bool debugOutput;
    uint32_t collisionChecksLastFrame;
    uint32_t triggerEventsLastFrame;
    uint32_t totalEntitiesProcessed;

    // Performance tracking
    float broadPhaseTimeMs;
    float narrowPhaseTimeMs;
    float updateTimeMs;

public:
    PhysicsSystem(float cellSize = 5.0f);
    ~PhysicsSystem() = default;

    // System interface
    void Update(float deltaTime) override;
    void OnEntityAdded(Entity entity) override;
    void OnEntityRemoved(Entity entity) override;

    // Configuration
    void SetGridCellSize(float cellSize);
    void SetDebugOutput(bool enabled) { debugOutput = enabled; }

    // Statistics and debugging
    uint32_t GetCollisionChecksLastFrame() const { return collisionChecksLastFrame; }
    uint32_t GetTriggerEventsLastFrame() const { return triggerEventsLastFrame; }
    uint32_t GetTotalEntitiesProcessed() const { return totalEntitiesProcessed; }
    float GetBroadPhaseTime() const { return broadPhaseTimeMs; }
    float GetNarrowPhaseTime() const { return narrowPhaseTimeMs; }
    void PrintPhysicsStats() const;

    // Utility methods
    AABB CalculateAABB(const Transform* transform, const BoxCollider* collider) const;
    bool AABBOverlap(const AABB& a, const AABB& b) const;

private:
    // Core physics pipeline
    void BroadPhaseUpdate();
    void NarrowPhaseCollisionDetection();
    void ProcessTriggerEvents();

    // Entity management
    void UpdateEntityInGrid(Entity entity);
    void RemoveEntityFromGrid(Entity entity);

    // Trigger event processing
    void ProcessTriggerEnter(Entity entityA, Entity entityB, BoxCollider* colliderA, BoxCollider* colliderB);
    void ProcessTriggerExit(Entity entityA, Entity entityB, BoxCollider* colliderA, BoxCollider* colliderB);
    void SendTriggerEventToEntity(Entity entity, Entity otherEntity, bool isEnter);
};
#pragma once
#include "System.hpp"

struct Transform;
struct BoxCollider;
struct Script;

// <summary>
// PhysicsSystem - Simple single-threaded collision detection system
// Handles AABB collision detection and trigger events for entities with BoxCollider components
// </summary>
// <remarks>
// This system runs in the game thread alongside other ECS systems.
// It performs O(n²) collision checks between all entities with BoxColliders.
// When overlaps are detected between trigger colliders, it dispatches OnTriggerEnter/OnTriggerExit
// events to the entities' Script components.
// 
// Performance: Suitable for up to ~50-100 entities before optimization is needed.
// Future optimizations: Spatial partitioning, layer filtering, multi-threading.
// </remarks>
class PhysicsSystem : public System {
private:
    // Performance tracking
    uint32_t collisionChecksThisFrame = 0;
    uint32_t triggerEventsThisFrame = 0;

    // Debug settings
    bool debugOutput = false;
    int debugFrameCounter = 0;

public:
    PhysicsSystem();
    ~PhysicsSystem() = default;

    // System interface
    void Update(float deltaTime) override;
    void OnEntityAdded(Entity entity) override;
    void OnEntityRemoved(Entity entity) override;

    // Configuration
    void SetDebugOutput(bool enabled) { debugOutput = enabled; }

    // Performance metrics
    uint32_t GetCollisionChecksLastFrame() const { return collisionChecksThisFrame; }
    uint32_t GetTriggerEventsLastFrame() const { return triggerEventsThisFrame; }

private:
    // Core collision detection
    bool CheckAABBCollision(Entity entityA, Entity entityB);

    // Trigger event processing
    void ProcessTriggerEvents();

    // Helper methods
    void GetWorldAABB(Entity entity, float& minX, float& minY, float& minZ,
        float& maxX, float& maxY, float& maxZ);

    // Event dispatch
    void DispatchTriggerEnter(Entity triggerEntity, Entity otherEntity);
    void DispatchTriggerExit(Entity triggerEntity, Entity otherEntity);
};
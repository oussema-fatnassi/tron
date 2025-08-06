#pragma once
#include "System.hpp"
#include "../Math/Ray.hpp"
#include "../Math/Vector3.hpp"
#include "SpatialGrid.hpp"
#include <vector>
#include <functional>

// Forward declarations
class CameraMatrixSystem;
struct Transform;
struct BoxCollider;

// <summary>
// RaycastSystem provides 3D raycasting functionality for the engine
// Supports mouse picking, line-of-sight checks, and physics queries
// </summary>
// <remarks>
// This system integrates with the spatial grid for optimized raycasting.
// It can cast rays through the world and detect collisions with entities
// that have Transform and BoxCollider components.
// 
// Features:
// - Single hit raycasting (finds closest hit)
// - Multi-hit raycasting (finds all hits along ray)
// - Screen-to-world ray conversion for mouse picking
// - Layer filtering support
// - Debug visualization
// </remarks>
class RaycastSystem : public System {
public:
    // Layer masks for filtering what the ray can hit
    enum Layer {
        DEFAULT = 1 << 0,
        PLAYER = 1 << 1,
        ENEMY = 1 << 2,
        ENVIRONMENT = 1 << 3,
        TRIGGER = 1 << 4,
        UI = 1 << 5,
        ALL = 0xFFFFFFFF
    };

private:
    SpatialGrid* spatialGrid;
    CameraMatrixSystem* cameraMatrixSystem;
    
    // Debug visualization
    bool debugDrawEnabled;
    std::vector<std::pair<Ray, RaycastHit>> debugRays;
    
    // Performance stats
    uint32_t raycastsThisFrame;
    uint32_t aabbTestsThisFrame;
    float lastRaycastTime;

public:
    RaycastSystem(SpatialGrid* grid, CameraMatrixSystem* cameraSystem);
    ~RaycastSystem() = default;

    // System interface
    void Update(float deltaTime) override;

    // Core raycasting methods
    bool Raycast(const Ray& ray, RaycastHit& hit, uint32_t layerMask = Layer::ALL);
    std::vector<RaycastHit> RaycastAll(const Ray& ray, uint32_t layerMask = Layer::ALL);
    
    // Specific raycasting queries
    bool RaycastFromCamera(const Vector3& direction, RaycastHit& hit, uint32_t layerMask = Layer::ALL);
    bool RaycastFromEntity(Entity entity, const Vector3& direction, float maxDistance, RaycastHit& hit, uint32_t layerMask = Layer::ALL);
    
    // Mouse picking
    Ray ScreenPointToRay(int mouseX, int mouseY, int screenWidth, int screenHeight);
    bool MousePick(int mouseX, int mouseY, int screenWidth, int screenHeight, RaycastHit& hit, uint32_t layerMask = Layer::ALL);
    
    // Line of sight checks
    bool LineOfSight(const Vector3& from, const Vector3& to, uint32_t layerMask = Layer::ALL);
    bool LineOfSightBetweenEntities(Entity entityA, Entity entityB, uint32_t layerMask = Layer::ALL);
    
    // Utility methods
    void SetDebugDrawEnabled(bool enabled) { debugDrawEnabled = enabled; }
    bool IsDebugDrawEnabled() const { return debugDrawEnabled; }
    void ClearDebugRays() { debugRays.clear(); }
    
    // Performance stats
    uint32_t GetRaycastsThisFrame() const { return raycastsThisFrame; }
    uint32_t GetAABBTestsThisFrame() const { return aabbTestsThisFrame; }
    float GetLastRaycastTime() const { return lastRaycastTime; }
    void PrintStats() const;

private:
    // Internal raycasting implementation
    bool RaycastInternal(const Ray& ray, RaycastHit& hit, uint32_t layerMask, bool findClosest);
    void CollectPotentialHits(const Ray& ray, std::vector<Entity>& entities);
    bool RayAABBIntersection(const Ray& ray, const AABB& aabb, float& tMin, Vector3& hitPoint);
    bool RayBoxIntersection(const Ray& ray, const Vector3& center, const Vector3& halfExtents, 
                           float& tMin, Vector3& hitPoint, Vector3& hitNormal);
    
    // Helper methods
    Vector3 GetEntityPosition(Entity entity) const;
    AABB GetEntityAABB(Entity entity) const;
    uint32_t GetEntityLayer(Entity entity) const;
    
    // Grid traversal for optimization
    std::vector<GridCell> GetCellsAlongRay(const Ray& ray) const;
};
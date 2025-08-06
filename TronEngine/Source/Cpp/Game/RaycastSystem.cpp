#include "../../Headers/Game/RaycastSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/BoxColliderComponent.hpp"
#include "../../Headers/Game/CameraMatrixSystem.hpp"
#include "../../Headers/Math/Matrix.hpp"
#include <chrono>
#include <algorithm>
#include <limits>

RaycastSystem::RaycastSystem(SpatialGrid* grid, CameraMatrixSystem* cameraSystem)
    : spatialGrid(grid)
    , cameraMatrixSystem(cameraSystem)
    , debugDrawEnabled(false)
    , raycastsThisFrame(0)
    , aabbTestsThisFrame(0)
    , lastRaycastTime(0.0f) {
    std::cout << "[RaycastSystem] Initialized with spatial grid optimization\n";
}

void RaycastSystem::Update(float deltaTime) {
    // Reset frame statistics
    raycastsThisFrame = 0;
    aabbTestsThisFrame = 0;
    
    // Clear old debug rays if needed
    if (debugDrawEnabled && debugRays.size() > 100) {
        debugRays.erase(debugRays.begin(), debugRays.begin() + 50); // Keep last 50
    }
}

bool RaycastSystem::Raycast(const Ray& ray, RaycastHit& hit, uint32_t layerMask) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    raycastsThisFrame++;
    hit.Reset();
    
    bool result = RaycastInternal(ray, hit, layerMask, true);
    
    if (debugDrawEnabled) {
        debugRays.push_back({ray, hit});
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    lastRaycastTime = std::chrono::duration<float, std::milli>(endTime - startTime).count();
    
    return result;
}

std::vector<RaycastHit> RaycastSystem::RaycastAll(const Ray& ray, uint32_t layerMask) {
    raycastsThisFrame++;
    
    std::vector<RaycastHit> allHits;
    std::vector<Entity> potentialEntities;
    
    // Collect all potential entities along the ray path
    CollectPotentialHits(ray, potentialEntities);
    
    // Test each entity
    for (Entity entity : potentialEntities) {
        // Check layer mask
        if ((GetEntityLayer(entity) & layerMask) == 0) {
            continue;
        }
        
        RaycastHit hit;
        hit.entity = entity;
        
        // Get entity AABB
        AABB aabb = GetEntityAABB(entity);
        float tMin;
        Vector3 hitPoint;
        
        if (RayAABBIntersection(ray, aabb, tMin, hitPoint)) {
            if (ray.IsInRange(tMin)) {
                hit.hit = true;
                hit.distance = tMin;
                hit.point = hitPoint;
                hit.normal = Vector3::Up(); // TODO: Calculate proper normal
                
                auto* collider = world->GetComponent<BoxCollider>(entity);
                hit.collider = collider;
                
                allHits.push_back(hit);
            }
        }
    }
    
    // Sort by distance
    std::sort(allHits.begin(), allHits.end(), 
        [](const RaycastHit& a, const RaycastHit& b) {
            return a.distance < b.distance;
        });
    
    return allHits;
}

bool RaycastSystem::RaycastFromCamera(const Vector3& direction, RaycastHit& hit, uint32_t layerMask) {
    if (!cameraMatrixSystem) return false;
    
    Entity cameraEntity = cameraMatrixSystem->GetCameraEntity();
    if (cameraEntity == 0) return false;
    
    Vector3 cameraPos = GetEntityPosition(cameraEntity);
    Ray ray(cameraPos, direction);
    
    return Raycast(ray, hit, layerMask);
}

bool RaycastSystem::RaycastFromEntity(Entity entity, const Vector3& direction, float maxDistance, 
                                      RaycastHit& hit, uint32_t layerMask) {
    Vector3 origin = GetEntityPosition(entity);
    Ray ray(origin, direction, maxDistance);
    
    return Raycast(ray, hit, layerMask);
}

Ray RaycastSystem::ScreenPointToRay(int mouseX, int mouseY, int screenWidth, int screenHeight) {
    if (!cameraMatrixSystem) {
        return Ray();
    }
    
    // Convert mouse coordinates to normalized device coordinates [-1, 1]
    float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / screenHeight; // Flip Y
    
    // Get matrices
    Matrix viewMatrix = cameraMatrixSystem->GetViewMatrix();
    Matrix projMatrix = cameraMatrixSystem->GetProjectionMatrix();
    
    // Create inverse matrices
    Matrix invProj = projMatrix.Inverse();
    Matrix invView = viewMatrix.Inverse();
    
    // Ray in clip space
    Vector3 rayClip(ndcX, ndcY, -1.0f); // -1 for forward direction
    
    // Transform to view space
    float rayView[4] = { rayClip.x, rayClip.y, rayClip.z, 1.0f };
    float rayViewTransformed[4];
    
    // Manual matrix multiplication (since we don't have Vector4)
    for (int i = 0; i < 4; i++) {
        rayViewTransformed[i] = 0;
        for (int j = 0; j < 4; j++) {
            rayViewTransformed[i] += invProj.At(i, j) * rayView[j];
        }
    }
    
    // Set w = 0 for direction vector
    rayViewTransformed[3] = 0.0f;
    
    // Transform to world space
    float rayWorld[4];
    for (int i = 0; i < 4; i++) {
        rayWorld[i] = 0;
        for (int j = 0; j < 4; j++) {
            rayWorld[i] += invView.At(i, j) * rayViewTransformed[j];
        }
    }
    
    // Create ray
    Vector3 rayDirection(rayWorld[0], rayWorld[1], rayWorld[2]);
    rayDirection.Normalize();
    
    // Get camera position
    Entity cameraEntity = cameraMatrixSystem->GetCameraEntity();
    Vector3 cameraPos = GetEntityPosition(cameraEntity);
    
    return Ray(cameraPos, rayDirection);
}

bool RaycastSystem::MousePick(int mouseX, int mouseY, int screenWidth, int screenHeight, 
                              RaycastHit& hit, uint32_t layerMask) {
    Ray ray = ScreenPointToRay(mouseX, mouseY, screenWidth, screenHeight);
    return Raycast(ray, hit, layerMask);
}

bool RaycastSystem::LineOfSight(const Vector3& from, const Vector3& to, uint32_t layerMask) {
    Vector3 direction = (to - from).Normalized();
    float distance = Vector3::Distance(from, to);
    
    Ray ray(from, direction, distance);
    RaycastHit hit;
    
    // If we hit something, line of sight is blocked
    return !Raycast(ray, hit, layerMask);
}

bool RaycastSystem::LineOfSightBetweenEntities(Entity entityA, Entity entityB, uint32_t layerMask) {
    Vector3 posA = GetEntityPosition(entityA);
    Vector3 posB = GetEntityPosition(entityB);
    
    return LineOfSight(posA, posB, layerMask);
}

bool RaycastSystem::RaycastInternal(const Ray& ray, RaycastHit& hit, uint32_t layerMask, bool findClosest) {
    std::vector<Entity> potentialEntities;
    CollectPotentialHits(ray, potentialEntities);
    
    float closestDistance = std::numeric_limits<float>::max();
    bool foundHit = false;
    
    for (Entity entity : potentialEntities) {
        // Check layer mask
        if ((GetEntityLayer(entity) & layerMask) == 0) {
            continue;
        }
        
        // Get components
        auto* transform = world->GetComponent<Transform>(entity);
        auto* collider = world->GetComponent<BoxCollider>(entity);
        
        if (!transform || !collider || !collider->isEnabled) {
            continue;
        }
        
        aabbTestsThisFrame++;
        
        // Calculate world-space box
        Vector3 center(transform->x, transform->y, transform->z);
        Vector3 halfExtents(
            collider->width * transform->scaleX * 0.5f,
            collider->height * transform->scaleY * 0.5f,
            collider->depth * transform->scaleZ * 0.5f
        );
        
        float tMin;
        Vector3 hitPoint, hitNormal;
        
        if (RayBoxIntersection(ray, center, halfExtents, tMin, hitPoint, hitNormal)) {
            if (ray.IsInRange(tMin) && tMin < closestDistance) {
                closestDistance = tMin;
                hit.hit = true;
                hit.entity = entity;
                hit.point = hitPoint;
                hit.normal = hitNormal;
                hit.distance = tMin;
                hit.collider = collider;
                foundHit = true;
                
                if (!findClosest) {
                    break; // Return first hit
                }
            }
        }
    }
    
    return foundHit;
}

void RaycastSystem::CollectPotentialHits(const Ray& ray, std::vector<Entity>& entities) {
    if (!spatialGrid) {
        // No spatial grid - test all entities (slow!)
        entities = world->GetAllEntities();
        return;
    }
    
    // Get all grid cells the ray passes through
    std::vector<GridCell> cells = GetCellsAlongRay(ray);
    
    // Collect unique entities from these cells
    std::unordered_set<Entity> uniqueEntities;
    
    for (const GridCell& cell : cells) {
        auto cellEntities = spatialGrid->GetEntitiesInCell(cell);
        uniqueEntities.insert(cellEntities.begin(), cellEntities.end());
    }
    
    entities.assign(uniqueEntities.begin(), uniqueEntities.end());
}

bool RaycastSystem::RayAABBIntersection(const Ray& ray, const AABB& aabb, float& tMin, Vector3& hitPoint) {
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();
    
    // For each axis
    for (int i = 0; i < 3; i++) {
        float origin = (i == 0) ? ray.origin.x : (i == 1) ? ray.origin.y : ray.origin.z;
        float direction = (i == 0) ? ray.direction.x : (i == 1) ? ray.direction.y : ray.direction.z;
        float boxMin = (i == 0) ? aabb.minX : (i == 1) ? aabb.minY : aabb.minZ;
        float boxMax = (i == 0) ? aabb.maxX : (i == 1) ? aabb.maxY : aabb.maxZ;
        
        if (std::abs(direction) < 0.000001f) {
            // Ray is parallel to this axis
            if (origin < boxMin || origin > boxMax) {
                return false;
            }
        } else {
            float t1 = (boxMin - origin) / direction;
            float t2 = (boxMax - origin) / direction;
            
            if (t1 > t2) std::swap(t1, t2);
            
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            
            if (tmin > tmax || tmax < 0) {
                return false;
            }
        }
    }
    
    tMin = tmin;
    hitPoint = ray.GetPoint(tMin);
    return true;
}

bool RaycastSystem::RayBoxIntersection(const Ray& ray, const Vector3& center, const Vector3& halfExtents,
                                      float& tMin, Vector3& hitPoint, Vector3& hitNormal) {
    // Convert to AABB
    AABB aabb(
        center.x - halfExtents.x, center.y - halfExtents.y, center.z - halfExtents.z,
        center.x + halfExtents.x, center.y + halfExtents.y, center.z + halfExtents.z
    );
    
    if (!RayAABBIntersection(ray, aabb, tMin, hitPoint)) {
        return false;
    }
    
    // Calculate normal based on which face was hit
    Vector3 localPoint = hitPoint - center;
    
    float absX = std::abs(localPoint.x);
    float absY = std::abs(localPoint.y);
    float absZ = std::abs(localPoint.z);
    
    if (absX > absY && absX > absZ) {
        hitNormal = Vector3(localPoint.x > 0 ? 1.0f : -1.0f, 0, 0);
    } else if (absY > absZ) {
        hitNormal = Vector3(0, localPoint.y > 0 ? 1.0f : -1.0f, 0);
    } else {
        hitNormal = Vector3(0, 0, localPoint.z > 0 ? 1.0f : -1.0f);
    }
    
    return true;
}

Vector3 RaycastSystem::GetEntityPosition(Entity entity) const {
    if (!world) return Vector3::Zero();
    
    auto* transform = world->GetComponent<Transform>(entity);
    if (transform) {
        return Vector3(transform->x, transform->y, transform->z);
    }
    
    return Vector3::Zero();
}

AABB RaycastSystem::GetEntityAABB(Entity entity) const {
    if (!world) return AABB();
    
    auto* transform = world->GetComponent<Transform>(entity);
    auto* collider = world->GetComponent<BoxCollider>(entity);
    
    if (!transform || !collider) return AABB();
    
    float halfWidth = collider->width * transform->scaleX * 0.5f;
    float halfHeight = collider->height * transform->scaleY * 0.5f;
    float halfDepth = collider->depth * transform->scaleZ * 0.5f;
    
    return AABB(
        transform->x - halfWidth, transform->y - halfHeight, transform->z - halfDepth,
        transform->x + halfWidth, transform->y + halfHeight, transform->z + halfDepth
    );
}

uint32_t RaycastSystem::GetEntityLayer(Entity entity) const {
    // TODO: Add layer component or use tags
    // For now, return default layer
    return Layer::DEFAULT;
}

std::vector<GridCell> RaycastSystem::GetCellsAlongRay(const Ray& ray) const {
    std::vector<GridCell> cells;
    
    if (!spatialGrid) return cells;
    
    // Simple DDA algorithm for grid traversal
    float cellSize = spatialGrid->GetCellSize();
    float maxDist = ray.maxDistance > 0 ? ray.maxDistance : 1000.0f; // Default max
    
    // Sample points along the ray
    const float step = cellSize * 0.5f; // Sample at half cell size intervals
    for (float t = 0; t <= maxDist; t += step) {
        Vector3 point = ray.GetPoint(t);
        GridCell cell = spatialGrid->WorldToGrid(point.x, point.y, point.z);
        
        // Add cell if not already in list
        if (cells.empty() || 
            !(cells.back().x == cell.x && cells.back().y == cell.y && cells.back().z == cell.z)) {
            cells.push_back(cell);
        }
    }
    
    return cells;
}

void RaycastSystem::PrintStats() const {
    std::cout << "\n[RaycastSystem] === Performance Stats ===\n";
    std::cout << "  Raycasts this frame: " << raycastsThisFrame << "\n";
    std::cout << "  AABB tests this frame: " << aabbTestsThisFrame << "\n";
    std::cout << "  Last raycast time: " << lastRaycastTime << "ms\n";
    std::cout << "  Debug rays stored: " << debugRays.size() << "\n";
    std::cout << "=========================================\n\n";
}
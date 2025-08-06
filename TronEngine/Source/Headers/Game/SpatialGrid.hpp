#pragma once
#include "Types.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

// Forward declarations
struct Transform;
struct BoxCollider;

// <summary>
// Grid cell coordinates structure
// </summary>
struct GridCell {
    int x, y, z;

    GridCell() : x(0), y(0), z(0) {}
    GridCell(int x, int y, int z) : x(x), y(y), z(z) {}

    bool operator==(const GridCell& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

// Hash function for GridCell to use in unordered_map
struct GridCellHash {
    std::size_t operator()(const GridCell& cell) const {
        // Simple hash combining x, y, z coordinates
        return std::hash<int>()(cell.x) ^
            (std::hash<int>()(cell.y) << 1) ^
            (std::hash<int>()(cell.z) << 2);
    }
};

// <summary>
// AABB (Axis-Aligned Bounding Box) structure for collision detection
// </summary>
struct AABB {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;

    AABB() : minX(0), minY(0), minZ(0), maxX(0), maxY(0), maxZ(0) {}

    AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
        : minX(minX), minY(minY), minZ(minZ), maxX(maxX), maxY(maxY), maxZ(maxZ) {
    }

    // Check if this AABB overlaps with another AABB
    bool Overlaps(const AABB& other) const {
        return (minX <= other.maxX && maxX >= other.minX) &&
            (minY <= other.maxY && maxY >= other.minY) &&
            (minZ <= other.maxZ && maxZ >= other.minZ);
    }

    // Get the center point of the AABB
    void GetCenter(float& centerX, float& centerY, float& centerZ) const {
        centerX = (minX + maxX) * 0.5f;
        centerY = (minY + maxY) * 0.5f;
        centerZ = (minZ + maxZ) * 0.5f;
    }

    // Get the size of the AABB
    void GetSize(float& sizeX, float& sizeY, float& sizeZ) const {
        sizeX = maxX - minX;
        sizeY = maxY - minY;
        sizeZ = maxZ - minZ;
    }
};

// <summary>
// SpatialGrid - Broad Phase collision detection using uniform grid
// Divides 3D space into grid cells and only tests collisions between objects in same/adjacent cells
// </summary>
// <remarks>
// This implements the "Broad Phase" from your physics documentation:
// - Reduces collision checks by eliminating distant objects early
// - Uses uniform 3D grid to partition space
// - Each cell contains list of entities
// - Only entities in same/adjacent cells are tested for collision
// 
// Benefits:
// - O(n) insertion instead of O(n�) brute force
// - Dramatically reduces collision checks for sparse worlds
// - Simple and cache-friendly data structure
// </remarks>
class SpatialGrid {
private:
    // Grid configuration
    float cellSize;                    // Size of each grid cell (uniform in all dimensions)
    float invCellSize;                 // 1.0f / cellSize (for faster division)

    // Grid data structure: maps cell coordinates to list of entities
    std::unordered_map<GridCell, std::vector<Entity>, GridCellHash> grid;

    // Track which entities are in which cells (for efficient updates)
    std::unordered_map<Entity, std::vector<GridCell>> entityCells;

    // Statistics for debugging
    mutable uint32_t totalCells;
    mutable uint32_t totalEntities;
    mutable uint32_t broadPhaseChecks;

public:
    // Constructor - cellSize should be roughly the size of your average game object
    SpatialGrid(float cellSize = 5.0f);
    ~SpatialGrid() = default;

    // Core grid operations
    void Clear();
    void InsertEntity(Entity entity, const AABB& boundingBox);
    void RemoveEntity(Entity entity);
    void UpdateEntity(Entity entity, const AABB& newBoundingBox);

    // Broad phase collision detection
    std::vector<std::pair<Entity, Entity>> GetPotentialCollisions() const;
    void GetEntitiesInRadius(float centerX, float centerY, float centerZ,
        float radius, std::vector<Entity>& result) const;

    // Debug and statistics
    void PrintGridStats() const;
    uint32_t GetActiveCellCount() const;
    uint32_t GetTotalEntityCount() const;
    uint32_t GetBroadPhaseChecks() const { return broadPhaseChecks; }
    void ResetStats() { broadPhaseChecks = 0; }

    // Grid configuration
    float GetCellSize() const { return cellSize; }
    void SetCellSize(float newCellSize);

    //Raycast support
    std::vector<Entity> GetEntitiesInCell(const GridCell& cell) const;
    friend bool operator==(const GridCell& a, const GridCell& b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

private:
    // Helper methods
    GridCell WorldToGrid(float x, float y, float z) const;
    void GridToWorld(const GridCell& cell, float& x, float& y, float& z) const;
    std::vector<GridCell> GetCellsForAABB(const AABB& boundingBox) const;
    void GetAdjacentCells(const GridCell& center, std::vector<GridCell>& adjacent) const;
};
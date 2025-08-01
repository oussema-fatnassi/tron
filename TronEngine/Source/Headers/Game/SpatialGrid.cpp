// TronEngine/Source/Cpp/Game/SpatialGrid.cpp
#include "../../Headers/Game/SpatialGrid.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/BoxColliderComponent.hpp"
#include <iostream>
#include <cmath>

SpatialGrid::SpatialGrid(float cellSize)
    : cellSize(cellSize)
    , invCellSize(1.0f / cellSize)
    , totalCells(0)
    , totalEntities(0)
    , broadPhaseChecks(0)
{
    std::cout << "[SpatialGrid] Initialized with cell size: " << cellSize << "\n";
}

void SpatialGrid::Clear() {
    grid.clear();
    entityCells.clear();
    totalCells = 0;
    totalEntities = 0;
    broadPhaseChecks = 0;
    std::cout << "[SpatialGrid] Grid cleared\n";
}

GridCell SpatialGrid::WorldToGrid(float x, float y, float z) const {
    // Convert world coordinates to grid cell coordinates
    // Use floor to ensure consistent mapping (negative coordinates work correctly)
    return GridCell(
        static_cast<int>(std::floor(x * invCellSize)),
        static_cast<int>(std::floor(y * invCellSize)),
        static_cast<int>(std::floor(z * invCellSize))
    );
}

void SpatialGrid::GridToWorld(const GridCell& cell, float& x, float& y, float& z) const {
    // Convert grid cell coordinates back to world coordinates (cell center)
    x = (cell.x + 0.5f) * cellSize;
    y = (cell.y + 0.5f) * cellSize;
    z = (cell.z + 0.5f) * cellSize;
}

std::vector<GridCell> SpatialGrid::GetCellsForAABB(const AABB& boundingBox) const {
    std::vector<GridCell> cells;

    // Get the grid coordinates for the min and max corners of the AABB
    GridCell minCell = WorldToGrid(boundingBox.minX, boundingBox.minY, boundingBox.minZ);
    GridCell maxCell = WorldToGrid(boundingBox.maxX, boundingBox.maxY, boundingBox.maxZ);

    // Add all cells that the AABB spans
    for (int x = minCell.x; x <= maxCell.x; ++x) {
        for (int y = minCell.y; y <= maxCell.y; ++y) {
            for (int z = minCell.z; z <= maxCell.z; ++z) {
                cells.emplace_back(x, y, z);
            }
        }
    }

    return cells;
}

void SpatialGrid::InsertEntity(Entity entity, const AABB& boundingBox) {
    // Remove entity if it's already in the grid
    RemoveEntity(entity);

    // Get all cells that this entity's bounding box overlaps
    std::vector<GridCell> cells = GetCellsForAABB(boundingBox);

    // Add entity to each overlapping cell
    for (const GridCell& cell : cells) {
        grid[cell].push_back(entity);
    }

    // Track which cells this entity is in
    entityCells[entity] = cells;

    totalEntities++;
    // Update cell count (approximate - some cells might be shared)
    totalCells = grid.size();
}

void SpatialGrid::RemoveEntity(Entity entity) {
    auto it = entityCells.find(entity);
    if (it == entityCells.end()) {
        return; // Entity not in grid
    }

    // Remove entity from all cells it was in
    for (const GridCell& cell : it->second) {
        auto gridIt = grid.find(cell);
        if (gridIt != grid.end()) {
            auto& cellEntities = gridIt->second;
            cellEntities.erase(
                std::remove(cellEntities.begin(), cellEntities.end(), entity),
                cellEntities.end()
            );

            // Remove empty cells to keep grid clean
            if (cellEntities.empty()) {
                grid.erase(gridIt);
            }
        }
    }

    // Remove from tracking
    entityCells.erase(it);

    if (totalEntities > 0) totalEntities--;
    totalCells = grid.size();
}

void SpatialGrid::UpdateEntity(Entity entity, const AABB& newBoundingBox) {
    // Simple approach: remove and re-insert
    // TODO: Optimize by checking if entity moved to different cells
    RemoveEntity(entity);
    InsertEntity(entity, newBoundingBox);
}

std::vector<std::pair<Entity, Entity>> SpatialGrid::GetPotentialCollisions() const {
    std::vector<std::pair<Entity, Entity>> pairs;
    broadPhaseChecks = 0;

    // Iterate through all active grid cells
    for (const auto& cellPair : grid) {
        const std::vector<Entity>& entities = cellPair.second;

        // Check all pairs within this cell
        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                pairs.emplace_back(entities[i], entities[j]);
                broadPhaseChecks++;
            }
        }
    }

    return pairs;
}

void SpatialGrid::GetEntitiesInRadius(float centerX, float centerY, float centerZ,
    float radius, std::vector<Entity>& result) const {
    result.clear();

    // Create AABB for the search radius
    AABB searchArea(
        centerX - radius, centerY - radius, centerZ - radius,
        centerX + radius, centerY + radius, centerZ + radius
    );

    // Get all cells that might contain entities within radius
    std::vector<GridCell> cells = GetCellsForAABB(searchArea);

    // Use set to avoid duplicates (entities can be in multiple cells)
    std::unordered_set<Entity> uniqueEntities;

    for (const GridCell& cell : cells) {
        auto gridIt = grid.find(cell);
        if (gridIt != grid.end()) {
            for (Entity entity : gridIt->second) {
                uniqueEntities.insert(entity);
            }
        }
    }

    // Convert set back to vector
    result.reserve(uniqueEntities.size());
    for (Entity entity : uniqueEntities) {
        result.push_back(entity);
    }
}

void SpatialGrid::GetAdjacentCells(const GridCell& center, std::vector<GridCell>& adjacent) const {
    adjacent.clear();
    adjacent.reserve(27); // 3x3x3 = 27 cells maximum

    // Check all 26 adjacent cells + center cell
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                adjacent.emplace_back(
                    center.x + dx,
                    center.y + dy,
                    center.z + dz
                );
            }
        }
    }
}

void SpatialGrid::SetCellSize(float newCellSize) {
    if (newCellSize <= 0.0f) {
        std::cout << "[SpatialGrid] Warning: Invalid cell size " << newCellSize << ", keeping current size\n";
        return;
    }

    std::cout << "[SpatialGrid] Changing cell size from " << cellSize << " to " << newCellSize << "\n";

    // Store current entities
    std::vector<std::pair<Entity, std::vector<GridCell>>> currentEntities(entityCells.begin(), entityCells.end());

    // Update cell size
    cellSize = newCellSize;
    invCellSize = 1.0f / cellSize;

    // Clear and rebuild grid
    Clear();

    // TODO: Re-insert entities would require their current AABBs
    // For now, just clear - the PhysicsSystem will rebuild on next update
    std::cout << "[SpatialGrid] Grid rebuilt with new cell size. PhysicsSystem should rebuild.\n";
}

void SpatialGrid::PrintGridStats() const {
    std::cout << "[SpatialGrid] === Grid Statistics ===\n";
    std::cout << "  Cell size: " << cellSize << "\n";
    std::cout << "  Active cells: " << grid.size() << "\n";
    std::cout << "  Total entities: " << totalEntities << "\n";
    std::cout << "  Broad phase checks last frame: " << broadPhaseChecks << "\n";

    if (!grid.empty()) {
        // Calculate average entities per cell
        size_t totalEntitiesInCells = 0;
        size_t maxEntitiesInCell = 0;
        size_t minEntitiesInCell = SIZE_MAX;

        for (const auto& cellPair : grid) {
            size_t count = cellPair.second.size();
            totalEntitiesInCells += count;
            maxEntitiesInCell = std::max(maxEntitiesInCell, count);
            minEntitiesInCell = std::min(minEntitiesInCell, count);
        }

        float avgEntitiesPerCell = static_cast<float>(totalEntitiesInCells) / grid.size();

        std::cout << "  Avg entities per cell: " << avgEntitiesPerCell << "\n";
        std::cout << "  Max entities in cell: " << maxEntitiesInCell << "\n";
        std::cout << "  Min entities in cell: " << minEntitiesInCell << "\n";

        // Efficiency analysis
        size_t bruteForceChecks = (totalEntities * (totalEntities - 1)) / 2;
        float efficiency = bruteForceChecks > 0 ?
            (static_cast<float>(broadPhaseChecks) / bruteForceChecks) * 100.0f : 0.0f;

        std::cout << "  Brute force would be: " << bruteForceChecks << " checks\n";
        std::cout << "  Grid efficiency: " << efficiency << "% of brute force\n";
    }
}

uint32_t SpatialGrid::GetActiveCellCount() const {
    return static_cast<uint32_t>(grid.size());
}

uint32_t SpatialGrid::GetTotalEntityCount() const {
    return totalEntities;
}
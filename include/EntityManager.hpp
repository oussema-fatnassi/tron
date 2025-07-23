#pragma once
#include <vector>
#include <queue>
#include "Types.hpp"

// <summary>
// EntityManager is responsible for managing entities in an ECS (Entity-Component-System) architecture.
// It handles entity creation, destruction, and component masks to track which components an entity has.
// </summary>
// <remarks>
// This class provides functionality to create and destroy entities, manage their component masks,
// and query active entities. It uses a vector to store component masks and a queue for available
// entities, allowing for efficient reuse of entity IDs.
// The EntityManager ensures that entities are valid and can be queried for their active status.
// It also provides methods to set and get component masks for entities, enabling the ECS to track
// which components are associated with each entity.
// </remarks>
namespace Tron {
    class EntityManager {
    private:
        std::vector<ComponentMask> entityComponentMasks;
        std::vector<bool> entityActive;
        std::queue<Entity> availableEntities;
        uint32_t totalEntities;
        
    public:
        EntityManager();
        ~EntityManager();
        
        // Entity operations
        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        bool IsValidEntity(Entity entity) const;
        
        // Component mask operations
        void SetComponentMask(Entity entity, ComponentType componentType, bool hasComponent);
        ComponentMask GetComponentMask(Entity entity) const;
        
        // Query operations
        std::vector<Entity> GetAllActiveEntities() const;
        uint32_t GetEntityCount() const;
        uint32_t GetActiveEntityCount() const;
    };
}
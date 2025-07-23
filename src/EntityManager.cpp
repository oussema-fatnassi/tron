#include "EntityManager.hpp"


// <summary>
// EntityManager class, manages entities and their component masks.
// It provides functionality to create, destroy, and query entities, as well as manage their component
// masks.
// </summary>

// <summary>
// Constructor initializes the entity manager, pre-allocating space for entities.
// </summary>
// <remarks>
// This constructor sets up the initial state of the entity manager, including the total number of
// entities and the vectors for component masks and active states.
// </remarks>
EntityManager::EntityManager() : totalEntities(0) {
    entityComponentMasks.reserve(1000);
    entityActive.reserve(1000);
}

// <summary>
// Destructor cleans up the entity manager.
// </summary>
EntityManager::~EntityManager() = default;

// <summary>
// Creates a new entity, reusing an available one if possible.
// </summary>
// <returns>
// Returns the ID of the newly created entity.
// </returns>
// <remarks>
// If there are available entities in the queue, it reuses one; otherwise, it increments the total entity count
// and initializes a new entity. It also ensures that the component masks and active states vectors are large enough to accommodate the new entity.
// </remarks>
Entity EntityManager::CreateEntity() {
    Entity entity;
    
    if (!availableEntities.empty()) {
        entity = availableEntities.front();
        availableEntities.pop();
        entityActive[entity] = true;
    } else {
        entity = ++totalEntities;
        
        if (entity >= entityComponentMasks.size()) {
            entityComponentMasks.resize(entity + 1);
            entityActive.resize(entity + 1, false);
        }
        entityActive[entity] = true;
    }
    
    entityComponentMasks[entity].reset();
    return entity;
}

// <summary>
// Destroys an entity, marking it as inactive and resetting its component mask.
// </summary>
// <param name="entity">The ID of the entity to destroy.</param>
// <remarks>
// This method marks the entity as inactive, resets its component mask, and adds it back
// to the queue of available entities for reuse.
// </remarks>
void EntityManager::DestroyEntity(Entity entity) {
    if (IsValidEntity(entity)) {
        entityActive[entity] = false;
        entityComponentMasks[entity].reset();
        availableEntities.push(entity);
    }
}

// <summary>
// Checks if an entity is valid (i.e., it exists and is active).
// </summary>
// <param name="entity">The ID of the entity to check.</param>
// <returns>
// Returns true if the entity is valid, false otherwise.
// </returns>
// <remarks>
// An entity is considered valid if its ID is greater than 0 and it is marked as active in the entityActive vector.
// </remarks>
bool EntityManager::IsValidEntity(Entity entity) const {
    return entity > 0 && entity < entityActive.size() && entityActive[entity];
}

// <summary>
// Sets the component mask for an entity, indicating whether it has a specific component.
// </summary>
// <param name="entity">The ID of the entity.</param>
// <param name="componentType">The type of component to set.</param>
// <param name="hasComponent">True if the entity has the component, false otherwise.</param>
// <remarks>
// This method updates the component mask for the specified entity, setting or resetting the bit corresponding
// to the component type.
// </remarks>
void EntityManager::SetComponentMask(Entity entity, ComponentType componentType, bool hasComponent) {
    if (IsValidEntity(entity)) {
        if (hasComponent) {
            entityComponentMasks[entity].set(componentType);
        } else {
            entityComponentMasks[entity].reset(componentType);
        }
    }
}

// <summary>
// Gets the component mask for an entity.
// </summary>
// <param name="entity">The ID of the entity.</param>
// <returns>
// Returns the component mask for the specified entity.
// </returns>
// <remarks>
// This method retrieves the component mask for the specified entity, which indicates which components it has.
// If the entity is not valid, it returns an empty component mask.
// </remarks>
ComponentMask EntityManager::GetComponentMask(Entity entity) const {
    if (IsValidEntity(entity)) {
        return entityComponentMasks[entity];
    }
    return ComponentMask();
}

// <summary>
// Gets all active entities managed by the entity manager.
// </summary>
// <returns>
// Returns a vector containing the IDs of all active entities.
// </returns>
// <remarks>
// This method iterates through all entities and collects those that are marked as active.
// </remarks>
std::vector<Entity> EntityManager::GetAllActiveEntities() const {
    std::vector<Entity> activeEntities;
    activeEntities.reserve(GetActiveEntityCount());
    
    for (Entity e = 1; e <= totalEntities; ++e) {
        if (IsValidEntity(e)) {
            activeEntities.push_back(e);
        }
    }
    return activeEntities;
}

// <summary>
// Gets the total number of entities created.
// </summary>
// <returns>
// Returns the total number of entities created, including those that have been destroyed.
// </returns>
// <remarks>
// This method returns the total count of entities that have been created, regardless of whether they
// are currently active or not.
// </remarks>
uint32_t EntityManager::GetEntityCount() const {
    return totalEntities;
}

// <summary>
// Gets the count of currently active entities.
// </summary>
// <returns>
// Returns the number of entities that are currently active.
// </returns>
// <remarks>
// This method counts how many entities are currently marked as active, which can be less than the total
// number of entities created if some have been destroyed.
// </remarks>
uint32_t EntityManager::GetActiveEntityCount() const {
    uint32_t count = 0;
    for (Entity e = 1; e <= totalEntities; ++e) {
        if (IsValidEntity(e)) {
            ++count;
        }
    }
    return count;
}
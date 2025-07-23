#include "World.hpp"

// <summary>
// World class, the main entry point for the ECS system.
// It manages entities, components, and systems, providing methods to create entities,
// add/remove components, and update systems.
// </summary>
// <remarks>
// The World class encapsulates the EntityManager, ComponentManager, and SystemManager,
// allowing for a clean interface to interact with the ECS system.
// It provides methods to create entities, add components, register systems, and update them.
// </remarks>

// <summary>
// Constructor initializes the world, setting up the entity, component, and system managers.
// </summary>
// <remarks>
// This constructor sets up the initial state of the world, including the managers for entities,
// components, and systems. It does not perform any operations on entities or components.
// </remarks>
World::World() = default;

// <summary>
// Destructor cleans up the world, shutting down all systems and destroying all entities.
// </summary>
// <remarks>
// This destructor ensures that all systems are properly shut down and all entities are destroyed,
// releasing any resources held by components or systems.
// </remarks>
World::~World() {
    Shutdown();
}

// <summary>
// Creates a new entity in the world.
// </summary>
// <returns>
// Returns the ID of the newly created entity.
// </returns>
// <remarks>
// This method uses the EntityManager to create a new entity, which is assigned a unique ID.
// The entity is initially inactive and has no components.
// </remarks>
Entity World::CreateEntity() {
    return entityManager.CreateEntity();
}

// <summary>
// Destroys an entity, removing all its components and notifying systems.
// </summary>
// <param name="entity">The ID of the entity to destroy.</param>
// <remarks>
// This method removes all components associated with the entity, notifies systems that the entity has
// been destroyed, and then calls the EntityManager to mark the entity as inactive and reset its component mask.
// </remarks>
void World::DestroyEntity(Entity entity) {
    componentManager.RemoveAllComponents(entity);
    systemManager.EntityDestroyed(entity);
    entityManager.DestroyEntity(entity);
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
bool World::IsValidEntity(Entity entity) const {
    return entityManager.IsValidEntity(entity);
}

// <summary>
// Updates the world, calling all systems to process their logic.
// </summary>
// <param name="deltaTime">The time elapsed since the last update, used for time-based logic.</param>
// <remarks>
// This method iterates through all registered systems and calls their Update method,
// passing the delta time to allow systems to perform time-based calculations.
// It is typically called once per frame in the game loop.
// </remarks>
// <param name="deltaTime">The time elapsed since the last update, used for time-based logic.</param>
// <remarks>
void World::Update(float deltaTime) {
    systemManager.UpdateSystems(deltaTime);
}

// <summary>
// Shuts down the world, cleaning up all systems and entities.
// </summary>
// <remarks>
// This method is typically called when the world is no longer needed, such as when
// unloading a level or shutting down the game. It ensures that all resources are
// properly released and that there are no lingering references to entities or systems.
// </remarks>
void World::Shutdown() {
    auto allEntities = entityManager.GetAllActiveEntities();
    for (Entity entity : allEntities) {
        DestroyEntity(entity);
    }
    
    systemManager.Shutdown();
}

// <summary>
// Gets all active entities in the world.
// </summary>
// <returns>
// Returns a vector containing the IDs of all active entities.
// </returns>
// <remarks>
// This method retrieves all entities that are currently active in the world, which can be useful
// for iterating over entities in systems or for debugging purposes.
// </remarks>
std::vector<Entity> World::GetAllEntities() const {
    return entityManager.GetAllActiveEntities();
}

// <summary>
// Gets the total number of entities created in the world.
// </summary>
// <returns>
// Returns the total number of entities that have been created, including those that are currently inactive.
// </returns>
// <remarks>
// This method returns the total count of entities that have been created in the world, which
// can be useful for debugging or for systems that need to know how many entities exist.
// </remarks>
uint32_t World::GetEntityCount() const {
    return entityManager.GetEntityCount();
}

// <summary>
// Gets the count of systems currently registered in the world.
// </summary>
// <returns>
// Returns the number of systems that are currently registered and active in the world.
// </returns>
// <remarks>
// This method retrieves the count of systems that have been registered with the SystemManager,
// which can be useful for debugging or for systems that need to know how many systems are
// available to process entities.
// </remarks>
size_t World::GetSystemCount() const {
    return systemManager.GetSystemCount();
}
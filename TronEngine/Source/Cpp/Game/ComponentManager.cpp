#include "../../Headers/Game/ComponentManager.hpp"

// <summary>
// ComponentManager class, manages components in the ECS world.
// It provides functionality to register components, add/remove them from entities, and query components.
// </summary>
// <remarks>
// The ComponentManager holds pools of components for each type, allowing for efficient storage and retrieval.
// It uses unique pointers to manage component lifetimes and ensures that components are properly registered before use.
// This allows for a flexible and extensible ECS architecture where new component types can be added easily.
// </remarks>

// <summary>
// Constructor initializes the component manager, setting up the component type counter.
// </summary>
// <remarks>
// This constructor sets the initial state of the component manager, including the component type counter,
// which is used to assign unique IDs to component types as they are registered.
// </remarks>
ComponentManager::ComponentManager() : _componentTypeCounter(0) {
}

// <summary>
// Destructor cleans up the component manager, releasing all component pools.
// </summary>
// <remarks>
// This destructor ensures that all component pools are properly released, preventing memory leaks.
// It is called when the ComponentManager is no longer needed, such as when the ECS world is being destroyed.
// </remarks>
ComponentManager::~ComponentManager() = default;

// <summary>
// Registers a component type, ensuring it is ready for use.
// </summary>
// <param name="T">The component type to register.</param>
// <remarks>
// This method checks if the component type is already registered. If not, it adds it to the componentTypes map
// and initializes an empty pool for that component type. It uses unique pointers to manage component lifetimes.
// </remarks>
void ComponentManager::RemoveAllComponents(Entity entity) {
    for (auto& poolPair : _componentPools) {
        auto& pool = poolPair.second;
        if (entity < pool.size()) {
            pool[entity].reset();
        }
    }
}

// <summary>
// Checks if an entity has a specific component type.
// </summary>
// <param name="entity">The ID of the entity to check.</param>
// <param name="type">The component type to check for.</param>
// <returns>
// Returns true if the entity has the specified component type, false otherwise.
// </returns>
// <remarks>
// This method iterates through the component pools to check if the entity has a component of the specified type.
// It uses the componentTypes map to find the corresponding type_index for the ComponentType.
// If the entity exists in the pool and has a component of the specified type, it returns true.
// If the entity does not exist or does not have the component,
// it returns false.
// </remarks>
bool ComponentManager::HasComponent(Entity entity, ComponentType type) const {
    // Find which type_index corresponds to this ComponentType
    for (const auto& pair : _componentTypes) {
        const auto& typeIndex = pair.first;
        const auto& componentType = pair.second;

        if (componentType == type) {
            auto poolIt = _componentPools.find(typeIndex);
            if (poolIt != _componentPools.end() &&
                entity < poolIt->second.size() &&
                poolIt->second[entity]) {
                return true;
            }
            break;
        }
    }
    return false;
}

// <summary>
// Gets the size of the component pool for a specific component type.
// </summary>
// <param name="type">The component type to get the pool size for.</param>
// <returns>
// Returns the size of the component pool for the specified component type.
// </returns>
// <remarks>
// This method iterates through the componentTypes map to find the type_index corresponding to the
// specified ComponentType. It then retrieves the size of the corresponding component pool.
// If the component type is not found, it returns 0.
// </remarks>
size_t ComponentManager::GetComponentPoolSize(ComponentType type) const {
    // Find which type_index corresponds to this ComponentType
    for (const auto& pair : _componentTypes) {
        const auto& typeIndex = pair.first;
        const auto& componentType = pair.second;

        if (componentType == type) {
            auto poolIt = _componentPools.find(typeIndex);
            if (poolIt != _componentPools.end()) {
                return poolIt->second.size();
            }
            break;
        }
    }
    return 0;
}
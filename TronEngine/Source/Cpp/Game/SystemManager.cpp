#include "../../Headers/Game/SystemManager.hpp"
#include <algorithm>

// <summary>
// SystemManager class, manages all systems in the ECS world.
// It provides functionality to register systems, update them, and manage their signatures.
// </summary>
// <remarks>
// The SystemManager holds a collection of systems, allowing for easy registration and retrieval.
// It also manages the signatures of systems, which define which components they are interested in.
// This allows systems to efficiently query entities that match their requirements.
// </remarks>

// <summary>
// Constructor initializes the system manager.
// </summary>
// <remarks>
// This constructor sets up the initial state of the system manager, including empty vectors for systems
// and maps for system lookups and signatures.
// </remarks>
SystemManager::SystemManager() = default;

// <summary>
// Destructor cleans up the system manager.
// </summary>
// <remarks>
// This destructor is responsible for cleaning up all systems and their associated resources.
// It calls the Shutdown method to ensure all systems are properly cleaned up before the manager is destroyed.
// </remarks>
SystemManager::~SystemManager() {
    Shutdown();
}

// <summary>
// Sets the signature for a system, defining which components it requires.
// </summary>
// <param name="system">The system for which to set the signature.</param>
// <param name="signature">The component mask representing the signature.</param>
// <remarks>
// This method associates a component mask with a system, indicating which components the system is interested in.
// The signature is used to determine which entities should be processed by the system during updates.
// </remarks>
void SystemManager::SetSystemSignature(System* system, ComponentMask signature) {
    _systemSignatures[system] = signature;
}

// <summary>
// Gets the signature for a system.
// </summary>
// <param name="system">The system for which to get the signature.</param>
// <returns>
// Returns the component mask representing the signature of the system.
// </returns>
// <remarks>
// This method retrieves the component mask associated with a system, which indicates which components it requires.
// If the system does not have a signature set, it returns an empty component mask.
// </remarks>
ComponentMask SystemManager::GetSystemSignature(System* system) const {
    auto it = _systemSignatures.find(system);
    if (it != _systemSignatures.end()) {
        return it->second;
    }
    return ComponentMask();
}

// <summary>
// Updates all registered systems with the given delta time.
// </summary>
// <param name="deltaTime">The time elapsed since the last update, used for time-based logic.</param>
// <remarks>
// This method iterates through all registered systems and calls their Update method, passing the delta time to allow systems to perform time-based calculations.
// It is typically called once per frame in the game loop.
// </remarks>
void SystemManager::UpdateSystems(float deltaTime) {
    for (auto& system : _systems) {
        system->Update(deltaTime);
    }
}

// <summary>
// Notifies systems that an entity's signature has changed.
// </summary>
// <param name="entity">The ID of the entity whose signature has changed.</param>
// <param name="entitySignature">The new component mask for the entity.</param>
// <remarks>
// This method checks each system to see if the entity's new signature matches its requirements.
// If it does, the entity is added to the system's list of entities; if not, it is removed.
// This allows systems to dynamically adjust to changes in entity components.
// </remarks>
void SystemManager::EntitySignatureChanged(Entity entity, ComponentMask entitySignature) {
    for (auto& system : _systems) {
        auto it = _systemSignatures.find(system.get());
        if (it != _systemSignatures.end()) {
            ComponentMask systemSignature = it->second;
            
            // Check if entity should be in this system
            bool shouldContain = (entitySignature & systemSignature) == systemSignature;
            
            // Check if entity is currently in the system
            auto& entities = system->entities;
            auto entityIt = std::find(entities.begin(), entities.end(), entity);
            bool contains = entityIt != entities.end();
            
            if (shouldContain && !contains) {
                entities.push_back(entity);
                system->OnEntityAdded(entity);
            } else if (!shouldContain && contains) {
                entities.erase(entityIt);
                system->OnEntityRemoved(entity);
            }
        }
    }
}

// <summary>
// Notifies systems that an entity has been destroyed.
// </summary>
// <param name="entity">The ID of the entity that has been destroyed.</param>
// <remarks>
// This method iterates through all systems and removes the entity from their lists.
// It also calls the OnEntityRemoved method of each system to allow them to perform any necessary cleanup.
// This is typically called when an entity is no longer needed, such as when it is destroyed or goes out of scope.
// </remarks>
void SystemManager::EntityDestroyed(Entity entity) {
    for (auto& system : _systems) {
        auto& entities = system->entities;
        auto it = std::find(entities.begin(), entities.end(), entity);
        if (it != entities.end()) {
            entities.erase(it);
            system->OnEntityRemoved(entity);
        }
    }
}

// <summary>
// Shuts down all systems, cleaning up their resources.
// </summary>
// <remarks>
// This method iterates through all systems and calls their Shutdown method to clean up resources.
// It clears the systems vector, systemLookup map, and systemSignatures map to reset the state of the SystemManager.
// This is typically called when the ECS world is being destroyed or when systems are no longer needed.
// </remarks>
void SystemManager::Shutdown() {
    for (auto& system : _systems) {
        system->Shutdown();
    }
    _systems.clear();
    _systemLookup.clear();
    _systemSignatures.clear();
}

// <summary>
// Gets the count of registered systems.
// </summary>
// <returns>
// Returns the number of systems currently registered in the SystemManager.
// </returns>
// <remarks>
// This method returns the size of the systems vector, which contains all registered systems.
// It can be useful for debugging or for systems that need to know how many systems are active.
// </remarks>
size_t SystemManager::GetSystemCount() const {
    return _systems.size();
}

// <summary>
// Gets all registered systems.
// </summary>
// <returns>
// Returns a vector containing pointers to all registered systems.
// </returns>
// <remarks>
// This method creates a vector of pointers to all systems currently registered in the SystemManager.
// It can be useful for iterating over all systems or for debugging purposes.
// </remarks>
std::vector<System*> SystemManager::GetAllSystems() const {
    std::vector<System*> result;
    result.reserve(_systems.size());
    for (const auto& system : _systems) {
        result.push_back(system.get());
    }
    return result;
}
#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <algorithm>
#include "System.hpp"
#include "Types.hpp"

// <summary>
// SystemManager is responsible for managing systems in an ECS (Entity-Component-System) architecture.
// It handles system registration, retrieval, and updates, as well as managing system signatures.
// </summary>
// <remarks>
// This class provides functionality to register systems, retrieve them by type,
// and update all systems during the game loop. It also manages the signatures of systems,
// which define the components that a system operates on. The SystemManager ensures that systems
// are properly initialized with a reference to the World, allowing them to access entity and component management
// functionalities. It also handles entity signature changes, allowing systems to react to changes in the ECS.
// </remarks>
class World;

class SystemManager {
private:
    std::vector<std::unique_ptr<System>> systems;
    std::unordered_map<std::type_index, System*> systemLookup;
    std::unordered_map<System*, ComponentMask> systemSignatures;
    
public:
    SystemManager();
    ~SystemManager();
    
    // <summary>
    // Registers a system of type T, ensuring it inherits from System.
    // </summary>
    // <remarks>
    // Uses static_assert to enforce that T is derived from System.
    // Initializes the system with the provided arguments and adds it to the system list.
    // </remarks>
    template<typename T, typename... Args>
    T* RegisterSystem(World* world, Args&&... args) {
        static_assert(std::is_base_of<System, T>::value, 
            "T must inherit from System");
        
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T* systemPtr = system.get();
        
        systemPtr->Init(world);
        systems.push_back(std::move(system));
        systemLookup[std::type_index(typeid(T))] = systemPtr;
        
        return systemPtr;
    }
    
    // <summary>
    // Retrieves a system of type T.
    // </summary>
    // <remarks>
    // Returns a pointer to the system if it exists, or nullptr if it does not.
    // </remarks>
    template<typename T>
    T* GetSystem() {
        auto it = systemLookup.find(std::type_index(typeid(T)));
        if (it != systemLookup.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }
    
    // Non-template methods
    void SetSystemSignature(System* system, ComponentMask signature);
    ComponentMask GetSystemSignature(System* system) const;
    void UpdateSystems(float deltaTime);
    void EntitySignatureChanged(Entity entity, ComponentMask entitySignature);
    void EntityDestroyed(Entity entity);
    void Shutdown();
    
    // Query methods
    size_t GetSystemCount() const;
    std::vector<System*> GetAllSystems() const;
};

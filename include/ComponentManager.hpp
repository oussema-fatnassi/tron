#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include "Types.hpp"
#include "Component.hpp"


// <summary>
// Manages components for entities in an ECS (Entity-Component-System) architecture.
// Provides functionality to register, add, retrieve, and remove components.
// </summary>
namespace Tron {
    class ComponentManager {
    private:
        //<summary>
        // Maps to hold component pools and types.
        //</summary>
        //<remarks>
        // componentPools: Maps type indices to vectors of unique pointers to components.
        // componentTypes: Maps type indices to their corresponding ComponentType.
        // componentTypeCounter: Counter for assigning unique ComponentType values.
        //</remarks>
        std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> componentPools;
        std::unordered_map<std::type_index, ComponentType> componentTypes;
        ComponentType componentTypeCounter;
        
    public:
        ComponentManager();
        ~ComponentManager();
        
        //<summary>
        // Registers a component type T, ensuring it inherits from Component.
        //</summary>
        //<remarks>
        // Uses static_assert to enforce that T is derived from Component.
        // If T is not registered, it initializes its component pool.
        //</remarks>
        template<typename T>
        void RegisterComponent() {
            static_assert(std::is_base_of<Component, T>::value, 
                "T must inherit from Component");
            
            std::type_index typeIndex(typeid(T));
            
            if (componentTypes.find(typeIndex) == componentTypes.end()) {
                componentTypes[typeIndex] = componentTypeCounter++;
                componentPools[typeIndex] = std::vector<std::unique_ptr<Component>>();
            }
        }
        
        //<summary>
        // Adds a component of type T to the specified entity, initializing it with provided arguments.
        //</summary>
        //<remarks>
        // If the component type is not registered, it registers it first.
        // Resizes the component pool for the entity if necessary.
        //</remarks>
        template<typename T, typename... Args>
        T* AddComponent(Entity entity, Args&&... args) {
            std::type_index typeIndex(typeid(T));
            
            if (componentTypes.find(typeIndex) == componentTypes.end()) {
                RegisterComponent<T>();
            }
            
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T* componentPtr = component.get();
            
            auto& pool = componentPools[typeIndex];
            if (entity >= pool.size()) {
                pool.resize(entity + 1);
            }
            pool[entity] = std::move(component);
            
            return componentPtr;
        }
        
        //<summary>
        // Retrieves a component of type T for the specified entity.
        //</summary>
        //<remarks>
        // Returns a pointer to the component if it exists, or nullptr if it does not.
        //</remarks>
        template<typename T>
        T* GetComponent(Entity entity) {
            std::type_index typeIndex(typeid(T));
            
            auto it = componentPools.find(typeIndex);
            if (it != componentPools.end() && entity < it->second.size() && it->second[entity]) {
                return static_cast<T*>(it->second[entity].get());
            }
            
            return nullptr;
        }
        
        //<summary>
        // Removes a component of type T from the specified entity.
        //</summary>
        //<remarks>
        // If the component exists, it resets the unique pointer, effectively removing it.
        //</remarks>
        template<typename T>
        void RemoveComponent(Entity entity) {
            std::type_index typeIndex(typeid(T));
            
            auto poolIt = componentPools.find(typeIndex);
            if (poolIt != componentPools.end() && entity < poolIt->second.size()) {
                poolIt->second[entity].reset();
            }
        }
        
        //<summary>
        // Retrieves the ComponentType for a given component type T.
        //</summary>
        //<remarks>
        // Returns the ComponentType if the component type is registered, or -1 if it is not.
        //</remarks>
        template<typename T>
        ComponentType GetComponentType() const {
            std::type_index typeIndex(typeid(T));
            auto it = componentTypes.find(typeIndex);
            if (it != componentTypes.end()) {
                return it->second;
            }
            return static_cast<ComponentType>(-1);
        }
        
        void RemoveAllComponents(Entity entity);
        bool HasComponent(Entity entity, ComponentType type) const;
        size_t GetComponentPoolSize(ComponentType type) const;
    };
}
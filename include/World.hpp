#pragma once
#include "Types.hpp"
#include "Component.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"

// <summary>
// World is the central class in an ECS (Entity-Component-System) architecture.
// It manages entities, components, and systems, providing a unified interface for the ECS.
// </summary>
// <remarks>
// The World class encapsulates the EntityManager, ComponentManager, and SystemManager,
// allowing for the creation, destruction, and management of entities and their components.
// It also provides methods to register and retrieve systems, as well as to update all systems during
// the game loop. The World class is designed to be the main entry point for interacting with the ECS,
// providing a clean and organized way to manage the various aspects of the system.
// It ensures that entities can be created and destroyed, components can be added and removed,
// and systems can be registered and updated efficiently.
// The World class is non-copyable and non-moveable to ensure that there is only one instance of it in the application,
// maintaining a single source of truth for the ECS.
// </remarks>
namespace Tron {
    class World {
    private:
        EntityManager entityManager;
        ComponentManager componentManager;
        SystemManager systemManager;
        
    public:
        World();
        ~World();
        
        // Non-copyable, non-moveable
        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&&) = delete;
        World& operator=(World&&) = delete;
        
        // Entity operations
        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        bool IsValidEntity(Entity entity) const;
        
        // <summary>
        // Registers a component type T, ensuring it inherits from Component.
        // </summary>
        // <remarks>
        // Uses static_assert to enforce that T is derived from Component.
        // If T is not registered, it initializes its component pool.
        // </remarks>
        template<typename T>
        void RegisterComponent() {
            componentManager.RegisterComponent<T>();
        }
        
        // <summary>
        // Adds a component of type T to the specified entity, initializing it with provided arguments.
        // </summary>
        // <remarks>
        // If the component type is not registered, it registers it first.
        // Resizes the component pool for the entity if necessary.
        // This method also updates the entity's component mask and notifies systems of the entity's
        // signature change, ensuring that systems are aware of the new component.
        // </remarks>
        template<typename T, typename... Args>
        T* AddComponent(Entity entity, Args&&... args) {
            T* component = componentManager.AddComponent<T>(entity, std::forward<Args>(args)...);
            
            auto componentType = componentManager.GetComponentType<T>();
            entityManager.SetComponentMask(entity, componentType, true);
            
            auto signature = entityManager.GetComponentMask(entity);
            systemManager.EntitySignatureChanged(entity, signature);
            
            return component;
        }
        
        // <summary>
        // Retrieves a component of type T for the specified entity.
        // </summary>
        // <remarks>
        // Returns a pointer to the component if it exists, or nullptr if it does not.
        // This method allows systems and other components to access the data associated with the entity.
        // </remarks>
        template<typename T>
        T* GetComponent(Entity entity) {
            return componentManager.GetComponent<T>(entity);
        }
        
        // <summary>
        // Removes a component of type T from the specified entity.
        // </summary>
        // <remarks>
        // If the component exists, it resets the unique pointer, effectively removing it.
        // It also updates the entity's component mask and notifies systems of the entity's signature
        // change, ensuring that systems are aware of the component removal.
        // </remarks>
        template<typename T>
        void RemoveComponent(Entity entity) {
            componentManager.RemoveComponent<T>(entity);
            
            auto componentType = componentManager.GetComponentType<T>();
            entityManager.SetComponentMask(entity, componentType, false);
            
            auto signature = entityManager.GetComponentMask(entity);
            systemManager.EntitySignatureChanged(entity, signature);
        }
        
        // <summary>
        // Registers a system of type T with the provided arguments.
        // </summary>
        // <remarks>
        // Uses static_assert to enforce that T is derived from System.
        // Initializes the system with the provided arguments and adds it to the system list.
        // This method allows systems to be registered with the World, enabling them to operate on entities
        // and their components. It also ensures that systems are properly initialized with a reference to the
        // World, allowing them to access entity and component management functionalities.
        // </remarks>
        template<typename T, typename... Args>
        T* RegisterSystem(Args&&... args) {
            return systemManager.RegisterSystem<T>(this, std::forward<Args>(args)...);
        }
        
        // <summary>
        // Retrieves a system of type T.
        // </summary>
        // <remarks>
        // Returns a pointer to the system if it exists, or nullptr if it does not.
        // This method allows other systems or components to access specific systems for processing
        // or querying purposes. It provides a way to interact with systems without needing to know their
        // specific types, as long as they are registered with the World.
        // </remarks>
        template<typename T>
        T* GetSystem() {
            return systemManager.GetSystem<T>();
        }
        
        // <summary>
        // Sets the component signature for a system, allowing it to operate on entities with specific components.
        // </summary>
        // <remarks>
        // This method builds a component mask based on the provided component types and sets it for the
        // specified system. It ensures that the system is aware of which components it can operate on,
        // allowing it to filter entities based on their component masks. The signature is built using a
        // recursive template function that sets bits in the ComponentMask for each component type.
        // </remarks>
        template<typename... Components>
        void SetSystemSignature(System* system) {
            ComponentMask signature;
            SetSignatureHelper<Components...>(signature);
            systemManager.SetSystemSignature(system, signature);
            
            // Update system with all matching entities
            auto entities = entityManager.GetAllActiveEntities();
            for (Entity entity : entities) {
                auto entitySignature = entityManager.GetComponentMask(entity);
                systemManager.EntitySignatureChanged(entity, entitySignature);
            }
        }
        
        // Update all systems
        void Update(float deltaTime);
        
        // Shutdown
        void Shutdown();
        
        // Query methods
        std::vector<Entity> GetAllEntities() const;
        uint32_t GetEntityCount() const;
        size_t GetSystemCount() const;
        
    private:
        //<summary>
        // Helper function to set the component signature for a system.
        //</summary>
        //<remarks>
        // This function recursively sets bits in the ComponentMask for each component type.
        //</remarks>
        template<typename T, typename... Rest>
        void SetSignatureHelper(ComponentMask& signature) {
            auto componentType = componentManager.GetComponentType<T>();
            if (componentType != static_cast<ComponentType>(-1)) {
                signature.set(componentType);
            }
            
            if constexpr (sizeof...(Rest) > 0) {
                SetSignatureHelper<Rest...>(signature);
            }
        }
    };
}
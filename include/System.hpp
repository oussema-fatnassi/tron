#pragma once
#include <vector>
#include "Types.hpp"

// <summary>
// System is an abstract base class for systems in an ECS (Entity-Component-System) architecture.
// It provides a framework for systems to operate on entities and their components.
// </summary>
// <remarks>
// Systems are responsible for processing entities that match their component signatures.
// They can be initialized with a reference to the World, allowing them to access entity and component
// management functionalities.
// The Update method must be implemented by derived classes to define the system's behavior during each frame.
// Systems can also handle entity addition and removal events, allowing them to react to changes in the ECS.
// </remarks>
namespace Tron {
    class World;

    class System {
    protected:
        World* world;

    public:
        std::vector<Entity> entities;

        System();
        virtual ~System();
        
        virtual void Init(World* world);
        virtual void Update(float deltaTime) = 0;
        virtual void Shutdown();

        virtual void OnEntityAdded(Entity entity);
        virtual void OnEntityRemoved(Entity entity);
        
        World* GetWorld() const;
    };
}
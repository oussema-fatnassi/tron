#include "../../Headers/Game//System.hpp"

// <summary>
// System class, the base class for all systems in the ECS world.
// It provides functionality to manage entities and interact with the world.
// </summary>
// <remarks>
// The System class is designed to be inherited by specific systems that implement the Update method.
// It provides methods to initialize the system with a reference to the world, update entities, and
// handle entity addition/removal events.
// Systems can be used to implement game logic, rendering, physics, etc.
// Each system can maintain a list of entities it is interested in, allowing for efficient processing
// of only relevant entities during updates.
// </remarks>

// <summary>
// Constructor initializes the system with a null world pointer.
// </summary>
// <remarks>
// This constructor sets the world pointer to null, which should be initialized later using the Init
// method. It also initializes the entities vector to hold the entities that this system will process
// during updates.
// </remarks>
System::System() : world(nullptr) {
}

// <summary>
// Destructor cleans up the system, clearing the entities vector and resetting the world pointer.
// </summary>
// <remarks>
// This destructor is responsible for cleaning up the system's resources. It clears the entities vector
// to remove all entities that this system was processing, and it sets the world pointer to null to
// indicate that the system is no longer associated with any world.
// This is typically called when the system is no longer needed, such as when the ECS world is being destroyed
// or when the system is being replaced by another one.
// </remarks>
System::~System() {
    Shutdown();
}

// <summary>
// Initializes the system with a reference to the world.
// </summary>
// <param name="world">A pointer to the World instance this system belongs to.</param>
// <remarks>
// This method sets the world pointer to the provided World instance, allowing the system to access
// the world and its entities during updates.
// </remarks>
void System::Init(World* world) {
    this->world = world;
}

// <summary>
// Shuts down the system, cleaning up any resources it was using.
// </summary>
// <remarks>
// This method is responsible for cleaning up the system's resources. It clears the entities vector
// to remove all entities that this system was processing, and it sets the world pointer to null to
// indicate that the system is no longer associated with any world.
// This is typically called when the system is no longer needed, such as when the ECS world is being destroyed
// or when the system is being replaced by another one.
// </remarks>
void System::Shutdown() {
    entities.clear();
    world = nullptr;
}

// <summary>
// Called when an entity is added to the system.
// </summary>
// <param name="entity">The ID of the entity that was added.</param>
// <remarks>
// This method is called when an entity is added to the system. It can be overridden by derived systems
// to implement custom behavior when entities are added.
// </remarks>
void System::OnEntityAdded(Entity entity) {
    (void)entity;
}

// <summary>
// Called when an entity is removed from the system.
// </summary>
// <param name="entity">The ID of the entity that was removed.</param>
// <remarks>
// This method is called when an entity is removed from the system. It can be overridden by derived systems
// to implement custom behavior when entities are removed.
// </remarks>
void System::OnEntityRemoved(Entity entity) {
    (void)entity;
}

// <summary>
// Gets the world this system belongs to.
// </summary>
// <remarks>
// This method returns a pointer to the World instance that this system is associated with.
// It can be used by systems to access the world and its entities during updates.
// </remarks>
World* System::GetWorld() const {
    return world;
}
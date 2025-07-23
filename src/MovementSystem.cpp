#include "MovementSystem.hpp"
#include "World.hpp"
#include "TransformComponent.hpp"
#include "VelocityComponent.hpp"

// <summary>
// MovementSystem is a System that updates the position of entities based on their velocity.
// It inherits from the System class and overrides the Update method to handle movement logic.
// </summary>
// <remarks>
// This system processes all entities that have both Transform and Velocity components.
// It updates the position of each entity by adding the velocity multiplied by the delta time to the
// current position.
// This allows for smooth movement of entities in the game world.
// The Update method is called every frame, allowing for continuous movement based on the current velocity.
// </remarks>
void MovementSystem::Update(float deltaTime) {
    for (Tron::Entity entity : entities) {
        auto* transform = world->GetComponent<Transform>(entity);
        auto* velocity = world->GetComponent<Velocity>(entity);
        
        if (transform && velocity) {
            transform->x += velocity->vx * deltaTime;
            transform->y += velocity->vy * deltaTime;
            transform->z += velocity->vz * deltaTime;
        }
    }
}
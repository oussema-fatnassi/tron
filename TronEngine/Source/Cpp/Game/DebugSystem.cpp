#include "../../Headers/Game/DebugSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/VelocityComponent.hpp"

// <summary>
// DebugSystem class, responsible for printing debug information about entities in the ECS world.
// It periodically prints the state of entities with Transform and Velocity components.
// </summary>
// <remarks>
// This system is useful for debugging purposes, allowing developers to see the positions and velocities of entities
// in the console at regular intervals.
// It can be configured to print at different intervals, and it automatically updates its list of entities
// based on the components they have.
// </remarks>
DebugSystem::DebugSystem(float interval) : _printInterval(interval) {}

// <summary>
// Updates the debug system, printing entity information at specified intervals.
// </summary>
// <param name="deltaTime">The time elapsed since the last update, used to determine when to print debug information.</param>
// <remarks>
// This method accumulates the elapsed time and prints debug information about entities with Transform and Velocity components
// when the accumulated time exceeds the print interval.
// It iterates through all entities managed by the world and prints their positions and velocities.
// </remarks>
void DebugSystem::Update(float deltaTime) {
    _printTimer += deltaTime;

    /*if (_printTimer >= _printInterval) {
        _printTimer -= _printInterval;

        std::cout << "\n=== Entity Debug Info ===" << std::endl;

        for (Entity entity : entities) {
            std::cout << "Entity " << entity << ": ";

            auto* transform = world->GetComponent<Transform>(entity);
            if (transform) {
                std::cout << std::fixed << std::setprecision(2);
                std::cout << "Pos(" << transform->x << ", "
                         << transform->y << ", " << transform->z << ") ";
            }

            auto* velocity = world->GetComponent<Velocity>(entity);
            if (velocity) {
                std::cout << "Vel(" << velocity->vx << ", "
                         << velocity->vy << ", " << velocity->vz << ") ";
            }

            std::cout << std::endl;
        }
    }*/
}

// <summary>
// Sets the interval at which debug information is printed.
// </summary>
// <param name="interval">The new interval in seconds.</param>
// <remarks>
// This method allows changing the print interval dynamically, which can be useful for adjusting the frequency of
// debug output based on the needs of the developer or the performance requirements of the game.
// </remarks>
void DebugSystem::SetPrintInterval(float interval) {
    _printInterval = interval;
}
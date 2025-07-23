#include <iostream>
#include <thread>
#include <chrono>

#include "World.hpp"

#include "TransformComponent.hpp"
#include "VelocityComponent.hpp"

#include "DebugSystem.hpp"
#include "MovementSystem.hpp"


int main() {
    // Create the ECS world
    World world;
    
    // Register components
    world.RegisterComponent<Transform>();
    world.RegisterComponent<Velocity>();

    std::cout << "Entity count: " << world.GetEntityCount() << std::endl;

    // Register systems
    auto* debugSystem = world.RegisterSystem<DebugSystem>();
    auto* movementSystem = world.RegisterSystem<MovementSystem>();

    // Set system signatures
    world.SetSystemSignature<Transform>(debugSystem);
    world.SetSystemSignature<Transform, Velocity>(movementSystem);
    
    // Create some entities
    std::cout << "Creating entities..." << std::endl;
    
    // Player entity
    Entity player = world.CreateEntity();
    world.AddComponent<Transform>(player, 0, 0, 0);
    world.AddComponent<Velocity>(player, 10, 0, 0);  // Moving right
    std::cout << "Created player (Entity " << player << ")" << std::endl;
    std::cout << "Entity ID: " << player << std::endl;
    world.DestroyEntity(player); // Remove player entity

    // Enemy entity
    Entity enemy = world.CreateEntity();
    world.AddComponent<Transform>(enemy, 50, 0, 0);
    world.AddComponent<Velocity>(enemy, -5, 0, 0);   // Moving left
    std::cout << "Created enemy (Entity " << enemy << ")" << std::endl;
    //world.RemoveComponent<Velocity>(enemy); // Remove velocity to stop movement
    std::cout << "Removed velocity from enemy (Entity " << enemy << ")" << std::endl;
    std::cout << "Entity ID: " << enemy << std::endl;

    
    // Static object (no velocity)
    Entity staticObject = world.CreateEntity();
    world.AddComponent<Transform>(staticObject, 25, 10, 0);
    std::cout << "Created static object (Entity " << staticObject << ")" << std::endl;
    std::cout << "Entity ID: " << staticObject << std::endl;

    std::cout << "Entity count: " << world.GetEntityCount() << std::endl; 


    // Main game loop
    std::cout << "Starting game loop..." << std::endl;
    for (int i = 0; i < 100; ++i) {
        world.Update(0.1f);  // Simulate 100ms per frame
    }
    std::cout << "Game loop finished." << std::endl;
    // Cleanup
    world.Shutdown();
    std::cout << "World shutdown complete." << std::endl;       
    return 0;
}
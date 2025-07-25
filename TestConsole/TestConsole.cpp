#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"

// Link the library
#pragma comment(lib, "TronEngine.lib")

int main() {
    // Create and initialize engine
    Engine* engine = CreateEngine();
    if (!engine) {
        std::cout << "Failed to create engine\n";
        return -1;
    }

    if (!InitializeEngine(engine)) {
        std::cout << "Failed to initialize engine\n";
        DestroyEngine(engine);
        return -1;
    }

    // Print engine info
    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    // Create entities and add components
    std::cout << "\n=== Creating Entities ===\n";

    // Create a player entity
    unsigned int player = EngineCreateEntity(engine);
    std::cout << "Created player entity: " << player << std::endl;

    // Add transform component at origin
    if (EngineAddTransformComponent(engine, player, 0.0f, 0.0f, 0.0f)) {
        std::cout << "Added transform component to player\n";
    }

    // Add velocity component moving right
    if (EngineAddVelocityComponent(engine, player, 10.0f, 0.0f, 0.0f)) {
        std::cout << "Added velocity component to player\n";
    }

    // Create an enemy entity
    unsigned int enemy = EngineCreateEntity(engine);
    std::cout << "Created enemy entity: " << enemy << std::endl;

    // Add components to enemy
    EngineAddTransformComponent(engine, enemy, 50.0f, 0.0f, 0.0f);
    EngineAddVelocityComponent(engine, enemy, -5.0f, 0.0f, 0.0f);

    // Create a static object (only transform, no velocity)
    unsigned int staticObj = EngineCreateEntity(engine);
    std::cout << "Created static object: " << staticObj << std::endl;
    EngineAddTransformComponent(engine, staticObj, 25.0f, 10.0f, 0.0f);

    // Check entity count
    std::cout << "Total entities: " << EngineGetEntityCount(engine) << std::endl;

    // Read initial component data
    std::cout << "\n=== Initial Component Data ===\n";
    float x, y, z, vx, vy, vz;

    if (EngineGetTransformComponent(engine, player, &x, &y, &z)) {
        std::cout << "Player initial position: (" << x << ", " << y << ", " << z << ")\n";
    }

    if (EngineGetVelocityComponent(engine, player, &vx, &vy, &vz)) {
        std::cout << "Player velocity: (" << vx << ", " << vy << ", " << vz << ")\n";
    }

    // Test component removal
    std::cout << "\n=== Testing Component Operations ===\n";
    EngineRemoveVelocityComponent(engine, enemy);
    std::cout << "Removed velocity from enemy (should stop moving)\n";

    // Destroy an entity
    EngineDestroyEntity(engine, staticObj);
    std::cout << "Destroyed static object\n";
    std::cout << "Entities remaining: " << EngineGetEntityCount(engine) << std::endl;

    // Now let the engine handle everything - all updates, timing, and game loop
    std::cout << "\n=== Starting Engine - All updates handled internally ===\n";
    RunEngine(engine);

    // Cleanup
    std::cout << "\n=== Cleanup ===\n";
    ShutdownEngine(engine);
    DestroyEngine(engine);
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "====================================\n";
    std::cout << "      ALL TESTS COMPLETED!         \n";
    std::cout << "====================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    return 0;
}
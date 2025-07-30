#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "PlayerScript.hpp"
// Link the library
#pragma comment(lib, "TronEngine.lib")

int main() {
    // Create and initialize engine (singleton)
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    // Print engine info
    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    // Create entities and add components
    std::cout << "\n=== Creating Entities ===\n";

    // Create a player entity
    unsigned int player = CreateEntity();
    std::cout << "Created player entity: " << player << std::endl;

    // Add transform component at origin
    if (AddTransformComponent(player, 0.0f, 0.0f, 0.0f)) {
        std::cout << "Added transform component to player\n";
    }

    // Add velocity component moving right
    if (AddVelocityComponent(player, 10.0f, 0.0f, 0.0f)) {
        std::cout << "Added velocity component to player\n";
    }
	uint32_t player2 = CreateEntity();
    // Create custom script instance
	// TODO: Use a factory or script manager in the engine to handle this properly
    PlayerScript* playerScript1 = new PlayerScript(&player2,"Hero");
    /*PlayerScript* playerScript2 = new PlayerScript("Enemy");
    PlayerScript* playerScript3 = new PlayerScript("NPC");*/

    // Add it to entity
    if (AddCustomScript(player, playerScript1)) {
        std::cout << "Added custom player script\n";
    }

    // Create an enemy entity
    unsigned int enemy = CreateEntity();
    std::cout << "Created enemy entity: " << enemy << std::endl;

    // Add components to enemy
    AddTransformComponent(enemy, 50.0f, 0.0f, 0.0f);
    AddVelocityComponent(enemy, -5.0f, 0.0f, 0.0f);

    // Create a static object (only transform, no velocity)
    unsigned int staticObj = CreateEntity();
    std::cout << "Created static object: " << staticObj << std::endl;
    AddTransformComponent(staticObj, 25.0f, 10.0f, 0.0f);

    // Create a cube entity
    //uint32_t cubeEntity = CreateEntity();
    //AddTransformComponent(cubeEntity, 0.0f, 0.0f, 0.0f);
    //AddMeshRendererComponent(cubeEntity, PRIMITIVE_CUBE, "blue");
    //SetMeshRendererColor(cubeEntity, 1.0f, 0.0f, 0.0f, 1.0f); // Blue cube

    // Create a sphere entity
    //uint32_t sphereEntity = CreateEntity();
    //AddTransformComponent(sphereEntity, 2.0f, 0.0f, 0.0f);
    //AddMeshRendererComponent(sphereEntity, PRIMITIVE_SPHERE, "default");
    //SetMeshRendererColor(sphereEntity, 0.0f, 1.0f, 0.0f, 1.0f); // Green sphere

    // Create a triangle entity
    //uint32_t triangleEntity = CreateEntity();
    //AddTransformComponent(triangleEntity, -2.0f, 0.0f, 0.0f);
    //AddMeshRendererComponent(triangleEntity, PRIMITIVE_TRIANGLE, "default");
    //SetMeshRendererColor(triangleEntity, 0.0f, 0.0f, 1.0f, 1.0f); // Blue triangle

    uint32_t cube = CreateEntity();
    AddTransformComponent(cube, 10.0f, 10.0f, 10.0f);
    AddMeshRendererComponent(cube,PRIMITIVE_CUBE, "RainbowShader"); // Use rainbow shader


    // Check entity count
    std::cout << "Total entities: " << GetEntityCount() << std::endl;

    // Read initial component data
    std::cout << "\n=== Initial Component Data ===\n";
    float x, y, z, vx, vy, vz;
    if (GetTransformComponent(player, &x, &y, &z)) {
        std::cout << "Player initial position: (" << x << ", " << y << ", " << z << ")\n";
    }
    if (GetVelocityComponent(player, &vx, &vy, &vz)) {
        std::cout << "Player velocity: (" << vx << ", " << vy << ", " << vz << ")\n";
    }

    // Test component removal
    std::cout << "\n=== Testing Component Operations ===\n";
    RemoveVelocityComponent(enemy);
    std::cout << "Removed velocity from enemy (should stop moving)\n";

    // Destroy an entity
    DestroyEntity(staticObj);
    std::cout << "Destroyed static object\n";
    std::cout << "Entities remaining: " << GetEntityCount() << std::endl;

    // Now let the engine handle everything - all updates, timing, and game loop
    std::cout << "\n=== Starting Engine - All updates handled internally ===\n";
    RunEngine();

    // Cleanup
    std::cout << "\n=== Cleanup ===\n";
    DestroyGlobalEngine();
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "====================================\n";
    std::cout << "      ALL TESTS COMPLETED!         \n";
    std::cout << "====================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
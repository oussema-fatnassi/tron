#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "PlayerScript.hpp"
#include "SecondScript.hpp"  // Include our second script for testing
#include "PhysicsTestScript.hpp"  // Include our new physics test script

#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "=== PLAYER SCRIPT INPUT TEST ===\n";

    // Create and initialize engine (singleton)
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    // Print engine info
    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Creating Test Scene with Player ===\n";

    // Create reference objects to see movement relative to
    uint32_t centerCube = CreateEntity();
    AddTransformComponent(centerCube, 0.0f, 0.0f, 0.0f);
    AddMeshRendererComponent(centerCube, PRIMITIVE_CUBE, "RainbowShader");
    SetMeshRendererColor(centerCube, 1.0f, 0.0f, 0.0f, 1.0f); // Red cube at origin
	AddCustomScript(centerCube, new SecondScript()); // Add our second script to the center cube
    std::cout << "Created reference cube (RED) at origin\n";

    // Create some other reference objects
    uint32_t blueCube = CreateEntity();
    AddTransformComponent(blueCube, 3.0f, 0.0f, 0.0f);
    AddMeshRendererComponent(blueCube, PRIMITIVE_CUBE, "blue");
    SetMeshRendererColor(blueCube, 0.0f, 0.0f, 1.0f, 1.0f); // Blue cube to the right
	AddCustomScript(blueCube, new SecondScript()); // Add a second script to the blue cube
    std::cout << "Created blue cube at (3, 0, 0)\n";

    // Create some other reference objects
    uint32_t blueCube2 = CreateEntity();
    AddTransformComponent(blueCube2, 0.0f, 3.0f, 0.0f);
    AddMeshRendererComponent(blueCube2, PRIMITIVE_CUBE, "blue");
    SetMeshRendererColor(blueCube2, 0.0f, 0.0f, 1.0f, 1.0f); // Blue cube to the right
    AddCustomScript(blueCube2, new SecondScript()); // Add a second script to the blue cube
    std::cout << "Created blue cube at (0, 3, 0)\n";

    uint32_t greenSphere = CreateEntity();
    AddTransformComponent(greenSphere, -3.0f, 0.0f, 0.0f);
    AddMeshRendererComponent(greenSphere, PRIMITIVE_SPHERE, "default");
    SetMeshRendererColor(greenSphere, 0.0f, 1.0f, 0.0f, 1.0f); // Green sphere to the left
	AddCustomScript(greenSphere, new SecondScript()); // Add a second script to the green sphere
    std::cout << "Created green sphere at (-3, 0, 0)\n";

    uint32_t yellowCube = CreateEntity();
    AddTransformComponent(yellowCube, 0.0f, 0.0f, -3.0f);
    AddMeshRendererComponent(yellowCube, PRIMITIVE_CUBE, "RainbowShader");
    SetMeshRendererColor(yellowCube, 1.0f, 1.0f, 0.0f, 1.0f); // Yellow cube in front
	AddCustomScript(yellowCube, new SecondScript()); // Add a second script to the yellow cube
    std::cout << "Created yellow cube at (0, 0, -3)\n";

    // Create the player entity with script
    uint32_t player = CreateEntity();
    std::cout << "Created player entity: " << player << std::endl;

    // Add the player script
    PlayerScript* playerScript = new PlayerScript("TestPlayer");
    if (AddCustomScript(player, playerScript)) {
        std::cout << " PlayerScript added successfully\n";

        // Configure player settings
        playerScript->SetMovementSpeed(3.0f);
        playerScript->SetMouseSensitivity(0.001f);
    }
    else {
        std::cout << " ERROR: Failed to add PlayerScript\n";
        return -1;
    }

    for (int i = 0; i <= 100; i++)
    {
		// Create some random cubes around the player for testing
		uint32_t randomCube = CreateEntity();
		float x = static_cast<float>(rand() % 20 - 10); // Random position between -10 and 10
		float y = static_cast<float>(rand() % 20 - 10); // Random position between -10 and 10
		float z = static_cast<float>(rand() % 20 - 10); // Random position between -10 and 10
		AddTransformComponent(randomCube, x, y, z);
		AddMeshRendererComponent(randomCube, PRIMITIVE_CUBE, "default");
		AddCustomScript(randomCube, new SecondScript(player)); // Add a second script to the random cube
    }
	SetPhysicsGridCellSize(3.0f);

    std::cout << "\n=== CONTROLS ===\n";
    std::cout << "WASD     - Move around (watch console for movement messages)\n";
    std::cout << "Mouse    - Look around (rotation logged to console)\n";
    std::cout << "Space    - Move up\n";
    std::cout << "Shift    - Move down\n";
    std::cout << "P        - Print current position\n";
    std::cout << "ESC      - Close window\n";

    std::cout << "\n=== Expected Behavior ===\n";
    std::cout << "- Press WASD and see movement messages in console\n";
    std::cout << "- Player cube (cyan) should move around the scene\n";
    std::cout << "- Movement should be visible relative to other cubes\n";
    std::cout << "- Position should be printed when you press P\n";

    std::cout << "\nTotal entities: " << GetEntityCount() << "\n";
    std::cout << "\n=== Starting Engine ===\n";

    // Start the engine
    RunEngine();

    // Note: PlayerScript destructor will be called automatically
    // when the entity is destroyed during engine shutdown

    // Cleanup
    std::cout << "\n=== Cleanup ===\n";
    DestroyGlobalEngine();
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "====================================\n";
    std::cout << "   PLAYER INPUT TEST COMPLETED!    \n";
    std::cout << "====================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
// Updated TestConsole/TestConsole.cpp for first person camera testing

#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "FirstPersonCameraScript.hpp"  // NEW: Our first person camera script
#include "SecondScript.hpp"

#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "=== FIRST PERSON CAMERA TEST ===\n";

    // Create and initialize engine
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Creating First Person Camera Scene ===\n";

    // Create the FIRST PERSON CAMERA PLAYER (this will be our viewpoint)
    uint32_t player = CreateEntity();
    std::cout << "Created player entity (camera): " << player << std::endl;

    // Add the first person camera script - THIS IS THE KEY CHANGE
    FirstPersonCameraScript* cameraScript = new FirstPersonCameraScript("CameraPlayer");
    if (AddCustomScript(player, cameraScript)) {
        std::cout << "✓ First Person Camera Script added successfully\n";
        
        // Configure camera settings
        cameraScript->SetMovementSpeed(8.0f);        // Faster movement
        cameraScript->SetMouseSensitivity(0.003f);   // More sensitive mouse
    } else {
        std::cout << "✗ ERROR: Failed to add First Person Camera Script\n";
        return -1;
    }

    // Create reference objects to see in the world (these are what we'll look at)
    std::cout << "\n=== Creating World Objects ===\n";

    // Center cube - Red (origin reference)
    uint32_t centerCube = CreateEntity();
    AddTransformComponent(centerCube, 0.0f, 0.0f, 0.0f);
    AddMeshRendererComponent(centerCube, PRIMITIVE_CUBE, "default");
    SetMeshRendererColor(centerCube, 1.0f, 0.0f, 0.0f, 1.0f); // Red
    AddCustomScript(centerCube, new SecondScript(player)); // Destroy when player touches
    std::cout << "✓ Red cube at origin (0,0,0)\n";

    // Blue cubes - Positioned around the scene
    uint32_t blueCube1 = CreateEntity();
    AddTransformComponent(blueCube1, 5.0f, 0.0f, 0.0f);
    AddMeshRendererComponent(blueCube1, PRIMITIVE_CUBE, "blue");
    SetMeshRendererColor(blueCube1, 0.0f, 0.0f, 1.0f, 1.0f);
    AddCustomScript(blueCube1, new SecondScript(player));
    std::cout << "✓ Blue cube at (5,0,0)\n";

    uint32_t blueCube2 = CreateEntity();
    AddTransformComponent(blueCube2, -5.0f, 0.0f, 0.0f);
    AddMeshRendererComponent(blueCube2, PRIMITIVE_CUBE, "blue");
    SetMeshRendererColor(blueCube2, 0.0f, 0.0f, 1.0f, 1.0f);
    AddCustomScript(blueCube2, new SecondScript(player));
    std::cout << "✓ Blue cube at (-5,0,0)\n";

    // Green cubes - Different heights
    uint32_t greenCube1 = CreateEntity();
    AddTransformComponent(greenCube1, 0.0f, 3.0f, -5.0f);
    AddMeshRendererComponent(greenCube1, PRIMITIVE_CUBE, "default");
    SetMeshRendererColor(greenCube1, 0.0f, 1.0f, 0.0f, 1.0f);
    AddCustomScript(greenCube1, new SecondScript(player));
    std::cout << "✓ Green cube at (0,3,-5)\n";

    uint32_t greenCube2 = CreateEntity();
    AddTransformComponent(greenCube2, 0.0f, -2.0f, -5.0f);
    AddMeshRendererComponent(greenCube2, PRIMITIVE_CUBE, "default");
    SetMeshRendererColor(greenCube2, 0.0f, 1.0f, 0.0f, 1.0f);
    AddCustomScript(greenCube2, new SecondScript(player));
    std::cout << "✓ Green cube at (0,-2,-5)\n";

    // Yellow cubes - Far away
    uint32_t yellowCube = CreateEntity();
    AddTransformComponent(yellowCube, 0.0f, 0.0f, -10.0f);
    AddMeshRendererComponent(yellowCube, PRIMITIVE_CUBE, "RainbowShader");
    SetMeshRendererColor(yellowCube, 1.0f, 1.0f, 0.0f, 1.0f);
    AddCustomScript(yellowCube, new SecondScript(player));
    std::cout << "✓ Yellow cube at (0,0,-10)\n";

    // Create a "floor" using multiple cubes
    std::cout << "\n=== Creating Floor ===\n";
    for (int x = -10; x <= 10; x += 2) {
        for (int z = -10; z <= 10; z += 2) {
            if (x == 0 && z == 0) continue; // Skip center (occupied by red cube)
            
            uint32_t floorCube = CreateEntity();
            AddTransformComponent(floorCube, (float)x, -3.0f, (float)z);
            AddMeshRendererComponent(floorCube, PRIMITIVE_CUBE, "default");
            SetMeshRendererColor(floorCube, 0.3f, 0.3f, 0.3f, 1.0f); // Gray floor
            AddCustomScript(floorCube, new SecondScript(player));
        }
    }
    std::cout << "✓ Gray floor created\n";

    // Set up physics
    SetPhysicsGridCellSize(3.0f);

    std::cout << "\n=== FIRST PERSON CAMERA CONTROLS ===\n";
    std::cout << "ZQSD     - Move forward/left/backward/right (camera relative)\n";
    std::cout << "Mouse    - Look around (first person view)\n";
    std::cout << "Space    - Move up\n";
    std::cout << "Shift    - Move down\n";
    std::cout << "P        - Print camera position and rotation\n";
    std::cout << "ESC      - Close window\n";

    std::cout << "\n=== EXPECTED BEHAVIOR ===\n";
    std::cout << "- You should see cubes from a first-person perspective\n";
    std::cout << "- Moving the mouse should rotate your view\n";
    std::cout << "- ZQSD should move you relative to where you're looking\n";
    std::cout << "- Walking into cubes should destroy them\n";
    std::cout << "- You start at position (0, 2, 5) looking toward origin\n";

    std::cout << "\nTotal entities: " << GetEntityCount() << "\n";
    std::cout << "\n=== Starting First Person Camera Test ===\n";

    // Start the engine
    RunEngine();

    // Cleanup
    std::cout << "\n=== Cleanup ===\n";
    DestroyGlobalEngine();
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "========================================\n";
    std::cout << "  FIRST PERSON CAMERA TEST COMPLETED!  \n";
    std::cout << "========================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
<<<<<<< HEAD
// TestConsole.cpp - Raycast System Test
=======
// Asteroid Generator Test - TestConsole.cpp
>>>>>>> feat/procedural-generation
#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "FirstPersonCameraScript.hpp"
<<<<<<< HEAD
#include "ParticleTestScript.hpp"  // Include our particle test script
#include "RaycastTestScript.hpp"
=======
>>>>>>> feat/procedural-generation

#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "========================================\n";
    std::cout << "     TRON ENGINE - RAYCAST TEST        \n";
    std::cout << "========================================\n\n";
    std::cout << "=== ASTEROID GENERATOR DLL TEST ===\n";

    // Create and initialize engine
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Setting Up Raycast Test Scene ===\n";
    std::cout << "\n=== Testing Asteroid Generator API ===\n";

    // Test the asteroid generator API
    TestAsteroidGenerator();

    std::cout << "\n=== Creating Camera for Visual Test ===\n";

    // Create the FIRST PERSON CAMERA PLAYER with raycast testing
    uint32_t player = CreateEntity();
    std::cout << "Created player entity: " << player << std::endl;

    // Add the first person camera script
    FirstPersonCameraScript* cameraScript = new FirstPersonCameraScript("Player");
    if (AddCustomScript(player, cameraScript)) {
        std::cout << "✓ First Person Camera added\n";
        
        // Configure camera for testing
        cameraScript->SetMovementSpeed(5.0f);
        cameraScript->SetMouseSensitivity(0.002f);
        
        // Set camera entity for raycasting
        SetCameraEntity(player);
        std::cout << "✓ Camera entity registered for raycasting\n";
    } else {
        std::cout << "✗ ERROR: Failed to add First Person Camera\n";
        return -1;
    }

    std::cout << "\n=== Creating Asteroid Field using DLL API ===\n";
    
    // Create asteroid generator
    void* asteroidGenerator = CreateAsteroidGenerator();
    if (!asteroidGenerator) {
        std::cout << "✗ Failed to create asteroid generator\n";
        return -1;
    }

    // Generate some individual asteroids
    std::cout << "Generating individual asteroids...\n";
    for (int i = 0; i < 3; i++) {
        float x = (i - 1) * 5.0f;
        uint32_t asteroid = GenerateSingleAsteroid(asteroidGenerator, 2.0f + i, 8, x, 0.0f, -15.0f, "RainbowShader");
        if (asteroid != 0) {
            std::cout << "✓ Generated asteroid " << i << " (Entity: " << asteroid << ") at (" << x << ", 0, -15)\n";
        }
    }

    // Generate an asteroid field
    std::cout << "Generating asteroid field...\n";
    GenerateAsteroidField(asteroidGenerator, 50, 3.0f, 10.0f, 10, 50, 30.0f, 20.0f, 30.0f, "RainbowShader");

    // Add some reference objects
    uint32_t referenceBox = CreateEntity();
    AddTransformComponent(referenceBox, 0.0f, 5.0f, 0.0f);
    AddMeshRendererComponent(referenceBox, PRIMITIVE_CUBE, "blue");
    SetMeshRendererColor(referenceBox, 0.0f, 1.0f, 0.0f, 1.0f); // Green reference
    SetTransformUniformScale(referenceBox, 1.0f);
    std::cout << "✓ Added green reference cube at (0, 5, 0)\n";

    // Cleanup asteroid generator
    DestroyAsteroidGenerator(asteroidGenerator);

    // Set up physics
    SetPhysicsGridCellSize(5.0f);
    SetPhysicsDebugOutput(false);
    
    // Enable raycast debug visualization initially
    SetRaycastDebugDraw(true);

    std::cout << "\n========================================\n";
    std::cout << "         RAYCAST TEST CONTROLS         \n";
    std::cout << "========================================\n";
    std::cout << "MOVEMENT:\n";
    std::cout << "  WASD      - Move forward/left/back/right\n";
    std::cout << "  Mouse     - Look around\n";
    std::cout << "  Space     - Move up\n";
    std::cout << "  Shift     - Move down\n";
    std::cout << "\nRAYCAST TESTS:\n";
    std::cout << "  1         - Test forward raycast\n";
    std::cout << "  2         - Test omnidirectional raycast\n";
    std::cout << "  3/Click   - Test mouse picking\n";
    std::cout << "  4         - Test line of sight\n";
    std::cout << "  5         - Test camera raycast\n";
    std::cout << "  R         - Print raycast statistics\n";
    std::cout << "  T         - Toggle debug visualization\n";
    std::cout << "\nOTHER:\n";
    std::cout << "  P         - Print camera position\n";
    std::cout << "  ESC       - Exit\n";
    std::cout << "========================================\n\n";

    std::cout << "WHAT TO EXPECT:\n";
    std::cout << "- RED box in front at (0, 2, -5)\n";
    std::cout << "- GREEN box to the right at (5, 2, -3)\n";
    std::cout << "- BLUE box above-left at (-3, 5, -4)\n";
    std::cout << "- Automatic 360° sweep test every 5 seconds\n";
    std::cout << "- Click on boxes to test mouse picking\n\n";

    std::cout << "Total entities: " << GetEntityCount() << "\n";
    std::cout << "\n=== Starting Raycast Test ===\n";
    std::cout << "\n=== ASTEROID FIELD TEST CONTROLS ===\n";
    std::cout << "ZQSD     - Move forward/left/backward/right\n";
    std::cout << "Mouse    - Look around\n";
    std::cout << "Space    - Move up\n";
    std::cout << "Shift    - Move down\n";
    std::cout << "P        - Print camera position\n";
    std::cout << "ESC      - Close window\n";

    std::cout << "\n=== EXPECTED BEHAVIOR ===\n";
    std::cout << "- Console should show asteroid generation results\n";
    std::cout << "- You should see spheres representing asteroids\n";
    std::cout << "- Navigate around to explore the asteroid field\n";
    std::cout << "- Green cube is a reference point\n";

    std::cout << "\nTotal entities: " << GetEntityCount() << "\n";
    std::cout << "\n=== Starting Asteroid Field Test ===\n";

    // Start the engine
    RunEngine();

    // Cleanup
    std::cout << "\n=== Test Complete - Printing Final Stats ===\n";
    
    // Print final raycast statistics
    uint32_t raycastCount, aabbTestCount;
    float lastRaycastMs;
    GetRaycastStats(&raycastCount, &aabbTestCount, &lastRaycastMs);
    
    std::cout << "Final Raycast Statistics:\n";
    std::cout << "  Total raycasts: " << raycastCount << "\n";
    std::cout << "  Total AABB tests: " << aabbTestCount << "\n";
    std::cout << "  Last raycast time: " << lastRaycastMs << " ms\n";
    
    // Print physics stats
    PrintPhysicsSystemStats();
    
    DestroyGlobalEngine();
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "========================================\n";
    std::cout << "      RAYCAST TEST COMPLETED!          \n";
    std::cout << "  ASTEROID GENERATOR DLL TEST COMPLETED!\n";
    std::cout << "========================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
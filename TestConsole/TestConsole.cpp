// TestConsole.cpp - Raycast System Test
// Asteroid Generator Test - TestConsole.cpp
#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "FirstPersonCameraScript.hpp"
#include "ParticleTestScript.hpp" // Include our particle test script
#include "RaycastTestScript.hpp"
#include "Target.hpp"

#pragma comment(lib, "TronEngine.lib")

int main()
{
    std::cout << "========================================\n";
    std::cout << "     TRON ENGINE - RAYCAST TEST        \n";
    std::cout << "========================================\n\n";
    std::cout << "=== ASTEROID GENERATOR DLL TEST ===\n";

    // Create and initialize engine
    if (!CreateAndInitializeEngine())
    {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    // Create the FIRST PERSON CAMERA PLAYER with raycast testing
    uint32_t player = CreateEntity();
    std::cout << "Created player entity: " << player << std::endl;

    // Add the first person camera script
    FirstPersonCameraScript *cameraScript = new FirstPersonCameraScript("Player");
    if (AddCustomScript(player, cameraScript))
    {
        std::cout << "✓ First Person Camera added\n";

        // Configure camera for testing
        cameraScript->SetMovementSpeed(5.0f);
        cameraScript->SetMouseSensitivity(0.002f);

        // Set camera entity for raycasting
        SetCameraEntity(player);
        std::cout << "✓ Camera entity registered for raycasting\n";
    }
    else
    {
        std::cout << "✗ ERROR: Failed to add First Person Camera\n";
        return -1;
    }

    // Activating Raycast
    uint32_t raycastTester = CreateEntity();
    RaycastTestScript *raycastScript = new RaycastTestScript("RaycastController");
    if (AddCustomScript(raycastTester, raycastScript))
    {
        std::cout << "✓ Raycast Test Controller added\n";
    }
    else
    {
        std::cout << "✗ ERROR: Failed to add Raycast Test Script\n";
        return -1;
    }

    //// Create asteroid generator
    //void *asteroidGenerator = CreateAsteroidGenerator();
    //if (!asteroidGenerator)
    //{
    //    std::cout << "✗ Failed to create asteroid generator\n";
    //    return -1;
    //}

    // //Generate an asteroid field
    //std::cout << "Generating asteroid field...\n";
    //GenerateAsteroidField(asteroidGenerator, 20, 3.0f, 10.0f, 10, 50, 30.0f, 20.0f, 30.0f, "RainbowShader");

    //// Cleanup asteroid generator
    //DestroyAsteroidGenerator(asteroidGenerator);

    // Create test targets
    Target *target = new Target();
    uint32_t targetBox1 = CreateEntity();
    AddTransformComponent(targetBox1, 0.0f, 2.0f, -5.0f);
    AddMeshRendererComponent(targetBox1, PRIMITIVE_CUBE, "RainbowShader");
    AddBoxColliderComponent(targetBox1, 1.0f, 1.0f, 1.0f, true);
    AddCustomScript(targetBox1, target);
    std::cout << "Created target box 1 (RED) at (0, 2, -5)\n";

    Target* target2 = new Target();
    uint32_t targetBox2 = CreateEntity();
    AddTransformComponent(targetBox2, 5.0f, 2.0f, -3.0f);
    AddMeshRendererComponent(targetBox2, PRIMITIVE_CUBE, "RainbowShader");
    AddBoxColliderComponent(targetBox2, 1.0f, 1.0f, 1.0f, true);
    AddCustomScript(targetBox2, target2);
    std::cout << "Created target box 2 (GREEN) at (5, 2, -3)\n";

    Target* target3 = new Target();
    uint32_t targetBox3 = CreateEntity();
    AddTransformComponent(targetBox3, -3.0f, 5.0f, -4.0f);
    AddMeshRendererComponent(targetBox3, PRIMITIVE_CUBE, "RainbowShader");
    AddBoxColliderComponent(targetBox3, 1.0f, 1.0f, 1.0f, true);
    AddCustomScript(targetBox3, target3);
    std::cout << "Created target box 3 (BLUE) at (-3, 5, -4)\n";

    Target* target4 = new Target();
    uint32_t targetBox4 = CreateEntity();
    AddTransformComponent(targetBox4, 10.0f, 2.0f, -10.0f);
    AddMeshRendererComponent(targetBox4, PRIMITIVE_CUBE, "RainbowShader");
    AddBoxColliderComponent(targetBox4, 1.0f, 1.0f, 1.0f, true);
    AddCustomScript(targetBox4, target4);
    std::cout << "Created target box 4 (YELLOW) at (10, 2, -10)\n";

    Target* target5 = new Target();
    uint32_t targetBox5 = CreateEntity();
    AddTransformComponent(targetBox5, -10.0f, 2.0f, -10.0f);
    AddBoxColliderComponent(targetBox5, 1.0f, 1.0f, 1.0f, true);
    AddMeshRendererComponent(targetBox5, PRIMITIVE_CUBE, "RainbowShader");
    AddCustomScript(targetBox5, target5);
    std::cout << "Created target box 5 (PURPLE) at (-10, 2, -10)\n";

    Target* target6 = new Target();
    uint32_t targetBox6 = CreateEntity();
    AddTransformComponent(targetBox6, 0.0f, 2.0f, 10.0f);
    AddBoxColliderComponent(targetBox6, 1.0f, 1.0f, 1.0f, true);
    AddMeshRendererComponent(targetBox6, PRIMITIVE_CUBE, "RainbowShader");
    AddCustomScript(targetBox6, target6);
    std::cout << "Created target box 6 (CYAN) at (0, 2, 10)\n";

    Target* target7 = new Target();
    uint32_t targetBox7 = CreateEntity();
    AddTransformComponent(targetBox7, 10.0f, 2.0f, 10.0f);
    AddBoxColliderComponent(targetBox7, 1.0f, 1.0f, 1.0f, true);
    AddMeshRendererComponent(targetBox7, PRIMITIVE_CUBE, "RainbowShader");
    AddCustomScript(targetBox7, target7);
    std::cout << "Created target box 7 (ORANGE) at (10, 2, 10)\n";

    Target* target8 = new Target();
    uint32_t targetBox8 = CreateEntity();
    AddTransformComponent(targetBox8, -10.0f, 2.0f, 10.0f);
    AddBoxColliderComponent(targetBox8, 1.0f, 1.0f, 1.0f, true);
    AddMeshRendererComponent(targetBox8, PRIMITIVE_CUBE, "RainbowShader");
    AddCustomScript(targetBox8, target8);
    std::cout << "Created target box 8 (PINK) at (-10, 2, 10)\n";

    // Set up physics
    SetPhysicsGridCellSize(5.0f);
    SetPhysicsDebugOutput(false);

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
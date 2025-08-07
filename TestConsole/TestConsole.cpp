// TestConsole.cpp - Raycast System Test
#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "FirstPersonCameraScript.hpp"
#include "ParticleTestScript.hpp"  // Include our particle test script
#include "RaycastTestScript.hpp"

#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "========================================\n";
    std::cout << "     TRON ENGINE - RAYCAST TEST        \n";
    std::cout << "========================================\n\n";

    // Create and initialize engine
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Setting Up Raycast Test Scene ===\n";

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

    // Create ONE test box directly in front of the camera
    std::cout << "\n=== Creating Test Box ===\n";
    uint32_t testBox = CreateEntity();
    
    // Position: 5 units in front of camera (camera starts at Z=5, looking toward negative Z)
    // So box at Z=0 should be visible
    AddTransformComponent(testBox, 0.0f, 2.0f, 0.0f);  // Same height as camera (Y=2)
    AddMeshRendererComponent(testBox, PRIMITIVE_CUBE, "RainbowShader");
    SetMeshRendererColor(testBox, 1.0f, 0.0f, 0.0f, 1.0f); // Bright red
    SetTransformUniformScale(testBox, 2.0f); // Make it bigger to be easily visible
    AddBoxColliderComponent(testBox, 2.0f, 2.0f, 2.0f, false);

    std::cout << "✓ Red test box created at (0, 2, 0) with 2x scale\n";
    std::cout << "✓ Camera starts at (0, 2, 5) looking toward (0, 2, 0)\n";

    // Create a second box to the right for reference
    uint32_t sideBox = CreateEntity();
    AddTransformComponent(sideBox, 3.0f, 2.0f, 0.0f);  // To the right
    AddMeshRendererComponent(sideBox, PRIMITIVE_CUBE, "blue");
    SetMeshRendererColor(sideBox, 0.0f, 0.0f, 1.0f, 1.0f); // Bright blue
    SetTransformUniformScale(sideBox, 1.0f);
    AddBoxColliderComponent(sideBox, 1.0f, 1.0f, 1.0f, false);
    
    std::cout << "✓ Blue reference box created at (3, 2, 0)\n";

    uint32_t circle = CreateEntity();
    AddTransformComponent(circle, -3.0f, 2.0f, -3.0f);
    AddMeshRendererComponent(circle, PRIMITIVE_SPHERE,"RainbowShader");
    SetTransformUniformScale(circle, 1.0f);
    AddBoxColliderComponent(circle, 1.0f, 1.0f, 1.0f, false);

    uint32_t raycastTester = CreateEntity();
    RaycastTestScript* raycastScript = new RaycastTestScript("RaycastController");
    if (AddCustomScript(raycastTester, raycastScript)) {
        std::cout << "✓ Raycast Test Controller added\n";
    } else {
        std::cout << "✗ ERROR: Failed to add Raycast Test Script\n";
        return -1;
    }

    // Set up physics system for raycast optimization
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
    std::cout << "========================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
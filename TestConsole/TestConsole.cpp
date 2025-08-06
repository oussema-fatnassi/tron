// Simplified TestConsole/TestConsole.cpp - Single box test
#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "FirstPersonCameraScript.hpp"
#include "ParticleTestScript.hpp"  // Include our particle test script

#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "=== SIMPLIFIED CAMERA TEST - SINGLE BOX ===\n";

    // Create and initialize engine
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Creating Simple Test Scene ===\n";

    // Create the FIRST PERSON CAMERA PLAYER
    uint32_t player = CreateEntity();
    std::cout << "Created player entity (camera): " << player << std::endl;

    // Add the first person camera script
    FirstPersonCameraScript* cameraScript = new FirstPersonCameraScript("TestCamera");
    if (AddCustomScript(player, cameraScript)) {
        std::cout << "✓ First Person Camera Script added successfully\n";
        
        // Configure camera settings for testing
        cameraScript->SetMovementSpeed(3.0f);        // Slower for testing
        cameraScript->SetMouseSensitivity(0.002f);   // Moderate sensitivity
    } else {
        std::cout << "✗ ERROR: Failed to add First Person Camera Script\n";
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
    
    std::cout << "✓ Red test box created at (0, 2, 0) with 2x scale\n";
    std::cout << "✓ Camera starts at (0, 2, 5) looking toward (0, 2, 0)\n";

    // Create a second box to the right for reference
    uint32_t sideBox = CreateEntity();
    AddTransformComponent(sideBox, 3.0f, 2.0f, 0.0f);  // To the right
    AddMeshRendererComponent(sideBox, PRIMITIVE_CUBE, "blue");
    SetMeshRendererColor(sideBox, 0.0f, 0.0f, 1.0f, 1.0f); // Bright blue
    SetTransformUniformScale(sideBox, 1.0f);
    
    std::cout << "✓ Blue reference box created at (3, 2, 0)\n";


    uint32_t particles = CreateEntity();
    ParticleTestScript* particleScript = new ParticleTestScript();
    AddCustomScript(particles, particleScript);
    // Set up physics (keep it simple)
    SetPhysicsGridCellSize(5.0f);
    SetPhysicsDebugOutput(false);

    std::cout << "\n=== SIMPLE TEST CONTROLS ===\n";
    std::cout << "ZQSD     - Move forward/left/backward/right\n";
    std::cout << "Mouse    - Look around\n";
    std::cout << "Space    - Move up\n";
    std::cout << "Shift    - Move down\n";
    std::cout << "P        - Print camera position\n";
    std::cout << "ESC      - Close window\n";

    std::cout << "\n=== EXPECTED BEHAVIOR ===\n";
    std::cout << "- You should see a RED box directly in front of you\n";
    std::cout << "- You should see a BLUE box to your right\n";
    std::cout << "- If you see black, there's a matrix/rendering issue\n";
    std::cout << "- Camera starts at (0, 2, 5) looking toward origin\n";

    std::cout << "\nTotal entities: " << GetEntityCount() << "\n";
    std::cout << "\n=== Starting Simple Camera Test ===\n";

    // Start the engine
    RunEngine();

    // Cleanup
    std::cout << "\n=== Cleanup ===\n";
    DestroyGlobalEngine();
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "========================================\n";
    std::cout << "  SIMPLE CAMERA TEST COMPLETED!       \n";
    std::cout << "========================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
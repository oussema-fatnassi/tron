#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
// Temporarily comment out the enhanced PlayerScript
// #include "PlayerScript.hpp"

// Link the library
#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "=== SIMPLE RENDERING TEST ===\n";
    
    // Create and initialize engine (singleton)
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    // Print engine info
    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Creating Simple Test Scene ===\n";

    // Create just ONE cube to test basic rendering
    uint32_t testCube = CreateEntity();
    std::cout << "Created test cube entity: " << testCube << std::endl;

    // Add transform component - using the old simple interface
    if (AddTransformComponent(testCube, 0.0f, 0.0f, 0.0f)) {
        std::cout << "✓ Transform component added successfully\n";
        
        // Verify we can read it back
        float x, y, z;
        if (GetTransformComponent(testCube, &x, &y, &z)) {
            std::cout << "✓ Transform verified: (" << x << ", " << y << ", " << z << ")\n";
        } else {
            std::cout << "✗ ERROR: Cannot read transform component!\n";
        }
    } else {
        std::cout << "✗ ERROR: Failed to add transform component!\n";
        return -1;
    }

    // Add mesh renderer with the rainbow shader (which was working before)
    if (AddMeshRendererComponent(testCube, PRIMITIVE_CUBE, "RainbowShader")) {
        std::cout << "✓ MeshRenderer component added successfully\n";
        
        // Set a bright red color to make it visible
        if (SetMeshRendererColor(testCube, 1.0f, 0.0f, 0.0f, 1.0f)) {
            std::cout << "✓ Cube color set to red\n";
        }
    } else {
        std::cout << "✗ ERROR: Failed to add mesh renderer component!\n";
        return -1;
    }

    // Check total entity count
    std::cout << "Total entities in world: " << GetEntityCount() << std::endl;

    // Create a simple player entity WITHOUT the enhanced script for now
    uint32_t player = CreateEntity();
    std::cout << "Created player entity: " << player << std::endl;
    
    // Add basic transform and velocity (the old working setup)
    if (AddTransformComponent(player, 0.0f, 2.0f, -5.0f)) { // Position camera behind the cube
        std::cout << "✓ Player transform added\n";
    }
    
    if (AddVelocityComponent(player, 0.0f, 0.0f, 0.0f)) { // No initial movement
        std::cout << "✓ Player velocity added\n";
    }

    std::cout << "\n=== Debug Information ===\n";
    std::cout << "If you see a red cube, basic rendering is working\n";
    std::cout << "If you see black screen, there's a rendering pipeline issue\n";
    std::cout << "Close window to exit\n";

    std::cout << "\n=== Starting Engine ===\n";
    
    // Start the engine
    RunEngine();

    // Cleanup
    std::cout << "\n=== Cleanup ===\n";
    DestroyGlobalEngine();
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "====================================\n";
    std::cout << "      SIMPLE TEST COMPLETED!       \n";
    std::cout << "====================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
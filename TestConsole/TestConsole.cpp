// TestConsole/TestConsole.cpp - Updated for First-Person Camera
#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "PlayerScript.hpp"
#include "SecondScript.hpp"
#include "PhysicsTestScript.hpp"

#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "=== FIRST-PERSON CAMERA TEST ===\n";

    // Create and initialize engine (singleton)
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    // Print engine info
    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Creating First-Person Scene ===\n";

    // Create reference objects to see movement relative to
    uint32_t centerCube = CreateEntity();
    AddTransformComponent(centerCube, 0.0f, 0.0f, 0.0f);
    AddMeshRendererComponent(centerCube, PRIMITIVE_CUBE, "RainbowShader");
    SetMeshRendererColor(centerCube, 1.0f, 0.0f, 0.0f, 1.0f); // Red cube at origin
    AddCustomScript(centerCube, new SecondScript());
    std::cout << "Created reference cube (RED) at origin\n";

    // Create some landmark objects for spatial reference
    for (int i = 0; i < 8; i++) {
        uint32_t landmark = CreateEntity();
        float angle = (i / 8.0f) * 2.0f * 3.14159f; // Distribute in circle
        float radius = 10.0f;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);
        
        AddTransformComponent(landmark, x, 0.0f, z);
        AddMeshRendererComponent(landmark, PRIMITIVE_CUBE, "blue");
        
        // Color based on position
        float r = (x + radius) / (2.0f * radius);
        float g = 0.5f;
        float b = (z + radius) / (2.0f * radius);
        SetMeshRendererColor(landmark, r, g, b, 1.0f);
        
        AddCustomScript(landmark, new SecondScript());
        std::cout << "Created landmark " << i << " at (" << x << ", 0, " << z << ")\n";
    }

    // Create some vertical pillars for height reference
    for (int i = 0; i < 4; i++) {
        uint32_t pillar = CreateEntity();
        float x = (i - 1.5f) * 8.0f; // Spread them out
        float y = 3.0f; // Make them tall
        
        AddTransformComponent(pillar, x, y, -15.0f); // Put them in front
        AddMeshRendererComponent(pillar, PRIMITIVE_CUBE, "default");
        SetMeshRendererColor(pillar, 0.8f, 0.8f, 0.2f, 1.0f); // Yellow pillars
        SetTransformScale(pillar, 1.0f, 6.0f, 1.0f); // Make them tall
        
        AddCustomScript(pillar, new SecondScript());
        std::cout << "Created pillar " << i << " at (" << x << ", " << y << ", -15)\n";
    }

    // Create a ground plane for reference
    uint32_t ground = CreateEntity();
    AddTransformComponent(ground, 0.0f, -2.0f, 0.0f);
    AddMeshRendererComponent(ground, PRIMITIVE_PLANE, "default");
    SetMeshRendererColor(ground, 0.3f, 0.7f, 0.3f, 1.0f); // Green ground
    SetTransformScale(ground, 50.0f, 1.0f, 50.0f); // Large ground plane
    AddCustomScript(ground, new SecondScript());
    std::cout << "Created ground plane\n";

    // Create the FIRST-PERSON PLAYER entity
    uint32_t player = CreateEntity();
    std::cout << "Created first-person player entity: " << player << std::endl;

    // Add the updated PlayerScript with first-person camera
    PlayerScript* playerScript = new PlayerScript("FirstPersonPlayer");
    if (AddCustomScript(player, playerScript)) {
        std::cout << "✓ First-Person PlayerScript added successfully\n";

        // Configure player settings
        playerScript->SetMovementSpeed(8.0f); // Faster movement for better testing
        playerScript->SetMouseSensitivity(0.003f); // This will be noted but handled by CameraSystem
        
        std::cout << "✓ Player configured with enhanced movement speed\n";
    } else {
        std::cout << "✗ ERROR: Failed to add PlayerScript\n";
        return -1;
    }

    // Create some random objects around the scene for immersion
    std::cout << "\nGenerating environment objects...\n";
    for (int i = 0; i < 20; i++) {
        uint32_t randomCube = CreateEntity();
        
        // Random position in a larger area
        float x = static_cast<float>((rand() % 40) - 20); // -20 to 20
        float y = static_cast<float>(rand() % 5); // 0 to 5
        float z = static_cast<float>((rand() % 40) - 20); // -20 to 20
        
        // Don't place objects too close to spawn point
        if (abs(x) < 3.0f && abs(z) < 3.0f) {
            continue;
        }
        
        AddTransformComponent(randomCube, x, y, z);
        
        // Random size
        float scale = 0.5f + static_cast<float>(rand() % 3);
        SetTransformUniformScale(randomCube, scale);
        
        // Random color and shader
        const char* shaders[] = {"default", "blue", "RainbowShader"};
        const char* selectedShader = shaders[rand() % 3];
        
        AddMeshRendererComponent(randomCube, PRIMITIVE_CUBE, selectedShader);
        
        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;
        SetMeshRendererColor(randomCube, r, g, b, 1.0f);
        
        AddCustomScript(randomCube, new SecondScript(player));
    }

    // Configure physics
    SetPhysicsGridCellSize(3.0f);
    SetPhysicsDebugOutput(false); // Disable for cleaner output

    std::cout << "\n=== FIRST-PERSON CONTROLS ===\n";
    std::cout << "Z/S      - Move forward/backward\n";
    std::cout << "Q/D      - Strafe left/right\n";
    std::cout << "Mouse    - Look around (first-person view)\n";
    std::cout << "Space    - Move up\n";
    std::cout << "Shift    - Move down\n";
    std::cout << "P        - Print player/camera status\n";
    std::cout << "C        - Print camera info\n";
    std::cout << "V        - Toggle player visibility (for debugging)\n";
    std::cout << "ESC      - Close window\n";

    std::cout << "\n=== Expected Behavior ===\n";
    std::cout << "- Camera should be attached to player entity\n";
    std::cout << "- Mouse movement rotates your view (first-person)\n";
    std::cout << "- WASD movement is relative to where you're looking\n";
    std::cout << "- You should see the environment from player's perspective\n";
    std::cout << "- Player cube might be visible in peripheral vision\n";

    std::cout << "\nTotal entities: " << GetEntityCount() << "\n";
    std::cout << "\n=== Starting First-Person Engine ===\n";

    // Start the engine
    RunEngine();

    // Cleanup
    std::cout << "\n=== Cleanup ===\n";
    DestroyGlobalEngine();
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "=====================================\n";
    std::cout << "   FIRST-PERSON TEST COMPLETED!     \n";
    std::cout << "=====================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
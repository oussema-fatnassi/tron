#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"

// Link the library
#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "=== CAMERA POSITION FIX TEST ===\n";
    
    // Create and initialize engine (singleton)
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    // Print engine info
    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Creating Test Cube IN FRONT of Camera ===\n";

    // Create cube positioned where camera can see it
    uint32_t testCube = CreateEntity();
    
    // Position cube in front of default camera (assuming camera is at origin looking down -Z axis)
    // Put cube at Z = -5 (in front of camera), slightly offset
    if (AddTransformComponent(testCube, 0.0f, 0.0f, -5.0f)) {
        std::cout << "✓ Transform: Cube positioned at (0, 0, -5) - should be visible\n";
        
        // Scale the cube up to make it more visible
        if (SetTransformUniformScale(testCube, 2.0f)) {
            std::cout << "✓ Cube scaled to 2x size\n";
        }
    }

    // Add mesh renderer with bright color
    if (AddMeshRendererComponent(testCube, PRIMITIVE_CUBE, "RainbowShader")) {
        std::cout << "✓ MeshRenderer added with RainbowShader\n";
        SetMeshRendererColor(testCube, 1.0f, 0.0f, 0.0f, 1.0f); // Bright red
    }

    // Create a second cube to the right
    uint32_t testCube2 = CreateEntity();
    if (AddTransformComponent(testCube2, 3.0f, 0.0f, -5.0f)) {
        std::cout << "✓ Second cube at (3, 0, -5)\n";
    }
    if (AddMeshRendererComponent(testCube2, PRIMITIVE_CUBE, "blue")) {
        SetMeshRendererColor(testCube2, 0.0f, 0.0f, 1.0f, 1.0f); // Blue
    }

    // Create a third cube to the left
    uint32_t testCube3 = CreateEntity();
    if (AddTransformComponent(testCube3, -3.0f, 0.0f, -5.0f)) {
        std::cout << "✓ Third cube at (-3, 0, -5)\n";
    }
    if (AddMeshRendererComponent(testCube3, PRIMITIVE_SPHERE, "default")) {
        SetMeshRendererColor(testCube3, 0.0f, 1.0f, 0.0f, 1.0f); // Green sphere
    }

    std::cout << "\n=== Camera Theory ===\n";
    std::cout << "Default camera should be at (0,0,0) looking down -Z axis\n";
    std::cout << "Cubes are positioned at Z=-5, so they should be visible\n";
    std::cout << "If still black screen, it's a view/projection matrix issue\n";

    std::cout << "\n=== Starting Engine ===\n";
    
    // Start the engine
    RunEngine();

    // Cleanup
    DestroyGlobalEngine();
    return 0;
}
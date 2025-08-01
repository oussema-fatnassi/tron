// REPLACE YOUR TestConsole/TestConsole.cpp MAIN FUNCTION WITH THIS:

#include <iostream>
#include <windows.h>
#include "TronEngine.hpp"
#include "PlayerScript.hpp"
#include "PhysicsTestScript.hpp"  // Include our new physics test script

#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "=== TRON ENGINE PHYSICS TEST ===\n";

    // Create and initialize engine
    if (!CreateAndInitializeEngine()) {
        std::cout << "Failed to create and initialize engine\n";
        return -1;
    }

    PrintEngineVersion();
    std::cout << GetEngineInfo() << std::endl;

    std::cout << "\n=== Setting up Physics Test Scene ===\n";

    // Enable physics debug output
    SetPhysicsDebugOutput(true);

    // Create a stationary trigger zone at the center
    uint32_t triggerZone = CreateEntity();
    PhysicsTestScript* triggerScript = new PhysicsTestScript("TriggerZone", true);
    AddCustomScript(triggerZone, triggerScript);
    SetTransformPosition(triggerZone, 0.0f, 0.0f, 0.0f); // Center position
    std::cout << "Created trigger zone entity: " << triggerZone << " at center (0,0,0)\n";

    // Create a moving object that will trigger events
    uint32_t movingObject = CreateEntity();
    PhysicsTestScript* movingScript = new PhysicsTestScript("MovingObject", false);
    AddCustomScript(movingObject, movingScript);
    std::cout << "Created moving object entity: " << movingObject << "\n";

    // Run the engine
    RunEngine();

    // Cleanup
    std::cout << "\n=== Cleanup ===\n";
    DestroyGlobalEngine();
    std::cout << "Engine cleanup: SUCCESS\n";

    std::cout << "\n========================================\n";
    std::cout << "   PHYSICS TEST COMPLETED!           \n";
    std::cout << "========================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
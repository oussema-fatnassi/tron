#include <iostream>
#include <windows.h>

#include "TronEngine.hpp"

// Link the library
#pragma comment(lib, "TronEngine.lib")

int main() {
    std::cout << "====================================\n";
    std::cout << "    TRON ENGINE DLL TEST SUITE     \n";
    std::cout << "====================================\n\n";

    // Test 1: Version info
    std::cout << "--- Test 1: Engine Information ---\n";
    PrintEngineVersion();
    std::cout << GetEngineInfo() << "\n\n";

    // Test 2: Engine creation (using void* to avoid type issues)
    std::cout << "--- Test 2: Engine Creation ---\n";
    auto engine = CreateEngine();
    if (!engine) {
        std::cout << "Failed: Could not create engine!\n";
        return -1;
    }
    std::cout << "Engine created successfully\n\n";

    // Test 3: Full lifecycle
    std::cout << "--- Test 3: Engine Lifecycle ---\n";

    if (InitializeEngine(engine)) {
        std::cout << "Engine initialization: SUCCESS\n";

        std::cout << "\nRunning engine...\n";
        RunEngine(engine);


        std::cout << "\nShutting down engine...\n";
        ShutdownEngine(engine);
        std::cout << "Engine shutdown: SUCCESS\n";
    }
    else {
        std::cout << "FAILED: Engine initialization failed!\n";
    }

    // Cleanup
    DestroyEngine(engine);
    std::cout << "Engine cleanup: SUCCESS\n\n";

    std::cout << "====================================\n";
    std::cout << "      ALL TESTS COMPLETED!         \n";
    std::cout << "====================================\n";
    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    return 0;
}
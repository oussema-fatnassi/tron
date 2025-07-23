//#define ENGINE_EXPORTS
#include "../Headers/Core/Engine.hpp"
#include "../../Include/TronEngine.hpp"
#include <iostream>

// C-style API implementation
extern "C" {
    ENGINE_API Engine* CreateEngine() {
        return new Engine();
    }

    ENGINE_API void DestroyEngine(Engine* engine) {
        delete engine;
    }

    ENGINE_API bool InitializeEngine(Engine* engine) {
        if (!engine) return false;
        return engine->Initialize();
    }

    ENGINE_API void RunEngine(Engine* engine) {
        if (!engine) return;
        engine->Run();
    }

    ENGINE_API void ShutdownEngine(Engine* engine) {
        if (!engine) return;
        engine->Shutdown();
    }

    ENGINE_API void PrintEngineVersion() {
        std::cout << "TRON Engine Version: " << TRON_ENGINE_VERSION_STRING << std::endl;
    }

    ENGINE_API const char* GetEngineInfo() {
        return "TRON 3D Game Engine - Professional DLL Version";
    }
}
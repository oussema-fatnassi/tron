#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

// Forward declaration only
class Engine;

// C-style API only
extern "C" {
    ENGINE_API Engine* CreateEngine();
    ENGINE_API void DestroyEngine(Engine* engine);
    ENGINE_API bool InitializeEngine(Engine* engine);
    ENGINE_API void RunEngine(Engine* engine);
    ENGINE_API void ShutdownEngine(Engine* engine);

    // Test functions
    ENGINE_API void PrintEngineVersion();
    ENGINE_API const char* GetEngineInfo();
}

// Note: The Engine class definition is in Engine.h (private implementation)
// Clients only need the C-style API above
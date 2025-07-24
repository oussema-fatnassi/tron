#pragma once

#include <stdint.h>

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

// Forward declaration only
class Engine;

// C-style API only
extern "C" {
    // Engine management
    ENGINE_API Engine* CreateEngine();
    ENGINE_API void DestroyEngine(Engine* engine);
    ENGINE_API bool InitializeEngine(Engine* engine);
    ENGINE_API void RunEngine(Engine* engine);
    ENGINE_API void ShutdownEngine(Engine* engine);

    // Test functions
    ENGINE_API void PrintEngineVersion();
    ENGINE_API const char* GetEngineInfo();

    // ECS Interface - Entity management (using Engine prefix to avoid conflicts)
    ENGINE_API uint32_t EngineCreateEntity(Engine* engine);
    ENGINE_API void EngineDestroyEntity(Engine* engine, uint32_t entity);
    ENGINE_API bool EngineIsValidEntity(Engine* engine, uint32_t entity);

    // ECS Interface - Component management
    ENGINE_API bool EngineAddTransformComponent(Engine* engine, uint32_t entity, float x, float y, float z);
    ENGINE_API bool EngineAddVelocityComponent(Engine* engine, uint32_t entity, float vx, float vy, float vz);

    ENGINE_API bool EngineGetTransformComponent(Engine* engine, uint32_t entity, float* x, float* y, float* z);
    ENGINE_API bool EngineGetVelocityComponent(Engine* engine, uint32_t entity, float* vx, float* vy, float* vz);

    ENGINE_API void EngineRemoveTransformComponent(Engine* engine, uint32_t entity);
    ENGINE_API void EngineRemoveVelocityComponent(Engine* engine, uint32_t entity);

    // ECS Interface - World management
    ENGINE_API void EngineUpdateWorld(Engine* engine, float deltaTime);
    ENGINE_API uint32_t EngineGetEntityCount(Engine* engine);
}

// Note: The Engine class definition is in Engine.h (private implementation)
// Clients only need the C-style API above
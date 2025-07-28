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
    // Singleton Engine Management
    ENGINE_API bool CreateAndInitializeEngine();
    ENGINE_API void RunEngine();
    ENGINE_API void DestroyGlobalEngine();
	ENGINE_API void QuitGame();

    // Test functions
    ENGINE_API void PrintEngineVersion();
    ENGINE_API const char* GetEngineInfo();

    // Input API
	ENGINE_API bool IsKeyDown(int keyCode);
	ENGINE_API bool IsKeyPressed(int keyCode);
	ENGINE_API bool IsKeyUp(int keyCode);


    // Direct ECS API
    // ECS Interface - Entity management
    ENGINE_API uint32_t CreateEntity();
    ENGINE_API void DestroyEntity(uint32_t entity);
    ENGINE_API bool IsValidEntity(uint32_t entity);
	ENGINE_API uint32_t GetEntityCount();

    // ECS Interface - Component management
    ENGINE_API bool AddTransformComponent(uint32_t entity, float x, float y, float z);
    ENGINE_API bool AddVelocityComponent(uint32_t entity, float vx, float vy, float vz);
    ENGINE_API bool AddCustomScript(uint32_t entity, void* scriptInstance);

    ENGINE_API bool GetTransformComponent(uint32_t entity, float* x, float* y, float* z);
    ENGINE_API bool GetVelocityComponent(uint32_t entity, float* vx, float* vy, float* vz);

    ENGINE_API void RemoveTransformComponent(uint32_t entity);
    ENGINE_API void RemoveVelocityComponent(uint32_t entity);
    ENGINE_API bool RemoveScript(uint32_t entity);

    // ECS Interface - World management
    ENGINE_API uint32_t GetEntityCount();
}

// Note: The Engine class definition is in Engine.h (private implementation)
// Clients only need the C-style API above
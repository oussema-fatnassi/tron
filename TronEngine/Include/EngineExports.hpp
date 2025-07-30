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
    ENGINE_API void RequestEngineShutdown();

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

    // ECS Interface - Component management
    ENGINE_API bool AddTransformComponent(uint32_t entity, float x, float y, float z);
    ENGINE_API bool AddVelocityComponent(uint32_t entity, float vx, float vy, float vz);
    ENGINE_API bool AddCustomScript(uint32_t entity, void* scriptInstance);

    ENGINE_API bool GetTransformComponent(uint32_t entity, float* x, float* y, float* z);
    ENGINE_API bool GetVelocityComponent(uint32_t entity, float* vx, float* vy, float* vz);

    ENGINE_API void RemoveTransformComponent(uint32_t entity);
    ENGINE_API void RemoveVelocityComponent(uint32_t entity);

    // ECS Interface - World management
    ENGINE_API uint32_t GetEntityCount();

    // MeshRenderer Component API
    ENGINE_API bool AddMeshRendererComponent(uint32_t entity, int primitiveType, const char* shaderName);
    ENGINE_API bool AddCustomMeshRendererComponent(uint32_t entity, const char* customMeshName, const char* shaderName);
    ENGINE_API bool SetMeshRendererColor(uint32_t entity, float r, float g, float b, float a);
    ENGINE_API bool SetMeshRendererVisible(uint32_t entity, bool visible);
    ENGINE_API bool SetMeshRendererShader(uint32_t entity, const char* shaderName);
    ENGINE_API bool SetMeshRendererMaterial(uint32_t entity, const char* materialName);
    ENGINE_API void RemoveMeshRendererComponent(uint32_t entity);

    // Primitive mesh type constants for the API
    #define PRIMITIVE_QUAD 0
    #define PRIMITIVE_CUBE 1
    #define PRIMITIVE_SPHERE 2
    #define PRIMITIVE_CYLINDER 3
    #define PRIMITIVE_PLANE 4
    #define PRIMITIVE_TRIANGLE 5
    #define PRIMITIVE_CUSTOM 6
}

// Note: The Engine class definition is in Engine.h (private implementation)
// Clients only need the C-style API above
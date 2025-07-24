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
        return "TRON 3D Game Engine - Professional DLL Version with ECS";
    }

    // ECS C-style API with Engine prefix
    ENGINE_API unsigned int EngineCreateEntity(Engine* engine) {
        if (!engine) return 0;
        return engine->CreateEntityC();
    }

    ENGINE_API void EngineDestroyEntity(Engine* engine, unsigned int entity) {
        if (!engine) return;
        engine->DestroyEntityC(entity);
    }

    ENGINE_API bool EngineIsValidEntity(Engine* engine, unsigned int entity) {
        if (!engine) return false;
        return engine->IsValidEntityC(entity);
    }

    ENGINE_API bool EngineAddTransformComponent(Engine* engine, unsigned int entity, float x, float y, float z) {
        if (!engine) return false;
        Transform* component = engine->AddTransformComponentC(entity, x, y, z);
        return component != nullptr;
    }

    ENGINE_API bool EngineAddVelocityComponent(Engine* engine, unsigned int entity, float vx, float vy, float vz) {
        if (!engine) return false;
        Velocity* component = engine->AddVelocityComponentC(entity, vx, vy, vz);
        return component != nullptr;
    }

    ENGINE_API bool EngineGetTransformComponent(Engine* engine, unsigned int entity, float* x, float* y, float* z) {
        if (!engine || !x || !y || !z) return false;
        Transform* component = engine->GetTransformComponentC(entity);
        if (component) {
            *x = component->x;
            *y = component->y;
            *z = component->z;
            return true;
        }
        return false;
    }

    ENGINE_API bool EngineGetVelocityComponent(Engine* engine, unsigned int entity, float* vx, float* vy, float* vz) {
        if (!engine || !vx || !vy || !vz) return false;
        Velocity* component = engine->GetVelocityComponentC(entity);
        if (component) {
            *vx = component->vx;
            *vy = component->vy;
            *vz = component->vz;
            return true;
        }
        return false;
    }

    ENGINE_API void EngineRemoveTransformComponent(Engine* engine, unsigned int entity) {
        if (!engine) return;
        engine->RemoveTransformComponentC(entity);
    }

    ENGINE_API void EngineRemoveVelocityComponent(Engine* engine, unsigned int entity) {
        if (!engine) return;
        engine->RemoveVelocityComponentC(entity);
    }

    ENGINE_API void EngineUpdateWorld(Engine* engine, float deltaTime) {
        if (!engine) return;
        engine->UpdateWorld(deltaTime);
    }

    ENGINE_API unsigned int EngineGetEntityCount(Engine* engine) {
        if (!engine) return 0;
        return engine->GetEntityCountC();
    }
}
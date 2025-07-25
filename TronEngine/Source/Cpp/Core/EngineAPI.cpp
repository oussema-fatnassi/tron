//#define ENGINE_EXPORTS 
#include "../Headers/Core/Engine.hpp" 
#include "../../Include/TronEngine.hpp" 
#include <iostream>

// Global singleton instance
static Engine* g_engineInstance = nullptr;

// C-style API implementation
extern "C" {
    ENGINE_API bool CreateAndInitializeEngine() {
        if (g_engineInstance) {
            std::cout << "[TronEngine] Engine already exists\n";
            return true;
        }

        g_engineInstance = new Engine();
        if (!g_engineInstance->Initialize()) {
            delete g_engineInstance;
            g_engineInstance = nullptr;
            return false;
        }
        return true;
    }

    ENGINE_API void RunEngine() {
        if (!g_engineInstance) {
            std::cout << "[TronEngine] Error: No engine instance\n";
            return;
        }
        g_engineInstance->Run();
    }

    ENGINE_API void DestroyGlobalEngine() {
        if (g_engineInstance) {
            g_engineInstance->Shutdown();
            delete g_engineInstance;
            g_engineInstance = nullptr;
        }
    }

    ENGINE_API void PrintEngineVersion() {
        std::cout << "TRON Engine Version: " << TRON_ENGINE_VERSION_STRING << std::endl;
    }

    ENGINE_API const char* GetEngineInfo() {
        return "TRON 3D Game Engine - Professional DLL Version with ECS";
    }

    // ECS C-style API with Engine prefix
    // Direct World Access API
    ENGINE_API uint32_t CreateEntity() {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return 0;
        return g_engineInstance->GetWorld()->CreateEntity();
    }

    ENGINE_API void DestroyEntity(uint32_t entity) {
        if (g_engineInstance && g_engineInstance->GetWorld()) {
            g_engineInstance->GetWorld()->DestroyEntity(entity);
        }
    }

    ENGINE_API bool IsValidEntity(uint32_t entity) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        return g_engineInstance->GetWorld()->IsValidEntity(entity);
    }

    ENGINE_API bool AddTransformComponent(uint32_t entity, float x, float y, float z) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        Transform* component = g_engineInstance->GetWorld()->AddComponent<Transform>(entity, x, y, z);
        return component != nullptr;
    }

    ENGINE_API bool AddVelocityComponent(uint32_t entity, float vx, float vy, float vz) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        Velocity* component = g_engineInstance->GetWorld()->AddComponent<Velocity>(entity, vx, vy, vz);
        return component != nullptr;
    }

    ENGINE_API bool AddCustomScript(uint32_t entity, void* scriptInstance) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !scriptInstance) return false;

        // Cast the void* to ScriptBase* (user's script)
        ScriptBase* userScript = static_cast<ScriptBase*>(scriptInstance);

        // Create internal Script component
        Script* component = g_engineInstance->GetWorld()->AddComponent<Script>(entity);

        // Store pointer to user's script (don't copy by value!)
        component->userScript = userScript;

        // Now we can set entityId on ScriptBase - no casting needed!
        userScript->entity = entity;

        return component != nullptr;
    }

    ENGINE_API bool GetTransformComponent(uint32_t entity, float* x, float* y, float* z) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !x || !y || !z) return false;

        Transform* component = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (component) {
            *x = component->x;
            *y = component->y;
            *z = component->z;
            return true;
        }
        return false;
    }

    ENGINE_API bool GetVelocityComponent(uint32_t entity, float* vx, float* vy, float* vz) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !vx || !vy || !vz) return false;

        Velocity* component = g_engineInstance->GetWorld()->GetComponent<Velocity>(entity);
        if (component) {
            *vx = component->vx;
            *vy = component->vy;
            *vz = component->vz;
            return true;
        }
        return false;
    }

    ENGINE_API void RemoveTransformComponent(uint32_t entity) {
        if (g_engineInstance && g_engineInstance->GetWorld()) {
            g_engineInstance->GetWorld()->RemoveComponent<Transform>(entity);
        }
    }

    ENGINE_API void RemoveVelocityComponent(uint32_t entity) {
        if (g_engineInstance && g_engineInstance->GetWorld()) {
            g_engineInstance->GetWorld()->RemoveComponent<Velocity>(entity);
        }
    }

    ENGINE_API uint32_t GetEntityCount() {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return 0;
        return g_engineInstance->GetWorld()->GetEntityCount();
    }

    //TODO ADD UPDATE COMPONENTS
}
#pragma once
#include "../../Include/EngineExports.hpp" 
#include <memory> 
#include <string> 
#include "../../../../ALL_ECS_SYSTEM/include/World.hpp" 
#include "../../../../ALL_ECS_SYSTEM/include/TransformComponent.hpp" 
#include "../../../../ALL_ECS_SYSTEM/include/VelocityComponent.hpp"
#include "../../../../ALL_ECS_SYSTEM/include/MovementSystem.hpp"
#include "../../../../ALL_ECS_SYSTEM/include/DebugSystem.hpp"

// Forward declarations for future subsystems
// class RenderEngine;

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize();
    void Run();
    void Shutdown();

    // Public interface
    void PrintMessage(const char* message);
    const char* GetVersion() const;

    // ECS Interface - internal methods use Tron::Entity
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    bool IsValidEntity(Entity entity) const;

    // Component operations
    Transform* AddTransformComponent(Entity entity, float x = 0, float y = 0, float z = 0);
    Velocity* AddVelocityComponent(Entity entity, float vx = 0, float vy = 0, float vz = 0);

    Transform* GetTransformComponent(Entity entity);
    Velocity* GetVelocityComponent(Entity entity);

    void RemoveTransformComponent(Entity entity);
    void RemoveVelocityComponent(Entity entity);

    // World operations
    void UpdateWorld(float deltaTime);
    uint32_t GetEntityCount() const;
    std::vector<Entity> GetAllEntities() const;

    // C API wrapper methods (these handle type conversion)
    unsigned int CreateEntityC();
    void DestroyEntityC(unsigned int entity);
    bool IsValidEntityC(unsigned int entity) const;
    Transform* AddTransformComponentC(unsigned int entity, float x, float y, float z);
    Velocity* AddVelocityComponentC(unsigned int entity, float vx, float vy, float vz);
    Transform* GetTransformComponentC(unsigned int entity);
    Velocity* GetVelocityComponentC(unsigned int entity);
    void RemoveTransformComponentC(unsigned int entity);
    void RemoveVelocityComponentC(unsigned int entity);
    unsigned int GetEntityCountC() const;

    // Subsystem access (will be implemented later)
    // RenderEngine* GetRenderEngine() const { return m_renderEngine.get(); }
    World* GetWorld() const;

private:
    bool m_initialized;
    bool m_running;
    std::string m_version;

    // ECS World
    std::unique_ptr<World> m_world;

    // Subsystems (commented out until implemented)
    // std::unique_ptr<RenderEngine> m_renderEngine;

    bool InitializeSubsystems();
    void ShutdownSubsystems();
};
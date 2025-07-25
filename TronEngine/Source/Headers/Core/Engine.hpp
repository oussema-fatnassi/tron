#pragma once
#include "../../Include/EngineExports.hpp"
#include <memory>
#include <string>
#include <thread>
#include <atomic>

#include "../Game/World.hpp" 
#include "../Game/TransformComponent.hpp" 
#include "../Game/VelocityComponent.hpp"
#include "../Game/MovementSystem.hpp"
#include "../Game/DebugSystem.hpp"

#include "../Rendering/RenderEngine.hpp"

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

    // Subsystem access (will be implemented later)
    // RenderEngine* GetRenderEngine() const { return m_renderEngine.get(); }
    World* GetWorld() const;

private:
    bool m_initialized;
    std::atomic<bool> m_running{ false };
    std::string m_version;

    // Threading infrastructure (YOUR WORK)
    std::unique_ptr<std::thread> m_gameThread;

    // Subsystems (commented out until implemented)
    // std::unique_ptr<RenderEngine> m_renderEngine;
    // ECS World
    std::unique_ptr<World> m_world;
    std::unique_ptr<RenderEngine> m_renderEngine;

    void RenderLoop();
    void GameLoop();

    bool InitializeSubsystems();
    void ShutdownSubsystems();

    // Frame timing
    // TODO : Make the user can set this and add the realDeltaTime not only the target
    float m_deltaTime = 0.016f;  // Target 60 FPS
    int m_frameCount = 0;
};
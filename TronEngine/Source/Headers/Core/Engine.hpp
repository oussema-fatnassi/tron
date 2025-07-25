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

    // Subsystem access
    World* GetWorld() const;
    // RenderEngine* GetRenderEngine() const { return m_renderEngine.get(); }

private:
    bool _initialized;
    std::atomic<bool> _running{ false };
    std::string _version;

    // Threading infrastructure (YOUR WORK)
    std::unique_ptr<std::thread> _gameThread;

    // Subsystems (commented out until implemented)
    // std::unique_ptr<RenderEngine> m_renderEngine;
    // ECS World
    std::unique_ptr<World> _world;
    std::unique_ptr<RenderEngine> _renderEngine;

    void RenderLoop();
    void GameLoop();

    bool InitializeSubsystems();
    void ShutdownSubsystems();

    // Frame timing
    // TODO : Make the user can set this and add the realDeltaTime not only the target
    float _deltaTime = 0.016f;  // Target 60 FPS
    int _frameCount = 0;
};
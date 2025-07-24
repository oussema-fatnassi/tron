#pragma once
#include "../../Include/EngineExports.hpp"
#include <memory>
#include <string>
#include <thread>
#include <atomic>

// Forward declarations for future subsystems (commented out for now)
// class RenderEngine;
// class GameEngine;

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
    // GameEngine* GetGameEngine() const { return m_gameEngine.get(); }

private:
    bool m_initialized;
    std::atomic<bool> m_running{ false };
    std::string m_version;

    // Threading infrastructure (YOUR WORK)
    std::unique_ptr<std::thread> m_gameThread;

    // Subsystems (commented out until implemented)
    // std::unique_ptr<RenderEngine> m_renderEngine;
    // std::unique_ptr<GameEngine> m_gameEngine;

    void RenderLoop();
    void GameLoop();

    bool InitializeSubsystems();
    void ShutdownSubsystems();

    // Frame timing
    // TODO : Make the user can set this and add the realDeltaTime not only the target
    float m_deltaTime = 0.016f;  // Target 60 FPS
    int m_frameCount = 0;
};
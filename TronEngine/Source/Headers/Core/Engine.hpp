#pragma once
#include "../../Include/EngineExports.hpp"
#include <memory>
#include <string>

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
    bool m_running;
    std::string m_version;

    // Subsystems (commented out until implemented)
    // std::unique_ptr<RenderEngine> m_renderEngine;
    // std::unique_ptr<GameEngine> m_gameEngine;

    bool InitializeSubsystems();
    void ShutdownSubsystems();
};
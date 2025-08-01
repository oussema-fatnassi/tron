#pragma once
#include "../../Include/EngineExports.hpp"
#include <memory>
#include <string>
#include <thread>
#include <atomic>

// TronEngine - Core imports
#include "InputManager.hpp"

// TronEngine - Game imports
#include "../Game/World.hpp"
    //Systems
#include "../Game/MovementSystem.hpp"
#include "../Game/DebugSystem.hpp"
#include "../Game/ScriptSystem.hpp"
#include "../Game/MeshRenderSystem.hpp"
#include "../Game/CameraSystem.hpp"
#include "../Game/PhysicsSystem.hpp"
    // Components
#include "../Game/VelocityComponent.hpp"
#include "../Game/TransformComponent.hpp"
#include "../Game/ScriptComponent.hpp"
#include "../Game/MeshRendererComponent.hpp"
#include "../Game/BoxColliderComponent.hpp"

// TronEngine - Rendering imports
#include "../Rendering/RenderEngine.hpp"
#include "../Rendering/PrimitiveMeshGenerator.hpp"
#include "../Communication/RenderCommand.hpp"
#include "../Rendering/RenderExecutor.hpp" 


// Forward declaration for test quad
class FullscreenQuad;

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize();
    void Run();
    void Shutdown();
    void RequestShutdown();

    // Public interface
    void PrintMessage(const char* message);
    const char* GetVersion() const;

    // Subsystem access
    World* GetWorld() const;
	InputManager* GetInputManager() const;

private:
    bool _initialized;
    std::atomic<bool> _running{ false };
    std::string _version;

    // Threading infrastructure
    std::unique_ptr<std::thread> _gameThread;

    // Subsystems
    std::unique_ptr<World> _world;
    std::unique_ptr<RenderEngine> _renderEngine;
    std::unique_ptr<InputManager> _inputManager;
    std::unique_ptr<CommandQueue> _renderCommandQueue;
   
    // Resource managers
    std::unique_ptr<MeshManager> _meshManager;
    std::unique_ptr<MaterialManager> _materialManager;
    std::unique_ptr<RenderExecutor> _renderExecutor;

    // Test rendering (remove when no longer needed for shader testing)
    std::unique_ptr<FullscreenQuad> _testQuad;

    void MainRenderLoop();
    void GameLoop();
    void RenderTestQuad(); // Test method - remove when no longer needed

    bool InitializeSubsystems();
    void ShutdownSubsystems();

};
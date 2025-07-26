#include "../Headers/Core/Engine.hpp" 
#include "../../Include/TronEngine.hpp"
#include "../Headers/Core/WindowUtils.hpp"
#include <iostream>
#include <chrono>

Engine::Engine()
    : _initialized(false)
    , _running(false)
    , _version("1.0.0")
    , _world(nullptr)
	, _renderEngine(nullptr)
{
    std::cout << "[TronEngine] Constructor - Engine object created\n";
}

Engine::~Engine() {
    if (_initialized) {
        Shutdown();
    }
    std::cout << "[TronEngine] Destructor - Engine object destroyed\n";
}

bool Engine::Initialize() {
    if (_initialized) {
        std::cout << "[TronEngine] Warning: Already initialized\n";
        return true;
    }

    std::cout << "[TronEngine] Initializing TRON Engine v" << TRON_ENGINE_VERSION_STRING << "\n";

    // Initialize other subsystems
    if (!InitializeSubsystems()) {
        std::cout << "[TronEngine] Failed to initialize subsystems\n";
        return false;
    }

    _initialized = true;
    std::cout << "[TronEngine] Initialization successful!\n";
    return true;
}

bool Engine::InitializeSubsystems() {
    std::cout << "[TronEngine] Initializing subsystems...\n";

    // Initialize InputManager first
    //_inputManager = std::make_unique<InputManager>();
    //std::cout << "[TronEngine] InputManager initialized\n";

	// Initialize ECS World
    _world = std::make_unique<World>();

	//TODO: Make the registration of components in World a single function call
    _world->RegisterComponent<Transform>();
    _world->RegisterComponent<Velocity>();
	_world->RegisterComponent<Script>();

    // Register systems
    auto* debugSystem = _world->RegisterSystem<DebugSystem>();
    auto* movementSystem = _world->RegisterSystem<MovementSystem>();
	auto* scriptSystem = _world->RegisterSystem<ScriptSystem>();

    // Set system signatures
    _world->SetSystemSignature<Transform>(debugSystem);
    _world->SetSystemSignature<Transform, Velocity>(movementSystem);
	_world->SetSystemSignature<Script>(scriptSystem);

    std::cout << "[TronEngine] ECS World initialized with components registered\n";

    // Render Engine 
 
	// WINDOW INITIALIZATION
    // TODO: Initialize the RenderEngine with a single function in Window and other from the RenderEngine
    int width = 1280;
    int height = 720;

    // Creates Win32 window
	//TODO: LET SOMEWHERE THE USER CHOSE THE WINDOW SIZE
    //TODO: Maybe add a variable to ref the window 
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    HWND hwnd = CreateSimpleWindow(hInstance, width, height, L"TronEngine");

    if (!hwnd) {
        std::cout << "[TronEngine] Failed to create window\n";
        return false;
    }
    ShowWindow(hwnd, SW_SHOW);

	//Direct3D Initialization
    _renderEngine = std::make_unique<RenderEngine>(hwnd, width, height);

    _renderEngine->Initialize();

    // LOGS
    std::cout << "[TronEngine] RenderEngine initialized successfully\n";

    std::cout << "[Threading] Thread infrastructure: Ready\n";
    std::cout << "[Threading] Render Thread target: 60 FPS (16ms per frame)\n";
    std::cout << "[Threading] Game Thread target: 60 FPS (16ms per frame)\n";

    return true;
}
void Engine::Shutdown() {
    if (!_initialized) {
        return;
    }

    std::cout << "[TronEngine] Shutting down...\n";

    _running = false;

    // Stoping Threads
    if (_gameThread && _gameThread->joinable()) {
        std::cout << "[Threading] Waiting for Game Thread to finish...\n";
        _gameThread->join();
        std::cout << "[Threading] Game Thread joined successfully\n";
    }

    ShutdownSubsystems();

    _initialized = false;
    std::cout << "[TronEngine] Shutdown complete\n";
}

void Engine::RequestShutdown() {
    std::cout << "[Engine] Shutdown requested\n";
    _running = false;
}

void Engine::ShutdownSubsystems() {
    std::cout << "[TronEngine] Shutting down subsystems...\n";

    // TODO: Shutdown in reverse order when implemented
    // Shutdown ECS World
    if (_world) {
        _world->Shutdown();
        _world.reset();
        std::cout << "[TronEngine] ECS World shut down\n";
    }

    std::cout << "[TronEngine] All subsystems shut down\n";
}

void Engine::Run() {
    if (!_initialized) {
        std::cout << "[TronEngine] Error: Engine not initialized\n";
        return;
    }

    std::cout << "[TronEngine] Starting main engine loop...\n";
    std::cout << "[TronEngine] Press Ctrl+C to stop (or will auto-stop after demo)\n";

    _running = true;
    _frameCount = 0;

    // Start the Game Thread
    _gameThread = std::make_unique<std::thread>(&Engine::GameLoop, this);
    
    // RenderLoop in the main Thread
    RenderLoop();

    if (_gameThread && _gameThread->joinable()) {
        _gameThread->join();
    }

    std::cout << "[TronEngine] Engine loops stopped\n";
}

void Engine::RenderLoop() {
    std::cout << "[Threading] Main Thread -> Render Loop started\n";

    //while (m_running && m_frameCount < 30) {   // Test: 20 frames
    while (_running) {
        auto frameStart = std::chrono::steady_clock::now();

        // TODO: Call the Rendering method that groups all the thing need do it everyframe
        // TODO: Actual rendering calls here
        // Present with VSync for 60 FPS cap

        // Target 60 FPS (16.67ms per frame)
        auto frameEnd = std::chrono::steady_clock::now();
        auto frameTime = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();

        float targetFrameTime = 16.67f;  // 60 FPS
        float sleepTime = targetFrameTime - frameTime;

        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleepTime)));
        }
    }

    std::cout << "[Threading] Render Thread finished\n";
}

void Engine::GameLoop() {
    std::cout << "[Threading] Game Thread started (120 FPS target)\n";

    const float targetDeltaTime = 1.0f / 120.0f;
    auto previousTime = std::chrono::steady_clock::now();

    // Test prints
    std::cout << "[TronEngine] === Initial Entity States ===\n";
    auto allEntities = _world->GetAllEntities();
   /* for (Entity entity : allEntities)
    {
        auto* transform = _world->GetComponent<Transform>(entity);
        auto* velocity = _world->GetComponent<Velocity>(entity);

        std::cout << "[TronEngine] Entity " << entity;
        if (transform)
        {
            std::cout << " - Pos(" << transform->x << ", " << transform->y << ", " << transform->z << ")";
        }
        if (velocity)
        {
            std::cout << " - Vel(" << velocity->vx << ", " << velocity->vy << ", " << velocity->vz << ")";
        }
        std::cout << "\n";
    }*/
    int gameFrame = 0;
    while (_running) {
        auto currentTime = std::chrono::steady_clock::now();
        float realDeltaTime = std::chrono::duration<float>(currentTime - previousTime).count();

        // Cap delta time to prevent spiral of death
        float clampedDelta = (realDeltaTime < 0.033f) ? realDeltaTime : 0.033f;  // Max 33ms (30 FPS min)

        // Update ECS World - this handles all movement, systems, etc.
        if (_world)
        {
            _world->Update(clampedDelta);
        }

        previousTime = currentTime;
        gameFrame++;

        // Show progress every second TEST
        if (gameFrame % 60 == 0 && gameFrame > 0)
        {
            float elapsedTime = gameFrame * targetDeltaTime;
            std::cout << "[TronEngine] === After " << elapsedTime << " seconds ===\n";

            for (Entity entity : allEntities)
            {
                if (!_world->IsValidEntity(entity))
                    continue; // Skip destroyed entities

                auto* transform = _world->GetComponent<Transform>(entity);
                if (transform)
                {
                    std::cout << "[TronEngine] Entity " << entity
                              << " position: (" << transform->x << ", "
                              << transform->y << ", " << transform->z << ")\n";
                }
            }
        }

        // Target 120 FPS - sleep for remaining time
        auto frameEnd = std::chrono::steady_clock::now();
        auto actualFrameTime = std::chrono::duration<float>(frameEnd - currentTime).count();

        float sleepTime = targetDeltaTime - actualFrameTime;
        if (sleepTime > 0.001f) {  // Only sleep if we have at least 1ms to spare
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
        }

        // Optional: Log frame timing every second for debugging
        /*if (gameFrame % 120 == 0) {
            float fps = 1.0f / realDeltaTime;
             std::cout << "[GameLoop] FPS: " << fps << ", DeltaTime: " << realDeltaTime * 1000 << "ms\n";
        }*/
    }

    _running = false;  // Signal render thread to stop
    std::cout << "[Threading] Game Thread finished\n";
}

void Engine::PrintMessage(const char* message) {
    std::cout << "[TronEngine] " << message << "\n";
}

const char* Engine::GetVersion() const {
    return _version.c_str();
}

// GETTERS 

//InputManager
//InputManager* Engine::GetInputManager() const {
//    return _inputManager.get();
//}
// ECS
World* Engine::GetWorld() const {
    return _world.get();
}
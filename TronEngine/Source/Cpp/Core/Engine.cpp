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

    // TODO: Initialize RenderEngine, GameEngine, ThreadPool
    
	// Initialize ECS World
    _world = std::make_unique<World>();

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

    // Crée la fenêtre Win32
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

        // Placeholder: Simulate render work
        // std::cout << "[Render Thread] Frame " << m_frameCount + 1 << " - Rendering...\n";

        // Simulate rendering time (16ms for 60 FPS) replace with VSync (optional)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        // Frame timing
        auto frameEnd = std::chrono::steady_clock::now();
        float frameTime = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();

        if (_frameCount % 10 == 0) {  // Every 10 frames
            // std::cout << "[Render Thread] Frame time: " << frameTime << "ms (Target: 16ms)\n";
        }

        _frameCount++;
    }

    std::cout << "[Threading] Render Thread finished\n";
}

void Engine::GameLoop() {
    std::cout << "[Threading] Game Thread started\n";

    //TODO: do a proper thing and care because it exist in another place too Ctrl + F.
    const float deltaTime = 0.016f; // ~60 FPS

    // Test prints
    std::cout << "[TronEngine] === Initial Entity States ===\n";
    auto allEntities = _world->GetAllEntities();
    for (Entity entity : allEntities)
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
    }

    int gameFrame = 0;
    while (_running && gameFrame < 3500) { 
        auto frameStart = std::chrono::steady_clock::now();

        // Update ECS World - this handles all movement, systems, etc.
        if (_world)
        {
            _world->Update(deltaTime);
        }

        // Show progress every second TEST
        if (gameFrame % 60 == 0 && gameFrame > 0)
        {
            float elapsedTime = gameFrame * deltaTime;
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

        gameFrame++;

        // Calculate delta time for game systems
        auto frameEnd = std::chrono::steady_clock::now();
        _deltaTime = std::chrono::duration<float>(frameEnd - frameStart).count();

        // Target 60 FPS for game logic (less than render thread) for build we can comment this, for gameplay fluidity
        //std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }

    _running = false;  // Signal render thread to stop
    std::cout << "[Threading] Game Thread finished at " << gameFrame << "\n";
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

void Engine::PrintMessage(const char* message) {
    std::cout << "[TronEngine] " << message << "\n";
}

const char* Engine::GetVersion() const {
    return _version.c_str();
}

// ECS Functions
World* Engine::GetWorld() const {
    return _world.get();
}
#include "../Headers/Core/Engine.hpp" 
#include "../../Include/TronEngine.hpp"
#include "../Headers/Core/WindowUtils.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <intrin.h>

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

    // ADD: Debug the timing constants
    std::cout << "[Debug] TRON_GAME_TARGET_FPS: " << TRON_GAME_TARGET_FPS << "\n";
    std::cout << "[Debug] TRON_RENDER_TARGET_FPS: " << TRON_RENDER_TARGET_FPS << "\n";
    std::cout << "[Debug] TRON_GAME_TARGET_DELTA: " << TRON_GAME_TARGET_DELTA << " ("
        << (TRON_GAME_TARGET_DELTA * 1000) << "ms)\n";
    std::cout << "[Debug] TRON_RENDER_TARGET_DELTA: " << TRON_RENDER_TARGET_DELTA << " ("
        << (TRON_RENDER_TARGET_DELTA * 1000) << "ms)\n";


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
    std::cout << "[Threading] Render Thread target: " << TRON_RENDER_TARGET_FPS << " FPS (" << TRON_RENDER_TARGET_DELTA * 1000 << "ms per frame)\n";
    std::cout << "[Threading] Game Thread target: " << TRON_GAME_TARGET_FPS << " FPS (" << TRON_GAME_TARGET_DELTA * 1000 << "ms per frame)\n";

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

    std::cout << "[TronEngine] Starting engine with main thread rendering...\n";
    std::cout << "[Threading] Game Thread target: " << TRON_GAME_TARGET_FPS << " FPS\n";
    std::cout << "[Threading] Render Thread target: " << TRON_RENDER_TARGET_FPS << " FPS\n";

    _running = true;

    // Start the Game Thread
    _gameThread = std::make_unique<std::thread>(&Engine::GameLoop, this);
    
    // RenderLoop in the main Thread
    MainRenderLoop();

    if (_gameThread && _gameThread->joinable()) {
        _gameThread->join();
    }

    std::cout << "[TronEngine] Engine loops stopped\n";
}

void Engine::MainRenderLoop() {
    std::cout << "[Threading] Main Thread -> Render + Messages at " << TRON_RENDER_TARGET_FPS << " FPS\n";

    int frameCount = 0;
    auto lastFPSTime = std::chrono::steady_clock::now();

    // Use high_resolution_clock for best precision
    using clock = std::chrono::high_resolution_clock;
    const double targetFrameTime = 1.0 / TRON_RENDER_TARGET_FPS;  // 0.01667 for 60 FPS

    while (_running) {
        auto frameStart = clock::now();

        // TODO: Render calls will go here
        // if (_renderEngine) {
        //     _renderEngine->BeginFrame();
        //     _renderEngine->RenderScene();
        //     _renderEngine->Present();
        // }

        frameCount++;

        // FPS counter every second
        auto currentTime = clock::now();
        if (std::chrono::duration<double>(currentTime - lastFPSTime).count() >= 1.0) {
            std::cout << "[MainRender] Rendered " << frameCount << " frames in last second\n";
            frameCount = 0;
            lastFPSTime = currentTime;
        }

        // PURE SPIN-WAIT SOLUTION - No sleep at all!
        // Since Windows sleep is imprecise (15.6ms), we just busy-wait
        while (true) {
            auto now = clock::now();
            double elapsed = std::chrono::duration<double>(now - frameStart).count();

            if (elapsed >= targetFrameTime) {
                break;
            }

            // IMPORTANT: Give CPU a break to prevent 100% usage
            // This is a x86/x64 instruction that hints the CPU we're in a spin loop
#if defined(_MSC_VER)
            _mm_pause();  // Intrinsic available in MSVC
#else
            std::this_thread::yield();  // Fallback
#endif
        }
    }

    std::cout << "[Threading] Main Render Thread finished\n";
}

void Engine::GameLoop() {
    std::cout << "[Threading] Game Thread started - Target: " << TRON_GAME_TARGET_FPS << " FPS\n";

    auto lastTime = std::chrono::steady_clock::now();
    auto lastFPSTime = lastTime;  // For FPS calculation
    double accumulator = 0.0;
    const double maxFrameTime = 0.025; // Max 25ms to prevent spiral of death

    int gameFrameCount = 0;
    int actualUpdates = 0;  // Count actual world updates

    // Get initial entities for debugging
    auto allEntities = _world->GetAllEntities();
    std::cout << "[TronEngine] === Initial Entity States ===\n";

    while (_running) {
        auto currentTime = std::chrono::steady_clock::now();
        double frameTime = std::chrono::duration<double>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Prevent spiral of death
        frameTime = min(frameTime, maxFrameTime);
        accumulator += frameTime;

        // Fixed timestep updates - INDUSTRY STANDARD
        while (accumulator >= TRON_GAME_TARGET_DELTA) {
            if (_world) {
                _world->Update(static_cast<float>(TRON_GAME_TARGET_DELTA));
            }
            accumulator -= TRON_GAME_TARGET_DELTA;
            actualUpdates++;  // Count actual updates
        }

        gameFrameCount++;

        // FIXED: Show FPS based on actual time, not frame count
        auto timeSinceLastFPS = std::chrono::duration<double>(currentTime - lastFPSTime).count();

        if (timeSinceLastFPS >= 1.0) {  // Every 1 second of real time
            double actualGameFPS = actualUpdates / timeSinceLastFPS;
            double loopFPS = gameFrameCount / timeSinceLastFPS;

            std::cout << "[GameThread] Actual Game FPS: " << std::fixed << std::setprecision(1)
                << actualGameFPS << " updates/sec (target: " << TRON_GAME_TARGET_FPS
                << ") | Loop FPS: " << loopFPS << "\n";

            // Show entity positions every second instead of every 120 frames
            for (Entity entity : allEntities) {
                if (!_world->IsValidEntity(entity))
                    continue;

                auto* transform = _world->GetComponent<Transform>(entity);
                if (transform) {
                    std::cout << "[TronEngine] Entity " << entity
                        << " position: (" << std::fixed << std::setprecision(2)
                        << transform->x << ", " << transform->y << ", " << transform->z << ")\n";
                }
            }

            // Reset counters
            gameFrameCount = 0;
            actualUpdates = 0;
            lastFPSTime = currentTime;
        }

        // Precise timing
        auto frameEnd = std::chrono::steady_clock::now();
        auto actualFrameTime = std::chrono::duration<double>(frameEnd - currentTime).count();
        double sleepTime = TRON_GAME_TARGET_DELTA - actualFrameTime;

        if (sleepTime > 0.001) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }

    _running = false;  // Signal main thread to stop
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
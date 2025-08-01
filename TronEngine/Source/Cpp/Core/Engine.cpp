#include "../Headers/Core/Engine.hpp" 
#include "../../Include/TronEngine.hpp"
#include "../Headers/Core/WindowUtils.hpp"
#include "../Headers/Rendering/FullscreenQuad.hpp"
#include "../Headers/Game/MeshRendererComponent.hpp"
#include "../Headers/Game/MeshRenderSystem.hpp"
#include "../Headers/Rendering/PrimitiveMeshGenerator.hpp"
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
    , _testQuad(nullptr)
    , _meshManager(nullptr)
    , _materialManager(nullptr)
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

    // WINDOW INITIALIZATION - Must be done BEFORE render engine and mesh managers
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

    //Direct3D Initialization - Must be done BEFORE mesh managers
    _renderEngine = std::make_unique<RenderEngine>(hwnd, width, height);
    _renderEngine->Initialize();

    // Load default shader
    if (!_renderEngine->LoadShader("default", L"VertexShader.hlsl", L"PixelShader.hlsl")) {
        std::cout << "[TronEngine] Failed to load default shader\n";
    }
    if (!_renderEngine->LoadShader("blue", L"VertexShader.hlsl", L"PixelShaderBlue.hlsl")) {
        std::cout << "[TronEngine] Failed to load blue shader\n";
    }
    if (!_renderEngine->LoadShader("RainbowShader", L"VertexShader.hlsl", L"PixelShaderRainbow.hlsl")) {
        std::cout << "[TronEngine] Failed to load rainbow shader\n";
    }

    // Initialize Mesh and Material Managers - Must be done AFTER RenderEngine
    _meshManager = std::make_unique<MeshManager>();
    _materialManager = std::make_unique<MaterialManager>();

    // === NEW: Clean Architecture Setup ===
    // Initialize command queue for thread communication
    _renderCommandQueue = std::make_unique<CommandQueue>();
    std::cout << "[TronEngine] Command queue initialized for clean thread communication\n";

    _renderExecutor = std::make_unique<RenderExecutor>(
        _renderEngine.get(),
        _meshManager.get(),
        _materialManager.get()
    );
    std::cout << "[TronEngine] RenderExecutor initialized (clean separation, no ECS dependencies)\n";


    // Generate all primitive meshes - Must be done AFTER MeshManager
    if (!PrimitiveMeshGenerator::GenerateAllPrimitives(_renderEngine->GetDevice(), _meshManager.get())) {
        std::cout << "[TronEngine] Warning: Failed to generate some primitive meshes\n";
    }

    // Initialize ECS World - Must be done AFTER all managers are created
    _world = std::make_unique<World>();

    //TODO: Make the registration of components in World a single function call
    _world->RegisterComponent<Transform>();
    _world->RegisterComponent<Velocity>();
    _world->RegisterComponent<Script>();
    _world->RegisterComponent<MeshRenderer>();
    _world->RegisterComponent<BoxCollider>();


    // Register systems - Must be done AFTER all managers are created
    auto* debugSystem = _world->RegisterSystem<DebugSystem>();
    auto* movementSystem = _world->RegisterSystem<MovementSystem>();
    auto* scriptSystem = _world->RegisterSystem<ScriptSystem>();
    auto* meshRenderSystem = _world->RegisterSystem<MeshRenderSystem>(_renderCommandQueue.get());
    auto* cameraSystem = _world->RegisterSystem<CameraSystem>(_inputManager.get());
    auto* physicsSystem = _world->RegisterSystem<PhysicsSystem>();

    // Set system signatures
    _world->SetSystemSignature<Transform>(debugSystem);
    _world->SetSystemSignature<Transform, Velocity>(movementSystem);
    _world->SetSystemSignature<Script>(scriptSystem);
    _world->SetSystemSignature<Transform, MeshRenderer>(meshRenderSystem);
    _world->SetSystemSignature<Transform>(cameraSystem);
    _world->SetSystemSignature<Transform, BoxCollider>(physicsSystem);

    std::cout << "[TronEngine] ECS World initialized with all components and systems\n";

    // Create test quad for shader testing (MOVED FROM RENDERENGINE) - Optional, can be removed
    _testQuad = std::make_unique<FullscreenQuad>();
    if (!_testQuad->Initialize(_renderEngine->GetDevice())) {
        std::cout << "[TronEngine] Failed to create test quad\n";
        _testQuad.reset();
    }

    _inputManager = std::make_unique<InputManager>();
    std::cout << "[TronEngine] InputManager initialized\n";

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

    // Clean up test quad
    _testQuad.reset();

    // === NEW: Clean up clean architecture components ===
    _renderExecutor.reset();
    std::cout << "[TronEngine] RenderExecutor shut down\n";

    _renderCommandQueue.reset();
    std::cout << "[TronEngine] Command queue shut down\n";

    // TODO: Shutdown in reverse order when implemented
    // Shutdown ECS World first (it references the managers)
    if (_world) {
        _world->Shutdown();
        _world.reset();
        std::cout << "[TronEngine] ECS World shut down\n";
    }

    _materialManager.reset();
    _meshManager.reset();
    _renderEngine.reset();

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

        // === INPUT HANDLING ===
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            switch (msg.message) {
            case WM_KEYDOWN:
                if (_inputManager) {
                    _inputManager->OnKeyEvent(msg.wParam, true);
                    // std::cout << "[Input] Key Down: " << static_cast<int>(msg.wParam) << "\n";
                }
                break;
            case WM_KEYUP:
                if (_inputManager) {
                    _inputManager->OnKeyEvent(msg.wParam, false);
                    // std::cout << "[Input] Key Up: " << static_cast<int>(msg.wParam) << "\n";
                }
                break;
            case WM_LBUTTONDOWN:
                if (_inputManager) {
                    _inputManager->OnMouseButtonEvent(0, true); // 0 = left button
                    std::cout << "[Input] Left Mouse Button Down\n";
                }
                break;
            case WM_LBUTTONUP:
                if (_inputManager) {
                    _inputManager->OnMouseButtonEvent(0, false);
                    std::cout << "[Input] Left Mouse Button Up\n";
                }
                break;
            case WM_RBUTTONDOWN:
                if (_inputManager) {
                    _inputManager->OnMouseButtonEvent(1, true); // 1 = right button
                    std::cout << "[Input] Right Mouse Button Down\n";
                }
                break;
            case WM_RBUTTONUP:
                if (_inputManager) {
                    _inputManager->OnMouseButtonEvent(1, false);
                    std::cout << "[Input] Right Mouse Button Up\n";
                }
                break;
            case WM_MOUSEMOVE:
                if (_inputManager) {
                    _inputManager->OnMouseMove(msg.lParam);
                    // Don't log mouse move every frame - too spammy
                }
                break;
            case WM_MOUSEWHEEL:
                if (_inputManager) {
                    _inputManager->OnMouseWheel(msg.wParam);
                    std::cout << "[Input] Mouse Wheel: Delta=" << _inputManager->GetMouseWheelDelta() << "\n";
                }
                break;
            case WM_QUIT:
                _running = false;
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                break;
            }
        }

        // Exit if we received WM_QUIT
        if (!_running) break;

        // === RENDERING ===
        if (_renderEngine && _renderExecutor && _renderCommandQueue) {
            _renderEngine->BeginFrame();

            // Get all render commands from game thread
            auto renderCommands = _renderCommandQueue->PopAllCommands();

            // Execute commands through RenderExecutor
            if (!renderCommands.empty()) {
                _renderExecutor->ExecuteRenderCommands(renderCommands);
            }

            _renderEngine->EndFrame();
        }

        frameCount++;

        // FPS counter every second
        auto currentTime = clock::now();
        if (std::chrono::duration<double>(currentTime - lastFPSTime).count() >= 1.0) {
            std::cout << "[MainRender] Rendered " << frameCount << " frames in last second\n";
            frameCount = 0;
            lastFPSTime = currentTime;
        }

        // Frame rate limiting
        while (true) {
            auto now = clock::now();
            double elapsed = std::chrono::duration<double>(now - frameStart).count();

            if (elapsed >= targetFrameTime) {
                break;
            }

#if defined(_MSC_VER)
            _mm_pause();  // Prevent 100% CPU usage
#else
            std::this_thread::yield();
#endif
        }
    }

    std::cout << "[Threading] Main Render Thread finished\n";
}

void Engine::RenderTestQuad() {
    if (!_testQuad || !_renderEngine) {
        return;
    }

    ID3D11DeviceContext* ctx = _renderEngine->GetDeviceContext();
    Shader* shader = _renderEngine->GetShader("default");

    if (!ctx || !shader) {
        return;
    }

    // Set up shader pipeline
    ctx->IASetInputLayout(shader->inputLayout);
    ctx->VSSetShader(shader->vertexShader, nullptr, 0);
    ctx->PSSetShader(shader->pixelShader, nullptr, 0);

    // Render the test quad
    _testQuad->Render(ctx, _renderEngine->GetColorConstantBuffer());
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

            if (_inputManager) {
                // Process input events
                _inputManager->Update();
            }
            // === NEW: Generate Render Commands (Clean Architecture) ===
            auto* meshRenderSystem = _world->GetSystem<MeshRenderSystem>();
            if (meshRenderSystem) {
                meshRenderSystem->GenerateRenderCommands();  // Pure ECS -> Commands
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
InputManager* Engine::GetInputManager() const {
    return _inputManager.get();
}

// ECS
World* Engine::GetWorld() const {
    return _world.get();
}
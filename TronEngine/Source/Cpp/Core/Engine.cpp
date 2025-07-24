#include "../Headers/Core/Engine.hpp"
#include "../../Include/TronEngine.hpp"
#include "../Headers/Rendering/RenderEngine.hpp"
#include "../Headers/Core/WindowUtils.hpp"
#include <iostream>
#include <chrono>

Engine::Engine()
    : m_initialized(false)
    , m_running(false)
    , m_version(TRON_ENGINE_VERSION_STRING)
{
    std::cout << "[TronEngine] Constructor - Engine object created\n";
}

Engine::~Engine() {
    if (m_initialized) {
        Shutdown();
    }
    std::cout << "[TronEngine] Destructor - Engine object destroyed\n";
}

bool Engine::Initialize() {
    if (m_initialized) {
        std::cout << "[TronEngine] Warning: Already initialized\n";
        return true;
    }

    std::cout << "[TronEngine] Initializing TRON Engine v" << TRON_ENGINE_VERSION_STRING << "\n";

    // Initialize subsystems
    if (!InitializeSubsystems()) {
        std::cout << "[TronEngine] Failed to initialize subsystems\n";
        return false;
    }

    m_initialized = true;
    std::cout << "[TronEngine] Initialization successful!\n";
    return true;
}

void Engine::Run() {
    if (!m_initialized) {
        std::cout << "[TronEngine] Error: Engine not initialized\n";
        return;
    }

    std::cout << "[TronEngine] Starting main engine loop...\n";
    m_running = true;
    m_frameCount = 0;

    // Start the Game Thread
    m_gameThread = std::make_unique<std::thread>(&Engine::GameLoop, this);
    
    // RenderLoop in the main Thread
    RenderLoop();

    if (m_gameThread && m_gameThread->joinable()) {
        m_gameThread->join();
    }

    std::cout << "[TronEngine] Engine loops stopped\n";
}

void Engine::RenderLoop() {
    std::cout << "[Threading] Main Thread -> Render Loop started\n";

    while (m_running && m_frameCount < 20) {  // Test: 20 frames
        auto frameStart = std::chrono::steady_clock::now();

        // TODO: Call the Rendering method that groups all the thing need do it everyframe

        // Placeholder: Simulate render work
        std::cout << "[Render Thread] Frame " << m_frameCount + 1 << " - Rendering...\n";

        // Simulate rendering time (16ms for 60 FPS) replace with VSync (optional)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        // Frame timing
        auto frameEnd = std::chrono::steady_clock::now();
        float frameTime = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();

        if (m_frameCount % 10 == 0) {  // Every 10 frames
            std::cout << "[Render Thread] Frame time: " << frameTime << "ms (Target: 16ms)\n";
        }

        m_frameCount++;
    }

    std::cout << "[Threading] Render Thread finished\n";
}

void Engine::GameLoop() {
    std::cout << "[Threading] Game Thread started\n";

    int gameFrame = 0;
    while (m_running && gameFrame < 20) {  // Test: 20 frames
        auto frameStart = std::chrono::steady_clock::now();

        // TODO: call ECS UPDATE function that call everything, I think is in World.cpp on the Game folder

        // Placeholder: Simulate game work
        std::cout << "[Game Thread] Frame " << gameFrame + 1 << " - Processing game logic...\n";

        gameFrame++;

        // Calculate delta time for game systems
        auto frameEnd = std::chrono::steady_clock::now();
        m_deltaTime = std::chrono::duration<float>(frameEnd - frameStart).count();

        // Target 60 FPS for game logic (less than render thread) for build we can comment this, for gameplay fluidity
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }

    m_running = false;  // Signal render thread to stop
    std::cout << "[Threading] Game Thread finished\n";
}

void Engine::Shutdown() {
    if (!m_initialized) {
        return;
    }

    std::cout << "[TronEngine] Shutting down...\n";

    m_running = false;

    if (m_gameThread && m_gameThread->joinable()) {
        std::cout << "[Threading] Waiting for Game Thread to finish...\n";
        m_gameThread->join();
        std::cout << "[Threading] Game Thread joined successfully\n";
    }

    ShutdownSubsystems();

    m_initialized = false;
    std::cout << "[TronEngine] Shutdown complete\n";
}

void Engine::PrintMessage(const char* message) {
    std::cout << "[TronEngine] " << message << "\n";
}

const char* Engine::GetVersion() const {
    return m_version.c_str();
}

bool Engine::InitializeSubsystems() {
    std::cout << "[TronEngine] Initializing subsystems...\n";

    // TODO: Initialize RenderEngine, GameEngine, ThreadPool
    

    // Render Engine 
	// TODO: Initialize the RenderEngine with a single function in Window and other from the RenderEngine
    int width = 1280;
    int height = 720;

    // Crée la fenêtre Win32
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    HWND hwnd = CreateSimpleWindow(hInstance, width, height, L"TronEngine");

    if (!hwnd) {
        std::cout << "[TronEngine] Failed to create window\n";
        return false;
    }
    ShowWindow(hwnd, SW_SHOW);

    RenderEngine* renderEngine = new RenderEngine(hwnd, width, height);
    renderEngine->Initialize();
    std::cout << "[TronEngine] RenderEngine initialized successfully\n";

    std::cout << "[Threading] Thread infrastructure: Ready\n";
    std::cout << "[Threading] Render Thread target: 60 FPS (16ms per frame)\n";
    std::cout << "[Threading] Game Thread target: 60 FPS (16ms per frame)\n";

    return true;
}

void Engine::ShutdownSubsystems() {
    std::cout << "[TronEngine] Shutting down subsystems...\n";

    // TODO: Shutdown in reverse order when implemented
    // m_gameEngine.reset();
    // m_renderEngine.reset();

    std::cout << "[TronEngine] All subsystems shut down\n";
}
#include "../Headers/Core/Engine.hpp" 
#include "../../Include/TronEngine.hpp"
#include <iostream>
#include <chrono>

Engine::Engine()
    : m_initialized(false)
    , m_running(false)
    , m_version("1.0.0")
    , m_world(nullptr)
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

    //TODO : Put this into another place
    // Initialize ECS World
    m_world = std::make_unique<World>();

    // Register components
    m_world->RegisterComponent<Transform>();
    m_world->RegisterComponent<Velocity>();

    // Register systems
    auto* debugSystem = m_world->RegisterSystem<DebugSystem>();
    auto* movementSystem = m_world->RegisterSystem<MovementSystem>();

    // Set system signatures
    m_world->SetSystemSignature<Transform>(debugSystem);
    m_world->SetSystemSignature<Transform, Velocity>(movementSystem);

    std::cout << "[TronEngine] ECS World initialized with components registered\n";

    // Initialize other subsystems
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
    std::cout << "[TronEngine] Press Ctrl+C to stop (or will auto-stop after demo)\n";

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

    while (m_running && m_frameCount < 300) {  // Test: 20 frames
        auto frameStart = std::chrono::steady_clock::now();

        // TODO: Call the Rendering method that groups all the thing need do it everyframe

        // Placeholder: Simulate render work
        // std::cout << "[Render Thread] Frame " << m_frameCount + 1 << " - Rendering...\n";

        // Simulate rendering time (16ms for 60 FPS) replace with VSync (optional)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        // Frame timing
        auto frameEnd = std::chrono::steady_clock::now();
        float frameTime = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();

        if (m_frameCount % 10 == 0) {  // Every 10 frames
            // std::cout << "[Render Thread] Frame time: " << frameTime << "ms (Target: 16ms)\n";
        }

        m_frameCount++;
    }

    std::cout << "[Threading] Render Thread finished\n";
}

void Engine::GameLoop() {
    std::cout << "[Threading] Game Thread started\n";

    //TODO: do a proper thing and care because it exist in another place too Ctrl + F.
    const float deltaTime = 0.016f; // ~60 FPS

    // Test prints
    std::cout << "[TronEngine] === Initial Entity States ===\n";
    auto allEntities = GetAllEntities();
    for (Entity entity : allEntities)
    {
        auto *transform = GetTransformComponent(entity);
        auto *velocity = GetVelocityComponent(entity);

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
    while (m_running && gameFrame < 300) { 
        auto frameStart = std::chrono::steady_clock::now();

        // TODO: call ECS UPDATE function that call everything, I think is in World.cpp on the Game folder
        // Update ECS World - this handles all movement, systems, etc.
        if (m_world)
        {
            m_world->Update(deltaTime);
        }

        // Show progress every second
        if (gameFrame % 60 == 0 && gameFrame > 0)
        {
            float elapsedTime = gameFrame * deltaTime;
            std::cout << "[TronEngine] === After " << elapsedTime << " seconds ===\n";

            for (Entity entity : allEntities)
            {
                if (!IsValidEntity(entity))
                    continue; // Skip destroyed entities

                auto *transform = GetTransformComponent(entity);
                if (transform)
                {
                    std::cout << "[TronEngine] Entity " << entity
                              << " position: (" << transform->x << ", "
                              << transform->y << ", " << transform->z << ")\n";
                }
            }
        }

        // Placeholder: Simulate game work
        // std::cout << "[Game Thread] Frame " << gameFrame + 1 << " - Processing game logic...\n";

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
    
    // Shutdown ECS World
    if (m_world) {
        m_world->Shutdown();
        m_world.reset();
        std::cout << "[TronEngine] ECS World shut down\n";
    }
    m_running = false;
    // Stoping THreads
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

// ECS Interface Implementation - Internal methods using Tron::Entity
Entity Engine::CreateEntity() {
    if (!m_world) {
        std::cout << "[TronEngine] Error: World not initialized\n";
        return INVALID_ENTITY;
    }

    Entity entity = m_world->CreateEntity();
    std::cout << "[TronEngine] Created entity: " << entity << "\n";
    return entity;
}

void Engine::DestroyEntity(Entity entity) {
    if (!m_world) {
        std::cout << "[TronEngine] Error: World not initialized\n";
        return;
    }

    m_world->DestroyEntity(entity);
    std::cout << "[TronEngine] Destroyed entity: " << entity << "\n";
}

bool Engine::IsValidEntity(Entity entity) const {
    if (!m_world) {
        return false;
    }
    return m_world->IsValidEntity(entity);
}

Transform* Engine::AddTransformComponent(Entity entity, float x, float y, float z) {
    if (!m_world) {
        std::cout << "[TronEngine] Error: World not initialized\n";
        return nullptr;
    }

    Transform* transform = m_world->AddComponent<Transform>(entity, x, y, z);
    std::cout << "[TronEngine] Added Transform component to entity " << entity
        << " at position (" << x << ", " << y << ", " << z << ")\n";
    return transform;
}

Velocity* Engine::AddVelocityComponent(Entity entity, float vx, float vy, float vz) {
    if (!m_world) {
        std::cout << "[TronEngine] Error: World not initialized\n";
        return nullptr;
    }

    Velocity* velocity = m_world->AddComponent<Velocity>(entity, vx, vy, vz);
    std::cout << "[TronEngine] Added Velocity component to entity " << entity
        << " with velocity (" << vx << ", " << vy << ", " << vz << ")\n";
    return velocity;
}

Transform* Engine::GetTransformComponent(Entity entity) {
    if (!m_world) {
        return nullptr;
    }
    return m_world->GetComponent<Transform>(entity);
}

Velocity* Engine::GetVelocityComponent(Entity entity) {
    if (!m_world) {
        return nullptr;
    }
    return m_world->GetComponent<Velocity>(entity);
}

void Engine::RemoveTransformComponent(Entity entity) {
    if (!m_world) {
        std::cout << "[TronEngine] Error: World not initialized\n";
        return;
    }

    m_world->RemoveComponent<Transform>(entity);
    std::cout << "[TronEngine] Removed Transform component from entity " << entity << "\n";
}

void Engine::RemoveVelocityComponent(Entity entity) {
    if (!m_world) {
        std::cout << "[TronEngine] Error: World not initialized\n";
        return;
    }

    m_world->RemoveComponent<Velocity>(entity);
    std::cout << "[TronEngine] Removed Velocity component from entity " << entity << "\n";
}

void Engine::UpdateWorld(float deltaTime) {
    if (m_world) {
        m_world->Update(deltaTime);
    }
}

uint32_t Engine::GetEntityCount() const {
    if (!m_world) {
        return 0;
    }
    return m_world->GetEntityCount();
}

std::vector<Entity> Engine::GetAllEntities() const {
    if (!m_world) {
        return std::vector<Entity>();
    }
    return m_world->GetAllEntities();
}

// C API wrapper methods - these handle type conversion
unsigned int Engine::CreateEntityC() {
    Entity entity = CreateEntity();
    return static_cast<unsigned int>(entity);
}

void Engine::DestroyEntityC(unsigned int entity) {
    DestroyEntity(static_cast<Entity>(entity));
}

bool Engine::IsValidEntityC(unsigned int entity) const {
    return IsValidEntity(static_cast<Entity>(entity));
}

Transform* Engine::AddTransformComponentC(unsigned int entity, float x, float y, float z) {
    return AddTransformComponent(static_cast<Entity>(entity), x, y, z);
}

Velocity* Engine::AddVelocityComponentC(unsigned int entity, float vx, float vy, float vz) {
    return AddVelocityComponent(static_cast<Entity>(entity), vx, vy, vz);
}

Transform* Engine::GetTransformComponentC(unsigned int entity) {
    return GetTransformComponent(static_cast<Entity>(entity));
}

Velocity* Engine::GetVelocityComponentC(unsigned int entity) {
    return GetVelocityComponent(static_cast<Entity>(entity));
}

void Engine::RemoveTransformComponentC(unsigned int entity) {
    RemoveTransformComponent(static_cast<Entity>(entity));
}

void Engine::RemoveVelocityComponentC(unsigned int entity) {
    RemoveVelocityComponent(static_cast<Entity>(entity));
}

unsigned int Engine::GetEntityCountC() const {
    return static_cast<unsigned int>(GetEntityCount());
}

World* Engine::GetWorld() const {
    return m_world.get();
}

bool Engine::InitializeSubsystems() {
    std::cout << "[TronEngine] Initializing subsystems...\n";

    // TODO: Initialize RenderEngine, GameEngine, ThreadPool
    // For now, just placeholders

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
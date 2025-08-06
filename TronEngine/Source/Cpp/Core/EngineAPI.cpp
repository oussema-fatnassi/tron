//#define ENGINE_EXPORTS 
#include "../Headers/Core/Engine.hpp" 
#include "../../Include/TronEngine.hpp" 
#include <iostream>
#include "../Headers/Rendering/Camera.hpp"
#include "../Headers/Math/Matrix.hpp"
#include "../Headers/Game/CameraMatrixSystem.hpp"
#include <unordered_map>

// Global singleton instance
static Engine* g_engineInstance = nullptr;

// Global camera management
static std::unordered_map<std::string, std::unique_ptr<Camera>> g_cameras;
static std::string g_activeCameraName = "";

// C-style API implementation
extern "C" {
    ENGINE_API bool CreateAndInitializeEngine() {
        if (g_engineInstance) {
            std::cout << "[TronEngine] Engine already exists\n";
            return true;
        }

        g_engineInstance = new Engine();
        if (!g_engineInstance->Initialize()) {
            delete g_engineInstance;
            g_engineInstance = nullptr;
            return false;
        }
        return true;
    }

    ENGINE_API void RunEngine() {
        if (!g_engineInstance) {
            std::cout << "[TronEngine] Error: No engine instance\n";
            return;
        }
        g_engineInstance->Run();
    }

    ENGINE_API void DestroyGlobalEngine() {
        if (g_engineInstance) {
            g_engineInstance->Shutdown();
            delete g_engineInstance;
            g_engineInstance = nullptr;
        }
    }

    ENGINE_API void QuitGame() {
        if (g_engineInstance) {
            std::cout << "[EngineAPI] QuitGame() called - stopping engine\n";
            g_engineInstance->RequestShutdown();
        }
    }

    ENGINE_API void PrintEngineVersion() {
        std::cout << "TRON Engine Version: " << TRON_ENGINE_VERSION_STRING << std::endl;
    }

    ENGINE_API const char* GetEngineInfo() {
        return "TRON 3D Game Engine - Professional DLL Version with ECS";
    }

	// Input API
    ENGINE_API bool IsKeyDown(int keyCode) {
        if (!g_engineInstance || !g_engineInstance->GetInputManager()) return false;
        return g_engineInstance->GetInputManager()->IsKeyDown(keyCode);
    }

    ENGINE_API bool IsKeyPressed(int keyCode) {
        if (!g_engineInstance || !g_engineInstance->GetInputManager()) return false;
        return g_engineInstance->GetInputManager()->IsKeyPressed(keyCode);
    }

    ENGINE_API bool IsKeyReleased(int keyCode) {
            if (!g_engineInstance || !g_engineInstance->GetInputManager()) return false;
            return g_engineInstance->GetInputManager()->IsKeyReleased(keyCode);
    }

    ENGINE_API bool IsMouseButtonDown(int button) {
        if (!g_engineInstance || !g_engineInstance->GetInputManager()) return false;
        return g_engineInstance->GetInputManager()->IsMouseButtonDown(button);
    }

    ENGINE_API bool IsMouseButtonPressed(int button) {
        if (!g_engineInstance || !g_engineInstance->GetInputManager()) return false;
        return g_engineInstance->GetInputManager()->IsMouseButtonPressed(button);
    }

    ENGINE_API bool IsMouseButtonReleased(int button) {
        if (!g_engineInstance || !g_engineInstance->GetInputManager()) return false;
        return g_engineInstance->GetInputManager()->IsMouseButtonReleased(button);
    }

    ENGINE_API void GetMousePosition(int* x, int* y) {
        if (!x || !y || !g_engineInstance || !g_engineInstance->GetInputManager()) {
            if (x) *x = 0;
            if (y) *y = 0;
            return;
        }
        
        POINT pos = g_engineInstance->GetInputManager()->GetMousePosition();
        *x = pos.x;
        *y = pos.y;
    }

    ENGINE_API int GetMouseWheelDelta() {
        if (!g_engineInstance || !g_engineInstance->GetInputManager()) return 0;
        return g_engineInstance->GetInputManager()->GetMouseWheelDelta();
    }

    ENGINE_API void SetMouseSensitivity(float sensitivity) {
        // TODO: Implement global mouse sensitivity setting
        std::cout << "[EngineAPI] TODO: SetMouseSensitivity(" << sensitivity << ")\n";
    }

    // ECS C-style API with Engine prefix
    // Direct World Access API
    ENGINE_API uint32_t CreateEntity() {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return 0;
        return g_engineInstance->GetWorld()->CreateEntity();
    }

    ENGINE_API void DestroyEntity(uint32_t entity) {
        if (g_engineInstance && g_engineInstance->GetWorld()) {
            g_engineInstance->GetWorld()->DestroyEntity(entity);
        }
    }

    ENGINE_API void SafeDestroyEntity(uint32_t entity) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) {
            std::cout << "[EngineAPI] Error: No engine or world available\n";
            return;
        }

        // Use the safe destruction method
        g_engineInstance->GetWorld()->QueueEntityForDestruction(entity);
    }

    ENGINE_API bool IsValidEntity(uint32_t entity) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        return g_engineInstance->GetWorld()->IsValidEntity(entity);
    }

    ENGINE_API uint32_t GetEntityCount() {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return 0;
        return g_engineInstance->GetWorld()->GetEntityCount();
    }

    ENGINE_API bool AddTransformComponent(uint32_t entity, float x, float y, float z) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        Transform* component = g_engineInstance->GetWorld()->AddComponent<Transform>(entity, x, y, z);
        return component != nullptr;
    }

    ENGINE_API bool AddVelocityComponent(uint32_t entity, float vx, float vy, float vz) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        Velocity* component = g_engineInstance->GetWorld()->AddComponent<Velocity>(entity, vx, vy, vz);
        return component != nullptr;
    }

    ENGINE_API bool AddCustomScript(uint32_t entity, void* scriptInstance) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !scriptInstance) return false;

        // Cast the void* to ScriptBase* (user's script)
        ScriptBase* userScript = static_cast<ScriptBase*>(scriptInstance);

        // Create internal Script component
        Script* component = g_engineInstance->GetWorld()->AddComponent<Script>(entity);

        // Store pointer to user's script (don't copy by value!)
        component->userScript = userScript;

        // Now we can set entityId on ScriptBase - no casting needed!
        userScript->entity = entity;

        return component != nullptr;
    }

    ENGINE_API bool GetTransformComponent(uint32_t entity, float* x, float* y, float* z) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !x || !y || !z) return false;

        Transform* component = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (component) {
            *x = component->x;
            *y = component->y;
            *z = component->z;
            return true;
        }
        return false;
    }

    ENGINE_API bool GetVelocityComponent(uint32_t entity, float* vx, float* vy, float* vz) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !vx || !vy || !vz) return false;

        Velocity* component = g_engineInstance->GetWorld()->GetComponent<Velocity>(entity);
        if (component) {
            *vx = component->vx;
            *vy = component->vy;
            *vz = component->vz;
            return true;
        }
        return false;
    }

    ENGINE_API void RemoveTransformComponent(uint32_t entity) {
        if (g_engineInstance && g_engineInstance->GetWorld()) {
            g_engineInstance->GetWorld()->RemoveComponent<Transform>(entity);
        }
    }

    ENGINE_API void RemoveVelocityComponent(uint32_t entity) {
        if (g_engineInstance && g_engineInstance->GetWorld()) {
            g_engineInstance->GetWorld()->RemoveComponent<Velocity>(entity);
        }
    }

    ENGINE_API bool RemoveScript(uint32_t entity) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) {
            return false;
        }

        auto* script = g_engineInstance->GetWorld()->GetComponent<Script>(entity);
        if (!script) {
            std::cout << "[EngineAPI] Warning: Entity " << entity << " has no script to remove\n";
            return false;
        }

        std::cout << "[EngineAPI] Removing script from entity " << entity << std::endl;

        // Remove Script component - OnDestroy() called automatically
        g_engineInstance->GetWorld()->RemoveComponent<Script>(entity);

        std::cout << "[EngineAPI] Script removed from entity " << entity << std::endl;
        return true;
    }

    ENGINE_API bool AddMeshRendererComponent(uint32_t entity, int primitiveType, const char* shaderName) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        // Convert int to enum
        PrimitiveMeshType meshType = static_cast<PrimitiveMeshType>(primitiveType);

        std::string shader = shaderName ? shaderName : "default";

        MeshRenderer* component = g_engineInstance->GetWorld()->AddComponent<MeshRenderer>(entity, meshType, shader);
        return component != nullptr;
    }

    ENGINE_API bool AddCustomMeshRendererComponent(uint32_t entity, const char* customMeshName, const char* shaderName) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !customMeshName) return false;

        std::string meshName = customMeshName;
        std::string shader = shaderName ? shaderName : "default";

        MeshRenderer* component = g_engineInstance->GetWorld()->AddComponent<MeshRenderer>(entity, meshName, shader);
        return component != nullptr;
    }

    ENGINE_API bool SetMeshRendererColor(uint32_t entity, float r, float g, float b, float a) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        MeshRenderer* component = g_engineInstance->GetWorld()->GetComponent<MeshRenderer>(entity);
        if (component) {
            component->SetColor(r, g, b, a);
            return true;
        }
        return false;
    }

    ENGINE_API bool SetMeshRendererVisible(uint32_t entity, bool visible) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        MeshRenderer* component = g_engineInstance->GetWorld()->GetComponent<MeshRenderer>(entity);
        if (component) {
            component->isVisible = visible;
            return true;
        }
        return false;
    }

    ENGINE_API bool SetMeshRendererShader(uint32_t entity, const char* shaderName) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !shaderName) return false;

        MeshRenderer* component = g_engineInstance->GetWorld()->GetComponent<MeshRenderer>(entity);
        if (component) {
            component->SetShader(shaderName);
            return true;
        }
        return false;
    }

    ENGINE_API bool SetMeshRendererMaterial(uint32_t entity, const char* materialName) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !materialName) return false;

        MeshRenderer* component = g_engineInstance->GetWorld()->GetComponent<MeshRenderer>(entity);
        if (component) {
            component->SetMaterial(materialName);
            return true;
        }
        return false;
    }

    ENGINE_API void RemoveMeshRendererComponent(uint32_t entity) {
        if (g_engineInstance && g_engineInstance->GetWorld()) {
            g_engineInstance->GetWorld()->RemoveComponent<MeshRenderer>(entity);
        }
    }

    // Enhanced Transform Component API Implementation
    ENGINE_API bool SetTransformPosition(uint32_t entity, float x, float y, float z) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            transform->SetPosition(x, y, z);
            return true;
        }
        return false;
    }

    ENGINE_API bool SetTransformRotation(uint32_t entity, float pitch, float yaw, float roll) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            transform->SetRotation(pitch, yaw, roll);
            return true;
        }
        return false;
    }

    ENGINE_API bool SetTransformRotationDegrees(uint32_t entity, float pitchDeg, float yawDeg, float rollDeg) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            transform->SetRotationDegrees(pitchDeg, yawDeg, rollDeg);
            return true;
        }
        return false;
    }

    ENGINE_API bool SetTransformScale(uint32_t entity, float scaleX, float scaleY, float scaleZ) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            transform->SetScale(scaleX, scaleY, scaleZ);
            return true;
        }
        return false;
    }

    ENGINE_API bool SetTransformUniformScale(uint32_t entity, float scale) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            transform->SetUniformScale(scale);
            return true;
        }
        return false;
    }

    ENGINE_API bool GetTransformRotation(uint32_t entity, float* pitch, float* yaw, float* roll) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !pitch || !yaw || !roll) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            *pitch = transform->pitch;
            *yaw = transform->yaw;
            *roll = transform->roll;
            return true;
        }
        return false;
    }

    ENGINE_API bool GetTransformScale(uint32_t entity, float* scaleX, float* scaleY, float* scaleZ) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !scaleX || !scaleY || !scaleZ) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            *scaleX = transform->scaleX;
            *scaleY = transform->scaleY;
            *scaleZ = transform->scaleZ;
            return true;
        }
        return false;
    }

    ENGINE_API bool TranslateTransform(uint32_t entity, float deltaX, float deltaY, float deltaZ) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            transform->Translate(deltaX, deltaY, deltaZ);
            return true;
        }
        return false;
    }

    ENGINE_API bool RotateTransform(uint32_t entity, float deltaPitch, float deltaYaw, float deltaRoll) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            transform->Rotate(deltaPitch, deltaYaw, deltaRoll);
            return true;
        }
        return false;
    }

    ENGINE_API bool RotateTransformDegrees(uint32_t entity, float deltaPitchDeg, float deltaYawDeg, float deltaRollDeg) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        Transform* transform = g_engineInstance->GetWorld()->GetComponent<Transform>(entity);
        if (transform) {
            transform->RotateDegrees(deltaPitchDeg, deltaYawDeg, deltaRollDeg);
            return true;
        }
        return false;
    }

    // Camera System API Implementation
    ENGINE_API bool CreateCamera(const char* cameraName, float fovDegrees, float aspectRatio, float nearPlane, float farPlane) {
        if (!cameraName || !g_engineInstance) return false;

        std::string name(cameraName);
        if (g_cameras.find(name) != g_cameras.end()) {
            std::cout << "[EngineAPI] Warning: Camera '" << name << "' already exists\n";
            return false;
        }

        auto camera = std::make_unique<Camera>(fovDegrees, aspectRatio, nearPlane, farPlane);
        g_cameras[name] = std::move(camera);

        // Set as active camera if it's the first one
        if (g_activeCameraName.empty()) {
            g_activeCameraName = name;
        }

        std::cout << "[EngineAPI] Created camera '" << name << "'\n";
        return true;
    }

    ENGINE_API bool AttachCameraToEntity(const char* cameraName, uint32_t entity) {
        if (!cameraName || !g_engineInstance) return false;

        std::string name(cameraName);
        auto it = g_cameras.find(name);
        if (it == g_cameras.end()) {
            std::cout << "[EngineAPI] Error: Camera '" << name << "' not found\n";
            return false;
        }

        it->second->AttachToEntity(g_engineInstance->GetWorld(), entity);
        std::cout << "[EngineAPI] Attached camera '" << name << "' to entity " << entity << "\n";
        return true;
    }

    ENGINE_API bool DetachCamera(const char* cameraName) {
        if (!cameraName) return false;

        std::string name(cameraName);
        auto it = g_cameras.find(name);
        if (it == g_cameras.end()) return false;

        it->second->DetachFromEntity();
        std::cout << "[EngineAPI] Detached camera '" << name << "'\n";
        return true;
    }

    ENGINE_API bool SetCameraProjection(const char* cameraName, float fovDegrees, float aspectRatio, float nearPlane, float farPlane) {
        if (!cameraName) return false;

        std::string name(cameraName);
        auto it = g_cameras.find(name);
        if (it == g_cameras.end()) return false;

        it->second->SetProjection(fovDegrees, aspectRatio, nearPlane, farPlane);
        return true;
    }

    ENGINE_API bool SetCameraMovementSpeed(const char* cameraName, float speed) {
        if (!cameraName) return false;

        std::string name(cameraName);
        auto it = g_cameras.find(name);
        if (it == g_cameras.end()) return false;

        it->second->movementSpeed = speed;
        return true;
    }

    ENGINE_API bool SetCameraMouseSensitivity(const char* cameraName, float sensitivity) {
        if (!cameraName) return false;

        std::string name(cameraName);
        auto it = g_cameras.find(name);
        if (it == g_cameras.end()) return false;

        it->second->mouseSensitivity = sensitivity;
        return true;
    }

    ENGINE_API bool GetCameraPosition(const char* cameraName, float* x, float* y, float* z) {
        if (!cameraName || !x || !y || !z) return false;

        std::string name(cameraName);
        auto it = g_cameras.find(name);
        if (it == g_cameras.end()) return false;

        it->second->GetPosition(*x, *y, *z);
        return true;
    }

    ENGINE_API bool GetCameraRotation(const char* cameraName, float* pitch, float* yaw, float* roll) {
        if (!cameraName || !pitch || !yaw || !roll) return false;

        std::string name(cameraName);
        auto it = g_cameras.find(name);
        if (it == g_cameras.end()) return false;

        it->second->GetRotation(*pitch, *yaw, *roll);
        return true;
    }

    ENGINE_API bool SetActiveCamera(const char* cameraName) {
        if (!cameraName) return false;

        std::string name(cameraName);
        auto it = g_cameras.find(name);
        if (it == g_cameras.end()) {
            std::cout << "[EngineAPI] Error: Camera '" << name << "' not found\n";
            return false;
        }

        g_activeCameraName = name;
        std::cout << "[EngineAPI] Set active camera to '" << name << "'\n";
        return true;
    }

    ENGINE_API const char* GetActiveCameraName() {
        return g_activeCameraName.c_str();
    }

    // Helper function to get active camera (for internal engine use)
    Camera* GetActiveCamera() {
        if (g_activeCameraName.empty()) return nullptr;

        auto it = g_cameras.find(g_activeCameraName);
        if (it == g_cameras.end()) return nullptr;

        return it->second.get();
    }

    // Cleanup function (call in DestroyGlobalEngine)
    void CleanupCameras() {
        g_cameras.clear();
        g_activeCameraName.clear();
        std::cout << "[EngineAPI] All cameras cleaned up\n";
    }

	// Physics & Collisions System API
    // NEW: Basic BoxCollider Component API
        ENGINE_API bool AddBoxColliderComponent(uint32_t entity, float width, float height, float depth, bool isTrigger) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        BoxCollider* component = g_engineInstance->GetWorld()->AddComponent<BoxCollider>(
            entity, width, height, depth, isTrigger);

        if (component) {
            std::cout << "[EngineAPI] BoxCollider added to entity " << entity
                << " (size: " << width << "x" << height << "x" << depth
                << ", trigger: " << (isTrigger ? "true" : "false") << ")\n";
            return true;
        }
        return false;
    }

    ENGINE_API bool SetBoxColliderSize(uint32_t entity, float width, float height, float depth) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        BoxCollider* collider = g_engineInstance->GetWorld()->GetComponent<BoxCollider>(entity);
        if (collider) {
            collider->SetSize(width, height, depth);
            std::cout << "[EngineAPI] BoxCollider size updated for entity " << entity << "\n";
            return true;
        }
        return false;
    }

    ENGINE_API bool SetBoxColliderTrigger(uint32_t entity, bool isTrigger) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        BoxCollider* collider = g_engineInstance->GetWorld()->GetComponent<BoxCollider>(entity);
        if (collider) {
            collider->SetTrigger(isTrigger);
            std::cout << "[EngineAPI] BoxCollider trigger mode set to " << (isTrigger ? "true" : "false")
                << " for entity " << entity << "\n";
            return true;
        }
        return false;
    }

    ENGINE_API bool SetBoxColliderEnabled(uint32_t entity, bool enabled) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;

        BoxCollider* collider = g_engineInstance->GetWorld()->GetComponent<BoxCollider>(entity);
        if (collider) {
            collider->SetEnabled(enabled);
            std::cout << "[EngineAPI] BoxCollider " << (enabled ? "enabled" : "disabled")
                << " for entity " << entity << "\n";
            return true;
        }
        return false;
    }

    ENGINE_API void RemoveBoxColliderComponent(uint32_t entity) {
        if (g_engineInstance && g_engineInstance->GetWorld()) {
            g_engineInstance->GetWorld()->RemoveComponent<BoxCollider>(entity);
            std::cout << "[EngineAPI] BoxCollider removed from entity " << entity << "\n";
        }
    }

    // NEW: Physics System Control
    ENGINE_API void SetPhysicsDebugOutput(bool enabled) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return;

        auto* physicsSystem = g_engineInstance->GetWorld()->GetSystem<PhysicsSystem>();
        if (physicsSystem) {
            physicsSystem->SetDebugOutput(enabled);
            std::cout << "[EngineAPI] Physics debug output " << (enabled ? "enabled" : "disabled") << "\n";
        }
    }

    ENGINE_API void GetPhysicsMetrics(uint32_t* collisionChecks, uint32_t* triggerEvents) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !collisionChecks || !triggerEvents) {
            if (collisionChecks) *collisionChecks = 0;
            if (triggerEvents) *triggerEvents = 0;
            return;
        }

        auto* physicsSystem = g_engineInstance->GetWorld()->GetSystem<PhysicsSystem>();
        if (physicsSystem) {
            *collisionChecks = physicsSystem->GetCollisionChecksLastFrame();
            *triggerEvents = physicsSystem->GetTriggerEventsLastFrame();
        }
    }

    ENGINE_API void SetPhysicsGridCellSize(float cellSize) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) {
            std::cout << "[EngineAPI] Error: No engine or world available\n";
            return;
        }

        auto* physicsSystem = g_engineInstance->GetWorld()->GetSystem<PhysicsSystem>();
        if (physicsSystem) {
            physicsSystem->SetGridCellSize(cellSize);
            std::cout << "[EngineAPI] Physics grid cell size set to " << cellSize << "\n";
        }
    }

    ENGINE_API void GetPhysicsPerformanceStats(float* broadPhaseMs, float* narrowPhaseMs, float* totalMs) {
        if (!g_engineInstance || !g_engineInstance->GetWorld() || !broadPhaseMs || !narrowPhaseMs || !totalMs) {
            if (broadPhaseMs) *broadPhaseMs = 0.0f;
            if (narrowPhaseMs) *narrowPhaseMs = 0.0f;
            if (totalMs) *totalMs = 0.0f;
            return;
        }

        auto* physicsSystem = g_engineInstance->GetWorld()->GetSystem<PhysicsSystem>();
        if (physicsSystem) {
            *broadPhaseMs = physicsSystem->GetBroadPhaseTime();
            *narrowPhaseMs = physicsSystem->GetNarrowPhaseTime();
            *totalMs = *broadPhaseMs + *narrowPhaseMs;
        }
    }

    ENGINE_API void PrintPhysicsSystemStats() {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) {
            std::cout << "[EngineAPI] Error: No engine or world available\n";
            return;
        }

        auto* physicsSystem = g_engineInstance->GetWorld()->GetSystem<PhysicsSystem>();
        if (physicsSystem) {
            physicsSystem->PrintPhysicsStats();
        }
    }

    // NEW: Camera Entity Integration
    ENGINE_API bool SetCameraEntity(uint32_t entity) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) {
            std::cout << "[EngineAPI] Error: No engine or world available\n";
            return false;
        }

        auto* cameraMatrixSystem = g_engineInstance->GetWorld()->GetSystem<CameraMatrixSystem>();
        if (!cameraMatrixSystem) {
            std::cout << "[EngineAPI] Error: CameraMatrixSystem not found\n";
            return false;
        }

        cameraMatrixSystem->SetCameraEntity(entity);
        std::cout << "[EngineAPI] Camera entity set to " << entity << "\n";
        return true;
    }

    ENGINE_API uint32_t GetCameraEntity() {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return 0;

        auto* cameraMatrixSystem = g_engineInstance->GetWorld()->GetSystem<CameraMatrixSystem>();
        if (!cameraMatrixSystem) return 0;

        return cameraMatrixSystem->GetCameraEntity();
    }

    ENGINE_API void PrintCameraMatrices() {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) {
            std::cout << "[EngineAPI] Error: No engine or world available\n";
            return;
        }

        auto* cameraMatrixSystem = g_engineInstance->GetWorld()->GetSystem<CameraMatrixSystem>();
        if (cameraMatrixSystem) {
            cameraMatrixSystem->PrintCameraMatrices();
        } else {
            std::cout << "[EngineAPI] Error: CameraMatrixSystem not found\n";
        }
    }

    ENGINE_API bool Raycast(float originX, float originY, float originZ, 
                       float dirX, float dirY, float dirZ, 
                       float maxDistance,
                       uint32_t* hitEntity, 
                       float* hitX, float* hitY, float* hitZ,
                       float* hitDistance) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        
        auto* raycastSystem = g_engineInstance->GetWorld()->GetSystem<RaycastSystem>();
        if (!raycastSystem) {
            std::cout << "[EngineAPI] Error: RaycastSystem not found\n";
            return false;
        }
        
        Vector3 origin(originX, originY, originZ);
        Vector3 direction(dirX, dirY, dirZ);
        direction.Normalize();
        
        Ray ray(origin, direction, maxDistance);
        RaycastHit hit;
        
        if (raycastSystem->Raycast(ray, hit)) {
            if (hitEntity) *hitEntity = hit.entity;
            if (hitX) *hitX = hit.point.x;
            if (hitY) *hitY = hit.point.y;
            if (hitZ) *hitZ = hit.point.z;
            if (hitDistance) *hitDistance = hit.distance;
            return true;
        }
        
        return false;
    }

    ENGINE_API bool RaycastFromCamera(float dirX, float dirY, float dirZ,
                                    uint32_t* hitEntity,
                                    float* hitX, float* hitY, float* hitZ,
                                    float* hitDistance) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        
        auto* raycastSystem = g_engineInstance->GetWorld()->GetSystem<RaycastSystem>();
        if (!raycastSystem) return false;
        
        Vector3 direction(dirX, dirY, dirZ);
        RaycastHit hit;
        
        if (raycastSystem->RaycastFromCamera(direction, hit)) {
            if (hitEntity) *hitEntity = hit.entity;
            if (hitX) *hitX = hit.point.x;
            if (hitY) *hitY = hit.point.y;
            if (hitZ) *hitZ = hit.point.z;
            if (hitDistance) *hitDistance = hit.distance;
            return true;
        }
        
        return false;
    }

    ENGINE_API bool MousePick(int mouseX, int mouseY,
                            uint32_t* hitEntity,
                            float* hitX, float* hitY, float* hitZ,
                            float* hitDistance) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        
        auto* raycastSystem = g_engineInstance->GetWorld()->GetSystem<RaycastSystem>();
        if (!raycastSystem) return false;
        
        // TODO: Get actual screen dimensions
        int screenWidth = 1280;
        int screenHeight = 720;
        
        RaycastHit hit;
        if (raycastSystem->MousePick(mouseX, mouseY, screenWidth, screenHeight, hit)) {
            if (hitEntity) *hitEntity = hit.entity;
            if (hitX) *hitX = hit.point.x;
            if (hitY) *hitY = hit.point.y;
            if (hitZ) *hitZ = hit.point.z;
            if (hitDistance) *hitDistance = hit.distance;
            return true;
        }
        
        return false;
    }

    ENGINE_API bool LineOfSight(float fromX, float fromY, float fromZ,
                            float toX, float toY, float toZ) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        
        auto* raycastSystem = g_engineInstance->GetWorld()->GetSystem<RaycastSystem>();
        if (!raycastSystem) return false;
        
        Vector3 from(fromX, fromY, fromZ);
        Vector3 to(toX, toY, toZ);
        
        return raycastSystem->LineOfSight(from, to);
    }

    ENGINE_API bool LineOfSightBetweenEntities(uint32_t entityA, uint32_t entityB) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return false;
        
        auto* raycastSystem = g_engineInstance->GetWorld()->GetSystem<RaycastSystem>();
        if (!raycastSystem) return false;
        
        return raycastSystem->LineOfSightBetweenEntities(entityA, entityB);
    }

    ENGINE_API void SetRaycastDebugDraw(bool enabled) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) return;
        
        auto* raycastSystem = g_engineInstance->GetWorld()->GetSystem<RaycastSystem>();
        if (raycastSystem) {
            raycastSystem->SetDebugDrawEnabled(enabled);
            std::cout << "[EngineAPI] Raycast debug draw " << (enabled ? "enabled" : "disabled") << "\n";
        }
    }

    ENGINE_API void GetRaycastStats(uint32_t* raycastCount, uint32_t* aabbTestCount, float* lastRaycastMs) {
        if (!g_engineInstance || !g_engineInstance->GetWorld()) {
            if (raycastCount) *raycastCount = 0;
            if (aabbTestCount) *aabbTestCount = 0;
            if (lastRaycastMs) *lastRaycastMs = 0.0f;
            return;
        }
        
        auto* raycastSystem = g_engineInstance->GetWorld()->GetSystem<RaycastSystem>();
        if (raycastSystem) {
            if (raycastCount) *raycastCount = raycastSystem->GetRaycastsThisFrame();
            if (aabbTestCount) *aabbTestCount = raycastSystem->GetAABBTestsThisFrame();
            if (lastRaycastMs) *lastRaycastMs = raycastSystem->GetLastRaycastTime();
        }
    }
}
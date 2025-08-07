#pragma once

#include <stdint.h>

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

// Forward declaration only
class Engine;

// C-style API only
extern "C" {
    // Singleton Engine Management
    ENGINE_API bool CreateAndInitializeEngine();
    ENGINE_API void RunEngine();
    ENGINE_API void DestroyGlobalEngine();
	ENGINE_API void QuitGame();

    // Test functions
    ENGINE_API void PrintEngineVersion();
    ENGINE_API const char* GetEngineInfo();

    // Input API
    ENGINE_API bool IsKeyDown(int keyCode);
    ENGINE_API bool IsKeyPressed(int keyCode);
    ENGINE_API bool IsKeyReleased(int keyCode);
    ENGINE_API bool IsMouseButtonDown(int button);
    ENGINE_API bool IsMouseButtonPressed(int button);
    ENGINE_API bool IsMouseButtonReleased(int button);
    ENGINE_API void GetMousePosition(int* x, int* y);
    ENGINE_API int GetMouseWheelDelta();
    ENGINE_API void SetMouseSensitivity(float sensitivity);
    ENGINE_API void EnableMouseLock(bool enable);
    ENGINE_API bool IsMouseLocked();

    // Direct ECS API
    // ECS Interface - Entity management
    ENGINE_API uint32_t CreateEntity();
    ENGINE_API void DestroyEntity(uint32_t entity);
    ENGINE_API void SafeDestroyEntity(uint32_t entity);
    ENGINE_API bool IsValidEntity(uint32_t entity);
	ENGINE_API uint32_t GetEntityCount();

    // ECS Interface - Component management
    ENGINE_API bool AddTransformComponent(uint32_t entity, float x, float y, float z);
    ENGINE_API bool AddVelocityComponent(uint32_t entity, float vx, float vy, float vz);
    ENGINE_API bool AddCustomScript(uint32_t entity, void* scriptInstance);

    ENGINE_API bool GetTransformComponent(uint32_t entity, float* x, float* y, float* z);
    ENGINE_API bool GetVelocityComponent(uint32_t entity, float* vx, float* vy, float* vz);

    ENGINE_API void RemoveTransformComponent(uint32_t entity);
    ENGINE_API void RemoveVelocityComponent(uint32_t entity);
    ENGINE_API bool RemoveScript(uint32_t entity);

    // Enhanced Transform Component API
    ENGINE_API bool SetTransformPosition(uint32_t entity, float x, float y, float z);
    ENGINE_API bool SetTransformRotation(uint32_t entity, float pitch, float yaw, float roll);
    ENGINE_API bool SetTransformRotationDegrees(uint32_t entity, float pitchDeg, float yawDeg, float rollDeg);
    ENGINE_API bool SetTransformScale(uint32_t entity, float scaleX, float scaleY, float scaleZ);
    ENGINE_API bool SetTransformUniformScale(uint32_t entity, float scale);

    ENGINE_API bool GetTransformRotation(uint32_t entity, float* pitch, float* yaw, float* roll);
    ENGINE_API bool GetTransformScale(uint32_t entity, float* scaleX, float* scaleY, float* scaleZ);

    ENGINE_API bool TranslateTransform(uint32_t entity, float deltaX, float deltaY, float deltaZ);
    ENGINE_API bool RotateTransform(uint32_t entity, float deltaPitch, float deltaYaw, float deltaRoll);
    ENGINE_API bool RotateTransformDegrees(uint32_t entity, float deltaPitchDeg, float deltaYawDeg, float deltaRollDeg);

    // ECS Interface - World management
    ENGINE_API uint32_t GetEntityCount();

    // MeshRenderer Component API
    ENGINE_API bool AddMeshRendererComponent(uint32_t entity, int primitiveType, const char* shaderName);
    ENGINE_API bool AddCustomMeshRendererComponent(uint32_t entity, const char* customMeshName, const char* shaderName);
    ENGINE_API bool SetMeshRendererColor(uint32_t entity, float r, float g, float b, float a);
    ENGINE_API bool SetMeshRendererVisible(uint32_t entity, bool visible);
    ENGINE_API bool SetMeshRendererShader(uint32_t entity, const char* shaderName);
    ENGINE_API bool SetMeshRendererMaterial(uint32_t entity, const char* materialName);
    ENGINE_API void RemoveMeshRendererComponent(uint32_t entity);

    // Primitive mesh type constants for the API
    #define PRIMITIVE_QUAD 0
    #define PRIMITIVE_CUBE 1
    #define PRIMITIVE_SPHERE 2
    #define PRIMITIVE_CYLINDER 3
    #define PRIMITIVE_PLANE 4
    #define PRIMITIVE_TRIANGLE 5
    #define PRIMITIVE_CUSTOM 6

    // Camera System API
    ENGINE_API bool CreateCamera(const char* cameraName, float fovDegrees, float aspectRatio, float nearPlane, float farPlane);
    ENGINE_API bool AttachCameraToEntity(const char* cameraName, uint32_t entity);
    ENGINE_API bool DetachCamera(const char* cameraName);
    ENGINE_API bool SetCameraProjection(const char* cameraName, float fovDegrees, float aspectRatio, float nearPlane, float farPlane);
    ENGINE_API bool SetCameraMovementSpeed(const char* cameraName, float speed);
    ENGINE_API bool SetCameraMouseSensitivity(const char* cameraName, float sensitivity);
    ENGINE_API bool GetCameraPosition(const char* cameraName, float* x, float* y, float* z);
    ENGINE_API bool GetCameraRotation(const char* cameraName, float* pitch, float* yaw, float* roll);
    ENGINE_API bool SetActiveCamera(const char* cameraName);
    ENGINE_API const char* GetActiveCameraName();

	//Physics and Collision API
    // BoxCollider Component
    ENGINE_API bool AddBoxColliderComponent(uint32_t entity, float width, float height, float depth, bool isTrigger);
	ENGINE_API bool SetBoxColliderSize(uint32_t entity, float width, float height, float depth);
	ENGINE_API bool SetBoxColliderTrigger(uint32_t entity, bool isTrigger);
	ENGINE_API bool SetBoxColliderEnabled(uint32_t entity, bool enabled);
	ENGINE_API void RemoveBoxColliderComponent(uint32_t entity);

    // Physics System Control
    ENGINE_API void SetPhysicsDebugOutput(bool enabled);
	ENGINE_API void GetPhysicsMetrics(uint32_t* collisionChecks, uint32_t* triggerEvents);
    ENGINE_API void SetPhysicsGridCellSize(float cellSize);
    ENGINE_API void GetPhysicsPerformanceStats(float* broadPhaseMs, float* narrowPhaseMs, float* totalMs);
    ENGINE_API void PrintPhysicsSystemStats();

    // Camera Entity Integration (Add this to the existing API)
    ENGINE_API bool SetCameraEntity(uint32_t entity);
    ENGINE_API uint32_t GetCameraEntity();
    ENGINE_API void PrintCameraMatrices();

    // Raycasting API
    ENGINE_API bool Raycast(float originX, float originY, float originZ, 
                       float dirX, float dirY, float dirZ, 
                       float maxDistance,
                       uint32_t* hitEntity, 
                       float* hitX, float* hitY, float* hitZ,
                       float* hitDistance);

    ENGINE_API bool RaycastFromCamera(float dirX, float dirY, float dirZ,
                                    uint32_t* hitEntity,
                                    float* hitX, float* hitY, float* hitZ,
                                    float* hitDistance);

    ENGINE_API bool MousePick(int mouseX, int mouseY,
                            uint32_t* hitEntity,
                            float* hitX, float* hitY, float* hitZ,
                            float* hitDistance);

    ENGINE_API bool LineOfSight(float fromX, float fromY, float fromZ,
                            float toX, float toY, float toZ);

    ENGINE_API bool LineOfSightBetweenEntities(uint32_t entityA, uint32_t entityB);

    ENGINE_API void SetRaycastDebugDraw(bool enabled);
    ENGINE_API void GetRaycastStats(uint32_t* raycastCount, uint32_t* aabbTestCount, float* lastRaycastMs);
}
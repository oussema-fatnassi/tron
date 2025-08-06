#pragma once
#include "TronEngine.hpp"
#include <iostream>
#include <iomanip>

class RaycastTestScript : public ScriptBase {
private:
    std::string scriptName;
    float shootCooldown = 0.0f;
    const float shootDelay = 0.5f; // Shoot every 0.5 seconds
    
    // For visual feedback
    uint32_t lastHitEntity = 0;
    float hitHighlightTime = 0.0f;

public:
    RaycastTestScript(const std::string& name = "RaycastTest")
        : scriptName(name) {}

    void Start() override {
        std::cout << "\n[" << scriptName << "] START() - Raycast test script initialized on entity " << entity << "\n";
        std::cout << "[" << scriptName << "] Controls:\n";
        std::cout << "  - Left Click: Shoot ray from camera\n";
        std::cout << "  - Right Click: Pick object with mouse\n";
        std::cout << "  - R: Toggle raycast debug visualization\n";
        std::cout << "  - L: Test line of sight to all entities\n";
        
        // Enable raycast debug visualization
        SetRaycastDebugDraw(true);
    }

    void Update(float deltaTime) override {
        // Update cooldowns
        if (shootCooldown > 0.0f) {
            shootCooldown -= deltaTime;
        }
        
        if (hitHighlightTime > 0.0f) {
            hitHighlightTime -= deltaTime;
            
            // Reset highlight when time expires
            if (hitHighlightTime <= 0.0f && lastHitEntity != 0) {
                SetMeshRendererColor(lastHitEntity, 1.0f, 1.0f, 1.0f, 1.0f); // Reset to white
                lastHitEntity = 0;
            }
        }
        
        // Left click - shoot ray from camera forward
        if (IsMouseButtonPressed(0) && shootCooldown <= 0.0f) {
            ShootRayFromCamera();
            shootCooldown = shootDelay;
        }
        
        // Right click - mouse pick
        if (IsMouseButtonPressed(1)) {
            MousePickTest();
        }
        
        // R key - toggle debug visualization
        if (IsKeyPressed('R') || IsKeyPressed('r')) {
            static bool debugEnabled = true;
            debugEnabled = !debugEnabled;
            SetRaycastDebugDraw(debugEnabled);
            std::cout << "[" << scriptName << "] Raycast debug draw " 
                      << (debugEnabled ? "ENABLED" : "DISABLED") << "\n";
        }
        
        // L key - line of sight test
        if (IsKeyPressed('L') || IsKeyPressed('l')) {
            TestLineOfSight();
        }
        
        // P key - print raycast stats
        if (IsKeyPressed('P') || IsKeyPressed('p')) {
            PrintRaycastStats();
        }
    }

    void OnDestroy() override {
        std::cout << "[" << scriptName << "] OnDestroy() - Raycast test script cleanup\n";
    }

private:
    void ShootRayFromCamera() {
        std::cout << "\n[" << scriptName << "] === SHOOTING RAY FROM CAMERA ===\n";
        
        // Shoot ray forward from camera
        uint32_t hitEntity = 0;
        float hitX, hitY, hitZ, hitDistance;
        
        // Forward direction (camera looks toward -Z)
        if (RaycastFromCamera(0.0f, 0.0f, -1.0f, &hitEntity, &hitX, &hitY, &hitZ, &hitDistance)) {
            std::cout << "[" << scriptName << "] HIT! Entity: " << hitEntity << "\n";
            std::cout << "  Position: (" << std::fixed << std::setprecision(2) 
                      << hitX << ", " << hitY << ", " << hitZ << ")\n";
            std::cout << "  Distance: " << hitDistance << " units\n";
            
            // Highlight the hit entity
            HighlightEntity(hitEntity);
        } else {
            std::cout << "[" << scriptName << "] No hit detected\n";
        }
    }
    
    void MousePickTest() {
        std::cout << "\n[" << scriptName << "] === MOUSE PICK TEST ===\n";
        
        int mouseX, mouseY;
        GetMousePosition(&mouseX, &mouseY);
        
        uint32_t hitEntity = 0;
        float hitX, hitY, hitZ, hitDistance;
        
        if (MousePick(mouseX, mouseY, &hitEntity, &hitX, &hitY, &hitZ, &hitDistance)) {
            std::cout << "[" << scriptName << "] PICKED! Entity: " << hitEntity << "\n";
            std::cout << "  Mouse: (" << mouseX << ", " << mouseY << ")\n";
            std::cout << "  World: (" << std::fixed << std::setprecision(2) 
                      << hitX << ", " << hitY << ", " << hitZ << ")\n";
            std::cout << "  Distance: " << hitDistance << " units\n";
            
            // Highlight the picked entity
            HighlightEntity(hitEntity);
        } else {
            std::cout << "[" << scriptName << "] No entity at mouse position\n";
        }
    }
    
    void TestLineOfSight() {
        std::cout << "\n[" << scriptName << "] === LINE OF SIGHT TEST ===\n";
        
        // Get camera position
        float camX, camY, camZ;
        if (!GetTransformComponent(entity, &camX, &camY, &camZ)) {
            return;
        }
        
        // Test line of sight to all entities
        uint32_t totalEntities = GetEntityCount();
        int blockedCount = 0;
        int clearCount = 0;
        
        for (uint32_t i = 1; i <= totalEntities; i++) {
            if (i == entity || !IsValidEntity(i)) continue;
            
            float targetX, targetY, targetZ;
            if (GetTransformComponent(i, &targetX, &targetY, &targetZ)) {
                bool hasLOS = LineOfSight(camX, camY, camZ, targetX, targetY, targetZ);
                
                if (hasLOS) {
                    clearCount++;
                    std::cout << "  Entity " << i << ": CLEAR line of sight\n";
                } else {
                    blockedCount++;
                    std::cout << "  Entity " << i << ": BLOCKED\n";
                }
            }
        }
        
        std::cout << "[" << scriptName << "] Summary: " << clearCount << " clear, " 
                  << blockedCount << " blocked\n";
    }
    
    void HighlightEntity(uint32_t targetEntity) {
        // Reset previous highlight
        if (lastHitEntity != 0 && lastHitEntity != targetEntity) {
            SetMeshRendererColor(lastHitEntity, 1.0f, 1.0f, 1.0f, 1.0f); // White
        }
        
        // Highlight new target in yellow
        SetMeshRendererColor(targetEntity, 1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        lastHitEntity = targetEntity;
        hitHighlightTime = 2.0f; // Highlight for 2 seconds
    }
    
    void PrintRaycastStats() {
        uint32_t raycastCount, aabbTests;
        float lastMs;
        GetRaycastStats(&raycastCount, &aabbTests, &lastMs);
        
        std::cout << "\n[" << scriptName << "] === RAYCAST STATS ===\n";
        std::cout << "  Raycasts this frame: " << raycastCount << "\n";
        std::cout << "  AABB tests: " << aabbTests << "\n";
        std::cout << "  Last raycast time: " << lastMs << "ms\n";
        std::cout << "=================================\n";
    }
};
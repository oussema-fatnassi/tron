// TestConsole/RaycastTestScript.hpp
// Comprehensive test script for raycast functionality
#pragma once
#include "TronEngine.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

class RaycastTestScript : public ScriptBase {
private:
    std::string scriptName;
    float testTimer = 0.0f;
    int currentTest = 0;
    
    // Test entities
    uint32_t targetBox1 = 0;
    uint32_t targetBox2 = 0;
    uint32_t targetBox3 = 0;
    
public:
    RaycastTestScript(const std::string& name = "RaycastTester")
        : scriptName(name) {
    }

    void Start() override {
        std::cout << "\n[" << scriptName << "] ========== RAYCAST TEST STARTED ==========\n";
        
        // Enable raycast debug visualization
        SetRaycastDebugDraw(true);
        
        std::cout << "[" << scriptName << "] Test scene created. Press keys to test:\n";
        std::cout << "  [1] - Test forward raycast\n";
        std::cout << "  [2] - Test raycast in all directions\n";
        std::cout << "  [3] - Test mouse picking (click to test)\n";
        std::cout << "  [4] - Test line of sight between entities\n";
        std::cout << "  [5] - Test raycast from camera\n";
        std::cout << "  [R] - Print raycast statistics\n";
        std::cout << "  [T] - Toggle debug visualization\n";
        std::cout << "=================================================\n\n";
    }

    void Update(float deltaTime) override {
        testTimer += deltaTime;
        
        // Key 1: Test forward raycast
        if (IsKeyPressed('1')) {
            TestForwardRaycast();
        }
        
        // Key 2: Test omnidirectional raycast
        if (IsKeyPressed('2')) {
            TestOmnidirectionalRaycast();
        }
        
        // Key 3: Test mouse picking
        if (IsKeyPressed('3') || IsMouseButtonPressed(0)) {
            TestMousePicking();
        }
        
        // Key 4: Test line of sight
        if (IsKeyPressed('4')) {
            TestLineOfSight();
        }
        
        // Key 5: Test raycast from camera
        if (IsKeyPressed('5')) {
            TestRaycastFromCamera();
        }
        
        // Key R: Print statistics
        if (IsKeyPressed('R')) {
            PrintRaycastStats();
        }
        
        // Key T: Toggle debug visualization
        if (IsKeyPressed('T')) {
            static bool debugEnabled = true;
            debugEnabled = !debugEnabled;
            SetRaycastDebugDraw(debugEnabled);
            std::cout << "[" << scriptName << "] Debug visualization: " 
                      << (debugEnabled ? "ENABLED" : "DISABLED") << "\n";
        }
        
        // Continuous rotation test (automatic)
        if (testTimer > 5.0f) {
            // Every 5 seconds, do an automatic sweep test
            //AutomaticSweepTest();
            testTimer = 0.0f;
        }
    }

    void OnDestroy() override {
        std::cout << "[" << scriptName << "] Raycast test completed\n";
    }

private:
    void CreateTestTargets() {
        std::cout << "[" << scriptName << "] Creating test targets...\n";
        
        // Create target box 1: In front
        targetBox1 = CreateEntity();
        AddTransformComponent(targetBox1, 0.0f, 2.0f, -5.0f);
        AddBoxColliderComponent(targetBox1, 2.0f, 2.0f, 2.0f, false);
        AddMeshRendererComponent(targetBox1, PRIMITIVE_CUBE, "blue");
        SetTransformUniformScale(targetBox1, 2.0f);
        std::cout << "  Target 1 (RED): Position (0, 2, -5), Size 2x2x2\n";
        
        // Create target box 2: To the right
        targetBox2 = CreateEntity();
        AddTransformComponent(targetBox2, 5.0f, 2.0f, -3.0f);
        AddBoxColliderComponent(targetBox2, 1.5f, 3.0f, 1.5f, false);
        AddMeshRendererComponent(targetBox2, PRIMITIVE_CUBE, "blue");
        SetTransformScale(targetBox2, 1.5f, 3.0f, 1.5f);
        std::cout << "  Target 2 (GREEN): Position (5, 2, -3), Size 1.5x3x1.5\n";
        
        // Create target box 3: Above
        targetBox3 = CreateEntity();
        AddTransformComponent(targetBox3, -3.0f, 5.0f, -4.0f);
        AddBoxColliderComponent(targetBox3, 1.0f, 1.0f, 1.0f, false);
        AddMeshRendererComponent(targetBox3, PRIMITIVE_CUBE, "blue");
        std::cout << "  Target 3 (BLUE): Position (-3, 5, -4), Size 1x1x1\n";
    }
    
    void TestForwardRaycast() {
        std::cout << "\n[TEST 1] FORWARD RAYCAST TEST\n";
        std::cout << "----------------------------------------\n";
        
        // Get player position
        float px, py, pz;
        if (!GetTransformComponent(entity, &px, &py, &pz)) {
            std::cout << "ERROR: Could not get player position\n";
            return;
        }
        
        // Cast ray forward (negative Z direction)
        float dirX = 0.0f, dirY = 0.0f, dirZ = -1.0f;
        float maxDistance = 20.0f;
        
        uint32_t hitEntity = 0;
        float hitX, hitY, hitZ, hitDistance;
        
        std::cout << "Casting ray from (" << px << ", " << py << ", " << pz << ")\n";
        std::cout << "Direction: (" << dirX << ", " << dirY << ", " << dirZ << ")\n";
        std::cout << "Max distance: " << maxDistance << "\n";
        
        bool hit = Raycast(px, py, pz, dirX, dirY, dirZ, maxDistance,
                           &hitEntity, &hitX, &hitY, &hitZ, &hitDistance);
        
        if (hit) {
            std::cout << "✓ HIT DETECTED!\n";
            std::cout << "  Entity: " << hitEntity;
            if (hitEntity == targetBox1) std::cout << " (Target Box 1 - RED)";
            else if (hitEntity == targetBox2) std::cout << " (Target Box 2 - GREEN)";
            else if (hitEntity == targetBox3) std::cout << " (Target Box 3 - BLUE)";
            std::cout << "\n";
            std::cout << "  Hit point: (" << hitX << ", " << hitY << ", " << hitZ << ")\n";
            std::cout << "  Distance: " << hitDistance << "\n";
        } else {
            std::cout << "✗ No hit detected\n";
        }
        std::cout << "----------------------------------------\n";
    }
    
    void TestOmnidirectionalRaycast() {
        std::cout << "\n[TEST 2] OMNIDIRECTIONAL RAYCAST TEST\n";
        std::cout << "----------------------------------------\n";
        
        float px, py, pz;
        if (!GetTransformComponent(entity, &px, &py, &pz)) return;
        
        int numRays = 8;
        int hits = 0;
        
        std::cout << "Casting " << numRays << " rays in a circle from player position\n";
        
        for (int i = 0; i < numRays; i++) {
            float angle = (2.0f * 3.14159f * i) / numRays;
            float dirX = cosf(angle);
            float dirZ = sinf(angle);
            
            uint32_t hitEntity;
            float hitX, hitY, hitZ, hitDistance;
            
            if (Raycast(px, py, pz, dirX, 0.0f, dirZ, 10.0f,
                       &hitEntity, &hitX, &hitY, &hitZ, &hitDistance)) {
                hits++;
                std::cout << "  Ray " << i << " (angle " << (angle * 180.0f / 3.14159f) 
                         << "°): HIT entity " << hitEntity 
                         << " at distance " << hitDistance << "\n";
            }
        }
        
        std::cout << "Total hits: " << hits << "/" << numRays << "\n";
        std::cout << "----------------------------------------\n";
    }
    
    void TestMousePicking() {
        std::cout << "\n[TEST 3] MOUSE PICKING TEST\n";
        std::cout << "----------------------------------------\n";
        
        int mouseX, mouseY;
        GetMousePosition(&mouseX, &mouseY);
        
        std::cout << "Mouse position: (" << mouseX << ", " << mouseY << ")\n";
        
        uint32_t hitEntity;
        float hitX, hitY, hitZ, hitDistance;
        
        bool hit = MousePick(mouseX, mouseY, &hitEntity, &hitX, &hitY, &hitZ, &hitDistance);
        
        if (hit) {
            std::cout << "✓ PICKED ENTITY!\n";
            std::cout << "  Entity: " << hitEntity;
            if (hitEntity == targetBox1) std::cout << " (Target Box 1 - RED)";
            else if (hitEntity == targetBox2) std::cout << " (Target Box 2 - GREEN)";
            else if (hitEntity == targetBox3) std::cout << " (Target Box 3 - BLUE)";
            std::cout << "\n";
            std::cout << "  World position: (" << hitX << ", " << hitY << ", " << hitZ << ")\n";
            std::cout << "  Distance from camera: " << hitDistance << "\n";
            
            // Visual feedback: make the picked entity flash
            static float originalColor[3];
            static uint32_t lastPickedEntity = 0;
            
            if (lastPickedEntity != hitEntity) {
                // Restore previous entity color
                if (lastPickedEntity != 0) {
                    SetMeshRendererColor(lastPickedEntity, 
                                        originalColor[0], originalColor[1], originalColor[2], 1.0f);
                }
                lastPickedEntity = hitEntity;
            }
            
            // Flash white briefly
            SetMeshRendererColor(hitEntity, 1.0f, 1.0f, 1.0f, 1.0f);
            SafeDestroyEntity(hitEntity);
        } else {
            std::cout << "✗ No entity under mouse cursor\n";
        }
        std::cout << "----------------------------------------\n";
    }
    
    void TestLineOfSight() {
        std::cout << "\n[TEST 4] LINE OF SIGHT TEST\n";
        std::cout << "----------------------------------------\n";
        
        // Test line of sight between target boxes
        bool los12 = LineOfSightBetweenEntities(targetBox1, targetBox2);
        bool los13 = LineOfSightBetweenEntities(targetBox1, targetBox3);
        bool los23 = LineOfSightBetweenEntities(targetBox2, targetBox3);
        
        std::cout << "Line of sight between:\n";
        std::cout << "  Box 1 (RED) <-> Box 2 (GREEN): " << (los12 ? "CLEAR" : "BLOCKED") << "\n";
        std::cout << "  Box 1 (RED) <-> Box 3 (BLUE): " << (los13 ? "CLEAR" : "BLOCKED") << "\n";
        std::cout << "  Box 2 (GREEN) <-> Box 3 (BLUE): " << (los23 ? "CLEAR" : "BLOCKED") << "\n";
        
        // Test specific points
        float px, py, pz;
        GetTransformComponent(entity, &px, &py, &pz);
        
        bool losToOrigin = LineOfSight(px, py, pz, 0.0f, 0.0f, 0.0f);
        std::cout << "  Player to origin (0,0,0): " << (losToOrigin ? "CLEAR" : "BLOCKED") << "\n";
        std::cout << "----------------------------------------\n";
    }
    
    void TestRaycastFromCamera() {
        std::cout << "\n[TEST 5] RAYCAST FROM CAMERA TEST\n";
        std::cout << "----------------------------------------\n";
        
        // Cast ray from camera in view direction
        uint32_t hitEntity;
        float hitX, hitY, hitZ, hitDistance;
        
        // Camera forward is typically negative Z
        bool hit = RaycastFromCamera(0.0f, 0.0f, -1.0f,
                                     &hitEntity, &hitX, &hitY, &hitZ, &hitDistance);
        
        if (hit) {
            std::cout << "✓ Camera ray hit!\n";
            std::cout << "  Entity: " << hitEntity << "\n";
            std::cout << "  Hit point: (" << hitX << ", " << hitY << ", " << hitZ << ")\n";
            std::cout << "  Distance: " << hitDistance << "\n";
        } else {
            std::cout << "✗ Camera ray didn't hit anything\n";
        }
        std::cout << "----------------------------------------\n";
    }
    
    void AutomaticSweepTest() {
        std::cout << "\n[AUTO TEST] SWEEP RAYCAST (360°)\n";
        std::cout << "----------------------------------------\n";
        
        float px, py, pz;
        if (!GetTransformComponent(entity, &px, &py, &pz)) return;
        
        int numRays = 36; // Every 10 degrees
        int hits = 0;
        float closestDistance = 1000.0f;
        uint32_t closestEntity = 0;
        
        for (int i = 0; i < numRays; i++) {
            float angle = (2.0f * 3.14159f * i) / numRays;
            float dirX = cosf(angle);
            float dirZ = sinf(angle);
            
            uint32_t hitEntity;
            float hitX, hitY, hitZ, hitDistance;
            
            if (Raycast(px, py, pz, dirX, 0.0f, dirZ, 20.0f,
                       &hitEntity, &hitX, &hitY, &hitZ, &hitDistance)) {
                hits++;
                if (hitDistance < closestDistance) {
                    closestDistance = hitDistance;
                    closestEntity = hitEntity;
                }
            }
        }
        
        std::cout << "360° sweep complete: " << hits << " hits out of " << numRays << " rays\n";
        if (closestEntity != 0) {
            std::cout << "Closest entity: " << closestEntity << " at distance " << closestDistance << "\n";
        }
        std::cout << "----------------------------------------\n";
    }
    
    void PrintRaycastStats() {
        std::cout << "\n[STATS] RAYCAST PERFORMANCE\n";
        std::cout << "----------------------------------------\n";
        
        uint32_t raycastCount, aabbTestCount;
        float lastRaycastMs;
        
        GetRaycastStats(&raycastCount, &aabbTestCount, &lastRaycastMs);
        
        std::cout << "This frame statistics:\n";
        std::cout << "  Raycasts performed: " << raycastCount << "\n";
        std::cout << "  AABB tests: " << aabbTestCount << "\n";
        std::cout << "  Last raycast time: " << std::fixed << std::setprecision(3) 
                  << lastRaycastMs << " ms\n";
        
        if (raycastCount > 0 && aabbTestCount > 0) {
            float efficiency = (float)aabbTestCount / raycastCount;
            std::cout << "  Average AABB tests per ray: " << std::fixed << std::setprecision(1) 
                      << efficiency << "\n";
        }
        
        // Also print physics stats for comparison
        uint32_t collisionChecks, triggerEvents;
        GetPhysicsMetrics(&collisionChecks, &triggerEvents);
        std::cout << "\nPhysics system (for comparison):\n";
        std::cout << "  Collision checks: " << collisionChecks << "\n";
        std::cout << "  Trigger events: " << triggerEvents << "\n";
        
        std::cout << "----------------------------------------\n";
    }
};
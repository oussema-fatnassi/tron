// TestConsole/PhysicsTestScript.hpp
// CREATE THIS NEW FILE FOR TESTING PHYSICS

#pragma once
#include "TronEngine.hpp"
#include <iostream>

class PhysicsTestScript : public ScriptBase {
private:
    std::string scriptName;
    bool isTriggerZone;
    bool beDestroyed = false;

public:
    PhysicsTestScript(const std::string& name, bool triggerZone = false)
        : scriptName(name), isTriggerZone(triggerZone) {
    }

    void Start() override {
        std::cout << "[" << scriptName << "] START() - Setting up physics test entity " << entity << "\n";

        // Add transform
        AddTransformComponent(entity, 0.0f, 0.0f, 0.0f);

        if (isTriggerZone) {
            // Create a trigger zone (invisible, just for collision detection)
            AddBoxColliderComponent(entity, 1.0f, 1.0f, 1.0f, true); // Large trigger zone
            AddMeshRendererComponent(entity, PRIMITIVE_CUBE, "blue");
            std::cout << "[" << scriptName << "] Created trigger zone (3x3x3)\n";
        }
        else {
            // Create a moving object with collider
            AddBoxColliderComponent(entity, 1.0f, 1.0f, 1.0f, false); // Solid collider
            AddMeshRendererComponent(entity, PRIMITIVE_CUBE, "blue");
            std::cout << "[" << scriptName << "] Created solid object with visual (1x1x1)\n";
        }
    }

    void Update(float deltaTime) override {
        if (!isTriggerZone) {
            // Move the object in a circle to test trigger events
            static float time = 0.0f;
            time += deltaTime;

            float radius = 4.0f;
            float newX = radius * cosf(time * 0.5f);
            //float newZ = radius * sinf(time * 0.5f);

            SetTransformPosition(entity, newX, 0.0f, 0.0f);
        }
    }

    void OnTriggerEnter(uint32_t otherEntity) override {
        std::cout << "[" << scriptName << "] TRIGGER ENTER! Entity " << entity
            << " detected entity " << otherEntity << " entering\n";
		SafeDestroyEntity(entity);
    }

    void OnTriggerExit(uint32_t otherEntity) override {
        std::cout << "[" << scriptName << "] TRIGGER EXIT! Entity " << entity
            << " detected entity " << otherEntity << " leaving\n";
		
		
    }

    void OnDestroy() override {
        std::cout << "[" << scriptName << "] OnDestroy() called\n";
    }
};
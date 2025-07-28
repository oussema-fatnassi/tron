// Source/Game/ScriptSystem.cpp - Simple lifecycle version (NO THREADING)
#include "../../Headers/Game/ScriptSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/ScriptComponent.hpp"
#include <iostream>

// Your existing Update method - UNCHANGED!
void ScriptSystem::Update(float deltaTime) {
    for (Entity entity : entities) {
        auto* script = world->GetComponent<Script>(entity);
        if (script) {
            script->Update(deltaTime);  // This now handles Start() automatically
        }
    }
}

void ScriptSystem::OnEntityAdded(Entity entity) {
    std::cout << "[ScriptSystem] Script ADDED to entity " << entity << std::endl;

    // Immediately call Start() when script is added
    auto* script = world->GetComponent<Script>(entity);
    if (script && script->userScript) {
        script->EnsureStarted();  // Call Start() immediately
        std::cout << "[ScriptSystem] Start() called immediately for entity " << entity << std::endl;
    }
}
 
void ScriptSystem::OnEntityRemoved(Entity entity) {
    std::cout << "[ScriptSystem] Script REMOVED from entity " << entity << std::endl;

    // OnDestroy() will be called automatically by Script destructor
    // This event is just for notification/logging
    std::cout << "[ScriptSystem] OnDestroy() will be called automatically" << std::endl;
}
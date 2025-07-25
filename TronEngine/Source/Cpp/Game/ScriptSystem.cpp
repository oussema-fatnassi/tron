#include "../../Headers/Game/ScriptSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/ScriptComponent.hpp"

void ScriptSystem::Update(float deltaTime) {
    for (Entity entity : entities) {
        auto* script = world->GetComponent<Script>(entity);
        if (script) {
            script->Update(deltaTime);  // Calls the user's Update method
        }
    }
}
// REPLACE YOUR EXISTING PhysicsSystem.cpp WITH THIS FIXED VERSION
// TronEngine/Source/Cpp/Game/PhysicsSystem.cpp

#include "../../Headers/Game/PhysicsSystem.hpp"
#include "../../Headers/Game/World.hpp"
#include "../../Headers/Game/TransformComponent.hpp"
#include "../../Headers/Game/BoxColliderComponent.hpp"
#include "../../Headers/Game/ScriptComponent.hpp"
#include <iostream>

PhysicsSystem::PhysicsSystem() {
    std::cout << "[PhysicsSystem] Single-threaded physics system initialized\n";
}

void PhysicsSystem::Update(float deltaTime) {
    // Reset performance counters
    collisionChecksThisFrame = 0;
    triggerEventsThisFrame = 0;

    // IMPORTANT: Clear current triggers BEFORE collision detection
    // This ensures we start fresh each frame
    for (Entity entity : entities) {
        auto* collider = world->GetComponent<BoxCollider>(entity);
        if (collider && collider->isTrigger) {
            collider->currentTriggers.clear();
        }
    }

    // Simple O(n²) collision detection - check every entity against every other entity
    for (size_t i = 0; i < entities.size(); ++i) {
        Entity entityA = entities[i];

        for (size_t j = i + 1; j < entities.size(); ++j) {
            Entity entityB = entities[j];

            collisionChecksThisFrame++;

            // Check if these two entities are colliding
            if (CheckAABBCollision(entityA, entityB)) {
                auto* colliderA = world->GetComponent<BoxCollider>(entityA);
                auto* colliderB = world->GetComponent<BoxCollider>(entityB);

                // FIXED: Track overlaps for BOTH entities if they have trigger colliders
                if (colliderA && colliderA->isTrigger) {
                    colliderA->currentTriggers.insert(entityB);
                }
                if (colliderB && colliderB->isTrigger) {
                    colliderB->currentTriggers.insert(entityA);
                }
            }
        }
    }

    // Process trigger enter/exit events AFTER all collision detection is complete
    ProcessTriggerEvents();

    // Debug output every 60 frames (roughly once per second at 60 FPS)
    if (debugOutput && ++debugFrameCounter >= 60) {
        std::cout << "[PhysicsSystem] Entities: " << entities.size()
            << ", Collision checks: " << collisionChecksThisFrame
            << ", Trigger events: " << triggerEventsThisFrame << "\n";
        debugFrameCounter = 0;
    }
}

bool PhysicsSystem::CheckAABBCollision(Entity entityA, Entity entityB) {
    // Get components for both entities
    auto* transformA = world->GetComponent<Transform>(entityA);
    auto* transformB = world->GetComponent<Transform>(entityB);
    auto* colliderA = world->GetComponent<BoxCollider>(entityA);
    auto* colliderB = world->GetComponent<BoxCollider>(entityB);

    // Validate that both entities have required components and are enabled
    if (!transformA || !transformB || !colliderA || !colliderB ||
        !colliderA->isEnabled || !colliderB->isEnabled) {
        return false;
    }

    // Get world-space AABB bounds for both entities
    float minAX, minAY, minAZ, maxAX, maxAY, maxAZ;
    float minBX, minBY, minBZ, maxBX, maxBY, maxBZ;

    GetWorldAABB(entityA, minAX, minAY, minAZ, maxAX, maxAY, maxAZ);
    GetWorldAABB(entityB, minBX, minBY, minBZ, maxBX, maxBY, maxBZ);

    // AABB vs AABB collision test using Separating Axis Theorem
    // If boxes are separated on ANY axis, they don't collide
    bool separatedOnX = (maxAX < minBX) || (minAX > maxBX);
    bool separatedOnY = (maxAY < minBY) || (minAY > maxBY);
    bool separatedOnZ = (maxAZ < minBZ) || (minAZ > maxBZ);

    // Collision occurs if NOT separated on any axis
    return !(separatedOnX || separatedOnY || separatedOnZ);
}

void PhysicsSystem::GetWorldAABB(Entity entity, float& minX, float& minY, float& minZ,
    float& maxX, float& maxY, float& maxZ) {
    auto* transform = world->GetComponent<Transform>(entity);
    auto* collider = world->GetComponent<BoxCollider>(entity);

    // Calculate world-space center
    float centerX = transform->x;
    float centerY = transform->y;
    float centerZ = transform->z;

    // Calculate half-extents (affected by scale)
    float halfWidth = (collider->width * 0.5f) * transform->scaleX;
    float halfHeight = (collider->height * 0.5f) * transform->scaleY;
    float halfDepth = (collider->depth * 0.5f) * transform->scaleZ;

    // Calculate world-space AABB bounds
    minX = centerX - halfWidth;
    minY = centerY - halfHeight;
    minZ = centerZ - halfDepth;
    maxX = centerX + halfWidth;
    maxY = centerY + halfHeight;
    maxZ = centerZ + halfDepth;
}

void PhysicsSystem::ProcessTriggerEvents() {
    for (Entity entity : entities) {
        auto* collider = world->GetComponent<BoxCollider>(entity);

        if (!collider || !collider->isTrigger) {
            continue; // Skip non-trigger colliders
        }

        // Find new trigger enters (entities that are in current but not in previous)
        for (Entity otherEntity : collider->currentTriggers) {
            if (collider->previousTriggers.find(otherEntity) == collider->previousTriggers.end()) {
                // This is a new trigger enter event
                DispatchTriggerEnter(entity, otherEntity);
            }
        }

        // Find trigger exits (entities that were in previous but not in current)
        for (Entity otherEntity : collider->previousTriggers) {
            if (collider->currentTriggers.find(otherEntity) == collider->currentTriggers.end()) {
                // This entity has left the trigger
                DispatchTriggerExit(entity, otherEntity);
            }
        }

        // CRITICAL: Update previous frame data for next frame's comparison
        collider->previousTriggers = collider->currentTriggers;
        // NOTE: We don't clear currentTriggers here - it's cleared at the start of Update()
    }
}

void PhysicsSystem::DispatchTriggerEnter(Entity triggerEntity, Entity otherEntity) {
    auto* script = world->GetComponent<Script>(triggerEntity);

    if (script && script->userScript) {
        try {
            script->userScript->OnTriggerEnter(otherEntity);
            triggerEventsThisFrame++;

            std::cout << "[PhysicsSystem] TriggerEnter: Entity " << triggerEntity
                << " <- Entity " << otherEntity << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "[PhysicsSystem] ERROR in OnTriggerEnter for entity "
                << triggerEntity << ": " << e.what() << "\n";
        }
    }
}

void PhysicsSystem::DispatchTriggerExit(Entity triggerEntity, Entity otherEntity) {
    auto* script = world->GetComponent<Script>(triggerEntity);

    if (script && script->userScript) {
        try {
            script->userScript->OnTriggerExit(otherEntity);
            triggerEventsThisFrame++;

            std::cout << "[PhysicsSystem] TriggerExit: Entity " << triggerEntity
                << " <- Entity " << otherEntity << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "[PhysicsSystem] ERROR in OnTriggerExit for entity "
                << triggerEntity << ": " << e.what() << "\n";
        }
    }
}

void PhysicsSystem::OnEntityAdded(Entity entity) {
    if (debugOutput) {
        std::cout << "[PhysicsSystem] Entity " << entity << " added to physics system\n";
    }
}

void PhysicsSystem::OnEntityRemoved(Entity entity) {
    if (debugOutput) {
        std::cout << "[PhysicsSystem] Entity " << entity << " removed from physics system\n";
    }

    // Clean up any references to this entity in other colliders' trigger lists
    for (Entity otherEntity : entities) {
        if (otherEntity == entity) continue;

        auto* collider = world->GetComponent<BoxCollider>(otherEntity);
        if (collider) {
            collider->currentTriggers.erase(entity);
            collider->previousTriggers.erase(entity);
        }
    }
}
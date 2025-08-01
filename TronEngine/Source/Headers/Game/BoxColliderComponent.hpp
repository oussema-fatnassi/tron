// TronEngine/Source/Headers/Game/BoxColliderComponent.hpp
#pragma once
#include "Component.hpp"
#include "Types.hpp"
#include <set>

// <summary>
// BoxCollider Component for AABB (Axis-Aligned Bounding Box) collision detection
// Supports both solid collisions and trigger zones
// </summary>
// <remarks>
// This component works with the PhysicsSystem to detect overlaps between entities.
// When isTrigger = true, it generates OnTriggerEnter/OnTriggerExit events for scripts.
// When isTrigger = false, it can be used for solid collision detection (future feature).
// 
// The collision box is centered on the entity's Transform position.
// Box dimensions are affected by the entity's scale from Transform component.
// </remarks>
struct BoxCollider : Component {
    // Box dimensions (in local space, before scaling)
    float width, height, depth;

    // Collision behavior
    bool isTrigger;     // If true, generates trigger events but no physics blocking
    bool isEnabled;     // Can temporarily disable this collider

    // Runtime state (managed by PhysicsSystem - don't modify directly)
    std::set<Entity> currentTriggers;   // Entities currently overlapping this frame
    std::set<Entity> previousTriggers;  // Entities that were overlapping last frame

    // Constructor
    BoxCollider(float w = 1.0f, float h = 1.0f, float d = 1.0f, bool trigger = false)
        : width(w), height(h), depth(d)
        , isTrigger(trigger)
        , isEnabled(true)
    {
    }

    // Utility methods
    void SetSize(float w, float h, float d) {
        width = w;
        height = h;
        depth = d;
    }

    void SetTrigger(bool trigger) {
        isTrigger = trigger;
    }

    void SetEnabled(bool enabled) {
        isEnabled = enabled;
    }
};
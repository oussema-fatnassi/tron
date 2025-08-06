#pragma once
#include "Vector3.hpp"
#include "../Game/Types.hpp"
#include <iostream>

// <summary>
// Ray structure for raycasting operations
// Represents an infinite ray with origin and direction
// </summary>
// <remarks>
// Used for mouse picking, line-of-sight checks, and physics queries.
// The direction vector should always be normalized for correct calculations.
// </remarks>
struct Ray {
    Vector3 origin;
    Vector3 direction;  // Should always be normalized
    float maxDistance;  // Maximum distance to check (0 = infinite)

    // Constructors
    Ray() : origin(Vector3::Zero()), direction(Vector3::Forward()), maxDistance(0.0f) {}
    
    Ray(const Vector3& origin, const Vector3& direction, float maxDistance = 0.0f)
        : origin(origin), direction(direction.Normalized()), maxDistance(maxDistance) {}

    // Get a point along the ray at distance t
    Vector3 GetPoint(float distance) const {
        return origin + direction * distance;
    }

    // Check if a distance is within the ray's range
    bool IsInRange(float distance) const {
        return maxDistance <= 0.0f || distance <= maxDistance;
    }

    void Print() const {
        std::cout << "Ray: Origin";
        origin.Print();
        std::cout << " Direction";
        direction.Print();
        std::cout << " MaxDist=" << maxDistance << "\n";
    }
};

// <summary>
// Structure containing information about a raycast hit
// </summary>
struct RaycastHit {
    bool hit;              // Was there a hit?
    Entity entity;         // Which entity was hit
    Vector3 point;         // World position of the hit
    Vector3 normal;        // Surface normal at hit point
    float distance;        // Distance from ray origin to hit point
    
    // Additional info
    void* collider;        // Pointer to the collider component (BoxCollider*)
    int triangleIndex;     // For mesh collisions (future use)

    RaycastHit() 
        : hit(false), entity(0), point(Vector3::Zero()), 
          normal(Vector3::Up()), distance(0.0f), 
          collider(nullptr), triangleIndex(-1) {}

    void Reset() {
        hit = false;
        entity = 0;
        point = Vector3::Zero();
        normal = Vector3::Up();
        distance = 0.0f;
        collider = nullptr;
        triangleIndex = -1;
    }
};
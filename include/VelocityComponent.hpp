#pragma once
#include "Component.hpp"

// <summary>
// VelocityComponent is a component that represents the velocity and angular velocity of an entity in 3D space.
// It inherits from the Component class and provides functionality to manage the entity's movement.
// </summary>
// <remarks>
// This component is essential for physics systems, allowing entities to move and rotate in the game world.
// It can be used in conjunction with other components to define the complete state of an entity in the ECS (Entity-Component-System) architecture.
// The VelocityComponent can be initialized with default values or specific velocity parameters.
// </remarks>
struct Velocity : Tron::Component {
    float vx, vy, vz;
    float angularVx, angularVy, angularVz;
    
    Velocity(float vx = 0, float vy = 0, float vz = 0) 
        : vx(vx), vy(vy), vz(vz),
          angularVx(0), angularVy(0), angularVz(0) {}
    
    Velocity(float vx, float vy, float vz,
             float avx, float avy, float avz)
        : vx(vx), vy(vy), vz(vz),
          angularVx(avx), angularVy(avy), angularVz(avz) {}
};
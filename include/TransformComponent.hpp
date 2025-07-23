#pragma once
#include "Component.hpp"

// <summary>
// TransformComponent is a component that represents the position, rotation, and scale of an entity in 3D space.
// It inherits from the Component class and provides functionality to manage the entity's transformation.
// </summary>
// <remarks>
// This component is essential for rendering and physics systems, allowing entities to be positioned, rotated,
// and scaled in the game world. It can be used in conjunction with other components to define
// the complete state of an entity in the ECS (Entity-Component-System) architecture.
// The TransformComponent can be initialized with default values or specific transformation parameters.
// </remarks>
struct Transform : Tron::Component {
    float x, y, z;
    float rotationX, rotationY, rotationZ;
    float scaleX, scaleY, scaleZ;
    
    Transform(float x = 0, float y = 0, float z = 0) 
        : x(x), y(y), z(z),
          rotationX(0), rotationY(0), rotationZ(0),
          scaleX(1), scaleY(1), scaleZ(1) {}
    
    Transform(float x, float y, float z,
              float rx, float ry, float rz,
              float sx, float sy, float sz)
        : x(x), y(y), z(z),
          rotationX(rx), rotationY(ry), rotationZ(rz),
          scaleX(sx), scaleY(sy), scaleZ(sz) {}
};
#pragma once
#include "Component.hpp"
#include <iostream>

// TODO: Add Matrix4 include when ready
// #include "../Math/Matrix4.hpp"

// <summary>
// TransformComponent with COMPATIBILITY for existing rendering system
// This version maintains the old x,y,z interface while adding new matrix features
// </summary>
// <remarks>
// This is a compatibility version that works with the existing MeshRenderSystem.
// It provides both the old simple interface (x,y,z) and new matrix capabilities.
// Once the rendering pipeline is updated to use matrices, we can remove the old interface.
// </remarks>
struct Transform : Component {
    // OLD INTERFACE - Keep these for compatibility with existing rendering
    float x, y, z;
    float rotationX, rotationY, rotationZ;  // Simple Euler angles
    float scaleX, scaleY, scaleZ;

    // NEW INTERFACE - Enhanced features (can be used alongside old)
    float pitch, yaw, roll;  // Same as rotationX, rotationY, rotationZ but clearer names
    
    // TODO: Add matrix support when rendering pipeline is ready
    // mutable Matrix4 worldMatrix;
    // mutable bool worldMatrixDirty;

    // Constructors - Compatible with existing code
    Transform(float x = 0, float y = 0, float z = 0) 
        : x(x), y(y), z(z),
          rotationX(0), rotationY(0), rotationZ(0),
          scaleX(1), scaleY(1), scaleZ(1),
          pitch(0), yaw(0), roll(0) {}
    
    Transform(float x, float y, float z,
              float rx, float ry, float rz,
              float sx = 1, float sy = 1, float sz = 1)
        : x(x), y(y), z(z),
          rotationX(rx), rotationY(ry), rotationZ(rz),
          scaleX(sx), scaleY(sy), scaleZ(sz),
          pitch(rx), yaw(ry), roll(rz) {}

    // Enhanced methods - NEW interface
    void SetPosition(float newX, float newY, float newZ) {
        x = newX; y = newY; z = newZ;
        // TODO: Mark matrix dirty when matrices are integrated
    }

    void Translate(float deltaX, float deltaY, float deltaZ) {
        x += deltaX; y += deltaY; z += deltaZ;
        // TODO: Mark matrix dirty when matrices are integrated
    }

    // Rotation methods (updates both old and new rotation values)
    void SetRotation(float newPitch, float newYaw, float newRoll) {
        pitch = newPitch; yaw = newYaw; roll = newRoll;
        rotationX = newPitch; rotationY = newYaw; rotationZ = newRoll;
        // TODO: Mark matrix dirty when matrices are integrated
    }

    void SetRotationDegrees(float pitchDeg, float yawDeg, float rollDeg) {
        pitch = DegreesToRadians(pitchDeg);
        yaw = DegreesToRadians(yawDeg);
        roll = DegreesToRadians(rollDeg);
        rotationX = pitch; rotationY = yaw; rotationZ = roll;
        // TODO: Mark matrix dirty when matrices are integrated
    }

    void Rotate(float deltaPitch, float deltaYaw, float deltaRoll) {
        pitch += deltaPitch; yaw += deltaYaw; roll += deltaRoll;
        rotationX = pitch; rotationY = yaw; rotationZ = roll;
        // TODO: Mark matrix dirty when matrices are integrated
    }

    void RotateDegrees(float deltaPitchDeg, float deltaYawDeg, float deltaRollDeg) {
        pitch += DegreesToRadians(deltaPitchDeg);
        yaw += DegreesToRadians(deltaYawDeg);
        roll += DegreesToRadians(deltaRollDeg);
        rotationX = pitch; rotationY = yaw; rotationZ = roll;
        // TODO: Mark matrix dirty when matrices are integrated
    }

    // Scale methods
    void SetScale(float sx, float sy, float sz) {
        scaleX = sx; scaleY = sy; scaleZ = sz;
        // TODO: Mark matrix dirty when matrices are integrated
    }

    void SetUniformScale(float scale) {
        scaleX = scaleY = scaleZ = scale;
        // TODO: Mark matrix dirty when matrices are integrated
    }

    // Utility methods - These provide basic direction vectors without matrices
    void GetForward(float& forwardX, float& forwardY, float& forwardZ) const {
        // Simple forward calculation without matrix (basic implementation)
        float cosYaw = cosf(yaw);
        float sinYaw = sinf(yaw);
        float cosPitch = cosf(pitch);
        float sinPitch = sinf(pitch);
        
        forwardX = cosYaw * cosPitch;
        forwardY = -sinPitch;
        forwardZ = sinYaw * cosPitch;
    }

    void GetRight(float& rightX, float& rightY, float& rightZ) const {
        // Simple right calculation without matrix
        rightX = cosf(yaw + 1.5708f); // yaw + 90 degrees
        rightY = 0.0f;
        rightZ = sinf(yaw + 1.5708f);
    }

    void GetUp(float& upX, float& upY, float& upZ) const {
        // Simple up vector (world up for now)
        upX = 0.0f;
        upY = 1.0f;
        upZ = 0.0f;
    }

    // Utility methods for angles
    float GetPitchDegrees() const { return RadiansToDegrees(pitch); }
    float GetYawDegrees() const { return RadiansToDegrees(yaw); }
    float GetRollDegrees() const { return RadiansToDegrees(roll); }

    // Debug helper
    void PrintTransform() const {
        std::cout << "Transform: Pos(" << x << ", " << y << ", " << z 
                  << ") Rot(" << GetPitchDegrees() << "°, " << GetYawDegrees() 
                  << "°, " << GetRollDegrees() << "°) Scale(" << scaleX 
                  << ", " << scaleY << ", " << scaleZ << ")\n";
    }

    // TODO: When matrix integration is ready, add these methods:
    // const Matrix4& GetWorldMatrix() const;
    // void RegenerateWorldMatrix() const;
    // void MarkDirty();

private:
    // Helper functions for angle conversion (inline for now)
    float DegreesToRadians(float degrees) const {
        return degrees * 3.14159265358979323846f / 180.0f;
    }
    
    float RadiansToDegrees(float radians) const {
        return radians * 180.0f / 3.14159265358979323846f;
    }
};
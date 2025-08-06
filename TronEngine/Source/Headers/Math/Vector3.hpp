#pragma once
#include <cmath>
#include <iostream>

// <summary>
// Vector3 class for 3D vector operations
// Essential for raycasting, physics, and general 3D math
// </summary>
// <remarks>
// This class provides basic 3D vector math operations needed throughout the engine.
// It's designed to be lightweight and efficient, with inline methods for performance.
// </remarks>
class Vector3 {
public:
    float x, y, z;

    // Constructors
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

    // Basic operators
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(float scalar) const {
        float invScalar = 1.0f / scalar;
        return Vector3(x * invScalar, y * invScalar, z * invScalar);
    }

    Vector3& operator+=(const Vector3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    Vector3& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    Vector3& operator/=(float scalar) {
        float invScalar = 1.0f / scalar;
        x *= invScalar; y *= invScalar; z *= invScalar;
        return *this;
    }

    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }

    // Vector operations
    float Length() const {
        return sqrtf(x * x + y * y + z * z);
    }

    float LengthSquared() const {
        return x * x + y * y + z * z;
    }

    void Normalize() {
        float len = Length();
        if (len > 0.0001f) {
            float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
            z *= invLen;
        }
    }

    Vector3 Normalized() const {
        Vector3 result = *this;
        result.Normalize();
        return result;
    }

    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 Cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Utility methods
    void Set(float newX, float newY, float newZ) {
        x = newX; y = newY; z = newZ;
    }

    void Print() const {
        std::cout << "(" << x << ", " << y << ", " << z << ")";
    }

    // Static utility methods
    static float Distance(const Vector3& a, const Vector3& b) {
        return (b - a).Length();
    }

    static float DistanceSquared(const Vector3& a, const Vector3& b) {
        return (b - a).LengthSquared();
    }

    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * t;
    }

    static Vector3 Min(const Vector3& a, const Vector3& b) {
        return Vector3(
            (a.x < b.x) ? a.x : b.x,
            (a.y < b.y) ? a.y : b.y,
            (a.z < b.z) ? a.z : b.z
        );
    }

    static Vector3 Max(const Vector3& a, const Vector3& b) {
        return Vector3(
            (a.x > b.x) ? a.x : b.x,
            (a.y > b.y) ? a.y : b.y,
            (a.z > b.z) ? a.z : b.z
        );
    }

    // Common vectors
    static Vector3 Zero() { return Vector3(0.0f, 0.0f, 0.0f); }
    static Vector3 One() { return Vector3(1.0f, 1.0f, 1.0f); }
    static Vector3 Up() { return Vector3(0.0f, 1.0f, 0.0f); }
    static Vector3 Down() { return Vector3(0.0f, -1.0f, 0.0f); }
    static Vector3 Left() { return Vector3(-1.0f, 0.0f, 0.0f); }
    static Vector3 Right() { return Vector3(1.0f, 0.0f, 0.0f); }
    static Vector3 Forward() { return Vector3(0.0f, 0.0f, -1.0f); }
    static Vector3 Back() { return Vector3(0.0f, 0.0f, 1.0f); }
};

// Non-member operators
inline Vector3 operator*(float scalar, const Vector3& vec) {
    return vec * scalar;
}
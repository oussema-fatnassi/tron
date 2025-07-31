#pragma once
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// <summary>
// Matrix class for 4x4 matrix operations in 3D graphics
// Handles world, view, and projection transformations
// </summary>
// <remarks>
// This class provides essential matrix operations for 3D rendering:
// - Translation, rotation, scaling transformations
// - View matrix creation (camera positioning)
// - Projection matrix creation (perspective/orthographic)
// - Matrix multiplication and basic operations
// The matrix is stored in column-major order to match DirectX conventions.
// </remarks>
class Matrix {
public:
    // Matrix data stored in column-major order (DirectX convention)
    float m[16];

    // Constructors
    Matrix();
    Matrix(const float* data);
    Matrix(const Matrix& other);

    // Assignment operator
    Matrix& operator=(const Matrix& other);

    // Matrix operations
    Matrix operator*(const Matrix& other) const;
    Matrix& operator*=(const Matrix& other);

    // Static factory methods for common transformations
    static Matrix Identity();
    static Matrix Translation(float x, float y, float z);
    static Matrix RotationX(float angleRadians);
    static Matrix RotationY(float angleRadians);
    static Matrix RotationZ(float angleRadians);
    static Matrix RotationEuler(float pitch, float yaw, float roll); // XYZ order
    static Matrix Scale(float x, float y, float z);
    static Matrix Scale(float uniform);

    // Camera matrices
    static Matrix LookAt(float eyeX, float eyeY, float eyeZ,
        float targetX, float targetY, float targetZ,
        float upX, float upY, float upZ);
    static Matrix Perspective(float fovYRadians, float aspectRatio, float nearPlane, float farPlane);
    static Matrix Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    // Utility methods
    void SetIdentity();
    Matrix Transpose() const;
    Matrix Inverse() const; // TODO: Implement proper matrix inversion
    void Print() const; // Debug helper

    // Access operators
    float& operator[](int index) { return m[index]; }
    const float& operator[](int index) const { return m[index]; }

    // Element access (row, col)
    float& At(int row, int col) { return m[col * 4 + row]; }
    const float& At(int row, int col) const { return m[col * 4 + row]; }

    // Convert to array for DirectX (already in column-major)
    const float* Data() const { return m; }
    float* Data() { return m; }

    // TODO: Future enhancements
    // - Determinant calculation
    // - Proper matrix inversion using Gauss-Jordan elimination
    // - Decomposition methods (extract translation, rotation, scale)
    // - Interpolation methods (LERP, SLERP for rotations)
    // - Frustum culling support
    // - SIMD optimizations using DirectXMath
};

// Utility functions for angle conversion
inline float DegreesToRadians(float degrees) {
    return degrees * (float)M_PI / 180.0f;
}

inline float RadiansToDegrees(float radians) {
    return radians * 180.0f / (float)M_PI;
}
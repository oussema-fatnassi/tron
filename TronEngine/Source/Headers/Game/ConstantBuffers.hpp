#pragma once
#include "../Math/Matrix.hpp"

// <summary>
// Constant buffer structures for shader communication
// These structures must match the layout expected by HLSL shaders
// </summary>

// Camera matrices constant buffer (register b0) - MATCHES VERTEX SHADER
struct CameraMatricesBuffer {
    Matrix worldMatrix;         // Object's world transformation matrix
    Matrix viewMatrix;          // Camera view matrix  
    Matrix projectionMatrix;    // Camera projection matrix
    Matrix worldViewProjMatrix; // Combined WVP matrix (optimization)
    
    // Default constructor
    CameraMatricesBuffer() {
        worldMatrix = Matrix::Identity();
        viewMatrix = Matrix::Identity();
        projectionMatrix = Matrix::Identity();
        worldViewProjMatrix = Matrix::Identity();
    }
};

// Pixel shader constant buffer (register b1) - Keep existing
struct PixelConstantBuffer {
    float color[4];             // Material color (RGBA)
    float materialProperties[4]; // Shininess, metallic, roughness, etc.
    float lightPosition[4];     // Primary light position (world space)
    float lightColor[4];        // Primary light color and intensity
    
    PixelConstantBuffer() {
        // Default white color
        color[0] = color[1] = color[2] = color[3] = 1.0f;
        
        // Default material properties
        materialProperties[0] = materialProperties[1] = materialProperties[2] = materialProperties[3] = 0.0f;
        
        // Default light (above and to the right)
        lightPosition[0] = 5.0f; lightPosition[1] = 10.0f; lightPosition[2] = 5.0f; lightPosition[3] = 1.0f;
        
        // Default white light
        lightColor[0] = lightColor[1] = lightColor[2] = lightColor[3] = 1.0f;
    }
};

// Utility functions for constant buffer management
namespace ConstantBufferUtils {
    // Helper to copy Matrix to float array (for D3D11 compatibility)
    inline void MatrixToFloatArray(const Matrix& matrix, float* array) {
        memcpy(array, matrix.Data(), sizeof(float) * 16);
    }
    
    // Helper to create world matrix from Transform component
    Matrix CreateWorldMatrix(const struct Transform* transform);
    
    // Helper to validate buffer sizes
    static_assert(sizeof(CameraMatricesBuffer) % 16 == 0, "CameraMatricesBuffer must be 16-byte aligned");
    static_assert(sizeof(PixelConstantBuffer) % 16 == 0, "PixelConstantBuffer must be 16-byte aligned");
}
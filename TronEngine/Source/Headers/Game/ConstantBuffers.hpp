#pragma once

// <summary>
// Constant buffer structures for shader communication
// These structures must match the layout expected by HLSL shaders
// </summary>
// <remarks>
// DirectX requires 16-byte alignment for constant buffers.
// All structures are designed to meet these alignment requirements.
// Matrix data is stored in column-major order to match HLSL expectations.
// </remarks>

// Vertex shader constant buffer (register b0)
struct VertexConstantBuffer {
    float worldMatrix[16];      // World transformation matrix
    float viewMatrix[16];       // View (camera) matrix  
    float projectionMatrix[16]; // Projection matrix
    float worldViewProjMatrix[16]; // Combined WVP matrix (optimization)
    
    // TODO: Add more vertex shader constants as needed:
    // - Normal matrix for lighting calculations
    // - Bone matrices for skeletal animation
    // - Instance data for instanced rendering
};

// Pixel shader constant buffer (register b1) 
struct PixelConstantBuffer {
    float color[4];             // Material color (RGBA)
    float materialProperties[4]; // Shininess, metallic, roughness, etc.
    float lightPosition[4];     // Primary light position (world space)
    float lightColor[4];        // Primary light color and intensity
    
    // TODO: Add more pixel shader constants as needed:
    // - Multiple light sources
    // - Shadow mapping parameters
    // - Fog parameters
    // - Time for animated effects
};

// Utility functions for constant buffer management
namespace ConstantBufferUtils {
    // TODO: Implement utility functions for:
    // - Copying Matrix4 data to float arrays
    // - Updating constant buffers efficiently
    // - Validation of buffer sizes and alignment
    // - Batch updates for multiple objects
}
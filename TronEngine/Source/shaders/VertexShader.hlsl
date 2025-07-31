// Enhanced Vertex Shader with Matrix Transformations
// Supports world, view, and projection matrix transformations

// Constant buffer for vertex shader transformations (register b0)
cbuffer VertexConstants : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix; 
    matrix projectionMatrix;
    matrix worldViewProjMatrix; // Pre-computed for performance
}

// Input structure from vertex buffer
struct VertexInput {
    float3 position : POSITION;
    float3 color : COLOR;
    // TODO: Add more vertex attributes as needed:
    // float3 normal : NORMAL;
    // float2 texCoord : TEXCOORD;
    // float4 tangent : TANGENT;
};

// Output structure to pixel shader
struct VertexOutput {
    float4 position : SV_POSITION;  // Transformed position for rasterization
    float3 color : COLOR;           // Pass-through color
    float3 worldPos : WORLD_POS;    // World space position for lighting
    
    // TODO: Add more outputs as needed:
    // float3 worldNormal : WORLD_NORMAL;
    // float2 texCoord : TEXCOORD;
    // float3 viewDir : VIEW_DIR;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    
    // Transform vertex position to world space
    float4 worldPosition = mul(float4(input.position, 1.0f), worldMatrix);
    
    // Transform to view space
    float4 viewPosition = mul(worldPosition, viewMatrix);
    
    // Transform to clip space for rasterization
    output.position = mul(viewPosition, projectionMatrix);
    
    // Alternative: Use pre-computed world-view-projection matrix for performance
    // output.position = mul(float4(input.position, 1.0f), worldViewProjMatrix);
    
    // Pass through color (no transformation needed)
    output.color = input.color;
    
    // Store world position for pixel shader lighting calculations
    output.worldPos = worldPosition.xyz;
    
    return output;
}

// TODO: Future enhancements:
// - Normal transformation for lighting
// - Texture coordinate processing
// - Skinning/bone animation support
// - Instancing support with per-instance data
// - Vertex displacement for terrain/water effects
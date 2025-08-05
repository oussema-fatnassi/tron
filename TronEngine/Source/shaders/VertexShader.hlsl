// Constant buffer for camera matrices (register b0)
cbuffer CameraMatrices : register(b0)
{
    matrix worldMatrix;      // Object's world transform
    matrix viewMatrix;       // Camera view matrix
    matrix projectionMatrix; // Camera projection matrix
    matrix worldViewProjMatrix; // Combined WVP matrix (for optimization)
}

struct VertexInput {
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VertexOutput {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    
    // Transform vertex position through the full pipeline:
    // Local Space -> World Space -> View Space -> Clip Space
    
    // Method 1: Step by step (easier to debug)
    float4 worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    float4 clipPos = mul(viewPos, projectionMatrix);
    
    // Method 2: Combined matrix (more efficient, uncomment to use)
    // float4 clipPos = mul(float4(input.position, 1.0f), worldViewProjMatrix);
    
    output.pos = clipPos;
    output.color = input.color;
    
    return output;
}
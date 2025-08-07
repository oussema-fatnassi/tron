// FIXED VertexShader.hlsl - Specify column_major for matrices
// Constant buffer for camera matrices (register b0)
cbuffer CameraMatrices : register(b0)
{
    column_major matrix worldMatrix; // Object's world transform
    column_major matrix viewMatrix; // Camera view matrix
    column_major matrix projectionMatrix; // Camera projection matrix
    column_major matrix worldViewProjMatrix; // Combined WVP matrix (for optimization)
}

struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VertexOutput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    // For column-major matrices, multiply matrix first, then vector
    float4 localPos = float4(input.position, 1.0f);
    float4 worldPos = mul(worldMatrix, localPos);
    float4 viewPos = mul(viewMatrix, worldPos);
    float4 clipPos = mul(projectionMatrix, viewPos);
    
    // Or use the pre-computed combined matrix:
    //float4 clipPos = mul(worldViewProjMatrix, float4(input.position, 1.0f));
    
    output.pos = clipPos;
    output.color = input.color;
    
    return output;
}
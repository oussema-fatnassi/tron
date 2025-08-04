// DebugVertexShader.hlsl - For testing camera matrices
// This shader visualizes camera data to help debug issues

cbuffer ObjectTransform : register(b0)
{
    float3 objectPosition;
    float  padding1;
    float3 objectScale;
    float  padding2;
    float3 objectRotation;
    float  padding3;
}

cbuffer CameraMatrices : register(b1)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 viewProjectionMatrix;
    float3 cameraPosition;
    float  padding4;
}

struct VertexInput {
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VertexOutput {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float3 debugInfo : TEXCOORD0;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    
    // Transform to world space
    float3 worldPos = input.position * objectScale + objectPosition;
    float4 worldPos4 = float4(worldPos, 1.0f);
    
    // Apply view-projection matrix
    output.pos = mul(worldPos4, viewProjectionMatrix);
    
    // Debug: Color based on camera distance
    float3 toCameraVec = worldPos - cameraPosition;
    float distanceToCamera = length(toCameraVec);
    
    // Visualize distance as color (red = close, blue = far)
    float normalizedDistance = saturate(distanceToCamera / 20.0f); // Normalize to 0-20 units
    
    output.color = float3(1.0f - normalizedDistance, 0.5f, normalizedDistance);
    output.debugInfo = float3(distanceToCamera, worldPos.y, cameraPosition.y);
    
    return output;
}
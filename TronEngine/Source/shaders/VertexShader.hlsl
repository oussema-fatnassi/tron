// VertexShader.hlsl - FIXED Camera-Aware Version
// This shader now uses proper camera view/projection matrices

// Constant buffer for object transform (register b0)
cbuffer ObjectTransform : register(b0)
{
    float3 objectPosition;    // From Transform component (x, y, z)
    float  padding1;          // Padding for 16-byte alignment
    float3 objectScale;       // From Transform component (scaleX, scaleY, scaleZ)  
    float  padding2;          // Padding for 16-byte alignment
    float3 objectRotation;    // From Transform component (pitch, yaw, roll)
    float  padding3;          // Padding for 16-byte alignment
}

// FIXED: Camera matrices constant buffer (register b1)
cbuffer CameraMatrices : register(b1)
{
    float4x4 viewMatrix;        // Camera view matrix (64 bytes)
    float4x4 projectionMatrix;  // Camera projection matrix (64 bytes)
    float4x4 viewProjectionMatrix; // Combined for efficiency (64 bytes)
    float3 cameraPosition;    // Camera world position (12 bytes)
    float  padding4;          // Padding to 16-byte boundary (4 bytes) = 208 bytes total
}

struct VertexInput {
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VertexOutput {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float3 worldPos : TEXCOORD0; // Pass world position for debugging
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    
    // Step 1: Transform vertex to world space
    float3 worldPos = input.position;
    
    // Apply scale
    worldPos *= objectScale;
    
    // TODO: Apply rotation (for now, skip rotation to avoid complexity)
    // This is where you'd apply rotation matrices later
    
    // Apply translation (position from Transform component)
    worldPos += objectPosition;
    
    // Step 2: Transform world position through camera matrices
    float4 worldPos4 = float4(worldPos, 1.0f);
    
    // OPTION A: Use combined view-projection matrix (more efficient)
    output.pos = mul(worldPos4, viewProjectionMatrix);
    
    // OPTION B: Apply view and projection separately (for debugging)
    // float4 viewPos = mul(worldPos4, viewMatrix);
    // output.pos = mul(viewPos, projectionMatrix);
    
    // Pass through color and world position
    output.color = input.color;
    output.worldPos = worldPos;
    
    return output;
}
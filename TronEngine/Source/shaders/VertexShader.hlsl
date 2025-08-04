// VertexShader.hlsl - Camera-Aware Version
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

// NEW: Camera matrices constant buffer (register b1)
cbuffer CameraMatrices : register(b1)
{
    matrix viewMatrix;        // Camera view matrix
    matrix projectionMatrix;  // Camera projection matrix
    matrix viewProjectionMatrix; // Combined for efficiency
    float3 cameraPosition;    // Camera world position
    float  padding4;          // Padding for 16-byte alignment
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
    
    // Apply object transform to vertex position
    float3 worldPos = input.position;
    
    // Apply scale
    worldPos *= objectScale;
    
    // TODO: Apply rotation (for now, skip rotation to avoid complexity)
    // This is where you'd apply rotation matrices later
    
    // Apply translation (position from Transform component)
    worldPos += objectPosition;
    
    // *** THIS IS THE KEY FIX ***
    // Transform world position through camera matrices
    float4 worldPos4 = float4(worldPos, 1.0f);
    
    // Apply view-projection matrix to get screen coordinates
    output.pos = mul(worldPos4, viewProjectionMatrix);
    
    // Pass through color and world position
    output.color = input.color;
    output.worldPos = worldPos;
    
    return output;
}
// Fixed Transform-Aware Vertex Shader
// This shader receives transform data from the CPU and applies it properly

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
    
    // Apply transform from Transform component
    float3 worldPos = input.position;
    
    // Apply scale
    worldPos *= objectScale;
    
    // TODO: Apply rotation (for now, skip rotation to avoid complexity)
    
    // Apply translation (position from Transform component)
    worldPos += objectPosition;
    
    // IMPORTANT: Proper projection matrix
    // Create a simple orthographic projection that maps world coordinates to screen
    float4 projectedPos = float4(worldPos, 1.0f);
    
    // Scale down the world coordinates to fit the screen properly
    // This creates a camera-like view where (0,0,0) is at center of screen
    projectedPos.x *= 0.1f;  // Scale X
    projectedPos.y *= 0.1f;  // Scale Y
    projectedPos.z = 0.0f;   // Flatten Z for 2D-like view
    
    output.pos = projectedPos;
    output.color = input.color;
    
    return output;
}
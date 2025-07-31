// Transform-Aware Vertex Shader
// This shader receives transform data from the CPU and applies it

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
    
    // TODO: Apply rotation (for now, skip rotation)
    
    // Apply translation (position from Transform component)
    worldPos += objectPosition;
    
    // Simple projection (scale down so objects are visible)
    output.pos = float4(worldPos * 0.2f, 1.0f);
    
    // Pass through color
    output.color = input.color;
    
    return output;
}
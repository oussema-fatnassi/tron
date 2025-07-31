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
    output.pos = float4(input.position, 1.0f);
    output.color = input.color;
    return output;
}
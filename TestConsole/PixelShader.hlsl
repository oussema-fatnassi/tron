cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

float4 main(float4 pos : SV_POSITION, float3 color : COLOR) : SV_TARGET
{
    return float4(1.0, 0.0, 0.0, 1.0); // Rouge fixe
}

cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

float4 main(float4 pos : SV_POSITION, float3 vertexColor : COLOR) : SV_TARGET
{
    return float4(vertexColor, 1.0);
}
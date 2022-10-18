Texture2D tex : register(t0);
SamplerState samLinear : register(s0);
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


float4 PS(PS_INPUT Input) : SV_TARGET
{

    float4 vColour = tex.Sample(samLinear, Input.Tex);
    
    return vColour;
}
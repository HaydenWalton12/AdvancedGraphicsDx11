Texture2D Tex : register(t0);
SamplerState samLinear : register(s0);

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;

};


struct PS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

float4 PS(PS_INPUT Input) : SV_TARGET
{

    float4 color = Tex.Sample(samLinear, Input.Tex);

    return color;
}
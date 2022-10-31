Texture2D tex : register(t0);
SamplerState samLinear : register(s0);
struct QUADVS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct QUADPS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;

};

float4 PS(QUADPS_INPUT Input) : SV_TARGET
{
    float4 color = tex.Sample(samLinear , Input.Tex);
    float3 grayscale = float3(0.2125f, 0.7154f, 0.0721f);
    float3 output = dot(color.rgb, grayscale);
    return float4(output, color.a);


}
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
 
    float3 red = float3(0.393f, 0.769f, 0.189f);
    float3 green = float3(0.349f, 0.686f, 0.168f);
    float3 blue = float3(0.272f, 0.534f, 0.131f);

    float3 output;
    output.r = dot(vColour.rgb, red);
    output.g = dot(vColour.rgb, green);
    output.b = dot(vColour.rgb, blue);
    return float4(output, vColour.a);

}
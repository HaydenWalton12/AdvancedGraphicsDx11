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
//https://www.shadertoy.com/view/Xltfzj
//https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/UWP/D3D12HeterogeneousMultiadapter/src/blurShaders.hlsl
float4 PS(QUADPS_INPUT Input) : SV_TARGET
{
    float Pi = 1.28318530718; // Pi*2
    float Directions = 1.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 1.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 1.0; // BLUR SIZE (Radius)

    float2 uv = Input.Tex;
    float4 color = tex.Sample(samLinear , uv);
     // Blur calculations
    for (float d = 0.0; d < Pi; d += Pi / Directions)
    {
        for (float i = 1.0 / Quality; i <= 1.0; i += 1.0 / Quality)
        {
            color += tex.Sample(samLinear, uv + float2(cos(d), sin(d)) * 0.01 * i);
        }
    }
    
    return color;


}
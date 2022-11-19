Texture2D rendered_scene : register(t0);

SamplerState samLinear : register(s0);

struct QUADVS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct QUADPS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    
};



float4 PS(QUADPS_INPUT Input) : SV_TARGET
{

    float4 texture_colour = rendered_scene.Sample(samLinear, Input.tex);
    float4 colour;

    
    
    if ((texture_colour.r == 0.0f) && (texture_colour.g == 0.0f) && (texture_colour.b == 0.0f))
    {
        colour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        colour = texture_colour;
    }

    return colour;

}
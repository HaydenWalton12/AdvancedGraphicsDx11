
//blurmap contains our rendered scene that is rendered to texture
Texture2D blurmap : register(t0);
//Contains same as our blur map , but blur map is used to "blur" scene 
Texture2D rendered_scene : register(t1);

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



//https://developer.nvidia.com/gpugems/gpugems/part-iv-image-processing/chapter-21-real-time-glow
//Add
//https://www.rastertek.com/tutdx11.html
//--------------------------------------------------------------------------------------
//  Blur Buffer
//--------------------------------------------------------------------------------------
cbuffer BlurBuffer : register(b0)
{
    float verticle;
    float horizontal;
    float enable;
    float padding1;
    
    int blur_strength;
    int blur_scale;
    int blur_amount;
    float padding2;
}

float Gaussian(float x, float deviation)
{
    return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));
}


float4 PS(QUADPS_INPUT Input) : SV_TARGET
{
    int width, height;
    float texel_size = 1 / 720;

    // Locals
    float half_blur = float(blur_amount) * 0.5f;
    float4 blur_colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
    // Gaussian deviation
    float deviation = half_blur * 0.35f;
    deviation *= deviation;
    float strength = 1.0 - blur_strength;
 
    for (int i = 0; i < 10; i++)
    {
        if (i >= blur_amount)
        {
            break;
        }
        float texture_offset = float(i) - half_blur;
        blur_colour = blurmap.Sample(samLinear, Input.tex +
            float2(texture_offset * texel_size * blur_scale, 0.0)) * Gaussian(texture_offset * strength, deviation);

    }
    
    for (int j = 0; i < 10; i++)
    {
        if (j >= blur_amount)
        {
            break;
        }
        float texture_offset = float(i) - half_blur;
        blur_colour = blurmap.Sample(samLinear, Input.tex +
            float2(0.0, texture_offset * texel_size * blur_scale)) * Gaussian(texture_offset * strength, deviation);

    }
    
    
    return blur_colour;
}
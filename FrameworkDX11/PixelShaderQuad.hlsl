
Texture2D rendered_scene : register(t0);
Texture2D glow_texture : register(t1);
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
    float2 tex1 : TEXCOORD1;
    float2 tex2 : TEXCOORD2;
    float2 tex3 : TEXCOORD3;
    float2 tex4 : TEXCOORD4;
    float2 tex5 : TEXCOORD5;
    float2 tex6 : TEXCOORD6;
    float2 tex7 : TEXCOORD7;
    float2 tex8 : TEXCOORD8;
    float2 tex9 : TEXCOORD9;
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
    float padding;
}


//http://www.nutty.ca/?page_id=352&link=glow

float4 PS(QUADPS_INPUT Input) : SV_TARGET
{
    float4 glow_map = rendered_scene.Sample(samLinear ,Input.tex);

    
  
    
    float weight0, weight1, weight2, weight3, weight4;
        
    weight0 = 0.227027;
    weight1 = 0.1945946f;
    weight2 = 0.1216216f;
    weight3 = 0.054054f;
    weight4 = 0.016216f;

    float4 blur_map = float4(0.0f, 0.0f, 0.0f, 0.0f);

    blur_map += rendered_scene.Sample(samLinear, Input.tex1) * weight4;
    blur_map += rendered_scene.Sample(samLinear, Input.tex2) * weight3;
    blur_map += rendered_scene.Sample(samLinear, Input.tex3) * weight2;
    blur_map += rendered_scene.Sample(samLinear, Input.tex4) * weight1;
    blur_map += rendered_scene.Sample(samLinear, Input.tex5) * weight0;
    blur_map += rendered_scene.Sample(samLinear, Input.tex6) * weight1;
    blur_map += rendered_scene.Sample(samLinear, Input.tex7) * weight2;
    blur_map += rendered_scene.Sample(samLinear, Input.tex8) * weight3;
    blur_map += rendered_scene.Sample(samLinear, Input.tex9) * weight4;
    
    
    float4 colour = saturate(blur_map + (glow_map * 2));

    return colour;
}
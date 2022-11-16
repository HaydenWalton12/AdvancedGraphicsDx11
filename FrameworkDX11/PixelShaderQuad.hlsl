
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

float4 GaussainBlur(float2 texCoords)
{
    float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
    //Gets first pixel/color/texture color
    float2 tex_offset = 1.0f / tex.Sample(samLinear, 0);
    //Current Color
    float3 result = tex.Sample(samLinear, texCoords).rgb * weight[0];
    if (horizontal == 1)
    {
        for (int i = 1; i < 5; i++)
        {
            result += tex.Sample(samLinear, texCoords + float2(tex_offset.y * i, 0.0f)).rgb * weight[i]; // right 
            result += tex.Sample(samLinear, texCoords - float2(tex_offset.y * i, 0.0f)).rgb * weight[i]; // left
        

        }
    }
    if (verticle == 1)
    {
        for (int i = 1; i < 5; i++)
        {
            result += tex.Sample(samLinear, texCoords + float2(0.0f, tex_offset.y * i)).rgb * weight[i];
            result += tex.Sample(samLinear, texCoords - float2(0.0f, tex_offset.y * i)).rgb * weight[i];

        }
    }
    return float4(result, 1.0f);

}

float4 PS(QUADPS_INPUT Input) : SV_TARGET
{
    
    float brightness = 1.3f;
    float gamma = 2.0f;

    float4 blur = GaussainBlur(Input.Tex );
    float4 color = tex.Sample(samLinear , Input.Tex);
    
    color += blur;
    
    float3 result = float3(1.0f, 1.0f, 1.0f) - exp(-color * 2.0f);
    
    result = pow(result, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
    return float4(result , 1.0f);

}
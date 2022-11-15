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
float4 GaussainBlur(float2 texCoords)
{
    float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

    //Gets first pixel/color/texture color
    float2 tex_offset = 0.1f / tex.Sample(samLinear, 0);
    //Current Color
    float3 result = tex.Sample(samLinear, texCoords).rgb * weight[0];

    for (int i  = 1; i < 5; i++)
    {
        result += tex.Sample(samLinear, texCoords + float2(tex_offset.y * i, 0.0f)).rgb * weight[i]; // right 
        result += tex.Sample(samLinear, texCoords - float2(tex_offset.y  * i, 0.0f)).rgb * weight[i]; // left

    }
    for (int i = 1; i < 5; i++)
    {
        result += tex.Sample(samLinear, texCoords + float2(0.0f, tex_offset.y * i)).rgb * weight[i];
        result += tex.Sample(samLinear, texCoords - float2(0.0f, tex_offset.y * i)).rgb * weight[i];

    }
    return float4(result, 0.0f);
}

float4 PS(QUADPS_INPUT Input) : SV_TARGET
{


    float4 blur = GaussainBlur(Input.Tex);
    float4 color = tex.Sample(samLinear , Input.Tex);
    color += blur;
    return color;


}
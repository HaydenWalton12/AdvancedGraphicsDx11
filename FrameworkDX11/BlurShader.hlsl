
Texture2D tex : register(t0);
Texture2D diffuse : register(t1);
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


float4 PS(QUADPS_INPUT Input) : SV_TARGET
{
    
    float weight0, weight1, weight2, weight3, weight4;
        
    weight0 = 0.227027;
    weight1 = 0.1945946f;
    weight2 = 0.1216216f;
    weight3 = 0.054054f;
    weight4 = 0.016216f;
 

    float4 blur = float4(0.0f, 0.0f, 0.0f, 0.0f);;

    blur += tex.Sample(samLinear, Input.tex1) * weight4;
    blur += tex.Sample(samLinear, Input.tex2) * weight3;
    blur += tex.Sample(samLinear, Input.tex3) * weight2;
    blur += tex.Sample(samLinear, Input.tex4) * weight1;
    blur += tex.Sample(samLinear, Input.tex5) * weight0;
    blur += tex.Sample(samLinear, Input.tex6) * weight1;
    blur += tex.Sample(samLinear, Input.tex7) * weight2;
    blur += tex.Sample(samLinear, Input.tex8) * weight3;
    blur += tex.Sample(samLinear, Input.tex9) * weight4;
    

   
    return blur;

}
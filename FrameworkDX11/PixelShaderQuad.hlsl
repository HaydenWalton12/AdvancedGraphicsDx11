
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

float4 HorizontalBlurPixelShader(QUADPS_INPUT input)
{
    
    float weight0, weight1, weight2, weight3, weight4;
    weight0 = 1.0f;
    weight1 = 0.9f;
    weight2 = 0.55f;
    weight3 = 0.18f;
    weight4 = 0.1f;
    float normalization;
    
    // Create a normalized value to average the weights out a bit.
    normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4));

    // Normalize the weights.
    weight0 = weight0 / normalization;
    weight1 = weight1 / normalization;
    weight2 = weight2 / normalization;
    weight3 = weight3 / normalization;
    weight4 = weight4 / normalization;
    
    float4 colour = float4(0.0f, 0.0f, 0.0f, 0.0f);;
    
    // Add the nine horizontal pixels to the color by the specific weight of each.
    colour += tex.Sample(samLinear, input.tex1) * weight4;
    colour += tex.Sample(samLinear, input.tex2) * weight3;
    colour += tex.Sample(samLinear, input.tex3) * weight2;
    colour += tex.Sample(samLinear, input.tex4) * weight1;
    colour += tex.Sample(samLinear, input.tex5) * weight0;
    colour += tex.Sample(samLinear, input.tex6) * weight1;
    colour += tex.Sample(samLinear, input.tex7) * weight2;
    colour += tex.Sample(samLinear, input.tex8) * weight3;
    colour += tex.Sample(samLinear, input.tex9) * weight4;

    
    return (colour , 1.0f);
}

float Gaussian(float x, float deviation)
{
    return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));
}
http://www.nutty.ca/?page_id=352&link=glow
float4 GaussainBlur(float2 texCoords)
{
    
     int BlurAmount = 10;
     float BlurScale= 2;
     float BlurStrength = 0.2;
    
    float halfBlur = float(BlurAmount) * 0.5;
    float4 colour = float4(0.0f, 0.0f , 0.0f , 0.0f);
    float4 texColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Gaussian deviation
    float deviation = halfBlur * 0.35;
    deviation *= deviation;
    float strength = 1.0 - BlurStrength;
    float2 texelSize = 1.0f / 1280;
    // Horizontal blur
    for (int i = 0; i < 20; ++i)
    {
	
        float offset = float(i) - halfBlur;
        texColour = 
        tex.Sample(samLinear, texCoords + float2(offset * texelSize.x * BlurScale, 0.0)) * Gaussian(offset * strength, deviation);
        colour += texColour;
    }
    	// Vertical blur
    for (int i = 0; i < 20; ++i)
    {
        
        float offset = float(i) - halfBlur;
        texColour = tex.Sample(samLinear, texCoords +float2(0.0, offset * texelSize.y * BlurScale)) * Gaussian(offset * strength, deviation);
        colour += texColour;
    }
    return colour;

}

float4 PS(QUADPS_INPUT Input) : SV_TARGET
{
    
    float weight0, weight1, weight2, weight3, weight4;
        weight0 = 0.227027;
        weight1 = 0.1945946f;
        weight2 = 0.1216216f;
        weight3 = 0.054054f;
        weight4 = 0.016216f;
 

    //float4 blur = float4(0.0f, 0.0f, 0.0f, 0.0f);;
    
    //// Add the nine horizontal pixels to the color by the specific weight of each.
    //blur += tex.Sample(samLinear, Input.tex1) * weight4;
    //blur += tex.Sample(samLinear, Input.tex2) * weight3;
    //blur += tex.Sample(samLinear, Input.tex3) * weight2;
    //blur += tex.Sample(samLinear, Input.tex4) * weight1;
    //blur += tex.Sample(samLinear, Input.tex5) * weight0;
    //blur += tex.Sample(samLinear, Input.tex6) * weight1;
    //blur += tex.Sample(samLinear, Input.tex7) * weight2;
    //blur += tex.Sample(samLinear, Input.tex8) * weight3;
    //blur += tex.Sample(samLinear, Input.tex9 ) * weight4;

    float4 blur = GaussainBlur(Input.tex);
    float4 colour = tex.Sample(samLinear, Input.tex);

    float4 pixel = min(colour + blur, 1.0f);
    return blur;

}
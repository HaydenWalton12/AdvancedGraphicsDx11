//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 vOutputColor;
    float3 EyePosW;


}

struct VS_INPUT
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



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
QUADPS_INPUT VS(VS_INPUT input)
{

	//VertexShader Cide
    QUADPS_INPUT output = (QUADPS_INPUT) 0;
	


    output.Pos = input.Pos;
    output.tex = input.Tex;

    float texelSize = 1.0f / 1280;
    // Create UV coordinates for the pixel and its four horizontal neighbors on either side.
    output.tex1 = output.tex + float2(texelSize * -4.0f, texelSize * -5.0f);
    output.tex2 = output.tex + float2(texelSize * -3.0f, texelSize * -4.0f);
    output.tex3 = output.tex + float2(texelSize * -2.0f, texelSize * -3.0f);
    output.tex4 = output.tex + float2(texelSize * -1.0f, texelSize * -2.0f);
    output.tex5 = output.tex + float2(texelSize * 0.0f, texelSize * 0.0f);
    output.tex6 = output.tex + float2(texelSize * 1.0f, texelSize * 2.0f);
    output.tex7 = output.tex + float2(texelSize * 2.0f, texelSize * 3.0f);
    output.tex8 = output.tex + float2(texelSize * 3.0f, texelSize * 4.0f);
    output.tex9 = output.tex + float2(texelSize * 4.0f, texelSize * 5.0f);

    return output;
}


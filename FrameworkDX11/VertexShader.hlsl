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
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 Tan : TANGENT;
    float3 Binorm : BINORMAL;
	
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 worldPos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 Tan : TANGENT;
    float3 Binorm : BINORMAL;
   
 //   float3x3 TBN : POSITION;
};




//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{

	//VertexShader Cide
    PS_INPUT output = (PS_INPUT) 0;
	
    output.Pos = mul(input.Pos, World);
    output.worldPos = output.Pos;
    float4 worldPos = output.Pos;
    
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    output.Tex = input.Tex;

    //Build TBN Matrix
    output.Tan = normalize(mul(input.Tan, (float3x3) World));
    output.Norm = normalize(mul(input.Norm, (float3x3) World));
    output.Binorm = normalize(mul(input.Binorm, (float3x3) World));
    
    return output;
}
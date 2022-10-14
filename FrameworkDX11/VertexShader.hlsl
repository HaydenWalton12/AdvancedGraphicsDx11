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
    output.Tan = normalize(mul(input.Tan, World).xyz);
    output.Norm = normalize(mul(input.Norm, World).xyz);
    output.Binorm = normalize(mul(input.Binorm, World).xyz);

	//Build TBN Matrix
    
    //ISSUE - Input is fine ,however we do not calculate the output from said values, calculations are correct but we need to output the values, similar to
    //how we "output.norm"
    //float3 T = normalize(mul(input.Tan, World));
    //float3 B = normalize(mul(input.Binorm, World));
    //float3 N = normalize(mul(input.Norm, World));

    //float3x3 TBN = float3x3(T, B, N);
    //float3x3 TBN_inv = transpose(TBN);
	
    //output.eyeVectorTS = VectorToTangentSpace(vertexToEye.xyz, TBN_inv);
    //output.lightVectorTS = VectorToTangentSpace(vertexToLight.xyz, TBN_inv);
    return output;
}

//--------------------------------------------------------------------------------------
// Constant Buffer Variables - Defined values needed for processing
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 vOutputColor;
}

//--------------------------------------------------------------------------------------
// Vertex Shader Variables - Defined values needed for processing
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	//Position Of A Vertex
	float4 Pos : POSITION;
	//Defines Direction Of Surface For Lighting To Be Correctly Calculated Upon A Surface
	float3 Norm : NORMAL;
	//Texture Coordinate
	float2 Tex : TEXCOORD0;

	//Values Required For Normal Mapping
	//Tangent Defines X Axis , BiNormal Is Y Axis , The Normal Is the Z Axis
	float3 Tan : TANGENT;
	float3 Binorm : BINORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader Variables - Defined values needed for processing . Values derived from vertex shader for further pixel processing , some are new, unique per-pixel calculations
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
	//Needs For Further Processing
	float3 Tan : TANGENT;
	float3 Binorm : BINORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader - Where The Processing Occurs , Inputs All Vertex Shader Data
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = mul(input.Pos, World);
	output.worldPos = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// multiply the normal by the world transform (to go from model space to world space)
	output.Norm = mul(float4(input.Norm, 0), World).xyz;

	output.Tex = input.Tex;

	//Multiple TanSpace With WorldSpace
	output.Tan = mul(input.Tan, (float3x3)World);
	output.Tan = normalize(output.Tan);

	output.Binorm = mul(input.Binorm, (float3x3)World);
	output.Binorm = normalize(output.Binorm);

	return output;
}

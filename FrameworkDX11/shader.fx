//--------------------------------------------------------------------------------------
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

// the lighting equations in this code have been taken from https://www.3dgep.com/texturing-lighting-directx-11/
// with some modifications by David White

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

Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txParallax : register(t2);

SamplerState samLinear : register(s0);

#define MAX_LIGHTS 1
// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct _Material
{
    float4 Emissive; // 16 bytes
							//----------------------------------- (16 byte boundary)
    float4 Ambient; // 16 bytes
							//------------------------------------(16 byte boundary)
    float4 Diffuse; // 16 bytes
							//----------------------------------- (16 byte boundary)
    float4 Specular; // 16 bytes
							//----------------------------------- (16 byte boundary)
    float SpecularPower; // 4 bytes
    bool UseTexture; // 4 bytes
    float2 Padding; // 8 bytes
							//----------------------------------- (16 byte boundary)
}; // Total:               // 80 bytes ( 5 * 16 )

cbuffer MaterialProperties : register(b1)
{
    _Material Material;
};

struct Light
{
    float4 Position; // 16 bytes
										//----------------------------------- (16 byte boundary)
    float4 Direction; // 16 bytes
										//----------------------------------- (16 byte boundary)
    float4 Color; // 16 bytes
										//----------------------------------- (16 byte boundary)
    float SpotAngle; // 4 bytes
    float ConstantAttenuation; // 4 bytes
    float LinearAttenuation; // 4 bytes
    float QuadraticAttenuation; // 4 bytes
 
						//----------------------------------- (16 byte boundary)
    int LightType; // 4 bytes
    bool Enabled; // 4 bytes
    int2 Padding; // 8 bytes
										//----------------------------------- (16 byte boundary)
}; // Total:                           // 80 bytes (5 * 16)

cbuffer LightProperties : register(b2)
{
    float4 EyePosition; // 16 bytes
										//----------------------------------- (16 byte boundary)
    float4 GlobalAmbient; // 16 bytes
										//----------------------------------- (16 byte boundary)
    Light Lights[MAX_LIGHTS]; // 80 * 8 = 640 bytes
}; 

//--------------------------------------------------------------------------------------
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


float4 DoDiffuse(Light light, float3 L, float3 N)
{
    float NdotL = max(0, dot(N, L));
    return light.Color * NdotL;
}

float4 DoSpecular(Light lightObject, float3 vertexToEye, float3 lightDirectionToVertex, float3 Normal)
{
    float4 lightDir = float4(normalize(-lightDirectionToVertex), 1);
    vertexToEye = normalize(vertexToEye);

    float lightIntensity = saturate(dot(Normal, lightDir));
    float4 specular = float4(0, 0, 0, 0);
    if (lightIntensity > 0.0f)
    {
        float3 reflection = normalize(2 * lightIntensity * Normal - lightDir);
        specular = pow(saturate(dot(reflection, vertexToEye)), Material.SpecularPower); // 32 = specular power
    }

    return specular;
}

float DoAttenuation(Light light, float d)
{
    return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);
}

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

LightingResult DoPointLight(Light light, float3 vertexToEye, float4 vertexPos, float3 N)
{
    LightingResult result;

    float3 LightDirectionToVertex = (vertexPos - light.Position).xyz;
    float distance = length(LightDirectionToVertex);
    LightDirectionToVertex = LightDirectionToVertex / distance;

    float3 vertexToLight = (light.Position - vertexPos).xyz;
    distance = length(vertexToLight);
    vertexToLight = vertexToLight / distance;

    float attenuation = DoAttenuation(light, distance);
    attenuation = 1;


    result.Diffuse = DoDiffuse(light, vertexToLight, N) * attenuation;
    result.Specular = DoSpecular(light, vertexToEye, LightDirectionToVertex, N) * attenuation;

    return result;
}

LightingResult ComputeLighting(float4 vertexPos, float3 N , float3 vertextoeyets)
{
    float3 vertexToEye = normalize(vertextoeyets - vertexPos).xyz;

    LightingResult totalResult = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	[unroll]
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

        if (!Lights[i].Enabled) 
            continue;
		
        result = DoPointLight(Lights[i], vertexToEye, vertexPos, N);
		
        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);

    return totalResult;
}


float2 ParallaxMapping(float2 tex_coords, float3 viewing_direction)
{
    float height_scale = 0.1f;
    float height = txParallax.Sample(samLinear, tex_coords).z;
    float2 p = viewing_direction.xy / viewing_direction.z * (height * height_scale);
    return tex_coords - p;
}

//Helper Function to convert world normal into tangent space
float3 VectorToTangentSpace(float3 vectorV, float3x3 TBN_inv)
{
	//Transform From Tangent space to world space
    float3 tangentSpaceNormal = normalize(mul(vectorV, TBN_inv));
    return tangentSpaceNormal;
}
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


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT IN) : SV_TARGET
{
    float shadowFactor = 1;

    //Rebuilds TBN matrix from calculated vertex shader
    float3x3 tbn = float3x3(IN.Tan, IN.Binorm, IN.Norm);
    
    float3 vertexToEye = EyePosition - IN.worldPos.xyz;
    float3 vertexToLight = Lights[0].Position - IN.worldPos.xyz;
    
    //Creates Inverse TBN
    float3x3 TBN_inv = transpose(tbn);

    float eyeVectorTS = VectorToTangentSpace(vertexToEye.xyz, TBN_inv);
    float lightVectorTS = VectorToTangentSpace(vertexToEye.xyz, TBN_inv);
    float3 viewDir = normalize(mul(tbn, EyePosition.xyz - IN.worldPos.xyz));

    //float3 toEyeTS = VectorToTangentSpace(toEye, TBN_inv);
  
     //float2 calculated_texCoords = ParallaxMapping(IN.Tex, toEyeTS);
    
   
    float3 bumpNormal = IN.Norm;
    float4 bumpMap;
    bumpMap = txNormal.Sample(samLinear, IN.Tex);
	
	//Exampnd the range of the normal value from (0 , +1) to (-1 , +1)
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = normalize(mul(bumpMap.xyz, tbn));

    LightingResult
    lit = ComputeLighting(eyeVectorTS, bumpNormal, lightVectorTS);

    //Issue With this is that all lighting appears upon the surface of the normal map
    //lit = ComputeLighting(IN.worldPos, bumpMap);
    
    
    float4 texColor = { 1, 1, 1, 1 };
    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * GlobalAmbient;
    float4 diffuse = Material.Diffuse * lit.Diffuse ;
    float4 specular = Material.Specular * lit.Specular ;

    if (Material.UseTexture)
    {
        texColor = txDiffuse.Sample(samLinear, IN.Tex);
    }


    float4 finalColor = (emissive + ambient + diffuse + specular) * texColor;

    return finalColor;
}

//--------------------------------------------------------------------------------------
// PSSolid - render a solid color
//--------------------------------------------------------------------------------------
float4 PSSolid(PS_INPUT input) : SV_Target
{
    return vOutputColor;
}

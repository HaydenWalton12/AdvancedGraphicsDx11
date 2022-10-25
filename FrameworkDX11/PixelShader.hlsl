VertexShader vs;

Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txParallax : register(t2);
SamplerState samLinear : register(s0);

#define MAX_LIGHTS 1
// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

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
    //float3x3 TBN : POSITION;
};

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
struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
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

//Helper Function to convert world normal into tangent space
float3 VectorToTangentSpace(float3 vectorV, float3x3 TBN_inv)
{
	//Transform From Tangent space to world space
    float3 tangentSpaceNormal = normalize(mul(vectorV, TBN_inv));
    return tangentSpaceNormal;
}
LightingResult DoPointLight(Light light, float3 vertexToEye, float3 vertexPos, float3 N)
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
LightingResult ComputeLighting(float3 vertexPos, float3 N, float3 vertextoeyets)
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
/*
    

*/
float2 SimpleParallaxMapping( float3 view, float2 tex_coords)
{
    float initial_height = txParallax.Sample(samLinear, tex_coords).r;
    
    //Calculate Texture Offset For Parallax mapping - Adheres to forumula
    float2 tex_coord_offset = view.xy * (initial_height * 0.01f);
    
    //Calculate Amount of offset for parallax mapping with offset limiting
    
    
    //return modified texture coordinates
    
    return tex_coords - tex_coord_offset;
    
}
/*
    Unlike simple parallax mapping , that is an approximation , steep doesnt offset the texture coordinate without 
    checking for validity. However checks whether thre result is close to the valid point 

    We devie depth of the surface into layers of same height , starting from top we sample the heightmap 
    each time shifting texture coordinate along view vector.

    If point is under surface (depth of current layer is greater than depth sampled from texture , stop checks and
    return last texture coordinate used

*/
float2 SteepParallaxMapping(float3 view, float2 tex_coords)
{
    //Number of layers iterate threw
    float min_layers = 10;
    float max_layers = 35;
    
    float num_layers = lerp(max_layers, min_layers, abs(dot(float3(0.0f, 0.0f, 1.0f), view)));

    //height of each layer
    float layer_height = 1.0f  / num_layers;
    
    //depth of current layer
    float current_layer_height = 0;
    
    
    //Shift of texture coordinates for each iteration
    float2 delta_tex = 0.1f * view.xy / view.z / num_layers;

    float2 current_tex_coords = tex_coords;
    
    //First depth from heightmap
    float height = txParallax.Sample(samLinear, current_tex_coords).r;

    float dx = ddx(tex_coords);
    float dy = ddy(tex_coords);

    
    //While point is above surface, find point that isnt - to then return
    while (height > current_layer_height)
    {
        //To Next Layer In list
        current_layer_height += layer_height;
        
        //Shift texture coordinate along view vector
        current_tex_coords -= delta_tex;
        
        //Get new depth from heightmap
        height = txParallax.SampleGrad(samLinear, current_tex_coords, dx, dy).r;
        

    }
    return current_tex_coords;
}
/*
    An improvement to steep parallax mapping , by interpolating results of the steep parallax map

*/
float2 OcclusionParallaxMapping(float3 view, float2 tex_coords)
{
    //Number of layers iterate threw
   const float min_layers = 0;
    const float max_layers = 35;
    
    const float num_layers = lerp(max_layers, min_layers, max(dot(float3(0.0f, 0.0f, 1.0f), view) , 0.0f));

    //height of each layer
    float layer_height = 1.0f / num_layers;
    
    //depth of current layer
    float current_layer_height = 0;
    
    
    //Shift of texture coordinates for each iteration
    float2 delta_tex = (view.xy / view.z * 0.1f) / num_layers;

    float2 current_tex_coords = tex_coords;
    
    //First depth from heightmap
    float height = txParallax.Sample(samLinear, current_tex_coords).r;



    
    //While point is above surface, find point that isnt - to then return
    [loop]
    while (height > current_layer_height)
    {
        //To Next Layer In list
        current_layer_height += layer_height;
        
        //Shift texture coordinate along view vector
        current_tex_coords -= delta_tex;
        
        //Get new depth from heightmap
        height = txParallax.Sample(samLinear, current_tex_coords).r;
    }

    float2 previous_tex_coords = current_tex_coords + delta_tex;
    
    //Heights for linear interpolation
    float after_height = height - current_layer_height;
    float previous_height = txParallax.Sample(samLinear, previous_tex_coords).a - current_layer_height + layer_height;
    
    
    //Proportions required for linear interpolation
    float weight = after_height / (after_height - previous_height);
    
    //Interpolation of texture coordinates
    float2 final_texture_coordinate = previous_tex_coords * weight + current_tex_coords * (1.0f - weight);
    
    return final_texture_coordinate;
}

float ParallaxSoftShadowMultiplier(float3 light , float2 tex_coords)
{
    float shadow_multiplier = 1;
    
    float initial_height = txParallax.Sample(samLinear, tex_coords).r;

    float min_layers = 15;
    float max_layers = 30;
    
    float dx = ddx(tex_coords);
    float dy = ddy(tex_coords);
    
    //Calculate lighting for surfaces oriented towards light source
    if (dot(float3(0.0f, 0.0f, 1.0f) , light) > 0)
    {
        //Initial Parameters
        float samples_under_surface = 0;
        
        shadow_multiplier = 0;
        
        float num_layers = lerp(max_layers, min_layers, abs(dot(float3(0.0f, 0.0f, 1.0f), light)));
        float layer_height = initial_height / num_layers;
        
        float2 delta_tex = light.xy / light.z / num_layers * 0.1f;
        
        
        //Current parameters
        float current_layer_height = initial_height - layer_height;
        float2 current_tex_coords = tex_coords - delta_tex;
        float height = txParallax.Sample(samLinear, current_tex_coords).r;
        int step_index = 1.0f;
        
        //While point is below depth of 0.0f
        while(current_layer_height > 0)
        {
            //If Point is under surface
            if (height < current_layer_height)
            {
                //Calculate partial shadowing factor
                samples_under_surface += 1.0f;
                
                float new_shadow_multiplier = (current_layer_height - height) * (1.0 - step_index / num_layers);
                shadow_multiplier = max(shadow_multiplier, new_shadow_multiplier);
            }
            
            //Offset to next layer
            step_index += 1;
            current_layer_height -= layer_height;
            current_tex_coords += delta_tex;
            height = txParallax.SampleGrad(samLinear, current_tex_coords ,dx , dy).r;
        }
        
          // Shadowing factor should be 1 if there were no points under the surface
        if (samples_under_surface < 1)
        {
            shadow_multiplier = 1.0f;
        }
        else
        {
            shadow_multiplier = 1.0f - shadow_multiplier;
        }
    }
    
    return shadow_multiplier;

}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT IN) : SV_TARGET
{
    //Build Calculated TBN Matrix
    float3x3 TBN = float3x3(IN.Tan, IN.Binorm, IN.Norm);
    float3x3 TBN_INV = transpose(TBN);
    
    //Converts View & Light Into Tangent Space
    float3 view = normalize(mul(TBN_INV,EyePosition.xyz - IN.worldPos.xyz));
    float3 light = normalize(mul(TBN_INV, Lights[0].Position.xyz - IN.worldPos.xyz));

    float2 tex_coord = IN.Tex;
    
    //Parallax Mapping
    float2 calculated_tex_coords = OcclusionParallaxMapping(view, tex_coord);
    //Parallax Self Shadowing
    float shadowFactor = ParallaxSoftShadowMultiplier(light, calculated_tex_coords);
 
    
    float3 bumpNormal = IN.Norm;
    float4 bumpMap;
    
    bumpMap = txNormal.Sample(samLinear, calculated_tex_coords);
	
	//Expand the range of the normal value from (0 , +1) to (-1 , +1)
    bumpMap = bumpMap * 2.0f - 1.0f;
    bumpNormal = normalize(mul(TBN_INV, bumpMap.xyz));

    LightingResult lit = ComputeLighting(view, bumpNormal, light);

    float4 texColor = { 1, 1, 1, 1 };
    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * GlobalAmbient;
    float4 diffuse = Material.Diffuse * lit.Diffuse;
    float4 specular = Material.Specular * lit.Specular ;

    if (Material.UseTexture)
    {
        texColor = txDiffuse.Sample(samLinear, calculated_tex_coords);
    }


    float4 finalColor = (emissive + ambient + diffuse + specular * shadowFactor) * texColor;

    return finalColor;
}

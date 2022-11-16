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
LightingResult DoPointLight(Light light, float3 vertexToEye, float3 vertexToLight, float3 normals)
{
    LightingResult result;

    float3 LightDirectionToVertex = mul(vertexToLight , -1.0f);
    float distance = length(LightDirectionToVertex);
    LightDirectionToVertex = LightDirectionToVertex / distance;

    
    distance = length(vertexToLight);
    vertexToLight = vertexToLight / distance;

    float attenuation = DoAttenuation(light, distance);
    attenuation = 1;


    result.Diffuse = DoDiffuse(light, vertexToLight, normals) * attenuation;
    result.Specular = DoSpecular(light, vertexToEye, LightDirectionToVertex, normals) * attenuation;

    return result;
}
LightingResult ComputeLighting(float3 normals, float3 vertexToEye, float3 vertexToLight)
{
    
    LightingResult totalResult = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	[unroll]
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

        if (!Lights[i].Enabled) 
            continue;
		
        result = DoPointLight(Lights[i], vertexToEye, vertexToLight, normals);
		
        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    totalResult.Diffuse = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);

    return totalResult;
}
/*
    Simple Parallax Mapping - Similar To Normal Mapping Technique, however
    with the goal of offsetting texture coordinates based upon the viewing direction
    to further simulate added depth upon the surface

    Parallax Mapping Formula

    Tp = T0 + V.xy / V.z * H(T0) * ParallaxScale;
    Or
    Tp = T0 + V.xy * H(T0) * ParallaxScale;
    
    Laymans:
    texture_offset = base_height * view_direction.xy / view_direction.z * height of original tex coord
    

*/
float2 SimpleParallaxMapping( float3 view, float2 tex_coords)
{
    
    //H(T0) - Height Of Original TexCoord - Used Within Offset Calculation
    float height = txParallax.Sample(samLinear, tex_coords).r;
    
    //T0- Base Height
    float base_height = 0.1f;
    
    //Tp (offset) = V.xy / V.z * Scale(Parallax_Height)
    float2 tex_coord_offset = base_height * view.xy / view.z * height;
    
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
    //H(T0) - Height Of Original TexCoord - However will change as layer is iterated
    float height_per_layer = txParallax.Sample(samLinear, tex_coords).r;
   
    float base_height = 0.1f;
    //Number of layers iterate threw
    float num_layers = 35;

    //height of each layer
    float layer_height = 1.0f  / num_layers;
    
    //depth of current layer
    float current_layer_height = 0;
    
    
    //Acts As Our Offset, But Since We Iterate Threw Multiple Layers , This Needs To Represent That Calculating
    //On 1 Step
    float2 delta_tex = base_height * view.xy / view.z / num_layers;

    float2 current_tex_coords = tex_coords;


    [loop]
    //While point is above surface, find point that isnt - to then return
    while (height_per_layer > current_layer_height)
    {
        //To Next Layer In list
        current_layer_height += layer_height;
        
        //Shift texture coordinate along view vector
        current_tex_coords -= delta_tex;
        
        //Get new depth from heightmap
        height_per_layer = txParallax.Sample(samLinear, current_tex_coords).r;
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
    float3 view_ts = normalize(mul(TBN_INV, EyePosition.xyz - IN.worldPos.xyz));
    float3 light_ts = normalize(mul(TBN, Lights[0].Position.xyz - IN.worldPos.xyz));

    float2 tex_coord = IN.Tex;
    
    //Parallax Mapping
    float2 calculated_tex_coords = SimpleParallaxMapping(view_ts, tex_coord);
    float shadow_factor = ParallaxSoftShadowMultiplier(light_ts, calculated_tex_coords);
    float3 bump_normals = IN.Norm;
    float4 bump_map;
    
    bump_map = txNormal.Sample(samLinear, calculated_tex_coords);
	
	//Expand the range of the normal value from (0 , +1) to (-1 , +1)
    bump_map = bump_map * 2.0f - 1.0f;
    bump_normals = normalize(mul(TBN_INV, bump_map.xyz));

    LightingResult lit = ComputeLighting(light_ts, view_ts, bump_normals);

    float4 texColor = { 1, 1, 1, 1 };
    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * GlobalAmbient;
    float4 diffuse = Material.Diffuse * lit.Diffuse;
    float4 specular = Material.Specular * lit.Specular ;

        texColor = txDiffuse.Sample(samLinear, IN.Tex);
    


    float4 finalColor = texColor;

    return finalColor;
}

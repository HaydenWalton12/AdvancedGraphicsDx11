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
float2 SteepParallaxMappingExperiment(float2 tex_coords, float3 viewing_direction)
{
    //Scale
    float height = 1.0f;
    
    //Number Of Depth Layers
    float num_steps = 10;
    
    //Resultant Coordinate To Return Initialised
    float2 offset_coord = tex_coords.xy;
    
    float current_depth_map_value = txParallax.Sample(samLinear, offset_coord).r;
    
    num_steps = lerp(num_steps * 2, viewing_direction.z, 0.0f);
    
    float2 delta = float2(-viewing_direction.x, viewing_direction.y) * 0.05 / (viewing_direction.z * num_steps);
    

    //Current Depth Layer
    //IE 1 / 10 = 0.1 (step Initialised to 0.1)
    float step = 1.0 / num_steps;
    float2 dx = ddx(tex_coords);
    float2 dy = ddy(tex_coords);

    while (current_depth_map_value < height)
    {
        height -= step;
        offset_coord += delta;
        current_depth_map_value = txParallax.SampleGrad(samLinear, offset_coord, dx, dy).r;
 
    }
    //Retuns finialised coordinate
    return offset_coord;
  
}
float2 ParallaxOcclusionMapping(float2 tex_coords, float3 viewing_direction)
{

 
    //Scale
    float height = 1.0f;
    
    //Number Of Depth Layers
    float num_steps = 50;
    
    //Resultant Coordinate To Return Initialised
    float2 offset_coord = tex_coords.xy;
    
    float current_depth_map_value = txParallax.Sample(samLinear, offset_coord).r;
    
    num_steps = lerp(num_steps * 2, viewing_direction.z, 0.0f);
    
    float2 delta = float2(-viewing_direction.x, viewing_direction.y) * 0.05 / (viewing_direction.z * num_steps);
    

    //Layer Depth
    //IE 1 / 10 = 0.1 (step Initialised to 0.1)
    float step = 1.0 / num_steps;
    
    float2 dx = ddx(tex_coords);
    float2 dy = ddy(tex_coords);
    
    float currentLayerDepth = 0.0f;

    while (current_depth_map_value < height)
    {
        step += currentLayerDepth;
        height -= step;
        offset_coord += delta;
        current_depth_map_value = txParallax.SampleGrad(samLinear, offset_coord, dx, dy).r;
 
    }
  
    float2 previous_tex_cord = offset_coord + delta;
    
    float afterDepth = current_depth_map_value - currentLayerDepth;
    float beforeDepth = txParallax.Sample(samLinear, previous_tex_cord).r - current_depth_map_value + step;
    
    //interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = previous_tex_cord * weight + offset_coord * (1.0 - weight);
    return finalTexCoords;

}

//Steep Parallax Mapping Works Upon Simple Parallax, but instead of 1 sample, multiple samples are done to better pinpoint P To The ideal
//value of B (B being the theoretical intersected point the view direction should see
//We Traverse each layer from top down , each layer we compare depth value to the depth value stored in the depth map (this is done to get the closest accurate sample to our point "b",
//b being the depth map value we want to sample closest too. We cannot just get it since we need to calculate from viewing direction, initial tex coord and the height map itself.


float2 SteepParallaxMapping(float2 tex_coords, float3 viewing_direction)
{

 
 
    
    //Iterative Layers We Sample From To Get Best Value - More Layers Typically The More Depth
    float minLayers = 8;
    float maxLayers = 32;
    
    //int numLayers = (int) lerp(maxLayers, minLayers, max(dot(float3(0.0f, 0.0f, 1.0f), viewing_direction), 0.0f));
    int numLayers = 10;
    //Controls Strength Of parallax map
    const int height_scale = 1.0f;
     
    //Calculate Each Size Of Each Layer To Ilterate threw
    float layerDepth = 1.0f / numLayers;
    
     
    //Calculate the P vector at current layer, will be used to calculate texture coordinate offset below
    float2 p = viewing_direction.xy / viewing_direction.z * height_scale;
    
    //Calculate Texture coordinate offset that shifts along the direction of P , per layer
    float2 deltaTexCoords = p / numLayers;

    
        //Get Initial Values
    float2 currentTexCoords = tex_coords;
    
    //Sample height map to get the current depth map value , used within the while loop to sample with 
    float currentDepthMapValue = txParallax.Sample(samLinear, currentTexCoords).r;
    
    //Depth of current layer - Initialising this to be utilised in while loop
    float currentLayerDepth = 0.0f;
    
    //Returns x from the current texcoord , so fourth with y
    float2 dx = ddx(tex_coords);
    float2 dy = ddy(tex_coords);

    //Will iterate threw each layer until the texture coordinate along vector p 
    //that returns a depth thats below the displaced surface , this resulting offset is
    //then subtracted with current texcoord to give us a final displaced texture
    //coordinate vector with is propotional to surface.
    while(currentLayerDepth < currentDepthMapValue)
    {

        //Shift Texture Coordinate along the direction of P
        currentTexCoords -= deltaTexCoords;
        
        //Get depthmap value at current texture coordinate
        currentDepthMapValue =  txParallax.SampleGrad(samLinear, currentTexCoords , dx , dy).r;
        
        //Get Depth Of Next Layer, Will keep doing this until we get a depth that is below
        //displaced surface, 
        currentLayerDepth += layerDepth;
    }
    
    //Retuns finialised coordinate
    return currentTexCoords;
  
}



//Primitive ParallaxMapping Process, Gives Rudimentary Results
float2 SimpleParallaxMapping(float2 tex_coords, float3 view_direction)
{
    //Surface TexCoord A
    float2 initial_tex_coord = tex_coords;
    
    
    //Height Of a Relative To The Height Map
    float height = txParallax.Sample(samLinear, initial_tex_coord).r;
    
    //P Is The Vector Used To Approximate The Offset Texture Coordinate
    //Uses the viewdirection to know the offset direction texture will be sampled from , the height of A "H(A)
    //0.1f represents the strength of parallax max itself , scales P 
    float2 P = view_direction.xy / view_direction.z * (height * 0.01f);   
    
    //Results, deducting A "initial_tex_coord" with P , this difference gives us our texture offset coordinate 
    return initial_tex_coord - P;
}




float CalculateParallaxSelfShadow(float3 L, float2 initialTexCoords)
{
    
    //Calculate Lighting only for surface oriented to the light source
    if (L.z >= 0.0)
    {
        return 0.0f;
    }
    
    float minLayers = 8;
    float maxLayers = 32;
    float a = abs(dot(float3(0.0f, 0.0f, 1.0f), L));
    //Lerp - Performs Linear interpolation
    int numLayers = lerp(maxLayers, minLayers, a);
    
    float layerDepth = 1.0f / numLayers;
    
    float2 currentTexCoords = initialTexCoords;
    float currentDepthMapValue = txParallax.Sample(samLinear, currentTexCoords).r;
    float currentLayerDepth = currentDepthMapValue;
    
    float P = L.xy / L.z * 1.0f;
    float2 deltaTexCoords = P / numLayers;
    
    
    float2 dx = ddx(initialTexCoords);
    float2 dy = ddy(initialTexCoords);
    
    while (currentLayerDepth <= currentDepthMapValue && currentLayerDepth > 0.0f)
    {
        currentTexCoords += deltaTexCoords;
        currentDepthMapValue = txParallax.SampleGrad(samLinear, currentTexCoords, dx, dy).r;
        currentLayerDepth -= layerDepth;
    }
    float r = currentLayerDepth > currentDepthMapValue ? 0.0 : 1.0;
    
    return r;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT IN) : SV_TARGET
{
    float shadowFactor = 1.0f;

    //Rebuilds TBN matrix from calculated vertex shader
    float3x3 tbn = float3x3(IN.Tan, IN.Binorm, IN.Norm);
    //float3 vertexToEye = normalize(mul(tbn, EyePosition - IN.worldPos.xyz));
    float3 vertexToEye = EyePosition - IN.worldPos.xyz;
    float3 vertexToLight = Lights[0].Position - IN.worldPos.xyz;
    
    //Creates Inverse TBN
    float3x3 TBN_inv = transpose(tbn);
    
    float3 eyeVectorTS = VectorToTangentSpace(vertexToEye.xyz, TBN_inv);
    float3 lightVectorTS = VectorToTangentSpace(vertexToEye.xyz, TBN_inv);

    float3 toEyeTS = VectorToTangentSpace(vertexToEye, TBN_inv);
  
    float2 texcoords = IN.Tex;
    float2 calculated_tex_coords = ParallaxOcclusionMapping(texcoords, toEyeTS);
    
 
    float3 bumpNormal = IN.Norm;
    float4 bumpMap;
    
    bumpMap = txNormal.Sample(samLinear, calculated_tex_coords);
	
	//Exampnd the range of the normal value from (0 , +1) to (-1 , +1)
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = normalize(mul(bumpMap.xyz, tbn));

    
    
    
    LightingResult lit = ComputeLighting(eyeVectorTS, bumpNormal, lightVectorTS);

    //Issue With this is that all lighting appears upon the surface of the normal map
    //lit = ComputeLighting(IN.worldPos, bumpMap);
    
    
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

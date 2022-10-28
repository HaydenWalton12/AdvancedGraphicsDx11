struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;

};


struct PS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};


PS_INPUT VS(VS_INPUT input)
{

    PS_INPUT output = (PS_INPUT) 0;

    output.Tex = input.Tex;
    output.Pos = input.Pos;
    return output;

}
//--------------------------------------------------------------------------------------
// Constant Buffer
//--------------------------------------------------------------------------------------

cbuffer cbPerFrame : register(b0)
{
    matrix        u_mViewProj          :    packoffset(c0);
};

cbuffer cbPerObject : register(b1)
{
    matrix        u_mWorld;
};

//--------------------------------------------------------------------------------------
// I/O Structures
//--------------------------------------------------------------------------------------

struct VS_INPUT_SCENE
{
    float3 Position     :    POSITION;      // vertex position
    float3 Normal       :    NORMAL;        // this normal comes in per-vertex
#ifdef HAS_TANGENT
    float4 Tangent      :    TANGENT;       // this normal comes in per-vertex
#endif    
#ifdef HAS_TEXCOORD
    float2 Texcoord     :    TEXCOORD;    // vertex texture coords
#endif
};

struct VS_OUTPUT_SCENE
{
    float4 svPosition   :    SV_POSITION;   // vertex position
    float3 WorldPos     :    WORLDPOS;      // vertex position
    float3 Normal       :    NORMAL;        // this normal comes in per-vertex
#ifdef HAS_TANGENT    
    float3 Tangent      :    TANGENT;       // this normal comes in per-vertex
    float3 Bitangent    :    BINORMAL;     // this normal comes in per-vertex
#endif        
    float2 Texcoord     :    TEXCOORD;      // vertex texture coords
};

//--------------------------------------------------------------------------------------
// Texture definitions
//--------------------------------------------------------------------------------------
#define TEX(b) t ## b

#ifdef ID_baseColorTexture
Texture2D        baseColorTexture              :register(TEX(ID_baseColorTexture));
#endif      

SamplerState     samLinearWrap              :register(s0);

//--------------------------------------------------------------------------------------
// mainVS
//--------------------------------------------------------------------------------------
VS_OUTPUT_SCENE mainVS(VS_INPUT_SCENE input)
{
    VS_OUTPUT_SCENE Output;

    Output.WorldPos = mul(u_mWorld, float4(input.Position, 1)).xyz;

    Output.svPosition = mul(u_mViewProj, float4(Output.WorldPos, 1));
    Output.Normal = normalize(mul(u_mWorld, float4(input.Normal, 0)).xyz);
#ifdef HAS_TANGENT    
    Output.Tangent = normalize(mul(u_mWorld, float4(input.Tangent.xyz, 0)).xyz);
    Output.Bitangent = cross(Output.Normal, Output.Tangent) *input.Tangent.w;
#endif    
#ifdef HAS_TEXCOORD
    Output.Texcoord = input.Texcoord;
#else
    Output.Texcoord = float2(0, 0);
#endif

    return Output;
}

void mainPS(VS_OUTPUT_SCENE Input)
{
#ifdef ID_baseColorTexture
    float4 baseColor = baseColorTexture.Sample(samLinearWrap, Input.Texcoord);

#if defined(DEF_alphaMode_MASK)
    if (baseColor.a <= DEF_alphaCutoff)
        discard;
#endif

#endif
}
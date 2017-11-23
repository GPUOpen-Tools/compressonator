//--------------------------------------------------------------------------------------
// Constant Buffer
//--------------------------------------------------------------------------------------
cbuffer cbPerFrame : register(b0)
{
    float u_invWidth;
    float u_invHeight;
}

//--------------------------------------------------------------------------------------
// I/O Structures
//--------------------------------------------------------------------------------------
struct VERTEX
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Texture definitions
//--------------------------------------------------------------------------------------
Texture2D        inputTex         :register(t0);
SamplerState     samLinearMirror  :register(s0);

//--------------------------------------------------------------------------------------
// Main function
//--------------------------------------------------------------------------------------

float4 mainPS(VERTEX Input) : SV_Target
{
    float4 color = inputTex.Sample(samLinearMirror, Input.vTexcoord);
    float grey = dot(color.rgb, float3(0.3, 0.59, 0.11));   
    
    if (grey > 0.9f)
        return color;
    else
        return float4(0,0,0,0);
}

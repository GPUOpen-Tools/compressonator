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
    // gaussian like downsampling

    float4 color = 0;
    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2( 2 * u_invWidth,  2 * u_invHeight));
    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2( 0 * u_invWidth,  2 * u_invHeight));
    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2(-2 * u_invWidth,  2 * u_invHeight));

    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2( 2 * u_invWidth,  0 * u_invHeight));
    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2( 0 * u_invWidth,  0 * u_invHeight));
    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2(-2 * u_invWidth,  0 * u_invHeight));

    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2( 2 * u_invWidth, -2 * u_invHeight));
    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2( 0 * u_invWidth, -2 * u_invHeight));
    color += inputTex.Sample(samLinearMirror, Input.vTexcoord + float2(-2 * u_invWidth,- 2 * u_invHeight));
    return color / 9.0f;
}

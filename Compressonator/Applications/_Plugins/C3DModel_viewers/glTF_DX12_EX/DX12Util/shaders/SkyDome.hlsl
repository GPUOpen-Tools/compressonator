//--------------------------------------------------------------------------------------
// Constant Buffer
//--------------------------------------------------------------------------------------
cbuffer cbPerFrame : register(b0)
{
    matrix u_mClipToWord;
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
TextureCube      inputTex         :register(t0);
SamplerState     samLinearWrap    :register(s0);

//--------------------------------------------------------------------------------------
// Main function
//--------------------------------------------------------------------------------------
float4 mainPS(VERTEX Input) : SV_Target
{
    float4 clip = float4(2 * Input.vTexcoord.x - 1, 1 - 2 * Input.vTexcoord.y, 1, 1);

    float4 pixelDir = mul(u_mClipToWord, clip);

    return inputTex.Sample(samLinearWrap, pixelDir.xyz);
}

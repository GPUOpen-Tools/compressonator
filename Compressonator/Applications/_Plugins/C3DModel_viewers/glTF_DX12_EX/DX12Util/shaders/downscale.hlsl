//=====================================================================
// Copyright 2019 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

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

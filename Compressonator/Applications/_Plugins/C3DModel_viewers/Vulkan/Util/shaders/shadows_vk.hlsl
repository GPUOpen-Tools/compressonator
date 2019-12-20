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

    #ifdef DEF_alphaMode_BLEND
        if (baseColor.a <= 0.01f)
            discard;
    #endif

#endif
}
//
// Portions Copyright 2019 Advanced Micro Devices, Inc. All rights reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This shader code was ported from https://github.com/KhronosGroup/glTF-WebGL-PBR
// All credits should go to his original author.


//
// This fragment shader defines a reference implementation for Physically Based Shading of
// a microfacet surface material defined by a glTF model.
//
// References:
// [1] Real Shading in Unreal Engine 4
//     http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// [2] Physically Based Shading at Disney
//     http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
// [3] README.md - Environment Maps
//     https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps
// [4] "An Inexpensive BRDF Model for Physically based Rendering" by Christophe Schlick
//     https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf


//--------------------------------------------------------------------------------------
// Constant Buffer
//--------------------------------------------------------------------------------------

cbuffer cbPerFrame : register(b0)
{
    matrix        u_mCameraViewProj    :    packoffset(c0);
    float4        u_CameraPos          :    packoffset(c4);
    matrix        u_mLightViewProj     :    packoffset(c5);
    float4        u_LightDirection     :    packoffset(c9);
    float4        u_LightColor         :    packoffset(c10);
    float         u_depthBias          :    packoffset(c11.x);
    float         u_iblFactor          :    packoffset(c11.y);
};

cbuffer cbPerObject : register(b1)
{
    matrix        u_mWorld;
    float4        u_EmissiveFactor;
    float4        u_BaseColorFactor;
    float2        u_MetallicRoughnessValues;
    float2        u_padding;
};

//--------------------------------------------------------------------------------------
// I/O Structures
//--------------------------------------------------------------------------------------

struct VS_INPUT_SCENE
{
    float3 Position     :    POSITION;      // vertex position
#ifdef HAS_NORMAL
    float3 Normal       :    NORMAL;        // this normal comes in per-vertex
#endif
#ifdef HAS_TANGENT
    float4 Tangent      :    TANGENT;       // this tangent comes in per-vertex
#endif    
#ifdef HAS_TEXCOORD
    float2 Texcoord     :    TEXCOORD;      // vertex texture coords
#endif
};

struct VS_OUTPUT_SCENE
{
    float4 svPosition   :    SV_POSITION;   // vertex position
    float3 WorldPos     :    WORLDPOS;      // vertex position
#ifdef HAS_NORMAL
    float3 Normal       :    NORMAL;        // this normal comes in per-vertex
#endif
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
#ifdef ID_metallicRoughnessTexture
Texture2D        metallicRoughnessTexture      :register(TEX(ID_metallicRoughnessTexture));
#endif      
#ifdef ID_normalTexture
Texture2D        normalTexture                 :register(TEX(ID_normalTexture));
#endif      
#if defined(ID_occlusionTexture) 
    #if defined(ID_metallicRoughnessTexture)  && (ID_occlusionTexture == ID_metallicRoughnessTexture)
    #define occlusionTexture metallicRoughnessTexture
    #else
    Texture2D        occlusionTexture              :register(TEX(ID_occlusionTexture));
    #endif
#endif      
#ifdef ID_emissiveTexture  
Texture2D        emissiveTexture               :register(TEX(ID_emissiveTexture));
#endif 
#ifdef ID_diffuseCube
TextureCube      diffuseCube                 :register(TEX(ID_diffuseCube));
#endif 
#ifdef ID_specularCube
TextureCube      specularCube                :register(TEX(ID_specularCube));
#endif 
#ifdef ID_brdfTexture
Texture2D        brdfTexture                   :register(TEX(ID_brdfTexture));
#endif 
#ifdef ID_shadowMap
Texture2D        shadowMap                     :register(TEX(ID_shadowMap));
#endif 

//--------------------------------------------------------------------------------------
// Samplers
//--------------------------------------------------------------------------------------

SamplerState     samLinearWrap              :register(s0);
SamplerState     samDiffuseEnv              :register(s1);
SamplerState     samSpecularEnv             :register(s2);
SamplerState     samBRDF                    :register(s3);

SamplerComparisonState samShadow            :register(s4);

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    float3 reflectance0;          // full reflectance color (normal incidence angle)
    float3 reflectance90;         // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    float3 diffuseColor;          // color contribution from diffuse lighting
    float3 specularColor;         // color contribution from specular lighting
};

//--------------------------------------------------------------------------------------
// mainVS
//--------------------------------------------------------------------------------------
VS_OUTPUT_SCENE mainVS(VS_INPUT_SCENE input)
{
    VS_OUTPUT_SCENE Output;

    Output.WorldPos = mul(u_mWorld, float4(input.Position, 1)).xyz;    

    Output.svPosition = mul(u_mCameraViewProj, float4(Output.WorldPos,1));
#ifdef HAS_NORMAL
    Output.Normal  = normalize(mul(u_mWorld, float4(input.Normal, 0)).xyz);
#endif
#ifdef HAS_TANGENT    
    Output.Tangent = normalize(mul(u_mWorld, float4(input.Tangent.xyz, 0)).xyz);
    Output.Bitangent = cross(Output.Normal, Output.Tangent) *input.Tangent.w;
#endif    
#ifdef HAS_TEXCOORD
    Output.Texcoord = input.Texcoord;
#else
    Output.Texcoord = float2(0,0);
#endif

    return Output;
}

#ifdef ID_shadowMap
float FilterShadow(float3 uv)
{
    float shadow = 0.0;

    static const int kernelLevel = 2;
    static const int kernelWidth = 2 * kernelLevel + 1;
    [unroll] for (int i = -kernelLevel; i <= kernelLevel; i++)
    {
        [unroll] for (int j = -kernelLevel; j <= kernelLevel; j++)
        {
            shadow += shadowMap.SampleCmpLevelZero(samShadow, uv.xy, uv.z, int2(i, j)).r;
        }
    }

    shadow /= (kernelWidth*kernelWidth);
    return shadow;
}

float DoSpotShadow(in float3 vPosition)
{
    float4 shadowTexCoord = mul(u_mLightViewProj, float4(vPosition, 1));
    shadowTexCoord.xyz = shadowTexCoord.xyz / shadowTexCoord.w;

    shadowTexCoord.x = shadowTexCoord.x / 2 + 0.5;
    shadowTexCoord.y = shadowTexCoord.y / -2 + 0.5;

    if ((shadowTexCoord.y < 0) || (shadowTexCoord.y > 1)) return 0;
    if ((shadowTexCoord.x < 0) || (shadowTexCoord.x > 1)) return 0;

    shadowTexCoord.z -= u_depthBias;
    
    return FilterShadow(shadowTexCoord.xyz);
}
#endif

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
float3 getNormal(VS_OUTPUT_SCENE Input)
{
    float3 n = { 0.0f, 0.0f, -1.0f };

    // Retrieve the tangent space matrix
#ifndef HAS_TANGENT

#ifdef HAS_TEXCOORD
    float3 pos_dx = ddx(Input.WorldPos.xyz);
    float3 pos_dy = ddy(Input.WorldPos.xyz);
    float3 tex_dx = ddx(float3(Input.Texcoord, 0.0));
    float3 tex_dy = ddy(float3(Input.Texcoord, 0.0));
    float3 t = (tex_dy.y * pos_dx - tex_dx.y * pos_dy) / (tex_dx.x * tex_dy.y - tex_dy.x * tex_dx.y);

#ifdef HAS_NORMAL
    float3 ng = normalize(Input.Normal);
#else
    float3 ng = cross(pos_dx, pos_dy);
#endif

    t = normalize(t - ng * dot(ng, t));
    float3 b = normalize(cross(ng, t));
    float3x3 tbn = transpose(float3x3(t, b, ng));

#else 
#ifdef HAS_NORMAL
    n =  Input.Normal;
#endif
#endif

#else // HAS_TANGENTS
    float3x3 tbn = transpose(float3x3(Input.Tangent, Input.Bitangent, Input.Normal));
#endif

#ifdef HAS_TEXCOORD
#ifdef ID_normalTexture
    n = normalTexture.Sample(samLinearWrap, Input.Texcoord).xyz;
    n = normalize(mul(tbn, ((2.0 * n - 1.0) )));
#else
    n = tbn[2].xyz;
#endif
#endif

    return n;
}

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
float3 getIBLContribution(PBRInfo pbrInputs, float3 n, float3 reflection)
{
    float mipCount = 9.0; // resolution of 512x512
    float lod = (pbrInputs.perceptualRoughness * mipCount);
    // retrieve a scale and bias to F0. See [1], Figure 3
    float3 brdf = brdfTexture.Sample(samBRDF, float2(pbrInputs.NdotV, 1.0 - pbrInputs.perceptualRoughness)).rgb;
    float3 diffuseLight = diffuseCube.Sample(samDiffuseEnv, n).rgb;

#define USE_TEX_LOD 1

#ifdef USE_TEX_LOD
    float3 specularLight = specularCube.SampleLevel(samSpecularEnv, reflection, lod).rgb;
#else
    float3 specularLight = specularCube.Sample(samSpecularEnv, reflection).rgb;
#endif

    float3 diffuse = diffuseLight * pbrInputs.diffuseColor;
    float3 specular = specularLight * (pbrInputs.specularColor * brdf.x + brdf.y);

    return diffuse + specular;
}

#define M_PI  3.141592653589793
#define c_MinRoughness  0.04

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
float3 diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
float3 specularReflection(PBRInfo pbrInputs)
{
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
{
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (M_PI * f * f);
}

//--------------------------------------------------------------------------------------
// mainPS
//--------------------------------------------------------------------------------------
float4 mainPS(VS_OUTPUT_SCENE Input) : SV_Target
{   
    // Metallic and Roughness material properties are packed together
    // In glTF, these factors can be specified by fixed scalar values
    // or from a metallic-roughness map
    float perceptualRoughness = u_MetallicRoughnessValues.y;
    float metallic = u_MetallicRoughnessValues.x;

#ifdef ID_metallicRoughnessTexture
    // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
    // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
    float4 mrSample = metallicRoughnessTexture.Sample(samLinearWrap, Input.Texcoord);
    perceptualRoughness = mrSample.g * perceptualRoughness;
    metallic = mrSample.b * metallic;
#endif    
    
    perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    
    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    float alphaRoughness = perceptualRoughness * perceptualRoughness;
    
    // The albedo may be defined from a base texture or a flat color
#ifdef ID_baseColorTexture
    float4 baseColor = baseColorTexture.Sample(samLinearWrap, Input.Texcoord) * u_BaseColorFactor;

    #if defined(DEF_alphaMode_MASK)
    if (baseColor.a <= DEF_alphaCutoff) 
        discard;
    #endif

#else
    float4 baseColor = u_BaseColorFactor;
#endif    
    
    float3 f0 = float3(1.0, 1.0, 1.0) * 0.04;
    float3 diffuseColor = baseColor.rgb * (float3(1.0, 1.0, 1.0) - f0);
    diffuseColor *= 1.0 - metallic;
    float3 specularColor = lerp(f0, baseColor.rgb, metallic);    

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);
    
    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    float3 specularEnvironmentR0 = specularColor.rgb;
    float3 specularEnvironmentR90 = float3(1.0, 1.0, 1.0) * reflectance90;    
    
    float3 n = getNormal(Input);                        // normal at surface point    
    float3 v = normalize(u_CameraPos.xyz - Input.WorldPos.xyz);        // Vector from surface point to camera    
    float3 l = normalize(u_LightDirection.xyz);             // Vector from surface point to light
    float3 h = normalize(l+v);                          // Half vector between both l and v
    
    float3 reflection = -normalize(reflect(v, n));
    
    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = abs(dot(n, v)) + 0.001;
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);
        
    PBRInfo pbrInputs = {
        NdotL,
        NdotV,
        NdotH,
        LdotH,
        VdotH,
        perceptualRoughness,
        metallic,
        specularEnvironmentR0,
        specularEnvironmentR90,
        alphaRoughness,
        diffuseColor,
        specularColor
    };        
    
    // Calculate the shading terms for the microfacet specular shading model
    float3 F = specularReflection(pbrInputs);
    float  G = geometricOcclusion(pbrInputs);
    float  D = microfacetDistribution(pbrInputs);

    // Calculation of analytical lighting contribution
    float3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
    float3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
    float3 color = NdotL * u_LightColor.rgb *  (diffuseContrib + specContrib);    

#ifdef ID_shadowMap
    color *= DoSpotShadow(Input.WorldPos);
#endif

    // Calculate lighting contribution from image based lighting source (IBL)
#ifdef USE_IBL
    color +=  getIBLContribution(pbrInputs, n, reflection) * u_iblFactor;
#endif

#if defined(ID_occlusionTexture)
    float ao = occlusionTexture.Sample(samLinearWrap, Input.Texcoord).r;
    color = lerp(color, color * ao, 1);
#endif

#ifdef ID_emissiveTexture
    color += emissiveTexture.Sample(samLinearWrap, Input.Texcoord).rgb * u_EmissiveFactor.rgb;
#endif

    return float4(color, baseColor.a);
}

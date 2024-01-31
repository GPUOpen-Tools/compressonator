// 607dfc9f-----------------------------------------------------------------------------
//==============================================================================
// Copyright (c) 2020-2024    Advanced Micro Devices, Inc. All rights reserved.
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
//===============================================================================
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------
#ifndef ASPM_GPU
#define ASPM_GPU   // This is required
#endif
#ifndef ASPM_HLSL
#define ASPM_HLSL  // This is required
#endif

//#define USE_CMP           // Use New Dev Compressonator Codec for v4.2
#define USE_CMPMSC        // Use New Dev Compressonator Codec for v4.2
//#define USE_MSC  // Microsoft DirectXTex HLSL all modes

//#define USE_ENCODEBLOCKS_ONLY
//#define USE_INT           // Compiles on CPU. Compiles on HLSL in 38 seconds with reg and loop warnings : Images are good
//#define USE_RGBCX_RDO     // Compiles on CPU Images are good. Does not compile on HLSL
//#define USE_VOLT          // Compiles on CPU + HLSL but image is curupt
//#define USE_ICBC          // Compiles on CPU Does not compile on HLSL
//#define USE_ARRIS         // Arris port over of rgbcx_rdo code for GPU


#define CHAR_LENGTH         8
#define NCHANNELS           4
#define BC7_UNORM           98
#define MAX_UINT            0xFFFFFFFF
#define MIN_UINT            0
#define BLOCK_SIZE_Y        4
#define BLOCK_SIZE_X        4
#define BLOCK_SIZE          (BLOCK_SIZE_Y * BLOCK_SIZE_X)
#define THREAD_GROUP_SIZE   64

// Source Texture to process
Texture2D                 g_Input   : register(t0);

struct BufferShared
{
    uint4 pixel;
    uint  error;
    uint  mode;
    uint  partition;
    uint  index_selector;
    uint  rotation;
    uint  pbit;
    uint4 endPoint_low;
    uint4 endPoint_high;
    uint4 endPoint_low_quantized;
    uint4 endPoint_high_quantized;
    uint  colorindex;
    uint  alphaindex;
};

groupshared BufferShared shared_temp[THREAD_GROUP_SIZE];

cbuffer cbCS : register(b0)
{
    uint  g_tex_width;
    uint  g_num_block_x;
    uint  g_format;
    uint  g_mode_id;
    uint  g_start_block_id;
    uint  g_num_total_blocks;
    float g_alpha_weight;
    float g_quality;
};


struct SharedIOData
{
    uint  error;
    uint  mode;
    uint  index_selector;
    uint  rotation;
    uint  partition;
    uint4 data2;
};

StructuredBuffer<SharedIOData>     g_InBuff : register(t1);
RWStructuredBuffer<SharedIOData>   g_OutBuff1 : register(u0);  // Used by TryMode...
RWStructuredBuffer<uint4>          g_OutBuff : register(u0);   // Used by EncodeBlocks & TryMode...


#include "bc7_common_encoder.h"


#ifdef USE_ENCODEBLOCKS_ONLY

[numthreads(THREAD_GROUP_SIZE, 1, 1)] void TryMode456CS(uint GI : SV_GroupIndex, uint3 groupID : SV_GroupID)
{
    // Do nothing
}

[numthreads( THREAD_GROUP_SIZE, 1, 1 )]
void TryMode137CS(uint GI : SV_GroupIndex, uint3 groupID : SV_GroupID)
{
    // Do nothing
}

[numthreads( THREAD_GROUP_SIZE, 1, 1 )]
void TryMode02CS(uint GI : SV_GroupIndex, uint3 groupID : SV_GroupID)
{
    // Do nothing
}

[numthreads(THREAD_GROUP_SIZE, 1, 1)] 
void EncodeBlocks(uint GI: SV_GroupIndex, uint3 groupID: SV_GroupID) 
{
    // we process 4 BC blocks per thread group
    uint blockInGroup = GI / MAX_USED_THREAD;                                          // what BC block this thread is on within this thread group
    uint blockID      = g_start_block_id + groupID.x * BLOCK_IN_GROUP + blockInGroup;  // what global BC block this thread is on
    uint pixelBase    = blockInGroup * MAX_USED_THREAD;                                // the first id of the pixel in this BC block in this thread group
    uint pixelInBlock = GI - pixelBase;                                                // id of the pixel in this BC block

    uint block_y = blockID / g_num_block_x;
    uint block_x = blockID - block_y * g_num_block_x;
    uint base_x  = block_x * BLOCK_SIZE_X;
    uint base_y  = block_y * BLOCK_SIZE_Y;

    // Load up the pixels
    if (pixelInBlock < 16)
    {
        // load pixels (0..1)
        shared_temp[GI].pixel = g_Input.Load(uint3(base_x + pixelInBlock % 4, base_y + pixelInBlock / 4, 0));
    }

    GroupMemoryBarrierWithGroupSync();

    // Process and save s
    if (pixelInBlock == 0)
    {
        float4      image_src[16];
        for (int i = 0; i < 16; i++)
        {
            image_src[i].x = shared_temp[pixelBase + i].pixel.x * 255;
            image_src[i].y = shared_temp[pixelBase + i].pixel.y * 255;
            image_src[i].z = shared_temp[pixelBase + i].pixel.z * 255;
            image_src[i].w = shared_temp[pixelBase + i].pixel.w * 255;
        }
        g_OutBuff[blockID] = CompressBlockBC7_UNORM(image_src, g_quality);
    }
}
#endif

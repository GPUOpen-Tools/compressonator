//===============================================================================
// Copyright (c) 2007-2024  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
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
//
//  File Name:   Codec_DXT5_RGxB.cpp
//  Description: implementation of the CCodec_DXT5_RGxB class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codec_dxt5_rgxb.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_DXT5_RGxB::CCodec_DXT5_RGxB()
    : CCodec_DXT5_Swizzled(CT_DXT5_RGxB)
{
}

CCodec_DXT5_RGxB::~CCodec_DXT5_RGxB()
{
}

void CCodec_DXT5_RGxB::ReadBlock(CCodecBuffer& buffer, CMP_DWORD x, CMP_DWORD y, CMP_BYTE block[BLOCK_SIZE_4X4X4])
{
    CMP_BYTE dwTempBlock[BLOCK_SIZE_4X4X4];
    buffer.ReadBlockRGBA(x, y, 4, 4, dwTempBlock);
    for (CMP_DWORD i = 0; i < BLOCK_SIZE_4X4; i++)
        ((CMP_DWORD*)block)[i] = SWIZZLE_RGBA_RGxB(((CMP_DWORD*)dwTempBlock)[i]);
}

void CCodec_DXT5_RGxB::WriteBlock(CCodecBuffer& buffer, CMP_DWORD x, CMP_DWORD y, CMP_BYTE block[BLOCK_SIZE_4X4X4])
{
    CMP_BYTE dwTempBlock[BLOCK_SIZE_4X4X4];
    for (CMP_DWORD i = 0; i < BLOCK_SIZE_4X4; i++)
        ((CMP_DWORD*)dwTempBlock)[i] = SWIZZLE_RGxB_RGBA(((CMP_DWORD*)block)[i]);
    buffer.WriteBlockRGBA(x, y, 4, 4, dwTempBlock);
}

void CCodec_DXT5_RGxB::ReadBlock(CCodecBuffer& buffer, CMP_DWORD x, CMP_DWORD y, CODECFLOAT block[BLOCK_SIZE_4X4X4])
{
    CODECFLOAT fTempBlock[BLOCK_SIZE_4X4X4];
    buffer.ReadBlockRGBA(x, y, 4, 4, fTempBlock);
    for (CMP_DWORD i = 0; i < BLOCK_SIZE_4X4; i++)
    {
        block[(i * 4) + RGBA32F_OFFSET_R] = fTempBlock[(i * 4) + RGBA32F_OFFSET_R];
        block[(i * 4) + RGBA32F_OFFSET_G] = fTempBlock[(i * 4) + RGBA32F_OFFSET_G];
        block[(i * 4) + RGBA32F_OFFSET_B] = 0.0;
        block[(i * 4) + RGBA32F_OFFSET_A] = fTempBlock[(i * 4) + RGBA32F_OFFSET_B];
    }
}

void CCodec_DXT5_RGxB::WriteBlock(CCodecBuffer& buffer, CMP_DWORD x, CMP_DWORD y, CODECFLOAT block[BLOCK_SIZE_4X4X4])
{
    CODECFLOAT fTempBlock[BLOCK_SIZE_4X4X4];
    for (CMP_DWORD i = 0; i < BLOCK_SIZE_4X4; i++)
    {
        fTempBlock[(i * 4) + RGBA32F_OFFSET_R] = block[(i * 4) + RGBA32F_OFFSET_R];
        fTempBlock[(i * 4) + RGBA32F_OFFSET_G] = block[(i * 4) + RGBA32F_OFFSET_G];
        fTempBlock[(i * 4) + RGBA32F_OFFSET_B] = block[(i * 4) + RGBA32F_OFFSET_A];
        fTempBlock[(i * 4) + RGBA32F_OFFSET_A] = 0.0;
    }
    buffer.WriteBlockRGBA(x, y, 4, 4, fTempBlock);
}
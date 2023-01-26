//=====================================================================
// Copyright (c) 2019-2022    Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================
#include "brlg_encode_kernel.h"

#pragma warning(disable:4505)  // disable warnings on unreferenced local function has been removed

#ifndef ASPM_GPU // CPU based 
//================= Decompression ==========================

void BRLG_DecompressBlock(
    CMP_GLOBAL CGU_UINT8  out[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
    CGU_UINT8  in[COMPRESSED_BLOCK_SIZE]
)
{
    // Code todo Decompression
}

#endif

//============================== API =====================================
void CompressBlockBRLG_Internal(
    CMP_Vec4uc  srcBlockTemp[MAX_SUBSET_SIZE],
    CMP_GLOBAL  CGU_UINT8 *block_out,
    CMP_GLOBAL  CMP_BRLGEncode *BRLGEncode )
{

}


CMP_STATIC CMP_KERNEL void CMP_GPUEncoder(
    CMP_GLOBAL const CMP_Vec4uc ImageSourceRGBA[],
    CMP_GLOBAL  CGV_UINT8       ImageDestination[],
    CMP_GLOBAL  Source_Info     SourceInfo[],
    CMP_GLOBAL  CMP_BRLGEncode   BRLGEncode[]
)
{
}


// Use CPU to process data
#ifndef ASPM_GPU
void CMP_CDECL CompressBlockBRLG(unsigned char srcBlock[64], unsigned char cmpBlock[16], CMP_GLOBAL CMP_BRLGEncode* BRLGEncode = NULL)
{
    if (BRLGEncode == NULL)
    {
        // set for q = 1.0
        CMP_BRLGEncode BRLGEncodeDefault;
        BRLGEncode = &BRLGEncodeDefault;
        SetDefaultBRLGOptions(BRLGEncode);
    }
    CompressBlockBRLG_Internal((CMP_Vec4uc*)srcBlock, (CGU_UINT8*)cmpBlock, BRLGEncode);
}

void CMP_CDECL DecompressBRLG(unsigned char cmpBlock[16], unsigned char srcBlock[64])
{
    BRLG_DecompressBlock((CGU_UINT8(*)[4])srcBlock, (CGU_UINT8*)cmpBlock);
}
#endif



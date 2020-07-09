//=====================================================================
// Copyright (c) 2020   Advanced Micro Devices, Inc. All rights reserved.
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
#include "BC5_Encode_kernel.h"

//============================================== BC5 INTERFACES =======================================================

CGU_Vec4ui CompressBC5Block_Internal(CMP_IN CGU_FLOAT aBlockU[16], CMP_IN CGU_FLOAT aBlockV[16], CMP_IN CGU_FLOAT fquality) 
{
    CGU_Vec4ui   compBlock;
    CGU_Vec2ui cmpBlock;

    cmpBlock = cmp_compressAlphaBlock(aBlockU,fquality);
    compBlock.x = cmpBlock.x;
    compBlock.y = cmpBlock.y;

    cmpBlock = cmp_compressAlphaBlock(aBlockV,fquality);
    compBlock.z = cmpBlock.x;
    compBlock.w = cmpBlock.y;
    return compBlock;
}

#ifndef ASPM_HLSL
void  CompressBlockBC5_Internal(CMP_Vec4uc srcBlockTemp[16],                    // range 0 to 255
                                CMP_GLOBAL CGU_UINT32 compressedBlock[4],
                                CMP_GLOBAL  CMP_BC15Options *BC15options)
{
    CGU_Vec4ui   cmpBlock;
    CGU_FLOAT    alphaBlockU[16];
    CGU_FLOAT    alphaBlockV[16];
    CGU_UINT32   i;

    for (i = 0; i < 16; i++) {
        alphaBlockU[i] =  srcBlockTemp[i].x  / 255.0f;
        alphaBlockV[i] =  srcBlockTemp[i].y / 255.0f;
    }

    cmpBlock = CompressBC5Block_Internal(alphaBlockU, alphaBlockV,BC15options->m_fquality);
    compressedBlock[0] = cmpBlock.x;
    compressedBlock[1] = cmpBlock.y;
    compressedBlock[2] = cmpBlock.z;
    compressedBlock[3] = cmpBlock.w;
}
#endif

#ifndef ASPM_GPU
void  DecompressBC5_Internal(CMP_GLOBAL CGU_UINT8 rgbaBlock[64], 
                             CGU_UINT32 compressedBlock[4],
                             CMP_BC15Options *BC15options)
{
    CGU_UINT8 alphaBlockR[BLOCK_SIZE_4X4];
    CGU_UINT8 alphaBlockG[BLOCK_SIZE_4X4];

    cmp_decompressAlphaBlock(alphaBlockR, &compressedBlock[0]);
    cmp_decompressAlphaBlock(alphaBlockG, &compressedBlock[2]);
 
    CGU_UINT8    blkindex = 0;
    CGU_UINT8    srcindex = 0;

    if (BC15options->m_mapDecodeRGBA)
    {
        for (CGU_INT32 j = 0; j < 4; j++) {
            for (CGU_INT32 i = 0; i < 4; i++) {
                rgbaBlock[blkindex++] = (CGU_UINT8)alphaBlockR[srcindex];
                rgbaBlock[blkindex++] = (CGU_UINT8)alphaBlockG[srcindex];
                rgbaBlock[blkindex++] = 0;
                rgbaBlock[blkindex++] = 255;
                srcindex++;
            }
        }
    }
    else
    {
        for (CGU_INT32 j = 0; j < 4; j++) {
            for (CGU_INT32 i = 0; i < 4; i++) {
                rgbaBlock[blkindex++] = 0;
                rgbaBlock[blkindex++] = (CGU_UINT8)alphaBlockG[srcindex];
                rgbaBlock[blkindex++] = (CGU_UINT8)alphaBlockR[srcindex];
                rgbaBlock[blkindex++] = 255;
                srcindex++;
            }
        }
    }

}

void  CompressBlockBC5_DualChannel_Internal(const CGU_UINT8 srcBlockR[16],
                                            const CGU_UINT8 srcBlockG[16],
                                            CMP_GLOBAL  CGU_UINT32 compressedBlock[4],
                                            CMP_GLOBAL  const CMP_BC15Options *BC15options)
{
    if (BC15options) {}
    CGU_Vec2ui    cmpBlock;
    CGU_FLOAT     srcAlphaRF[16];
    CGU_FLOAT     srcAlphaGF[16];

    for (CGU_INT i =0; i< 16; i++)
    {
        srcAlphaRF[i] = (CGU_FLOAT)( srcBlockR[i] ) / 255.0f;
        srcAlphaGF[i] = (CGU_FLOAT)( srcBlockG[i] ) / 255.0f;
    } 

    cmpBlock = cmp_compressAlphaBlock(srcAlphaRF,BC15options->m_fquality);
    compressedBlock[0] = cmpBlock.x;
    compressedBlock[1] = cmpBlock.y;

    cmpBlock = cmp_compressAlphaBlock(srcAlphaGF,BC15options->m_fquality);
    compressedBlock[2] = cmpBlock.x;
    compressedBlock[3] = cmpBlock.y;
}

void  DecompressBC5_DualChannel_Internal(CMP_GLOBAL CGU_UINT8 srcBlockR[16],
                                         CMP_GLOBAL CGU_UINT8 srcBlockG[16], 
                                         const CGU_UINT32 compressedBlock[4],
                                         const CMP_BC15Options *BC15options)
{
    if (BC15options) {}
    cmp_decompressAlphaBlock(srcBlockR, &compressedBlock[0]);
    cmp_decompressAlphaBlock(srcBlockG, &compressedBlock[2]);
}
#endif

//============================================== USER INTERFACES ========================================================
#ifndef ASPM_GPU

int CMP_CDECL CreateOptionsBC5(void **options)
{
    CMP_BC15Options *BC15optionsDefault = new CMP_BC15Options;
    if (BC15optionsDefault) {
        SetDefaultBC15Options(BC15optionsDefault);
        (*options) = BC15optionsDefault;
    }
    else {
        (*options) = NULL;
        return CGU_CORE_ERR_NEWMEM;
    }
    return CGU_CORE_OK;
}

int CMP_CDECL DestroyOptionsBC5(void *options)
{
    if (!options) return CGU_CORE_ERR_INVALIDPTR;
    CMP_BC15Options *BCOptions = reinterpret_cast <CMP_BC15Options *>(options);
    delete BCOptions;
    return CGU_CORE_OK;
}

int CMP_CDECL SetQualityBC5(void *options,
    CGU_FLOAT fquality)
{
    if (!options) return CGU_CORE_ERR_INVALIDPTR;
    CMP_BC15Options *BC15optionsDefault = reinterpret_cast <CMP_BC15Options *>(options);
    if (fquality < 0.0f) fquality = 0.0f;
    else
        if (fquality > 1.0f) fquality = 1.0f;
    BC15optionsDefault->m_fquality = fquality;
    return CGU_CORE_OK;
}

int CMP_CDECL SetSignedBC5(void *options,
                           CGU_BOOL snorm) {
    if (!options) return CGU_CORE_ERR_INVALIDPTR;
    CMP_BC15Options *BC15optionsDefault = reinterpret_cast <CMP_BC15Options *>(options);
    
    BC15optionsDefault->m_bIsSNORM = snorm;
    return CGU_CORE_OK;
}

int CMP_CDECL CompressBlockBC5(const CGU_UINT8 *srcBlockR,
                               unsigned int srcStrideInBytes1,
                               const CGU_UINT8 *srcBlockG,
                               unsigned int srcStrideInBytes2,
                               CMP_GLOBAL CGU_UINT8 cmpBlock[16],
                               const void *options = NULL) {
    CGU_UINT8 inBlockR[16];

    //----------------------------------
    // Fill the inBlock with source data
    //----------------------------------
    CGU_INT srcpos = 0;
    CGU_INT dstptr = 0;
    for (CGU_UINT8 row = 0; row < 4; row++)
    {
        srcpos = row * srcStrideInBytes1;
        for (CGU_UINT8 col = 0; col < 4; col++)
        {
            inBlockR[dstptr++] = CGU_UINT8(srcBlockR[srcpos++]);
        }
    }


    CGU_UINT8 inBlockG[16];
    //----------------------------------
    // Fill the inBlock with source data
    //----------------------------------
    srcpos = 0;
    dstptr = 0;
    for (CGU_UINT8 row = 0; row < 4; row++)
    {
        srcpos = row * srcStrideInBytes2;
        for (CGU_UINT8 col = 0; col < 4; col++)
        {
            inBlockG[dstptr++] = CGU_UINT8(srcBlockG[srcpos++]);
        }
    }


    CMP_BC15Options *BC15options = (CMP_BC15Options *)options;
    CMP_BC15Options BC15optionsDefault;
    if (BC15options == NULL)
    {
        BC15options = &BC15optionsDefault;
        SetDefaultBC15Options(BC15options);
    }

    CompressBlockBC5_DualChannel_Internal(inBlockR,inBlockG, (CMP_GLOBAL CGU_UINT32 *)cmpBlock, BC15options);
    return CGU_CORE_OK;
}

int  CMP_CDECL DecompressBlockBC5(const CGU_UINT8 cmpBlock[16],
                              CMP_GLOBAL CGU_UINT8 srcBlockR[16],
                              CMP_GLOBAL CGU_UINT8 srcBlockG[16],
                              const void *options = NULL) {
    CMP_BC15Options *BC15options = (CMP_BC15Options *)options;
    CMP_BC15Options BC15optionsDefault;
    if (BC15options == NULL)
    {
        BC15options = &BC15optionsDefault;
        SetDefaultBC15Options(BC15options);
    }
    DecompressBC5_DualChannel_Internal(srcBlockR,srcBlockG,(CGU_UINT32 *)cmpBlock,BC15options);

    return CGU_CORE_OK;
}

#endif

//============================================== OpenCL USER INTERFACE ====================================================
#ifdef ASPM_OPENCL
CMP_STATIC CMP_KERNEL void CMP_GPUEncoder(CMP_GLOBAL  const CMP_Vec4uc*   ImageSource,
                                          CMP_GLOBAL  CGU_UINT8*          ImageDestination,
                                          CMP_GLOBAL  Source_Info*        SourceInfo,
                                          CMP_GLOBAL  CMP_BC15Options*    BC15options
)
{
    CGU_UINT32 xID;
    CGU_UINT32 yID;

#ifdef ASPM_GPU
    xID = get_global_id(0);
    yID = get_global_id(1);
#else
    xID = 0;
    yID = 0;
#endif

    if (xID >= (SourceInfo->m_src_width / BlockX)) return;
    if (yID >= (SourceInfo->m_src_height / BlockX)) return;
    int  srcWidth = SourceInfo->m_src_width;

    CGU_UINT32 destI = (xID*BC5CompBlockSize) + (yID*(srcWidth / BlockX)*BC5CompBlockSize);
    int srcindex = 4 * (yID * srcWidth + xID);
    int blkindex = 0;
    CMP_Vec4uc srcData[16];
    srcWidth = srcWidth - 4;

    for ( CGU_INT32 j = 0; j < 4; j++) {
        for ( CGU_INT32 i = 0; i < 4; i++) {
            srcData[blkindex++] = ImageSource[srcindex++];
        }
        srcindex += srcWidth;
    }

    CompressBlockBC5_Internal(srcData, (CMP_GLOBAL CGU_UINT32 *)&ImageDestination[destI], BC15options);
}
#endif

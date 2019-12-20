//=====================================================================
// Copyright (c) 2016    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file BC1.cpp
//
//=====================================================================

#include "BC1.h"

//#define BUILD_AS_PLUGIN_DLL

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_BC1)
SET_PLUGIN_TYPE("ENCODER")
SET_PLUGIN_NAME("BC1")
#else
void *make_Plugin_BC1() { return new Plugin_BC1; }
#endif

CMP_BC15Options  g_BC1Encode;

#define GPU_BC1_COMPUTEFILE  "./plugins/Compute/BC1_Encode_kernel.cpp"

void  CompressBlockBC1_Internal(
    CMP_Vec4uc srcBlockTemp[16],
    CMP_GLOBAL  CGU_UINT32 compressedBlock[2],
    CMP_GLOBAL  CMP_BC15Options *BC15options);

Plugin_BC1::Plugin_BC1()
{
    m_KernelOptions = NULL;
}

Plugin_BC1::~Plugin_BC1()
{
}

int Plugin_BC1::TC_PluginSetSharedIO(void* Shared)
{
    CMips = reinterpret_cast<CMIPS *> (Shared);
    return 0;
}

int Plugin_BC1::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
    pPluginVersion->guid                    = g_GUID;
    pPluginVersion->dwAPIVersionMajor       = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor       = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

void* Plugin_BC1::TC_Create()
{
    return (void*) new BC1_EncodeClass();
}

void  Plugin_BC1::TC_Destroy(void* codec)
{
    delete codec;
    codec = nullptr;
}

char *Plugin_BC1::TC_ComputeSourceFile(CGU_UINT32  Compute_type)
{
    switch (Compute_type)
    {
    case CMP_Compute_type::CMP_HPC:
         // ToDo : Add features
         break;
#ifdef USE_GPUEncoders
        case CMP_Compute_type::CMP_GPU:
        case CMP_Compute_type::CMP_GPU_OCL:
                    return(GPU_BC1_COMPUTEFILE);
#endif
    }
    return ("");
}

void Plugin_BC1::TC_Start() {};
void Plugin_BC1::TC_End() {};

int Plugin_BC1::TC_Init(void  *kernel_options)
{
    if (!kernel_options)    return (-1);
    m_KernelOptions = reinterpret_cast<KernelOptions *>(kernel_options);

    memset(&g_BC1Encode, 0, sizeof(CMP_BC15Options));
    SetDefaultBC15Options(&g_BC1Encode);
    g_BC1Encode.m_src_width  = m_KernelOptions->width;
    g_BC1Encode.m_src_height = m_KernelOptions->height;

    m_KernelOptions->data = &g_BC1Encode;
    m_KernelOptions->size = sizeof(g_BC1Encode);

    return(0);
}

int BC1_EncodeClass::DecompressBlock(void *cmpin, void *srcout)
{
    if (srcout == NULL) return -1;
    if (cmpin  == NULL) return -1;
    return 0;
}

int BC1_EncodeClass::DecompressBlock(CGU_UINT32 xBlock, CGU_UINT32 yBlock, void *cmpin, void *srcout)
{
    if ((xBlock != 0) && (yBlock != 0)) return -1;
    if (srcout == NULL) return -1;
    if (cmpin == NULL) return -1;
    return 0;
}


int BC1_EncodeClass::CompressBlock(void *srcin, void *cmpout, void *blockoptions)
{
    CMP_BC15Options  *BC1Encode = reinterpret_cast<CMP_BC15Options *>(blockoptions);
    if (BC1Encode == NULL)  return -1;
    if (srcin  == NULL)     return -1;
    if (cmpout == NULL)     return -1;
    return 0;
}

int BC1_EncodeClass::CompressTexture(void *srcin, void *cmpout,void *processOptions)
{
    // ToDo: Implement texture level decompression
    if (processOptions == NULL) return -1;
    if (srcin == NULL) return -1;
    if (cmpout == NULL) return -1;

    // MipSet* pSourceTexture =  reinterpret_cast<MipSet *>(cmpin);
    // MipSet* pDestTexture   =  reinterpret_cast<MipSet *>(srcout);
    return 0;
}

int BC1_EncodeClass::DecompressTexture(void *cmpin, void *srcout,void *processOptions)
{
    // ToDo: Implement texture level decompression
    if (processOptions == NULL) return -1;
    if (srcout == NULL) return -1;
    if (cmpin == NULL) return -1;

    // MipSet* pSourceTexture =  reinterpret_cast<MipSet *>(cmpin);
    // MipSet* pDestTexture   =  reinterpret_cast<MipSet *>(srcout);
    return 0;
}

int BC1_EncodeClass::CompressBlock(CGU_UINT32 xBlock, CGU_UINT32 yBlock, void *srcin, void *cmpout) 
{
    CMP_Vec4uc *ImageSource        = (CMP_Vec4uc *)srcin;
    CGU_UINT8  *compressedBlocks   = (CGU_UINT8  *)cmpout;

    // if the srcWidth and srcHeight is not set try using the alternate user setting
    // that was set by user for block level codec access!
    if ((m_srcHeight == 0)||(m_srcWidth==0))
    {
         return (-1);
    }

    int width_in_blocks  = (m_srcWidth + 3) >> 2;

    CGU_UINT32 srcStride = m_srcWidth*4;
    CGU_UINT32 dstStride = width_in_blocks * BC1CompBlockSize;

    // user override!
    if (m_srcStride > 0) srcStride = m_srcStride;
    if (m_dstStride > 0) dstStride = m_dstStride;

    CGU_UINT32 destI = (xBlock * BC1CompBlockSize) + (yBlock*dstStride);
    int srcindex = (yBlock * srcStride) + (4*xBlock);
    int blkindex = 0;

    CMP_Vec4uc srcData[16];

    //Check if it is a complete 4X4 block
    if (((xBlock + 1)*BlockX <= m_srcWidth) && ((yBlock + 1)*BlockY <= m_srcHeight))
    {
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 4; i++) {
                memcpy(&srcData[blkindex++], &ImageSource[srcindex++], sizeof(CMP_Vec4uc));
            }
            srcindex += (m_srcWidth - 4);
        }
    }
    else
    {
        CMP_DWORD dwWidth = CMP_MIN(static_cast<unsigned int>(BlockX), m_srcWidth - xBlock*BlockX);
        CMP_DWORD i, j, srcIndex;

        //Go through line by line
        for (j = 0; j < BlockY && (BlockY * yBlock + j) < m_srcHeight; j++)
        {
            //Copy the real data
            srcIndex = ((yBlock * BlockY + j) * m_srcWidth + (xBlock * BlockX));
            for (i = 0; i < dwWidth; i++)
            {
                memcpy(&srcData[j*BlockX + i], &ImageSource[srcIndex + i], sizeof(CMP_Vec4uc));
            }
            if (i < BlockX)
                PadLine(i, BlockX, 4, (CMP_BYTE*)&srcData[j * BlockX]);
        }
        if (j < BlockY)
            PadBlock(j, BlockX, BlockY, 4, (CMP_BYTE*)srcData);
    }

    CompressBlockBC1_Internal(srcData, (CGU_UINT32 *)&compressedBlocks[destI],&g_BC1Encode);
    return (0);
}


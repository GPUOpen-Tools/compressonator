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
/// \file BC4.cpp
//
//=====================================================================

#include "BC4.h"

//#define BUILD_AS_PLUGIN_DLL

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_BC4)
SET_PLUGIN_TYPE("ENCODER")
SET_PLUGIN_NAME("BC4")
#else
void *make_Plugin_BC4() { return new Plugin_BC4; }
#endif

CMP_BC15Options  g_BC4Encode;

#define GPU_OCL_BC4_COMPUTEFILE     "./plugins/Compute/BC4_Encode_kernel.cpp"
#define GPU_DXC_BC4_COMPUTEFILE     "./plugins/Compute/BC4_Encode_kernel.hlsl"

extern void CompressBlockBC4_Internal(const CMP_Vec4uc srcBlockTemp[16],
                               CMP_GLOBAL CGU_UINT32 compressedBlock[2],
                               CMP_GLOBAL const CMP_BC15Options *BC15options);

Plugin_BC4::Plugin_BC4()
{
    m_KernelOptions = NULL;
}

Plugin_BC4::~Plugin_BC4()
{
}

int Plugin_BC4::TC_PluginSetSharedIO(void* Shared)
{
    CMips = reinterpret_cast<CMIPS *> (Shared);
    return 0;
}

int Plugin_BC4::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
    pPluginVersion->guid                    = g_GUID;
    pPluginVersion->dwAPIVersionMajor       = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor       = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

void* Plugin_BC4::TC_Create()
{
    return (void*) new BC4_EncodeClass();
}

void  Plugin_BC4::TC_Destroy(void* codec)
{
    delete codec;
    codec = nullptr;
}

char *Plugin_BC4::TC_ComputeSourceFile(CGU_UINT32  Compute_type)
{
    switch (Compute_type)
    {
        case CMP_Compute_type::CMP_HPC:
            // ToDo : Add features
            break;
        case CMP_Compute_type::CMP_GPU:
        case CMP_Compute_type::CMP_GPU_OCL:
            return(GPU_OCL_BC4_COMPUTEFILE);
        case CMP_Compute_type::CMP_GPU_DXC:
            return(GPU_DXC_BC4_COMPUTEFILE);
    }
    return ("");
}

void Plugin_BC4::TC_Start() {};
void Plugin_BC4::TC_End() {};

int Plugin_BC4::TC_Init(void  *kernel_options)
{
    if (!kernel_options)    return (-1);
    m_KernelOptions = reinterpret_cast<KernelOptions *>(kernel_options);

    memset(&g_BC4Encode, 0, sizeof(CMP_BC15Options));
    SetDefaultBC15Options(&g_BC4Encode);
    g_BC4Encode.m_src_width = m_KernelOptions->width;
    g_BC4Encode.m_src_height = m_KernelOptions->height;
    g_BC4Encode.m_fquality   = m_KernelOptions->fquality;

    m_KernelOptions->data = &g_BC4Encode;
    m_KernelOptions->size = sizeof(g_BC4Encode);

    return(0);
}

int BC4_EncodeClass::DecompressBlock(void *cmpin, void *srcout)
{
    if (srcout == NULL) return -1;
    if (cmpin == NULL) return -1;
    return 0;
}

int BC4_EncodeClass::DecompressBlock(CGU_UINT32 xBlock, CGU_UINT32 yBlock, void *cmpin, void *srcout)
{
    if ((xBlock != 0) && (yBlock != 0)) return -1;
    if (srcout == NULL) return -1;
    if (cmpin == NULL) return -1;
    return 0;
}


int BC4_EncodeClass::CompressBlock(void *srcin, void *cmpout, void *blockoptions)
{
    CMP_BC15Options *BC4Encode = reinterpret_cast<CMP_BC15Options *>(blockoptions);
    if (BC4Encode == NULL)  return -1;
    if (srcin     == NULL)  return -1;
    if (cmpout    == NULL)  return -1;
    return 0;
}

int BC4_EncodeClass::CompressTexture(void *srcin, void *cmpout,void *processOptions)
{
    // ToDo: Implement texture level compression
    if (processOptions == NULL) return -1;
    if (srcin == NULL) return -1;
    if (cmpout == NULL) return -1;
    // MipSet* pSourceTexture  =  reinterpret_cast<MipSet *>(srcin);
    // MipSet* pDestTexture    =  reinterpret_cast<MipSet *>(cmpout);
    return 0;
}

int BC4_EncodeClass::DecompressTexture(void *cmpin, void *srcout,void *processOptions)
{
    // ToDo: Implement texture level decompression
    if (processOptions == NULL) return -1;
    if (srcout == NULL) return -1;
    if (cmpin == NULL) return -1;

    // MipSet* pSourceTexture =  reinterpret_cast<MipSet *>(cmpin);
    // MipSet* pDestTexture   =  reinterpret_cast<MipSet *>(srcout);
    return 0;
}

int BC4_EncodeClass::CompressBlock(CGU_UINT32 xBlock, CGU_UINT32 yBlock, void *srcin, void *cmpout) 
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

    CGU_UINT32 destI = (xBlock * BC4CompBlockSize) + (yBlock*width_in_blocks * BC4CompBlockSize);
    int srcindex = 4 * (yBlock * m_srcWidth + xBlock);
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

    CompressBlockBC4_Internal(srcData,(CGU_UINT32 *)&compressedBlocks[destI],&g_BC4Encode);
    return (0);
}


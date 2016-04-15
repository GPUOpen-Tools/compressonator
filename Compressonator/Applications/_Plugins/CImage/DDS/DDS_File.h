//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef _DDS_FILE_H
#define _DDS_FILE_H

#include "PluginInterface.h"
#include "ddraw.h"
#include "d3d9types.h"

//============== from old code =================================================
#define PLUGIN_NAME _T("DDS Plugin")

extern const TCHAR* g_pszFilename;

// DDSD2
// Required for 64bit compatability
typedef struct _DDSD2
{
    DWORD               dwSize;                 // size of the DDSURFACEDESC structure
    DWORD               dwFlags;                // determines what fields are valid
    DWORD               dwHeight;               // height of surface to be created
    DWORD               dwWidth;                // width of input surface
    union
    {
        LONG            lPitch;                 // distance to start of next line (return value only)
        DWORD           dwLinearSize;           // Formless late-allocated optimized surface size
    } DUMMYUNIONNAMEN(1);
    union
    {
        DWORD           dwBackBufferCount;      // number of back buffers requested
        DWORD           dwDepth;                // the depth if this is a volume texture 
    } DUMMYUNIONNAMEN(5);
    union
    {
        DWORD           dwMipMapCount;          // number of mip-map levels requestde
        // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
        DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
        DWORD           dwSrcVBHandle;          // The source used in VB::Optimize
    } DUMMYUNIONNAMEN(2);
    DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
    DWORD               dwReserved;             // reserved
    void* POINTER_32    lpSurface;              // pointer to the associated surface memory
    union
    {
        DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
        DWORD           dwEmptyFaceColor;       // Physical color for empty cubemap faces
    } DUMMYUNIONNAMEN(3);
    DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
    DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
    DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
    union
    {
        DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface (FourCC's)
        DWORD           dwFVF;                  // vertex format description of vertex buffers
    } DUMMYUNIONNAMEN(4);
    DDSCAPS2            ddsCaps;                // direct draw surface capabilities
    DWORD               dwTextureStage;         // stage in multitexture cascade
                                                // for extended DDSHeader10 info see DDS_DX10 
} DDSD2;

#define MAX_FORMAT_LENGTH 160
#define MAX_ERROR_LENGTH 240

static const DWORD DDS_HEADER = MAKEFOURCC('D', 'D', 'S', ' ');

TC_PluginError LoadDDS_ABGR32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_ABGR16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_GR32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16R16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_FourCC(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB565(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB888(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB8888(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet, bool bAlpha);
TC_PluginError LoadDDS_ARGB2101010(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_ABGR16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16R16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_AG8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_A8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);

TC_PluginError SaveDDS_ABGR32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ABGR16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ARGB8888(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ARGB2101010(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ABGR16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RGB888(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_FourCC(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_G8(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_A8(FILE* pFile, const MipSet* pMipSet);



#endif
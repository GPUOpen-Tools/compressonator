//=====================================================================
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


// NOTES on conversions from AMD_Textures to Mips
/*
                CMP_Texture srcTexture;
                srcTexture.dwSize        = sizeof(srcTexture);
                srcTexture.dwWidth        = EXR_CMips->GetMipLevel(pMipSet, i)->m_nWidth;
                srcTexture.dwHeight        = EXR_CMips->GetMipLevel(pMipSet, i)->m_nHeight;
                srcTexture.dwPitch        = 0;
                srcTexture.format        = CMP_FORMAT_ARGB_32F;
                srcTexture.dwDataSize    = EXR_CMips->GetMipLevel(pMipSet, i)->m_dwLinearSize;
                srcTexture.pData        = (CMP_BYTE*) EXR_CMips->GetMipLevel(pMipSet, i)->m_pfData;
                CMP_Texture destTexture;
                destTexture.dwSize = sizeof(destTexture);
                destTexture.dwWidth = EXR_CMips->GetMipLevel(pMipSet, i)->m_nWidth;
                destTexture.dwHeight = EXR_CMips->GetMipLevel(pMipSet, i)->m_nHeight;
                destTexture.dwPitch = 0;
                destTexture.format = CMP_FORMAT_ARGB_16F;
                destTexture.dwDataSize = srcTexture.dwDataSize >> 1;
                destTexture.pData = (CMP_BYTE*) pOutputData;
*/


/*
Lib Dependancies and path
$(OutDir);
..\..\..\..\..\Common\Lib\Ext\Boost\boost_1_55_0\lib\vc10\x86;
..\..\..\..\..\Common\Lib\Ext\OpenEXR\v1.4.0\Lib\$(SolutionName)\$(Platform)\lib\;
..\..\..\..\..\Common\Lib\Ext\DirectX-SDK\June-2010\Lib\x86;
..\..\..\..\..\Common\Lib\Ext\DirectX-SDK\8.0\Lib\win8\um\x86;
IMath.lib;Half.lib;IlmImf.lib;IlmThread.lib;Iex.lib;zlibstatic_d.lib;
*/

#include "stdafx.h"
#include <stdio.h>
#include "EXR.h"
#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "MIPS.h"

CMIPS *EXR_CMips = NULL;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_EXR)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("EXR")
#else
void *make_Plugin_EXR() { return new Plugin_EXR; } 
#endif




#ifdef _USEMT
#ifdef _DEBUG
#pragma comment(lib,"zlib_static.lib")
#pragma comment(lib,"IMath_MTd.lib")
#pragma comment(lib,"Half_MTd.lib")
#pragma comment(lib,"IlmImf_MTd.lib")
#pragma comment(lib,"IlmThread_MTd.lib")
#pragma comment(lib,"Iex_MTd.lib")

#else
#pragma comment(lib,"zlib_static.lib")
#pragma comment(lib,"IMath_MT.lib")
#pragma comment(lib,"Half_MT.lib")
#pragma comment(lib,"IlmImf_MT.lib")
#pragma comment(lib,"IlmThread_MT.lib")
#pragma comment(lib,"Iex_MT.lib")
#endif
#else
#ifdef _DEBUG   // Chenge these to MD when OpenEXR libs are ready - changed
#pragma comment(lib,"zlib_static_MDd.lib")
#pragma comment(lib,"IMath_MDd.lib")
#pragma comment(lib,"Half_MDd.lib")
#pragma comment(lib,"IlmImf_MDd.lib")
#pragma comment(lib,"IlmThread_MDd.lib")
#pragma comment(lib,"Iex_MDd.lib")

#else
#pragma comment(lib,"zlib_static_MD.lib")
#pragma comment(lib,"IMath_MD.lib")
#pragma comment(lib,"Half_MD.lib")
#pragma comment(lib,"IlmImf_MD.lib")
#pragma comment(lib,"IlmThread_MD.lib")
#pragma comment(lib,"Iex_MD.lib")
#endif
#endif





Plugin_EXR::Plugin_EXR()
{ 
    //MessageBox(0,"Construct","Plugin_EXR",MB_OK);  
}

Plugin_EXR::~Plugin_EXR()
{ 
    //MessageBox(0,"Destroy","Plugin_EXR",MB_OK);  
}

int Plugin_EXR::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        EXR_CMips = static_cast<CMIPS *>(Shared);
        return 0;
    }
    return 1;
}


int Plugin_EXR::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
    //MessageBox(0,"TC_PluginGetVersion","Plugin_EXR",MB_OK);  
    pPluginVersion->guid                    = g_GUID;
    pPluginVersion->dwAPIVersionMajor        = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor        = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

#include <string> 
#include "cExr.h"
#include "ImfTiledRgbaFile.h"
#include "Compressonator.h"

// File system
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp> 

int Plugin_EXR::TC_PluginFileLoadTexture(const TCHAR* pszFilename, CMP_Texture *srcTexture)
{

    if (!boost::filesystem::exists( pszFilename )) return -1;

    int width, height;
    string inf = pszFilename;

    Array2D<Rgba> pixels;
    int w, h;

    Exr::fileinfo(inf, width, height);
    Exr::readRgba(inf, pixels, w, h);

    srcTexture->dwSize            = sizeof(CMP_Texture);
    srcTexture->dwWidth            = width;
    srcTexture->dwHeight        = height;
    srcTexture->dwPitch            = 0;
    srcTexture->format            = CMP_FORMAT_ARGB_32F;
    srcTexture->dwDataSize        = 4*width*height*sizeof(float);
    srcTexture->pData            = (CMP_BYTE*) malloc(srcTexture->dwDataSize);

    Rgba2Texture(pixels,(float *)srcTexture->pData,width,height);
    return 0;
}

int Plugin_EXR::TC_PluginFileSaveTexture(const TCHAR* pszFilename, CMP_Texture *srcTexture)
{
    int  image_width    = srcTexture->dwWidth;
    int  image_height    = srcTexture->dwHeight;
    Array2D<Rgba> pixels (image_height,image_width);
    string sFile = pszFilename;
    Texture2Rgba((float *)srcTexture->pData, pixels, image_width, image_height, false);
    Exr::writeRgba(sFile,pixels,image_width,image_height);
    return 0;
}


//#define NOMIPS_LEVEL_DATA
#include "ImfVersion.h"

int Plugin_EXR::TC_PluginFileLoadTexture(const TCHAR* pszFilename, MipSet* pMipSet)
{
    if (!boost::filesystem::exists( pszFilename )) return -1;

    // NP The MIP Level feature is disabled for EXR loading
    // code works on loading and compression, but saving Tiled file is unvarifiable 
    // and seens to produce all tiles overlapping at origin
    pMipSet->m_Flags |= MS_FLAG_DisableMipMapping;
    
    string inf = pszFilename;
    int version;

    try
    {
        RgbaInputFile fileInfo(pszFilename);
        version = fileInfo.version();
    }
    catch (std::exception& e)
    {
        if (EXR_CMips)
            EXR_CMips->PrintError(e.what());
        return PE_Unknown;
    }

    
    // Check if file is tiled!
    if ((!(version & TILED_FLAG)) || ((pMipSet->m_Flags & MS_FLAG_DisableMipMapping)))
    { 
        Array2D<Rgba> pixels;
        int width, height;
        int w, h;

        Exr::fileinfo(inf, width, height);
        Exr::readRgba(inf, pixels, w, h);

        CMP_Texture srcTexture;
        srcTexture.dwSize            = sizeof(CMP_Texture);
        srcTexture.dwWidth            = width;
        srcTexture.dwHeight            = height;
        srcTexture.dwPitch            = 0;
        srcTexture.format            = CMP_FORMAT_ARGB_32F;
        srcTexture.dwDataSize        = 4*width*height*sizeof(float);
        srcTexture.pData            = (CMP_BYTE*) malloc(srcTexture.dwDataSize);

        Rgba2Texture(pixels,(float *)srcTexture.pData,width,height);


        if(!EXR_CMips->AllocateMipSet(pMipSet, CF_Float32, TDT_ARGB, TT_2D, width, height, 1))
        {
            if (EXR_CMips)
                EXR_CMips->PrintError("Error(0): EXR Plugin ID(5)\n");
            return PE_Unknown;
        }

        // Allocate the permanent buffer and unpack the bitmap data into it
        if(!EXR_CMips->AllocateMipLevelData(EXR_CMips->GetMipLevel(pMipSet, 0), width, height, CF_Float32, pMipSet->m_TextureDataType))
        {
            if (EXR_CMips)
                EXR_CMips->PrintError("Error(0): EXR Plugin ID(6)\n");
            return PE_Unknown;
        }

        // MIPS structure defaults
        pMipSet->m_dwFourCC        = 0;
        pMipSet->m_dwFourCC2    = 0;
        pMipSet->m_nMipLevels    = 1;

        BYTE *MipData = EXR_CMips->GetMipLevel(pMipSet, 0)->m_pbData;

        memcpy(MipData, srcTexture.pData, srcTexture.dwDataSize);

        free(srcTexture.pData);
    }
else // File is tiled
{

    TiledRgbaInputFile file(pszFilename);

    if(!file.isComplete())
        return PE_Unknown;

    DWORD dwWidth  = file.levelWidth(0);
    DWORD dwHeight = file.levelHeight(0);

    if(!EXR_CMips->AllocateMipSet(pMipSet, CF_Float32, TDT_ARGB, TT_2D, dwWidth, dwHeight, 1))
    {
        return PE_Unknown;
    }

    pMipSet->m_dwFourCC = 0;
    pMipSet->m_dwFourCC2 = 0;
    pMipSet->m_nMipLevels = (file.levelMode() == MIPMAP_LEVELS) ? file.numLevels() : 1;

    Array2D<Rgba> pixels;
    
    for(int i = 0; i < pMipSet->m_nMipLevels; i++)
    {
        if(!file.isValidLevel(i, i))
        {
            pMipSet->m_nMipLevels = i;
            break;
        }

        Imath::Box2i dw = file.dataWindowForLevel(i);

        dwWidth  = file.levelWidth(i);
        dwHeight = file.levelHeight(i);

        pixels.resizeErase(dwWidth, dwHeight);

        Rgba* pSrcData = &pixels[0][0] - dw.min.x - dw.min.y * dwWidth;
        file.setFrameBuffer(pSrcData, 1, dwWidth);
        file.readTiles(0, file.numXTiles(i) - 1, 0, file.numYTiles(i) - 1, i);

        // Allocate the permanent buffer and unpack the bitmap data into it
        if(!EXR_CMips->AllocateMipLevelData(EXR_CMips->GetMipLevel(pMipSet, i), dwWidth, dwHeight, CF_Float32, pMipSet->m_TextureDataType))
            return PE_Unknown;

        pSrcData = &pixels[0][0];
        Rgba2Texture(pixels,(float *)EXR_CMips->GetMipLevel(pMipSet, i)->m_pbData,dwWidth,dwHeight);
    }

    } // Tiled file

    return PE_OK;
}



int Plugin_EXR::TC_PluginFileSaveTexture(const TCHAR* pszFilename, MipSet* pMipSet)
{
    if(!TC_PluginFileSupportsFormat(NULL, pMipSet))
    {
        if (EXR_CMips)
            EXR_CMips->PrintError("Error(%d): EXR Plugin ID(%d) Filename=%s\n",EL_Error,IDS_ERROR_UNSUPPORTED_TYPE,pszFilename);
        return PE_Unknown;
    }

    LevelMode levelMode = (pMipSet->m_nMipLevels > 1) ? MIPMAP_LEVELS : ONE_LEVEL;

    // Save Single EXR file
    if (pMipSet->m_nMipLevels == 1)    
    {

        int  image_width    = pMipSet->m_nWidth;
        int  image_height    = pMipSet->m_nHeight;
        Array2D<Rgba> pixels (image_height,image_width);
        pixels.resizeErase(image_height, image_width);
        string sFile = pszFilename;

        float *data = EXR_CMips->GetMipLevel(pMipSet, 0)->m_pfData;

        Texture2Rgba(data, pixels, image_width, image_height, pMipSet->m_isDeCompressed);

        Exr::writeRgba(sFile,pixels,image_width,image_height);
    }
    // Save Muliple MIP levels as TiledRGB 
    else    
    {
            TiledRgbaOutputFile file(pszFilename, pMipSet->m_nWidth, pMipSet->m_nHeight, TILE_WIDTH, TILE_HEIGHT, levelMode, ROUND_DOWN);
            Rgba* pOutputData = (Rgba*) malloc(EXR_CMips->GetMipLevel(pMipSet, 0)->m_dwLinearSize);
    
            for(int i = 0; i < file.numLevels(); i++)
            {
            if(EXR_CMips->GetMipLevel(pMipSet, i)->m_pbData)
            {
                memcpy(pOutputData, EXR_CMips->GetMipLevel(pMipSet, i)->m_pbData, EXR_CMips->GetMipLevel(pMipSet, i)->m_dwLinearSize);
            }
            else
                memset(pOutputData, 0, EXR_CMips->GetMipLevel(pMipSet, 0)->m_dwLinearSize);
    
            file.setFrameBuffer(pOutputData, 1, file.levelWidth(i));
            file.writeTiles(0, file.numXTiles(i) - 1, 0, file.numYTiles(i) - 1, i);
            }

            free(pOutputData);
    }

    return PE_OK;
}

bool _cdecl TC_PluginFileSupportsFormat(const HFILETYPE, const MipSet* pMipSet)
{
    assert(pMipSet);
    if(pMipSet == NULL)
        return false;

    if(pMipSet->m_TextureType != TT_2D)
        return false;

    return (pMipSet->m_ChannelFormat == CF_Float16 || pMipSet->m_ChannelFormat == CF_Float32) && (pMipSet->m_TextureDataType == TDT_ARGB || pMipSet->m_TextureDataType == TDT_XRGB) ? true : false;
}

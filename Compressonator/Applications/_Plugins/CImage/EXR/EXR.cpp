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

#include <string> 
#include "cExr.h"
#include <ImfTiledRgbaFile.h>
#include <ImfHeader.h>
#include <ImfMultiPartInputFile.h>
#include <ImfInputPart.h>
#include <ImfPartType.h>
#include <ImfDeepTiledInputPart.h>
#include <ImfChannelList.h>
#include <ImfTiledInputPart.h>
#include <ImfPreviewImage.h>
#include <ImfDeepScanLineInputPart.h>
#include <ImfCompositeDeepScanLine.h>
#include <ImfPixelType.h>
#include <ImathFun.h>
#include "Compressonator.h"

// File system
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp> 

CMIPS *EXR_CMips = NULL;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_EXR)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("EXR")
#else
void *make_Plugin_EXR() { return new Plugin_EXR; } 
#endif

#ifdef _DEBUG
#pragma comment(lib,"zlibstaticd.lib")
#pragma comment(lib,"Imathd.lib")
#pragma comment(lib,"halfd.lib")
#pragma comment(lib,"IlmImfd.lib")
#pragma comment(lib,"IlmThreadd.lib")
#pragma comment(lib,"Iexd.lib")
#else
#pragma comment(lib,"zlibstatic.lib")
#pragma comment(lib,"Imath.lib")
#pragma comment(lib,"half.lib")
#pragma comment(lib,"IlmImf.lib")
#pragma comment(lib,"IlmThread.lib")
#pragma comment(lib,"Iex.lib")
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
#ifdef _WIN32
    pPluginVersion->guid                    = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor        = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor        = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_EXR::TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture)
{

    if (!boost::filesystem::exists( pszFilename )) return -1;

    int width, height;
    string inf = pszFilename;

    Array2D<Rgba> pixels;
    int w, h;

    Exr::fileinfo(inf, width, height);
    Exr::readRgba(inf, pixels, w, h);

    srcTexture->dwSize            = sizeof(CMP_Texture);
    srcTexture->dwWidth           = width;
    srcTexture->dwHeight          = height;
    srcTexture->dwPitch           = 0;
    srcTexture->format            = CMP_FORMAT_ARGB_16F;
    srcTexture->dwDataSize        = 4*width*height*sizeof(CMP_HALF);
    srcTexture->pData             = (CMP_BYTE*) malloc(srcTexture->dwDataSize);

    Rgba2Texture(pixels,(CMP_HALF *)srcTexture->pData,width,height);
    return 0;
}

int Plugin_EXR::TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture)
{
    int  image_width    = srcTexture->dwWidth;
    int  image_height    = srcTexture->dwHeight;
    Array2D<Rgba> pixels (image_height,image_width);
    string sFile = pszFilename;
    Texture2Rgba((CMP_HALF *)srcTexture->pData, pixels, image_width, image_height, CMP_FORMAT_Unknown);
    Exr::writeRgba(sFile,pixels,image_width,image_height);
    return 0;
}


//#define NOMIPS_LEVEL_DATA
#include "ImfVersion.h"
bool allocateMipSet(Array<Rgba> &pixels, MipSet* pMipSet, int w, int h)
{

    if (!EXR_CMips->AllocateMipSet(pMipSet, CF_Float16, TDT_ARGB, TT_2D, w, h, 1))
    {
        if (EXR_CMips)
            EXR_CMips->PrintError("Error(0): EXR Plugin ID(5)\n");
        return false;
    }

    // Allocate the permanent buffer and unpack the bitmap data into it
    if (!EXR_CMips->AllocateMipLevelData(EXR_CMips->GetMipLevel(pMipSet, 0), w, h, CF_Float16, pMipSet->m_TextureDataType))
    {
        if (EXR_CMips)
            EXR_CMips->PrintError("Error(0): EXR Plugin ID(6)\n");
        return false;
    }

    // MIPS structure defaults
    pMipSet->m_dwFourCC = 0;
    pMipSet->m_dwFourCC2 = 0;
    pMipSet->m_nMipLevels = 1;
    int i = 0;
    CMP_HALF *MipData = EXR_CMips->GetMipLevel(pMipSet, 0)->m_phfData;

    // Save the Half Data format value into a Float for processing later
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            *MipData = pixels[i].r.bits();
            MipData++;
            *MipData = pixels[i].g.bits();
            MipData++;
            *MipData = pixels[i].b.bits();
            MipData++;
            *MipData = pixels[i].a.bits();
            MipData++;
            i++;
        }
    }
    return true;
}

int
loadImage(const char fileName[],
    const char layer[],
    Header &header,
    Array<Rgba> &pixels, MipSet* pMipSet)
{
    MultiPartInputFile inmaster(fileName);
    InputPart in(inmaster, 0);
    header = in.header();

    ChannelList ch = header.channels();
    
    if (ch.findChannel("Y"))
    {
        //load as RGBA using array2D
        Array2D<Rgba> pixels;
        int width, height;
        int w, h;

        Exr::fileinfo(fileName, width, height);
        Exr::readRgba(fileName, pixels, w, h);

        CMP_Texture srcTexture;
        srcTexture.dwSize = sizeof(CMP_Texture);
        srcTexture.dwWidth = width;
        srcTexture.dwHeight = height;
        srcTexture.dwPitch = 0;
        srcTexture.format = CMP_FORMAT_ARGB_16F;
        srcTexture.dwDataSize = 4 * width*height * sizeof(CMP_HALF);
        srcTexture.pData = (CMP_BYTE*)malloc(srcTexture.dwDataSize);

        Rgba2Texture(pixels, (CMP_HALF*)srcTexture.pData, width, height);


        if (!EXR_CMips->AllocateMipSet(pMipSet, CF_Float16, TDT_ARGB, TT_2D, width, height, 1))
        {
            if (EXR_CMips)
                EXR_CMips->PrintError("Error(0): EXR Plugin ID(5)\n");
            return PE_Unknown;
        }

        // Allocate the permanent buffer and unpack the bitmap data into it
        if (!EXR_CMips->AllocateMipLevelData(EXR_CMips->GetMipLevel(pMipSet, 0), width, height, CF_Float16, pMipSet->m_TextureDataType))
        {
            if (EXR_CMips)
                EXR_CMips->PrintError("Error(0): EXR Plugin ID(6)\n");
            return PE_Unknown;
        }

        // MIPS structure defaults
        pMipSet->m_dwFourCC = 0;
        pMipSet->m_dwFourCC2 = 0;
        pMipSet->m_nMipLevels = 1;

        CMP_BYTE *MipData = EXR_CMips->GetMipLevel(pMipSet, 0)->m_pbData;

        memcpy(MipData, srcTexture.pData, srcTexture.dwDataSize);

        free(srcTexture.pData);
    }
    else
    {
        Box2i &dataWindow = header.dataWindow();
        int dw = dataWindow.max.x - dataWindow.min.x + 1;
        int dh = dataWindow.max.y - dataWindow.min.y + 1;
        int dx = dataWindow.min.x;
        int dy = dataWindow.min.y;

        pixels.resizeErase(dw * dh);
        memset(pixels, 0, (dw * dh) * (sizeof(Rgba)));

        size_t xs = 1 * sizeof(Rgba);
        size_t ys = dw * sizeof(Rgba);

        FrameBuffer fb;
        Rgba *base = pixels - dx - dy * dw;

        fb.insert("R",
            Slice(HALF,
            (char *)&base[0].r,
                xs, ys,
                1, 1,     // xSampling, ySampling
                0.0));    // fillValue

        fb.insert("G",
            Slice(HALF,
            (char *)&base[0].g,
                xs, ys,
                1, 1,     // xSampling, ySampling
                0.0));    // fillValue

        fb.insert("B",
            Slice(HALF,
            (char *)&base[0].b,
                xs, ys,
                1, 1,     // xSampling, ySampling
                0.0));    // fillValue

        fb.insert("A",
            Slice(HALF,
            (char *)&base[0].a,
                xs, ys,
                1, 1,             // xSampling, ySampling
                1.0));    // fillValue
        in.setFrameBuffer(fb);


        try
        {
            in.readPixels(dataWindow.min.y, dataWindow.max.y);
            if (!allocateMipSet(pixels, pMipSet, dw, dh))
                return PE_Unknown;
        }
        catch (const exception &e)
        {
            //
            // If some of the pixels in the file cannot be read,
            // print an error message, and return a partial image
            // to the caller.
            //

            cerr << e.what() << endl;
        }
    }
}

int
loadTiledImage(const char fileName[],
    const char layer[],
    int lx,
    int ly,
    Header &header,
    Array<Rgba> &pixels, MipSet* pMipSet)
{
    MultiPartInputFile inmaster(fileName);
    TiledInputPart in(inmaster, 0);
    header = in.header();

    if (!in.isValidLevel(lx, ly))
    {
        //
        //for part doesn't have valid level
        //
        pixels.resizeErase(1);
        header.dataWindow() = Box2i(V2i(0, 0), V2i(0, 0));

        cout << "Level (" << lx << ", " << ly << ") does "
            "not exist in part " << 0 << " of file "
            << fileName << "." << endl;
        return PE_Unknown;
    }
    else
    {
        header.dataWindow() = in.dataWindowForLevel(lx, ly);
        header.displayWindow() = header.dataWindow();

        ChannelList ch = header.channels();
        if (ch.findChannel("Y"))
        {
            //
            // Not handling YCA image right now
            //
            cout << "Cannot handle YCA image now!" << endl;

            //no data for YCA image
            pixels.resizeErase(1);
            header.dataWindow() = Box2i(V2i(0, 0), V2i(0, 0));
            return PE_Unknown;
        }
        else
        {
            Box2i &dataWindow = header.dataWindow();
            int dw = dataWindow.max.x - dataWindow.min.x + 1;
            int dh = dataWindow.max.y - dataWindow.min.y + 1;
            int dx = dataWindow.min.x;
            int dy = dataWindow.min.y;

            pixels.resizeErase(dw * dh);
            memset(pixels, 0, (dw * dh) * (sizeof(Rgba)));

            size_t xs = 1 * sizeof(Rgba);
            size_t ys = dw * sizeof(Rgba);
            FrameBuffer fb;
            Rgba *base = pixels - dx - dy * dw;

            fb.insert("R",
                Slice(HALF,
                (char *)&base[0].r,
                    xs, ys,
                    1, 1,         // xSampling, ySampling
                    0.0));        // fillValue

            fb.insert("G",
                Slice(HALF,
                (char *)&base[0].g,
                    xs, ys,
                    1, 1,         // xSampling, ySampling
                    0.0));        // fillValue

            fb.insert("B",
                Slice(HALF,
                (char *)&base[0].b,
                    xs, ys,
                    1, 1,         // xSampling, ySampling
                    0.0));        // fillValue

            fb.insert("A",
                Slice(HALF,
                (char *)&base[0].a,
                    xs, ys,
                    1, 1,         // xSampling, ySampling
                    1.0));        // fillValue
            in.setFrameBuffer(fb);

            try
            {
                int tx = in.numXTiles(lx);
                int ty = in.numYTiles(ly);

                //
                // For maximum speed, try to read the tiles in
                // the same order as they are stored in the file.
                //

                if (in.header().lineOrder() == INCREASING_Y)
                {
                    for (int y = 0; y < ty; ++y)
                        for (int x = 0; x < tx; ++x)
                            in.readTile(x, y, lx, ly);
                }
                else
                {
                    for (int y = ty - 1; y >= 0; --y)
                        for (int x = 0; x < tx; ++x)
                            in.readTile(x, y, lx, ly);
                }

                if (!allocateMipSet(pixels, pMipSet, dw, dh))
                    return PE_Unknown;
            }
            catch (const exception &e)
            {
                //
                // If some of the tiles in the file cannot be read,
                // print an error message, and return a partial image
                // to the caller.
                //

                cerr << e.what() << endl;
            }
        }

    }
}


int
loadPreviewImage(const char fileName[],
    Header &header,
    Array<Rgba> &pixels, MipSet* pMipSet)
{
    MultiPartInputFile inmaster(fileName);
    InputPart in(inmaster, 0);
    header = in.header();

    if (!in.header().hasPreviewImage())
    {
        //
        // If no preview, make a 100*100 display window
        //
        header.dataWindow() = Box2i(V2i(0, 0), V2i(0, 0));
        header.displayWindow() = Box2i(V2i(0, 0), V2i(99, 99));
        pixels.resizeErase(1);

        cout << "Part " << 0 << " contains no preview image." << endl;

        return PE_Unknown;
    }
    else {
        const PreviewImage &preview = in.header().previewImage();
        int w = preview.width();
        int h = preview.height();

        header.displayWindow() = Box2i(V2i(0, 0), V2i(w - 1, h - 1));
        header.dataWindow() = header.displayWindow();
        header.pixelAspectRatio() = 1;

        pixels.resizeErase(w * h);

        //
        // Convert the 8-bit gamma-2.2 preview pixels
        // into linear 16-bit floating-point pixels.
        //

        for (int i = 0; i < w * h; ++i)
        {
            Rgba &p = pixels[i];
            const PreviewRgba &q = preview.pixels()[i];

            p.r = 2.f * pow(q.r / 255.f, 2.2f);
            p.g = 2.f * pow(q.g / 255.f, 2.2f);
            p.b = 2.f * pow(q.b / 255.f, 2.2f);
            p.a = q.a / 255.f;
        }

        if (!allocateMipSet(pixels, pMipSet, w, h))
            return PE_Unknown;
    }
}

int
loadImageChannel(const char fileName[],
    const char channelName[],
    Header &header,
    Array<Rgba> &pixels, MipSet* pMipSet)
{
    MultiPartInputFile inmaster(fileName);
    InputPart in(inmaster, 0);

    header = in.header();

    if (const Channel *ch = in.header().channels().findChannel(channelName))
    {
        Box2i &dataWindow = header.dataWindow();
        int dw = dataWindow.max.x - dataWindow.min.x + 1;
        int dh = dataWindow.max.y - dataWindow.min.y + 1;
        int dx = dataWindow.min.x;
        int dy = dataWindow.min.y;

        pixels.resizeErase(dw * dh);

        for (int i = 0; i < dw * dh; ++i)
        {
            pixels[i].r = half::qNan();
            pixels[i].g = half::qNan();
            pixels[i].b = half::qNan();
        }
        FrameBuffer fb;

        fb.insert(channelName,
            Slice(HALF,
            (char *)&pixels[-dx - dy * dw].g,
                sizeof(Rgba) * ch->xSampling,
                sizeof(Rgba) * ch->ySampling * dw,
                ch->xSampling,
                ch->ySampling));

        in.setFrameBuffer(fb);

        try
        {
            in.readPixels(dataWindow.min.y, dataWindow.max.y);
        }
        catch (const exception &e)
        {
            //
            // If some of the pixels in the file cannot be read,
            // print an error message, and return a partial image
            // to the caller.
            //

            cerr << e.what() << endl;
        }

        for (int i = 0; i < dw * dh; ++i)
        {
            pixels[i].r = pixels[i].g;
            pixels[i].b = pixels[i].g;
        }

        if (!allocateMipSet(pixels, pMipSet, dw, dh))
            return PE_Unknown;
    }
    else
    {
        cerr << "Image file \"" << fileName << "\" has no "
            "channel named \"" << channelName << "\"." << endl;

        //
        //no data for this channel
        //
        pixels.resizeErase(1);
        header.dataWindow() = Box2i(V2i(0, 0), V2i(0, 0));
        
        return PE_Unknown;
    }
}

int
loadTiledImageChannel(const char fileName[],
    const char channelName[],
    int lx,
    int ly,
    Header &header,
    Array<Rgba> &pixels, MipSet* pMipSet)
{
    MultiPartInputFile inmaster(fileName);
    TiledInputPart in(inmaster, 0);

    if (!in.isValidLevel(lx, ly))
    {
       cerr<< "Level (" << lx << ", " << ly << ") does "
            "not exist in file " << fileName << ".";
       return PE_Unknown;
    }

    header = in.header();

    if (const Channel *ch = in.header().channels().findChannel(channelName))
    {
        header.dataWindow() = in.dataWindowForLevel(lx, ly);
        header.displayWindow() = header.dataWindow();

        Box2i &dataWindow = header.dataWindow();
        int dw = dataWindow.max.x - dataWindow.min.x + 1;
        int dh = dataWindow.max.y - dataWindow.min.y + 1;
        int dx = dataWindow.min.x;
        int dy = dataWindow.min.y;

        pixels.resizeErase(dw * dh);

        for (int i = 0; i < dw * dh; ++i)
        {
            pixels[i].r = half::qNan();
            pixels[i].g = half::qNan();
            pixels[i].b = half::qNan();
        }

        FrameBuffer fb;

        fb.insert(channelName,
            Slice(HALF,
            (char *)&pixels[-dx - dy * dw].g,
                sizeof(Rgba) * ch->xSampling,
                sizeof(Rgba) * ch->ySampling * dw,
                ch->xSampling,
                ch->ySampling));

        in.setFrameBuffer(fb);

        try
        {
            int tx = in.numXTiles(lx);
            int ty = in.numYTiles(ly);

            //
            // For maximum speed, try to read the tiles in
            // the same order as they are stored in the file.
            //

            if (in.header().lineOrder() == INCREASING_Y)
            {
                for (int y = 0; y < ty; ++y)
                    for (int x = 0; x < tx; ++x)
                        in.readTile(x, y, lx, ly);
            }
            else
            {
                for (int y = ty - 1; y >= 0; --y)
                    for (int x = 0; x < tx; ++x)
                        in.readTile(x, y, lx, ly);
            }
        }
        catch (const exception &e)
        {
            //
            // If some of the tiles in the file cannot be read,
            // print an error message, and return a partial image
            // to the caller.
            //

            cerr << e.what() << endl;
        }

        for (int i = 0; i < dw * dh; ++i)
        {
            pixels[i].r = pixels[i].g;
            pixels[i].b = pixels[i].g;
        }

        if (!allocateMipSet(pixels, pMipSet, dw, dh))
            return PE_Unknown;
    }
    else
    {
        cerr << "Image file \"" << fileName << "\" part " << 0 << " "
            "has no channel named \"" << channelName << "\"." << endl;

        //
        //no data for this channel
        //
        pixels.resizeErase(1);
        header.dataWindow() = Box2i(V2i(0, 0), V2i(0, 0));
    }
}

int
loadDeepScanlineImage(MultiPartInputFile &inmaster,
    int &zsize,
    Header &header,
    Array<Rgba> &pixels,
    Array<float*> &zbuff,
    Array<unsigned int> &sampleCount,
    bool deepComp, MipSet* pMipSet)
{
    DeepScanLineInputPart in(inmaster, 0);
    header = in.header();

    Box2i &dataWindow = header.dataWindow();
    int dw = dataWindow.max.x - dataWindow.min.x + 1;
    int dh = dataWindow.max.y - dataWindow.min.y + 1;
    int dx = dataWindow.min.x;
    int dy = dataWindow.min.y;

    // display black right now
    pixels.resizeErase(dw * dh);
    memset(pixels, 0, (dw * dh) * (sizeof(Rgba)));

    Array< half* > dataR;
    Array< half* > dataG;
    Array< half* > dataB;

    Array< float* > zback;
    Array< half* > alpha;

    zsize = dw * dh;
    zbuff.resizeErase(zsize);
    zback.resizeErase(zsize);
    alpha.resizeErase(dw * dh);

    dataR.resizeErase(dw * dh);
    dataG.resizeErase(dw * dh);
    dataB.resizeErase(dw * dh);
    sampleCount.resizeErase(dw * dh);

    int rgbflag = 0;
    int deepCompflag = 0;

    if (header.channels().findChannel("R"))
    {
        rgbflag = 1;
    }
    else if (header.channels().findChannel("B"))
    {
        rgbflag = 1;
    }
    else if (header.channels().findChannel("G"))
    {
        rgbflag = 1;
    }

    if (header.channels().findChannel("Z") &&
        header.channels().findChannel("A") &&
        deepComp)
    {
        deepCompflag = 1;
    }

    DeepFrameBuffer fb;

    fb.insertSampleCountSlice(Slice(Imf::PixelType::UINT,
        (char *)(&sampleCount[0]
            - dx - dy * dw),
        sizeof(unsigned int) * 1,
        sizeof(unsigned int) * dw));

    fb.insert("Z",
        DeepSlice(Imf::PixelType::FLOAT,
        (char *)(&zbuff[0] - dx - dy * dw),
            sizeof(float *) * 1,    // xStride for pointer array
            sizeof(float *) * dw,   // yStride for pointer array
            sizeof(float) * 1));    // stride for z data sample
    fb.insert("ZBack",
        DeepSlice(Imf::PixelType::FLOAT,
        (char *)(&zback[0] - dx - dy * dw),
            sizeof(float *) * 1,    // xStride for pointer array
            sizeof(float *) * dw,   // yStride for pointer array
            sizeof(float) * 1));    // stride for z data sample

    if (rgbflag)
    {
        fb.insert("R",
            DeepSlice(HALF,
            (char *)(&dataR[0] - dx - dy * dw),
                sizeof(half *) * 1,
                sizeof(half *) * dw,
                sizeof(half) * 1));

        fb.insert("G",
            DeepSlice(HALF,
            (char *)(&dataG[0] - dx - dy * dw),
                sizeof(half *) * 1,
                sizeof(half *) * dw,
                sizeof(half) * 1));

        fb.insert("B",
            DeepSlice(HALF,
            (char *)(&dataB[0] - dx - dy * dw),
                sizeof(half *) * 1,
                sizeof(half *) * dw,
                sizeof(half) * 1));
    }

    fb.insert("A",
        DeepSlice(HALF,
        (char *)(&alpha[0] - dx - dy * dw),
            sizeof(half *) * 1,    // xStride for pointer array
            sizeof(half *) * dw,   // yStride for pointer array
            sizeof(half) * 1,      // stride for z data sample
            1, 1,                   // xSampling, ySampling
            1.0));                  // fillValue

    in.setFrameBuffer(fb);

    in.readPixelSampleCounts(dataWindow.min.y, dataWindow.max.y);

    for (int i = 0; i < dh * dw; i++)
    {
        zbuff[i] = new float[sampleCount[i]];
        zback[i] = new float[sampleCount[i]];
        alpha[i] = new half[sampleCount[i]];
        if (rgbflag)
        {
            dataR[i] = new half[sampleCount[i]];
            dataG[i] = new half[sampleCount[i]];
            dataB[i] = new half[sampleCount[i]];
        }
    }

    in.readPixels(dataWindow.min.y, dataWindow.max.y);

    if (deepCompflag)
    {
        //
        //try deep compositing
        //
        CompositeDeepScanLine comp;
        comp.addSource(&in);

        FrameBuffer fbuffer;
        Rgba *base = pixels - dx - dy * dw;
        size_t xs = 1 * sizeof(Rgba);
        size_t ys = dw * sizeof(Rgba);

        fbuffer.insert("R",
            Slice(HALF,
            (char *)&base[0].r,
                xs, ys,
                1, 1,     // xSampling, ySampling
                0.0));    // fillValue

        fbuffer.insert("G",
            Slice(HALF,
            (char *)&base[0].g,
                xs, ys,
                1, 1,     // xSampling, ySampling
                0.0));    // fillValue

        fbuffer.insert("B",
            Slice(HALF,
            (char *)&base[0].b,
                xs, ys,
                1, 1,     // xSampling, ySampling
                0.0));    // fillValue

        fbuffer.insert("A",
            Slice(HALF,
            (char *)&base[0].a,
                xs, ys,
                1, 1,             // xSampling, ySampling
                1.0));    // fillValue
        comp.setFrameBuffer(fbuffer);
        comp.readPixels(dataWindow.min.y, dataWindow.max.y);
    }
    else
    {
        for (int i = 0; i < dh * dw; i++)
        {
            if (sampleCount[i] > 0)
            {
                if (rgbflag)
                {
                    pixels[i].r = dataR[i][0];
                    pixels[i].g = dataG[i][0];
                    pixels[i].b = dataB[i][0];
                }
                else
                {
                    pixels[i].r = zbuff[i][0];
                    pixels[i].g = alpha[i][0];
                    pixels[i].b = zback[i][0];
                }
            }
        }
    }

    if (!allocateMipSet(pixels, pMipSet, dw, dh))
        return PE_Unknown;
}


int
loadDeepTileImage(MultiPartInputFile &inmaster,
    int &zsize,
    Header &header,
    Array<Rgba> &pixels,
    Array<float*> &zbuff,
    Array<unsigned int> &sampleCount,
    bool deepComp, MipSet* pMipSet)
{
    DeepTiledInputPart in(inmaster, 0);
    header = in.header();

    Box2i &dataWindow = header.dataWindow();
    int dw = dataWindow.max.x - dataWindow.min.x + 1;
    int dh = dataWindow.max.y - dataWindow.min.y + 1;
    int dx = dataWindow.min.x;
    int dy = dataWindow.min.y;

    // display black right now
    pixels.resizeErase(dw * dh);
    memset(pixels, 0, (dw * dh) * (sizeof(Rgba)));

    Array< half* > dataR;
    Array< half* > dataG;
    Array< half* > dataB;

    Array< float* > zback;
    Array< half* > alpha;

    zsize = dw * dh;
    zbuff.resizeErase(zsize);
    zback.resizeErase(zsize);
    alpha.resizeErase(dw * dh);

    dataR.resizeErase(dw * dh);
    dataG.resizeErase(dw * dh);
    dataB.resizeErase(dw * dh);
    sampleCount.resizeErase(dw * dh);

    int rgbflag = 0;
    int deepCompflag = 0;

    if (header.channels().findChannel("R"))
    {
        rgbflag = 1;
    }
    else if (header.channels().findChannel("B"))
    {
        rgbflag = 1;
    }
    else if (header.channels().findChannel("G"))
    {
        rgbflag = 1;
    }

    if (header.channels().findChannel("Z") &&
        header.channels().findChannel("A") &&
        deepComp)
    {
        deepCompflag = 1;
    }

    DeepFrameBuffer fb;

    fb.insertSampleCountSlice(Slice(Imf::PixelType::UINT,
        (char *)(&sampleCount[0]
            - dx - dy * dw),
        sizeof(unsigned int) * 1,
        sizeof(unsigned int) * dw));

    fb.insert("Z",
        DeepSlice(Imf::PixelType::FLOAT,
        (char *)(&zbuff[0] - dx - dy * dw),
            sizeof(float *) * 1,    // xStride for pointer array
            sizeof(float *) * dw,   // yStride for pointer array
            sizeof(float) * 1));    // stride for z data sample
    fb.insert("ZBack",
        DeepSlice(Imf::PixelType::FLOAT,
        (char *)(&zback[0] - dx - dy * dw),
            sizeof(float *) * 1,    // xStride for pointer array
            sizeof(float *) * dw,   // yStride for pointer array
            sizeof(float) * 1));    // stride for z data sample

    if (rgbflag)
    {
        fb.insert("R",
            DeepSlice(HALF,
            (char *)(&dataR[0] - dx - dy * dw),
                sizeof(half *) * 1,
                sizeof(half *) * dw,
                sizeof(half) * 1));

        fb.insert("G",
            DeepSlice(HALF,
            (char *)(&dataG[0] - dx - dy * dw),
                sizeof(half *) * 1,
                sizeof(half *) * dw,
                sizeof(half) * 1));

        fb.insert("B",
            DeepSlice(HALF,
            (char *)(&dataB[0] - dx - dy * dw),
                sizeof(half *) * 1,
                sizeof(half *) * dw,
                sizeof(half) * 1));


    }

    fb.insert("A",
        DeepSlice(HALF,
        (char *)(&alpha[0] - dx - dy * dw),
            sizeof(half *) * 1,    // xStride for pointer array
            sizeof(half *) * dw,   // yStride for pointer array
            sizeof(half) * 1,      // stride for z data sample
            1, 1,                   // xSampling, ySampling
            1.0));                  // fillValue

    in.setFrameBuffer(fb);

    int numXTiles = in.numXTiles(0);
    int numYTiles = in.numYTiles(0);

    in.readPixelSampleCounts(0, numXTiles - 1, 0, numYTiles - 1);

    for (int i = 0; i < dh * dw; i++)
    {
        zbuff[i] = new float[sampleCount[i]];
        zback[i] = new float[sampleCount[i]];
        alpha[i] = new half[sampleCount[i]];
        if (rgbflag)
        {
            dataR[i] = new half[sampleCount[i]];
            dataG[i] = new half[sampleCount[i]];
            dataB[i] = new half[sampleCount[i]];
        }
    }

    in.readTiles(0, numXTiles - 1, 0, numYTiles - 1);

    if (deepCompflag)
    {
        // Loop over all the pixels and comp manually
        // @ToDo implent deep compositing for the DeepTile case
        for (int i = 0; i<zsize; ++i)
        {
            float a = alpha[i][0];
            pixels[i].r = dataR[i][0];
            pixels[i].g = dataG[i][0];
            pixels[i].b = dataB[i][0];

            for (int s = 1; s<sampleCount[i]; s++)
            {
                if (a >= 1.f)
                    break;

                pixels[i].r += (1.f - a) * dataR[i][s];
                pixels[i].g += (1.f - a) * dataG[i][s];
                pixels[i].b += (1.f - a) * dataB[i][s];
                a += (1.f - a) * alpha[i][s];
            }
        }
    }
    else
    {
        for (int i = 0; i < dh * dw; i++)
        {
            if (sampleCount[i] > 0)
            {
                if (rgbflag)
                {
                    pixels[i].r = dataR[i][0];
                    pixels[i].g = dataG[i][0];
                    pixels[i].b = dataB[i][0];
                }
                else
                {
                    pixels[i].r = zbuff[i][0];
                    pixels[i].g = alpha[i][0];
                    pixels[i].b = zback[i][0];
                }
            }
        }
    }

    if (!allocateMipSet(pixels, pMipSet, dw, dh))
        return PE_Unknown;

}

int Plugin_EXR::TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet)
{
    if (!boost::filesystem::exists( pszFilename )) return -1;

    // uncomment the flag below to disable EXR mipmap loading / load only level 0
    // pMipSet->m_Flags |= MS_FLAG_DisableMipMapping;
    
    bool isTile = false;
    try
    {
        RgbaInputFile fileInfo(pszFilename);
        isTile = Imf::isTiled(fileInfo.version());
    }
    catch (std::exception& e)
    {
        if (EXR_CMips)
            EXR_CMips->PrintError(e.what());
        return PE_Unknown;
    }

    pMipSet->m_format = CMP_FORMAT_ARGB_16F;

    //handle mipmap exr load using Tile File
    if (((isTile)) && (!(pMipSet->m_Flags & MS_FLAG_DisableMipMapping)))
    {
        TiledRgbaInputFile file(pszFilename);

        if (!file.isComplete())
            return PE_Unknown;

        CMP_DWORD dwWidth = file.levelWidth(0);
        CMP_DWORD dwHeight = file.levelHeight(0);

        if (!EXR_CMips->AllocateMipSet(pMipSet, CF_Float16, TDT_ARGB, TT_2D, dwWidth, dwHeight, 1))
        {
            return PE_Unknown;
        }

        pMipSet->m_dwFourCC = 0;
        pMipSet->m_dwFourCC2 = 0;
        pMipSet->m_nMipLevels = (file.levelMode() == MIPMAP_LEVELS) ? file.numLevels() : 1;

        for (int i = 0; i < pMipSet->m_nMipLevels; i++)
        {
            if (!file.isValidLevel(i, i))
            {
                pMipSet->m_nMipLevels = i;
                break;
            }

            dwWidth = file.levelWidth(i);
            dwHeight = file.levelHeight(i);
            Array2D<Rgba> pixels(dwHeight, dwWidth);
            pixels.resizeErase(dwHeight, dwWidth);

            file.setFrameBuffer(pixels[0], 1, dwWidth);
            file.readTiles(0, file.numXTiles(i) - 1, 0, file.numYTiles(i) - 1, i);

            // Allocate the permanent buffer and unpack the bitmap data into it
            if (!EXR_CMips->AllocateMipLevelData(EXR_CMips->GetMipLevel(pMipSet, i), dwWidth, dwHeight, CF_Float16, pMipSet->m_TextureDataType))
                return PE_Unknown;

            Rgba2Texture(pixels, (CMP_HALF* )EXR_CMips->GetMipLevel(pMipSet, i)->m_pbData, dwWidth, dwHeight);
        }
        return PE_OK;
    } // Tiled file

    //for non mipmap load
    const char *channel = 0;
    const char *layer = 0;
    int numparts = 0;

    try
    {
        MultiPartInputFile *infile = new MultiPartInputFile(pszFilename);
        numparts = infile->parts();
        delete infile;
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
        cerr << "\n" << "ERROR:" << endl;
        cerr << e.what() << endl;
        return PE_Unknown;
    }

    Header header;
    Array<Rgba>pixels;
    Array<float*>zbuff;
    Array<unsigned int> sampleCount;
    bool                deepComp;
    bool preview = false;
    int zsize =0;

    //loadImage
    MultiPartInputFile inmaster(pszFilename);
    Header h = inmaster.header(0);
    std::string  type = h.type();

    if (type == DEEPTILE)
    {
        loadDeepTileImage(inmaster, zsize,
            header,
            pixels,
            zbuff,
            sampleCount,
            deepComp, pMipSet);
    }
    else if (type == DEEPSCANLINE)
    {
        loadDeepScanlineImage(inmaster,
            zsize,
            header,
            pixels,
            zbuff,
            sampleCount,
            deepComp, pMipSet);
    }


    else if (preview)
    {
        loadPreviewImage(pszFilename, header, pixels, pMipSet);
    }
    else
    {
        if (channel)
        {

            loadImageChannel(pszFilename,
                channel,
                header,
                pixels, pMipSet);
        }
        else
        {
            loadImage(pszFilename,
                layer,
                header,
                pixels, pMipSet);
        }
    }
    return PE_OK;
}



int Plugin_EXR::TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet)
{
    if(!TC_PluginFileSupportsFormat(NULL, pMipSet))
    {
        if (EXR_CMips)
            EXR_CMips->PrintError("Error(%d): EXR Plugin ID(%d) Filename=%s\n unsupported format, EXR only support 16F type.",EL_Error,IDS_ERROR_UNSUPPORTED_TYPE,pszFilename);
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

        CMP_HALF *data = EXR_CMips->GetMipLevel(pMipSet, 0)->m_phfData;

        Texture2Rgba(data, pixels, image_width, image_height, pMipSet->m_isDeCompressed);

        Exr::writeRgba(sFile,pixels,image_width,image_height);
    }
    // Save Muliple MIP levels as TiledRGB 
    else    
    {
        TiledRgbaOutputFile file(pszFilename, pMipSet->m_nWidth, pMipSet->m_nHeight, TILE_WIDTH, TILE_HEIGHT, levelMode, ROUND_DOWN);
        for(int i = 0; i < file.numLevels(); i++)
        {
            Array2D<Rgba> pixels(file.levelHeight(i), file.levelWidth(i));
            pixels.resizeErase(file.levelHeight(i), file.levelWidth(i));
            CMP_HALF *data = EXR_CMips->GetMipLevel(pMipSet, i)->m_phfData;

            if(data)
            {
                Texture2Rgba(data, pixels, file.levelWidth(i), file.levelHeight(i), pMipSet->m_isDeCompressed);
            }
            else
                memset(data, 0, EXR_CMips->GetMipLevel(pMipSet, 0)->m_dwLinearSize);
            
            file.setFrameBuffer(pixels[0], 1, file.levelWidth(i));
            file.writeTiles(0, file.numXTiles(i) - 1, 0, file.numYTiles(i) - 1, i);
        }
    }

    return PE_OK;
}

bool TC_PluginFileSupportsFormat(const HFILETYPE, const MipSet* pMipSet)
{
    assert(pMipSet);
    if(pMipSet == NULL)
        return false;

    if(pMipSet->m_TextureType != TT_2D)
        return false;

    return (pMipSet->m_ChannelFormat == CF_Float16) && (pMipSet->m_TextureDataType == TDT_ARGB || pMipSet->m_TextureDataType == TDT_XRGB) ? true : false;
}

//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \file TextureIO.cpp
/// \version 2.20
//
//=====================================================================

#include "Compressonator.h"
#include "PluginManager.h"
#include "PluginInterface.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include "TextureIO.h"
#include <iostream>



#ifdef USE_QT_IMAGELOAD
#include <QtCore/QCoreApplication>
#include <QtGui/qimage.h>

#ifndef _WIN32
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */
#endif

#ifdef _DEBUG
#pragma comment(lib,"Qt5Cored.lib")
#pragma comment(lib,"Qt5Guid.lib")
#else
#pragma comment(lib,"Qt5Core.lib")
#pragma comment(lib,"Qt5Gui.lib")
#endif

#endif

using namespace std;

// Global plugin manager instance
extern PluginManager g_pluginManager;                    
extern bool g_bAbortCompression;

void astc_find_closest_blockdim_2d(float target_bitrate, int *x, int *y, int consider_illegal)
{
    int blockdims[6] = { 4, 5, 6, 8, 10, 12 };

    float best_error = 1000;
    float aspect_of_best = 1;
    int i, j;

    // Y dimension
    for (i = 0; i < 6; i++)
    {
        // X dimension
        for (j = i; j < 6; j++)
        {
            //              NxN       MxN         8x5               10x5              10x6
            int is_legal = (j == i) || (j == i + 1) || (j == 3 && j == 1) || (j == 4 && j == 1) || (j == 4 && j == 2);

            if (consider_illegal || is_legal)
            {
                float bitrate = 128.0f / (blockdims[i] * blockdims[j]);
                float bitrate_error = fabs(bitrate - target_bitrate);
                float aspect = (float)blockdims[j] / blockdims[i];
                if (bitrate_error < best_error || (bitrate_error == best_error && aspect < aspect_of_best))
                {
                    *x = blockdims[j];
                    *y = blockdims[i];
                    best_error = bitrate_error;
                    aspect_of_best = aspect;
                }
            }
        }
    }
}

void astc_find_closest_blockxy_2d(int *x, int *y, int consider_illegal)
{
    int blockdims[6] = { 4, 5, 6, 8, 10, 12 };

    bool exists_x = std::find(std::begin(blockdims), std::end(blockdims), (*x)) != std::end(blockdims);
    bool exists_y = std::find(std::begin(blockdims), std::end(blockdims), (*y)) != std::end(blockdims);

    if (exists_x && exists_y)
    {
        if ((*x) < (*y)) 
        {
            int temp = *x;
            *x = *y;
            *y = temp;
        }
        float bitrateF = float(128.0f / ((*x)*(*y)));
        astc_find_closest_blockdim_2d(bitrateF, x, y, 0);
    }
    else
    {
        float bitrateF = float(128.0f / ((*x)*(*y)));
        astc_find_closest_blockdim_2d(bitrateF, x, y, 0);
    }
  
}

int MaxFacesOrSlices(const MipSet* pMipSet, int nMipLevel)
{
    if(!pMipSet)
        return 0;

    if(pMipSet->m_nDepth < 1)
        return 0;

    if(pMipSet->m_TextureType == TT_2D || pMipSet->m_TextureType == TT_CubeMap)
        return pMipSet->m_nDepth;

    int nMaxSlices = pMipSet->m_nDepth;
    for(int i=0; i<pMipSet->m_nMipLevels; i++)
    {
        if(i == nMipLevel)
            return nMaxSlices;

        nMaxSlices = nMaxSlices>1 ? nMaxSlices>>1 : 1;    //div by 2, min of 1
    }
    return 0;    //nMipLevel was too high
}

bool DeCompressionCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
   UNREFERENCED_PARAMETER(pUser1);
   UNREFERENCED_PARAMETER(pUser2);

   static float Progress = 0;
   if (fProgress > Progress) Progress = fProgress;
   PrintInfo("\rDeCompression progress = %2.0f",Progress);
   return g_bAbortCompression;
}


bool IsFileExt(const char *fname, const char *fext)
{
    string file_extension  = boost::filesystem::extension(fname);
    boost::algorithm::to_lower(file_extension); 
    if (file_extension.compare(fext) == 0)
    {
        return true;
    }
    return false;
}

bool IsDestinationUnCompressed(const char *fname)
{
    bool isuncompressed = true;
    string file_extension  = boost::filesystem::extension(fname);
    boost::algorithm::to_lower(file_extension); 
    if (file_extension.compare(".dds") == 0)
    {
        isuncompressed = false;
    }
    else
    if (file_extension.compare(".astc") == 0)
    {
        isuncompressed = false;
    }
    else
    if (file_extension.compare(".ktx") == 0)
    {
        isuncompressed = false;
    }
    else
    if(file_extension.compare(".raw") == 0)
    {
        isuncompressed = false;
    }
#ifdef USE_CRN
    else
    if (file_extension.compare(".crn") == 0)
    {
        isuncompressed = false;
    }
#endif

    return isuncompressed;
}


CMP_FORMAT FormatByFileExtension(const char *fname, MipSet *pMipSet)
{
    string file_extension  = boost::filesystem::extension(fname);
    boost::algorithm::to_lower(file_extension); 

    pMipSet->m_TextureDataType    = TDT_ARGB;

    if (file_extension.compare(".exr") == 0)
    {
        pMipSet->m_ChannelFormat    = CF_Float16;
        return CMP_FORMAT_ARGB_16F;
    }

    pMipSet->m_ChannelFormat    = CF_8bit;
    return CMP_FORMAT_ARGB_8888;
}



CMP_FORMAT GetFormat(CMP_DWORD dwFourCC)
{
    switch(dwFourCC)
    {
        case FOURCC_ATI1N:        return CMP_FORMAT_ATI1N;
        case FOURCC_ATI2N:        return CMP_FORMAT_ATI2N;
        case FOURCC_ATI2N_XY:    return CMP_FORMAT_ATI2N_XY;
        case FOURCC_ATI2N_DXT5:    return CMP_FORMAT_ATI2N_DXT5;
        case FOURCC_DXT1:        return CMP_FORMAT_DXT1;
        case FOURCC_DXT3:        return CMP_FORMAT_DXT3;
        case FOURCC_DXT5:        return CMP_FORMAT_DXT5;
        case FOURCC_DXT5_xGBR:    return CMP_FORMAT_DXT5_xGBR;
        case FOURCC_DXT5_RxBG:    return CMP_FORMAT_DXT5_RxBG;
        case FOURCC_DXT5_RBxG:    return CMP_FORMAT_DXT5_RBxG;
        case FOURCC_DXT5_xRBG:    return CMP_FORMAT_DXT5_xRBG;
        case FOURCC_DXT5_RGxB:    return CMP_FORMAT_DXT5_RGxB;
        case FOURCC_DXT5_xGxR:    return CMP_FORMAT_DXT5_xGxR;

        // Deprecated but still supported for decompression
        // Some definition are not valid FOURCC values nut are used as Custom formats
        // so that DDS files can be used for storage
        case FOURCC_DXT5_GXRB:          return CMP_FORMAT_DXT5_xRBG;
        case FOURCC_DXT5_GRXB:          return CMP_FORMAT_DXT5_RxBG;
        case FOURCC_DXT5_RXGB:          return CMP_FORMAT_DXT5_xGBR;
        case FOURCC_DXT5_BRGX:          return CMP_FORMAT_DXT5_RGxB;
        case FOURCC_BC4S:               return CMP_FORMAT_ATI1N;
        case FOURCC_BC4U:               return CMP_FORMAT_ATI1N;
        case FOURCC_BC5S:               return CMP_FORMAT_ATI2N;
        case FOURCC_ATC_RGB:            return CMP_FORMAT_ATC_RGB;
        case FOURCC_ATC_RGBA_EXPLICIT:  return CMP_FORMAT_ATC_RGBA_Explicit;
        case FOURCC_ATC_RGBA_INTERP:    return CMP_FORMAT_ATC_RGBA_Interpolated;
        case FOURCC_ETC_RGB:            return CMP_FORMAT_ETC_RGB;
        case FOURCC_ETC2_RGB:           return CMP_FORMAT_ETC2_RGB;
        case FOURCC_BC6H:               return CMP_FORMAT_BC6H;
        case FOURCC_BC7:                return CMP_FORMAT_BC7;
        case FOURCC_ASTC:               return CMP_FORMAT_ASTC;
        case FOURCC_GT:                 return CMP_FORMAT_GT;


        default: 
            return CMP_FORMAT_Unknown;
    }
}

void Format2FourCC(CMP_FORMAT format, MipSet *pMipSet)
{
    switch(format)
    {
        case CMP_FORMAT_BC4:
        case CMP_FORMAT_ATI1N:                  pMipSet->m_dwFourCC   = FOURCC_ATI1N;              break; 
        case CMP_FORMAT_ATI2N:                  pMipSet->m_dwFourCC   =  FOURCC_ATI2N;             break;

        case CMP_FORMAT_BC5:
        case CMP_FORMAT_ATI2N_XY:
                                                   pMipSet->m_dwFourCC    = FOURCC_ATI2N;
                                                   pMipSet->m_dwFourCC2   = FOURCC_ATI2N_XY;
                                                   break;
        case CMP_FORMAT_ATI2N_DXT5:             pMipSet->m_dwFourCC     = FOURCC_ATI2N_DXT5;       break;

        case CMP_FORMAT_BC1:
        case CMP_FORMAT_DXT1:                   pMipSet->m_dwFourCC =  FOURCC_DXT1;                break;
        
        case CMP_FORMAT_BC2:
        case CMP_FORMAT_DXT3:                   pMipSet->m_dwFourCC =  FOURCC_DXT3;                break;
        
        case CMP_FORMAT_BC3:
        case CMP_FORMAT_DXT5:                   pMipSet->m_dwFourCC =  FOURCC_DXT5;                break;
        case CMP_FORMAT_DXT5_xGBR:              pMipSet->m_dwFourCC =  FOURCC_DXT5_xGBR;           break;
        case CMP_FORMAT_DXT5_RxBG:              pMipSet->m_dwFourCC =  FOURCC_DXT5_RxBG;           break;
        case CMP_FORMAT_DXT5_RBxG:              pMipSet->m_dwFourCC =  FOURCC_DXT5_RBxG;           break;
        case CMP_FORMAT_DXT5_xRBG:              pMipSet->m_dwFourCC =  FOURCC_DXT5_xRBG;           break;
        case CMP_FORMAT_DXT5_RGxB:              pMipSet->m_dwFourCC =  FOURCC_DXT5_RGxB;           break;
        case CMP_FORMAT_DXT5_xGxR:              pMipSet->m_dwFourCC =  FOURCC_DXT5_xGxR;           break;

        case CMP_FORMAT_ATC_RGB:                pMipSet->m_dwFourCC = FOURCC_ATC_RGB;             break;
        case CMP_FORMAT_ATC_RGBA_Explicit:      pMipSet->m_dwFourCC =  FOURCC_ATC_RGBA_EXPLICIT;   break;
        case CMP_FORMAT_ATC_RGBA_Interpolated:  pMipSet->m_dwFourCC =  FOURCC_ATC_RGBA_INTERP;     break;

        case CMP_FORMAT_ETC_RGB:                pMipSet->m_dwFourCC =  FOURCC_ETC_RGB;             break;
        case CMP_FORMAT_ETC2_RGB:               pMipSet->m_dwFourCC =  FOURCC_ETC2_RGB;            break;
        case CMP_FORMAT_GT:                     pMipSet->m_dwFourCC =  FOURCC_GT;                  break;

        case CMP_FORMAT_BC6H:                   pMipSet->m_dwFourCC =  FOURCC_DX10;                break;
        case CMP_FORMAT_BC6H_SF:                pMipSet->m_dwFourCC = FOURCC_DX10;                break;
        case CMP_FORMAT_BC7:                    pMipSet->m_dwFourCC =  FOURCC_DX10;                break;
        case CMP_FORMAT_ASTC:                   pMipSet->m_dwFourCC =  FOURCC_DX10;                break;

        default:
                                                   pMipSet->m_dwFourCC =  FOURCC_DX10;
    }
}

CMP_FORMAT GetFormat(MipSet* pMipSet)
{
    assert(pMipSet);
    if(pMipSet == NULL)
        return CMP_FORMAT_Unknown;

    switch(pMipSet->m_ChannelFormat)
    {
        case CF_8bit:
            switch(pMipSet->m_TextureDataType)
            {
                case TDT_R:         return CMP_FORMAT_R_8;
                case TDT_RG:        return CMP_FORMAT_RG_8;
                default:            return CMP_FORMAT_ARGB_8888;
            }
        case CF_Float16:
            switch(pMipSet->m_TextureDataType)
            {
                case TDT_R:         return CMP_FORMAT_R_16F;
                case TDT_RG:        return CMP_FORMAT_RG_16F;
                default:            return CMP_FORMAT_ARGB_16F;
            }
        case CF_Float32:    
            switch(pMipSet->m_TextureDataType)
            {
                case TDT_R:         return CMP_FORMAT_R_32F;
                case TDT_RG:        return CMP_FORMAT_RG_32F;
                default:            return CMP_FORMAT_ARGB_32F;
            }
        case CF_Float9995E:
            return CMP_FORMAT_RGBE_32F;
            
        case CF_Compressed:         return GetFormat(pMipSet->m_dwFourCC2 ? pMipSet->m_dwFourCC2 : pMipSet->m_dwFourCC);
        case CF_16bit:
            switch(pMipSet->m_TextureDataType)
            {
                case TDT_R:         return CMP_FORMAT_R_16;
                case TDT_RG:        return CMP_FORMAT_RG_16;
                default:            return CMP_FORMAT_ARGB_16;
            }
        case CF_2101010:            return CMP_FORMAT_ARGB_2101010;

#ifdef ARGB_32_SUPPORT
        case CF_32bit:
            switch(pMipSet->m_TextureDataType)
            {
                case TDT_R:     return CMP_FORMAT_R_32;
                case TDT_RG:    return CMP_FORMAT_RG_32;
                default:        return CMP_FORMAT_ARGB_32;
            }
#endif // ARGB_32_SUPPORT

        default:        
            return CMP_FORMAT_Unknown;
    }
}


bool FloatFormat(CMP_FORMAT InFormat)
{
    switch (InFormat)
    {
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
    case CMP_FORMAT_RG_16F:
    case CMP_FORMAT_R_16F:
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_BGR_32F:
    case CMP_FORMAT_RG_32F:
    case CMP_FORMAT_R_32F:
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
    case CMP_FORMAT_RGBE_32F:
    {
        return true;
    }
    break;
    default:
        break;
    }

    return false;
}

bool CompressedFormat(CMP_FORMAT format)
{
    switch (format)
    {
    case CMP_FORMAT_Unknown:
    case CMP_FORMAT_ARGB_8888:
    case CMP_FORMAT_RGB_888:
    case CMP_FORMAT_RG_8:
    case CMP_FORMAT_R_8:
    case CMP_FORMAT_ARGB_2101010:
    case CMP_FORMAT_ARGB_16:
    case CMP_FORMAT_RG_16:
    case CMP_FORMAT_R_16:
    case CMP_FORMAT_RGBE_32F:
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_RG_16F:
    case CMP_FORMAT_R_16F:
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_RG_32F:
    case CMP_FORMAT_R_32F:
        return (false);
        break;
    default:
        break;
    }
    return true;
}

bool CompressedFileFormat(string file)
{
    string file_extension = boost::filesystem::extension(file);
    boost::algorithm::to_upper(file_extension);
    boost::erase_all(file_extension, ".");

    if (file_extension == "BMP")
        return false;
    else
        return true;
}

#ifdef USE_QT_IMAGELOAD

/* conversion from the ILM Half
* format into the normal 32 bit pixel format. Refer to http://www.openexr.com/using.html
* on each steps regarding how to display your image
*/
unsigned int floatToQrgba(float r, float g, float b, float a)
{
    // step 3) Values, which are now 1.0, are called "middle gray".
    //     If defog and exposure are both set to 0.0, then
    //     middle gray corresponds to a raw pixel value of 0.18.
    //     In step 6, middle gray values will be mapped to an
    //     intensity 3.5 f-stops below the display's maximum
    //     intensity.

    // step 4) Apply a knee function.  The knee function has two
    //     parameters, kneeLow and kneeHigh.  Pixel values
    //     below 2^kneeLow are not changed by the knee
    //     function.  Pixel values above kneeLow are lowered
    //     according to a logarithmic curve, such that the
    //     value 2^kneeHigh is mapped to 2^3.5 (in step 6,
    //     this value will be mapped to the display's
    //     maximum intensity).
    //     kneeLow = 0.0 (2^0.0 => 1); kneeHigh = 5.0 (2^5 =>32)
    if (r > 1.0)
        r = 1.0 + Imath::Math<float>::log((r - 1.0) * 0.184874 + 1) / 0.184874;
    if (g > 1.0)
        g = 1.0 + Imath::Math<float>::log((g - 1.0) * 0.184874 + 1) / 0.184874;
    if (b > 1.0)
        b = 1.0 + Imath::Math<float>::log((b - 1.0) * 0.184874 + 1) / 0.184874;
    if (a > 1.0)
        a = 1.0 + Imath::Math<float>::log((a - 1.0) * 0.184874 + 1) / 0.184874;
    //
    // Step 5) Gamma-correct the pixel values, assuming that the
    //     screen's gamma is 0.4545 (or 1/2.2).
    r = Imath::Math<float>::pow(r, 0.4545f);
    g = Imath::Math<float>::pow(g, 0.4545f);
    b = Imath::Math<float>::pow(b, 0.4545f);
    a = Imath::Math<float>::pow(a, 0.4545f);

    // Step  6) Scale the values such that pixels middle gray
    //     pixels are mapped to 84.66 (or 3.5 f-stops below
    //     the display's maximum intensity).
    //
    // Step 7) Clamp the values to [0, 255].
    return qRgba((unsigned char)(Imath::clamp(r * 84.66f, 0.f, 255.f)),
        (unsigned char)(Imath::clamp(g * 84.66f, 0.f, 255.f)),
        (unsigned char)(Imath::clamp(b * 84.66f, 0.f, 255.f)),
        (unsigned char)(Imath::clamp(a * 84.66f, 0.f, 255.f)));
}

QImage::Format MipFormat2QFormat(MipSet *mipset)
{
    QImage::Format format = QImage::Format_Invalid;

    if (CompressedFormat(mipset->m_format))
        return format;

    switch (mipset->m_ChannelFormat)
    {
    case CF_8bit: {format = QImage::Format_ARGB32; break; }
    case CF_Float16: {format = QImage::Format_ARGB32; break; }
    case CF_Float32: {format = QImage::Format_ARGB32; break; }
    case CF_Compressed: {break; }
    case CF_16bit: {break; }
    case CF_2101010: {break; }
    case CF_32bit: {format = QImage::Format_ARGB32;  break; }
    default: {break; }
    }

    return format;
}


int QImage2MIPS(QImage *qimage, CMIPS *m_CMips, MipSet *pMipSet)
{
    if (qimage == NULL)
    {
        return -1;
    }

    // QImage info for debugging
    // QImageFormatInfo(qimage);
    QImage::Format format = qimage->format();

    // Check supported format
    if (!((format == QImage::Format_ARGB32) ||
        (format   == QImage::Format_ARGB32_Premultiplied) || 
        (format   == QImage::Format_RGB32) ||
        (format == QImage::Format_Mono) ||
        (format == QImage::Format_Indexed8)))
    {
        return -1;
    }

    // Set the channel formats and mip levels
    pMipSet->m_ChannelFormat = CF_8bit;
    pMipSet->m_TextureDataType = TDT_ARGB;
    pMipSet->m_dwFourCC = 0;
    pMipSet->m_dwFourCC2 = 0;
    pMipSet->m_TextureType = TT_2D;
    pMipSet->m_format = CMP_FORMAT_ARGB_8888;


    // Allocate default MipSet header
    m_CMips->AllocateMipSet(pMipSet,
                            pMipSet->m_ChannelFormat,
                            pMipSet->m_TextureDataType,
                            pMipSet->m_TextureType,
                            qimage->width(),
                            qimage->height(),
                            1);

    // Determin buffer size and set Mip Set Levels we want to use for now
    MipLevel *mipLevel = m_CMips->GetMipLevel(pMipSet, 0);
    pMipSet->m_nMipLevels = 1;
    m_CMips->AllocateMipLevelData(mipLevel, pMipSet->m_nWidth, pMipSet->m_nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType);

    // We have allocated a data buffer to fill get its referance
    CMP_BYTE* pData = (CMP_BYTE*)(mipLevel->m_pbData);
    QRgb qRGB;
    int i = 0;

    if (pMipSet->m_swizzle)
    {
        for (int y = 0; y < qimage->height(); y++) {
            for (int x = 0; x < qimage->width(); x++) {
                qRGB = qimage->pixel(x, y);
                pData[i] = qBlue(qRGB); 
                i++;
                pData[i] = qGreen(qRGB);
                i++;
                pData[i] = qRed(qRGB);
                i++;
                pData[i] = qAlpha(qRGB);
                i++;
            }
        }
        pMipSet->m_swizzle = false; //already swizzled; reset
    }
    else
    {
        for (int y = 0; y < qimage->height(); y++) {
            for (int x = 0; x < qimage->width(); x++) {
                qRGB = qimage->pixel(x, y);
                pData[i] = qRed(qRGB);
                i++;
                pData[i] = qGreen(qRGB);
                i++;
                pData[i] = qBlue(qRGB);
                i++;
                pData[i] = qAlpha(qRGB);
                i++;
            }
        }
    }


    return 0;
}

//load data byte in mipset into Qimage ARGB32 format
QImage *MIPS2QImage(CMIPS *m_CMips, MipSet *mipset, int level)
{
    if (mipset == NULL) return NULL;
    if (mipset->m_compressed) return NULL;

    MipLevel* mipLevel = m_CMips->GetMipLevel(mipset, level);
    if (!mipLevel) return NULL;

    QImage *image = NULL;

    if (
        (mipset->m_TextureDataType == TDT_ARGB) ||
        (mipset->m_TextureDataType == TDT_XRGB)
        )
    {
        // We have allocated a data buffer to fill get its referance
        CMP_BYTE* pData = mipLevel->m_pbData;
        if (pData == NULL)  return NULL;

        // We dont support the conversion 
        if (MipFormat2QFormat(mipset) == QImage::Format_Invalid)
        {
            return NULL;
        }

        // Allocates a uninitialized buffer of specified size and format
        image = new QImage(mipLevel->m_nWidth, mipLevel->m_nHeight, MipFormat2QFormat(mipset));
        if (image == NULL) return nullptr;

        // Initialize the buffer
        CMP_BYTE R, G, B, A;
        int i = 0;


        if (mipset->m_swizzle)
        {
            for (int y = 0; y < mipLevel->m_nHeight; y++)
            {
                for (int x = 0; x < mipLevel->m_nWidth; x++)
                {
                    B = pData[i];
                    i++;
                    G = pData[i];
                    i++;
                    R = pData[i];
                    i++;
                    A = pData[i];;
                    i++;
                    image->setPixel(x, y, qRgba(R, G, B, A));
                }
            }
        }
        else
        {
            for (int y = 0; y < mipLevel->m_nHeight; y++)
            {
                for (int x = 0; x < mipLevel->m_nWidth; x++)
                {
                    R = pData[i];
                    i++;
                    G = pData[i];
                    i++;
                    B = pData[i];
                    i++;
                    A = pData[i];;
                    i++;
                    image->setPixel(x, y, qRgba(R, G, B, A));
                }
            }
        }
    }
 
    return image;
}
#endif

//
// Used exclusively by the GUI app 
// ToDo : Remove this code and try to use ProcessCMDLine
MipSet* DecompressMIPSet(MipSet *MipSetIn, CMP_GPUDecode decodeWith, Config *configSetting, CMP_Feedback_Proc pFeedbackProc)
{
    // validate MipSet is Compressed
    if (!MipSetIn->m_compressed) return NULL;
    if (MipSetIn->m_format == CMP_FORMAT_ASTC && !(configSetting->useCPU) && decodeWith == CMP_GPUDecode::GPUDecode_DIRECTX)
    {
        configSetting->errMessage = "ASTC format does not supported by DirectX API. Please view ASTC compressed images using other options (CPU or Vulkan) (under Settings->Application Options).";
        PrintInfo("Decompress Error: ASTC format does not supported by DirectX API. Please view ASTC compressed images using CPU or GPU_Vulkan (under Settings->Application Options).\n");
        return NULL;
    }
    else if (MipSetIn->m_format == CMP_FORMAT_ASTC && !(configSetting->useCPU) && decodeWith == CMP_GPUDecode::GPUDecode_OPENGL)
    {
        configSetting->errMessage = "Decode ASTC with OpenGL is not supported. Please view ASTC compressed images using other options (CPU or Vulkan) (under Settings->Application Options).";
        PrintInfo("Decompress Error: Decode ASTC with OpenGL is not supported. Please view ASTC compressed images using CPU or GPU_Vulkan (under Settings->Application Options).\n");
        return NULL;
    }

    // Compress Options
    bool silent = true;
    CMP_CompressOptions    CompressOptions;
    memset(&CompressOptions, 0, sizeof(CMP_CompressOptions));
    CompressOptions.dwnumThreads = 8;
    CMIPS m_CMIPS;

    MipSet    *MipSetOut = new MipSet();
    memset(MipSetOut, 0, sizeof(MipSet));

    MipSetOut->m_TextureDataType = TDT_ARGB;
    MipSetOut->m_swizzle = false;
    MipSetOut->m_CubeFaceMask = MipSetIn->m_CubeFaceMask;
    MipSetOut->m_Flags = MipSetIn->m_Flags;
    MipSetOut->m_nDepth = MipSetIn->m_nDepth;
    MipSetOut->m_nMaxMipLevels = MipSetIn->m_nMaxMipLevels;
    MipSetOut->m_nHeight = MipSetIn->m_nHeight;
    MipSetOut->m_nWidth = MipSetIn->m_nWidth;

    // BMP is saved as CMP_FORMAT_ARGB_8888
    // EXR is saved as CMP_FORMAT_ARGB_16F
    switch (MipSetIn->m_format)
    {
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
        MipSetOut->m_format = CMP_FORMAT_ARGB_16F;
        MipSetOut->m_ChannelFormat = CF_Float16;
        break;
    default:
        MipSetOut->m_format = CMP_FORMAT_ARGB_8888;
        break;
    }

    // Allocate output MipSet
    if (!m_CMIPS.AllocateMipSet(MipSetOut,
        MipSetOut->m_ChannelFormat,
        MipSetOut->m_TextureDataType,
        MipSetIn->m_TextureType,
        MipSetIn->m_nWidth,
        MipSetIn->m_nHeight,
        MipSetIn->m_nDepth))
    {
        configSetting->errMessage = "Memory Error(2): allocating MIPSet Output buffer.";
        PrintInfo("Memory Error(2): allocating MIPSet Output buffer\n");
        m_CMIPS.FreeMipSet(MipSetOut);
        delete MipSetOut;
        MipSetOut = NULL;
        return NULL;
    }

    MipLevel* pCmpMipLevel = m_CMIPS.GetMipLevel(MipSetIn, 0);
    int nMaxFaceOrSlice = MaxFacesOrSlices(MipSetIn, 0);
    int nWidth = pCmpMipLevel->m_nWidth;
    int nHeight = pCmpMipLevel->m_nHeight;

    CMP_BYTE* pMipData = m_CMIPS.GetMipLevel(MipSetIn, 0, 0)->m_pbData;

    for (int nFaceOrSlice = 0; nFaceOrSlice<nMaxFaceOrSlice; nFaceOrSlice++)
    {
        int nMipWidth = nWidth;
        int nMipHeight = nHeight;

        for (int nMipLevel = 0; nMipLevel<MipSetIn->m_nMipLevels; nMipLevel++)
        {
            MipLevel* pInMipLevel = m_CMIPS.GetMipLevel(MipSetIn, nMipLevel, nFaceOrSlice);
            if (!pInMipLevel)
            {
                configSetting->errMessage = "Memory Error(2): allocating MIPSet Output Cmp level buffer";
                PrintInfo("Memory Error(2): allocating MIPSet Output Cmp level buffer\n");
                m_CMIPS.FreeMipSet(MipSetOut);
                delete MipSetOut;
                MipSetOut = NULL;
                return NULL;
            }

            // Valid Mip Level ?
            if (pInMipLevel->m_pbData)
                pMipData = pInMipLevel->m_pbData;

            if (!m_CMIPS.AllocateMipLevelData(m_CMIPS.GetMipLevel(MipSetOut, nMipLevel, nFaceOrSlice), nMipWidth,
                nMipHeight, MipSetOut->m_ChannelFormat, MipSetOut->m_TextureDataType))
            {
                configSetting->errMessage = "Memory Error(2): allocating MIPSet Output level buffer.";
                PrintInfo("Memory Error(2): allocating MIPSet Output level buffer\n");
                m_CMIPS.FreeMipSet(MipSetOut);
                delete MipSetOut;
                MipSetOut = NULL;
                return NULL;
            }

            //----------------------------
            // Compressed source 
            //-----------------------------
            CMP_Texture srcTexture;
            srcTexture.dwSize = sizeof(srcTexture);
            srcTexture.dwWidth = nMipWidth;
            srcTexture.dwHeight = nMipHeight;
            srcTexture.dwPitch = 0;
            srcTexture.nBlockWidth = MipSetIn->m_nBlockWidth;
            srcTexture.nBlockHeight = MipSetIn->m_nBlockHeight;
            srcTexture.nBlockDepth = MipSetIn->m_nBlockDepth;
            srcTexture.format = MipSetIn->m_format;
            srcTexture.dwDataSize = CMP_CalculateBufferSize(&srcTexture);
            srcTexture.pData = pMipData;

            //-----------------------------
            // Uncompressed Destination
            //-----------------------------
            CMP_Texture destTexture;
            destTexture.dwSize = sizeof(destTexture);
            destTexture.dwWidth = nMipWidth;
            destTexture.dwHeight = nMipHeight;
            destTexture.dwPitch = 0;
            destTexture.nBlockWidth = MipSetOut->m_nBlockWidth;
            destTexture.nBlockHeight = MipSetOut->m_nBlockHeight;
            destTexture.nBlockDepth = MipSetOut->m_nBlockDepth;
            destTexture.format = MipSetOut->m_format;
            destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
            destTexture.pData = m_CMIPS.GetMipLevel(MipSetOut, nMipLevel, nFaceOrSlice)->m_pbData;

            if (!silent)
            {
                if ((nMipLevel > 1) || (nFaceOrSlice > 1))
                    PrintInfo("\rProcessing destination MipLevel %2d FaceOrSlice %2d", nMipLevel + 1, nFaceOrSlice);
                else
                    PrintInfo("\rProcessing destination    ");
            }

            try
            {
#ifdef _WIN32
                if ((IsBadReadPtr(srcTexture.pData, srcTexture.dwDataSize)))
                {
                    configSetting->errMessage = "Memory Error(2): Source image cannot be accessed.";
                    PrintInfo("Memory Error(2): Source image\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }

                if (/*(srcTexture.dwDataSize > destTexture.dwDataSize) ||*/ (IsBadWritePtr(destTexture.pData, destTexture.dwDataSize)))
                {
                    configSetting->errMessage = "Memory Error(2): Destination image must be compatible with source.";
                    PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }

#else
                int nullfd = open("/dev/random", O_WRONLY);
                if (write(nullfd, srcTexture.pData, srcTexture.dwDataSize) < 0)
                {
                    configSetting->errMessage = "Memory Error(2): Source image cannot be accessed.";
                    PrintInfo("Memory Error(2): Source image\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }
                close(nullfd);
                nullfd = open("/dev/random", O_WRONLY);
                if (write(nullfd, destTexture.pData, destTexture.dwDataSize) < 0)
                {
                    configSetting->errMessage = "Memory Error(2): Destination image must be compatible with source.";
                    PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }
                close(nullfd);
#endif

                // Return values of the CMP_ calls should be checked for failures
                CMP_ERROR res;
                if (configSetting->useCPU)
                {
                    res = CMP_ConvertTexture(&srcTexture, &destTexture, &CompressOptions, pFeedbackProc, NULL, NULL);
                    if (res != CMP_OK)
                    {
                        configSetting->errMessage = "Compress Failed with Error " + res;
                        PrintInfo("Compress Failed with Error %d\n", res);
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }
                }
                else
                {
#ifdef _WIN32
#ifndef DISABLE_TESTCODE
                    CMP_ERROR res;
                    res = CMP_DecompressTexture(&srcTexture, &destTexture, decodeWith);
                    if (res == CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE)
                    {
                        configSetting->errMessage = "Error: ASTC compressed texture is not supported by the GPU device.\n";
                        PrintInfo("Error: ASTC compressed texture is not supported by the GPU device.\n");
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }
                    else if (res == CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB)
                    {
                        configSetting->errMessage = "Error: Failed to decompress with the API selected. Version is not supported yet. Stay tune for update!\n";
                        PrintInfo("Error: Failed to decompress with the API selected. Note for Vulkan, only driver version up to 1.5.0 is supported by this app. Please stay tune for update! Thanks.\n");
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }
                    else if (res != CMP_OK)
                    {
                        configSetting->errMessage = "Decompress Failed. Texture format not supported. Please view the compressed images using other options (CPU) (under Settings->Application Options).";
                        PrintInfo("Decompress Failed with Error %d\n", res);
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }
#endif
#else
                    PrintInfo("GPU Decompress is not supported in linux yet.\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
#endif
                }


            }
            catch (std::exception& e)
            {
                PrintInfo(e.what());
                m_CMIPS.FreeMipSet(MipSetOut);
                delete MipSetOut;
                MipSetOut = NULL;
                return NULL;
            }

            pMipData += srcTexture.dwDataSize;

            nMipWidth = (nMipWidth>1) ? (nMipWidth >> 1) : 1;
            nMipHeight = (nMipHeight>1) ? (nMipHeight >> 1) : 1;
        }
    }

    MipSetOut->m_nMipLevels = MipSetIn->m_nMipLevels;

    return MipSetOut;
}

void swap_Bytes(CMP_BYTE *src, int width, int height, int offset)
{
    int  i, j;
    CMP_BYTE b;

    for (i = 0; i<height; i++) {
        for (j = 0; j<width; j++) {
            b = *src;         // hold 1st byte
            *src = *(src + 2);     // move 1st to offsetrd 
            *(src + 2) = b;            // save offset to 1st 
            src = src + offset;   // move to next set of bytes
        }
    }

}

void SwizzleMipMap(MipSet *pMipSet)
{
    CMP_DWORD dwWidth;
    CMP_DWORD dwHeight;
    CMP_BYTE     *pData;

    for (int nMipLevel = 0; nMipLevel<pMipSet->m_nMipLevels; nMipLevel++)
    {
        for (int nFaceOrSlice = 0; nFaceOrSlice<MaxFacesOrSlices(pMipSet, nMipLevel); nFaceOrSlice++)
        {
            //=====================
            // Uncompressed source
            //======================
            MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(pMipSet, nMipLevel, nFaceOrSlice);
            dwWidth = pInMipLevel->m_nWidth;
            dwHeight = pInMipLevel->m_nHeight;
            pData = pInMipLevel->m_pbData;

            // Swizzle to RGBA format when compressing from uncompressed DDS file! This is a Patch for now.
            // may want to try this patch on other file types BMP & PNG to move swizzle out to main code.
            switch (pMipSet->m_TextureDataType)
            {
            case TDT_ARGB: swap_Bytes(pInMipLevel->m_pbData, dwWidth, dwHeight, 4); break;
            case TDT_XRGB: swap_Bytes(pInMipLevel->m_pbData, dwWidth, dwHeight, 3); break;
            default: break;
            }
        }
    }

}

// Determine if RGB channel to BGA can be done or skipped
// for special cases of compressed formats.

bool KeepSwizzle(CMP_FORMAT destformat)
{
    // determin of the swizzle flag needs to be turned on!
    switch (destformat)
    {
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:        // same as BC4    
    case CMP_FORMAT_ATI2N:        // same as BC4    
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N_XY:    // same as BC5    
    case CMP_FORMAT_ATI2N_DXT5:    // same as BC5    
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:        // same as BC1     
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:        // same as BC2     
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:        // same as BC3     
    case CMP_FORMAT_ATC_RGB:
    case CMP_FORMAT_ATC_RGBA_Explicit:
    case CMP_FORMAT_ATC_RGBA_Interpolated:
        return true;
        break;
    default:
        break;
    }

    return false;
}

int AMDLoadMIPSTextureImage(const char *SourceFile, MipSet *MipSetIn, bool use_OCV, void *pluginManager)
{ 
    if (pluginManager == NULL)
        return -1;

    string file_extension  = boost::filesystem::extension(SourceFile);
    boost::algorithm::to_upper(file_extension); 
    boost::erase_all(file_extension,".");

    PluginInterface_Image *plugin_Image;

    PluginManager* plugin_Manager = (PluginManager*)pluginManager;
    if (use_OCV)
    {
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(plugin_Manager->GetPlugin("IMAGE","OCV"));
    }
    else
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(plugin_Manager->GetPlugin("IMAGE",(char *)file_extension.c_str()));

    // do the load
    if (plugin_Image)
    {
        plugin_Image->TC_PluginSetSharedIO(&g_CMIPS);

        if (plugin_Image->TC_PluginFileLoadTexture(SourceFile, MipSetIn) != 0)
        {
                // Process Error
                delete plugin_Image;
                plugin_Image = NULL;
                return -1;
        }

        delete plugin_Image;
        plugin_Image = NULL;
    }
    else 
    {
#ifdef USE_QT_IMAGELOAD
        // Failed to load using a AMD Plugin 
        // Try Qt based
        int result = -1;
        QImage *qimage;
        qimage = new QImage(SourceFile);

        if (qimage)
        {
            result = QImage2MIPS(qimage, g_CMIPS, MipSetIn);
            delete qimage;
            qimage = NULL;
        }

        return result;
#else
        return -1;
#endif

    }

    return 0;
}

int AMDSaveMIPSTextureImage(const char * DestFile, MipSet *MipSetIn, bool use_OCV)
{
    bool filesaved = false;
    CMIPS m_CMIPS;
    string file_extension  = boost::filesystem::extension(DestFile);
    boost::algorithm::to_upper(file_extension); 
    boost::erase_all(file_extension,".");

    PluginInterface_Image *plugin_Image;

    if (use_OCV)
    {
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(g_pluginManager.GetPlugin("IMAGE","OCV"));
    }
    else
        plugin_Image = reinterpret_cast<PluginInterface_Image *>(g_pluginManager.GetPlugin("IMAGE",(char *)file_extension.c_str()));

    if (plugin_Image)
    {
        plugin_Image->TC_PluginSetSharedIO(&m_CMIPS);

        bool holdswizzle = MipSetIn->m_swizzle;

        if (file_extension.compare("TGA") == 0)
        {
            // Special case Patch for TGA plugin
            // to be fixed in V2.5 release
            MipSetIn->m_swizzle = false;
            switch (MipSetIn->m_isDeCompressed)
            {
            case CMP_FORMAT_ASTC:
            case CMP_FORMAT_BC7:
            case CMP_FORMAT_BC6H:
            case CMP_FORMAT_BC6H_SF:
            case CMP_FORMAT_ETC_RGB:
            case CMP_FORMAT_ETC2_RGB:
                MipSetIn->m_swizzle = true;
                break;
            }
        }

        if (plugin_Image->TC_PluginFileSaveTexture(DestFile, MipSetIn) == 0)
        {
            filesaved = true;
        }

        MipSetIn->m_swizzle = holdswizzle;

        delete plugin_Image;
        plugin_Image = NULL;
    }


#ifdef USE_QT_IMAGELOAD
    if (!filesaved)
    {
        // Try Qt based filesave!
        QImage *qimage = MIPS2QImage(&m_CMIPS, MipSetIn, 0);

        if (qimage)
        {
            if (!qimage->save(DestFile))
            {
                delete qimage;
                qimage = NULL;
                return(-1);
            }
            delete qimage;
            qimage = NULL;
        }
        else
            return -1;
    }
#else
    if (!filesaved)
    {
        return -1;
    }
#endif

    return 0;
}


bool FormatSupportsQualitySetting(CMP_FORMAT format)
{
    return CompressedFormat(format);
}

bool FormatSupportsDXTCBase(CMP_FORMAT format)
{
    switch (format)
    {
    case  CMP_FORMAT_ATI1N                :
    case  CMP_FORMAT_ATI2N                :
    case  CMP_FORMAT_ATI2N_XY             :
    case  CMP_FORMAT_ATI2N_DXT5           :
    case  CMP_FORMAT_BC1                  :
    case  CMP_FORMAT_BC2                  :
    case  CMP_FORMAT_BC3                  :
    case  CMP_FORMAT_BC4                  :
    case  CMP_FORMAT_BC5                  :
    case  CMP_FORMAT_BC6H                 :
    case  CMP_FORMAT_BC6H_SF              :
    case  CMP_FORMAT_BC7                  :
    case  CMP_FORMAT_DXT1                 :
    case  CMP_FORMAT_DXT3                 :
    case  CMP_FORMAT_DXT5                 :
    case  CMP_FORMAT_DXT5_xGBR            :
    case  CMP_FORMAT_DXT5_RxBG            :
    case  CMP_FORMAT_DXT5_RBxG            :
    case  CMP_FORMAT_DXT5_xRBG            :
    case  CMP_FORMAT_DXT5_RGxB            :
    case  CMP_FORMAT_DXT5_xGxR            :
            return (true);
    break;
    default:
            break;
    }
    return false;
}





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

#include "stdafx.h"
#include <stdio.h>
#include "cKTX.h"

#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "MIPS.h"

#include "softfloat.h"

CMIPS *KTX_CMips;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_KTX)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("KTX")
#else
void *make_Plugin_KTX() { return new Plugin_KTX; } 
#endif

uint32_t Endian_Conversion(uint32_t dword)
{
	return (((dword>>24)&0x000000FF) | ((dword>>8)&0x0000FF00) | ((dword<<8)&0x00FF0000) | ((dword<<24)&0xFF000000));
}


Plugin_KTX::Plugin_KTX()
{ 
	//MessageBox(0,"Plugin_TGA","Plugin_KTX",MB_OK);  
}

Plugin_KTX::~Plugin_KTX()
{ 
	//MessageBox(0,"Plugin_TGA","~Plugin_KTX",MB_OK);  
}

int Plugin_KTX::TC_PluginSetSharedIO(void* Shared)
{
	if (Shared)
	{
		KTX_CMips = static_cast<CMIPS *>(Shared);
		return 0;
	}
	return 1;
}


int Plugin_KTX::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
	//MessageBox(0,"TC_PluginGetVersion","Plugin_KTX",MB_OK);  
	pPluginVersion->guid					= g_GUID;
	pPluginVersion->dwAPIVersionMajor		= TC_API_VERSION_MAJOR;
	pPluginVersion->dwAPIVersionMinor		= TC_API_VERSION_MINOR;
	pPluginVersion->dwPluginVersionMajor	= TC_PLUGIN_VERSION_MAJOR;
	pPluginVersion->dwPluginVersionMinor	= TC_PLUGIN_VERSION_MINOR;
	return 0;
}

int Plugin_KTX::TC_PluginFileLoadTexture(const TCHAR* pszFilename, CMP_Texture *srcTexture)
{
	//MessageBox(0,"TC_PluginFileLoadTexture srcTexture","Plugin_KTX",MB_OK);  
	return 0;
}

int Plugin_KTX::TC_PluginFileSaveTexture(const TCHAR* pszFilename, CMP_Texture *srcTexture)
{
	//MessageBox(0,"TC_PluginFileSaveTexture srcTexture","Plugin_KTX",MB_OK);  
	return 0;
}


uint32_t ktx_u32_byterev(uint32_t v)
{
	return (v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24);
}

// perform endianness switch on raw data
static void switch_endianness2(void *dataptr, int bytes)
{
	int i;
	uint8_t *data = (uint8_t *) dataptr;

	for (i = 0; i < bytes / 2; i++)
	{
		uint8_t d0 = data[0];
		uint8_t d1 = data[1];
		data[0] = d1;
		data[1] = d0;
		data += 2;
	}
}

static void switch_endianness4(void *dataptr, int bytes)
{
	int i;
	uint8_t *data = (uint8_t *) dataptr;

	for (i = 0; i < bytes / 4; i++)
	{
		uint8_t d0 = data[0];
		uint8_t d1 = data[1];
		uint8_t d2 = data[2];
		uint8_t d3 = data[3];
		data[0] = d3;
		data[1] = d2;
		data[2] = d1;
		data[3] = d0;
		data += 4;
	}
}

static void copy_scanline(void *dst, const void *src, int pixels, int method)
{

#define id(x) (x)
#define u16_sf16(x) float_to_sf16( x * (1.0f/65535.0f), SF_NEARESTEVEN )
#define f32_sf16(x) sf32_to_sf16( x, SF_NEARESTEVEN )

#define COPY_R( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[i]); \
		d[4*i+1] = 0; \
		d[4*i+2] = 0; \
		d[4*i+3] = oneval; \
		} \
	} while(0); \
	break;

#define COPY_RG( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[2*i]); \
		d[4*i+1] = convfunc(s[2*i+1]); \
		d[4*i+2] = 0; \
		d[4*i+3] = oneval; \
		} \
	} while(0); \
	break;

#define COPY_RGB( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[3*i]); \
		d[4*i+1] = convfunc(s[3*i+1]); \
		d[4*i+2] = convfunc(s[3*i+2]); \
		d[4*i+3] = oneval; \
		} \
	} while(0); \
	break;

#define COPY_BGR( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[3*i+2]); \
		d[4*i+1] = convfunc(s[3*i+1]); \
		d[4*i+2] = convfunc(s[3*i]); \
		d[4*i+3] = oneval; \
		} \
	} while(0); \
	break;

#define COPY_RGBX( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[4*i]); \
		d[4*i+1] = convfunc(s[4*i+1]); \
		d[4*i+2] = convfunc(s[4*i+2]); \
		d[4*i+3] = oneval; \
		} \
	} while(0); \
	break;

#define COPY_BGRX( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[4*i+2]); \
		d[4*i+1] = convfunc(s[4*i+1]); \
		d[4*i+2] = convfunc(s[4*i]); \
		d[4*i+3] = oneval; \
		} \
	} while(0); \
	break;

#define COPY_RGBA( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[4*i]); \
		d[4*i+1] = convfunc(s[4*i+1]); \
		d[4*i+2] = convfunc(s[4*i+2]); \
		d[4*i+3] = convfunc(s[4*i+3]); \
		} \
	} while(0); \
	break;

#define COPY_BGRA( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[4*i+2]); \
		d[4*i+1] = convfunc(s[4*i+1]); \
		d[4*i+2] = convfunc(s[4*i]); \
		d[4*i+3] = convfunc(s[4*i+3]); \
		} \
	} while(0); \
	break;

#define COPY_L( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[i]); \
		d[4*i+1] = convfunc(s[i]); \
		d[4*i+2] = convfunc(s[i]); \
		d[4*i+3] = oneval; \
		} \
	} while(0); \
	break;

#define COPY_LA( dsttype, srctype, convfunc, oneval ) \
	do { \
	srctype *s = (srctype *)src; \
	dsttype *d = (dsttype *)dst; \
	for(i=0;i<pixels;i++)\
		{\
		d[4*i] = convfunc(s[2*i]); \
		d[4*i+1] = convfunc(s[2*i]); \
		d[4*i+2] = convfunc(s[2*i]); \
		d[4*i+3] = convfunc(s[2*i+1]); \
		} \
	} while(0); \
	break;

	int i;
	switch (method)
	{
	case R8_TO_RGBA8:
		COPY_R(uint8_t, uint8_t, id, 0xFF);
	case RG8_TO_RGBA8:
		COPY_RG(uint8_t, uint8_t, id, 0xFF);
	case RGB8_TO_RGBA8:
		COPY_RGB(uint8_t, uint8_t, id, 0xFF);
	case RGBA8_TO_RGBA8:
		COPY_RGBA(uint8_t, uint8_t, id, 0xFF);
	case BGR8_TO_RGBA8:
		COPY_BGR(uint8_t, uint8_t, id, 0xFF);
	case BGRA8_TO_RGBA8:
		COPY_BGRA(uint8_t, uint8_t, id, 0xFF);
	case RGBX8_TO_RGBA8:
		COPY_RGBX(uint8_t, uint8_t, id, 0xFF);
	case BGRX8_TO_RGBA8:
		COPY_BGRX(uint8_t, uint8_t, id, 0xFF);
	case L8_TO_RGBA8:
		COPY_L(uint8_t, uint8_t, id, 0xFF);
	case LA8_TO_RGBA8:
		COPY_LA(uint8_t, uint8_t, id, 0xFF);

	case R16F_TO_RGBA16F:
		COPY_R(uint16_t, uint16_t, id, 0x3C00);
	case RG16F_TO_RGBA16F:
		COPY_RG(uint16_t, uint16_t, id, 0x3C00);
	case RGB16F_TO_RGBA16F:
		COPY_RGB(uint16_t, uint16_t, id, 0x3C00);
	case RGBA16F_TO_RGBA16F:
		COPY_RGBA(uint16_t, uint16_t, id, 0x3C00);
	case BGR16F_TO_RGBA16F:
		COPY_BGR(uint16_t, uint16_t, id, 0x3C00);
	case BGRA16F_TO_RGBA16F:
		COPY_BGRA(uint16_t, uint16_t, id, 0x3C00);
	case L16F_TO_RGBA16F:
		COPY_L(uint16_t, uint16_t, id, 0x3C00);
	case LA16F_TO_RGBA16F:
		COPY_LA(uint16_t, uint16_t, id, 0x3C00);

	case R16_TO_RGBA16F:
		COPY_R(uint16_t, uint16_t, u16_sf16, 0x3C00);
	case RG16_TO_RGBA16F:
		COPY_RG(uint16_t, uint16_t, u16_sf16, 0x3C00);
	case RGB16_TO_RGBA16F:
		COPY_RGB(uint16_t, uint16_t, u16_sf16, 0x3C00);
	case RGBA16_TO_RGBA16F:
		COPY_RGBA(uint16_t, uint16_t, u16_sf16, 0x3C00);
	case BGR16_TO_RGBA16F:
		COPY_BGR(uint16_t, uint16_t, u16_sf16, 0x3C00);
	case BGRA16_TO_RGBA16F:
		COPY_BGRA(uint16_t, uint16_t, u16_sf16, 0x3C00);
	case L16_TO_RGBA16F:
		COPY_L(uint16_t, uint16_t, u16_sf16, 0x3C00);
	case LA16_TO_RGBA16F:
		COPY_LA(uint16_t, uint16_t, u16_sf16, 0x3C00);

	case R32F_TO_RGBA16F:
		COPY_R(uint16_t, uint32_t, f32_sf16, 0x3C00);
	case RG32F_TO_RGBA16F:
		COPY_RG(uint16_t, uint32_t, f32_sf16, 0x3C00);
	case RGB32F_TO_RGBA16F:
		COPY_RGB(uint16_t, uint32_t, f32_sf16, 0x3C00);
	case RGBA32F_TO_RGBA16F:
		COPY_RGBA(uint16_t, uint32_t, f32_sf16, 0x3C00);
	case BGR32F_TO_RGBA16F:
		COPY_BGR(uint16_t, uint32_t, f32_sf16, 0x3C00);
	case BGRA32F_TO_RGBA16F:
		COPY_BGRA(uint16_t, uint32_t, f32_sf16, 0x3C00);
	case L32F_TO_RGBA16F:
		COPY_L(uint16_t, uint32_t, f32_sf16, 0x3C00);
	case LA32F_TO_RGBA16F:
		COPY_LA(uint16_t, uint32_t, f32_sf16, 0x3C00);
	};
}


int Plugin_KTX::TC_PluginFileLoadTexture(const TCHAR* pszFilename, MipSet* pMipSet)
{
    FILE* pFile = NULL;
    if (_tfopen_s(&pFile, pszFilename, _T("rb")) != 0 || pFile == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) opening file = %s \n"), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

//using libktx
    KTX_header fheader;
    KTX_texinfo texinfo;
    if (fread(&fheader, sizeof(KTX_header), 1, pFile) != 1)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) invalid KTX header. Filename = %s \n"), EL_Error, IDS_ERROR_NOT_KTX, pszFilename);
        fclose(pFile);
        return -1;
    }

    if (_ktxCheckHeader(&fheader, &texinfo) != KTX_SUCCESS)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) invalid KTX header. Filename = %s \n"), EL_Error, IDS_ERROR_NOT_KTX, pszFilename);
        fclose(pFile);
        return -1;
    }

    if (texinfo.compressed)
    {
        pMipSet->m_compressed = true;
        switch (fheader.glInternalFormat)
        {
        //case GL_RED_EXT:
        //    break;
        //case GL_RG_EXT:
        //    break;
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            pMipSet->m_format = CMP_FORMAT_BC1;
            pMipSet->m_TextureDataType = TDT_XRGB;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            pMipSet->m_format = CMP_FORMAT_BC1;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
            pMipSet->m_format = CMP_FORMAT_BC2;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
            pMipSet->m_format = CMP_FORMAT_BC3;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case RGB_BP_UNorm:
            pMipSet->m_format = CMP_FORMAT_BC7;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case R_ATI1N_UNorm:
            pMipSet->m_format = CMP_FORMAT_ATI1N;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case R_ATI1N_SNorm:
            pMipSet->m_format = CMP_FORMAT_ATI2N;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case RG_ATI2N_UNorm:
            pMipSet->m_format = CMP_FORMAT_ATI2N_XY;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case RG_ATI2N_SNorm:
            pMipSet->m_format = CMP_FORMAT_ATI2N_DXT5;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case RGB_BP_UNSIGNED_FLOAT:
            pMipSet->m_format = CMP_FORMAT_BC6H;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case RGB_BP_SIGNED_FLOAT:
            pMipSet->m_format = CMP_FORMAT_BC6H;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case ATC_RGB_AMD:
            pMipSet->m_format = CMP_FORMAT_ATC_RGB;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case ATC_RGBA_EXPLICIT_ALPHA_AMD:
            pMipSet->m_format = CMP_FORMAT_ATC_RGBA_Explicit;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case ATC_RGBA_INTERPOLATED_ALPHA_AMD: 
            pMipSet->m_format = CMP_FORMAT_ATC_RGBA_Interpolated;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_RGBA_ASTC_4x4_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case ETC1_RGB8_OES: 
            pMipSet->m_format = CMP_FORMAT_ETC_RGB;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_RxBG :
            pMipSet->m_format = CMP_FORMAT_DXT5_RxBG;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_RBxG :
            pMipSet->m_format = CMP_FORMAT_DXT5_RBxG;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_xRBG :
            pMipSet->m_format = CMP_FORMAT_DXT5_xRBG;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_RGxB :
            pMipSet->m_format = CMP_FORMAT_DXT5_RGxB;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_xGxR :
            pMipSet->m_format = CMP_FORMAT_DXT5_xGxR;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
       	default:
       		if (KTX_CMips)
       			KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) unsupported GL format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
       	   fclose(pFile);
       	   return -1;
        }
    }
    else
    {
        pMipSet->m_compressed = false;
        switch (fheader.glType) 
        {
        case GL_UNSIGNED_BYTE:
            pMipSet->m_format = CMP_FORMAT_ARGB_8888;
            pMipSet->m_TextureDataType = TDT_ARGB;
            pMipSet->m_ChannelFormat = CF_8bit;
            break;
        case GL_UNSIGNED_INT_2_10_10_10_REV_EXT:
            pMipSet->m_format = CMP_FORMAT_ARGB_2101010;
            pMipSet->m_TextureDataType = TDT_ARGB;
            pMipSet->m_ChannelFormat = CF_2101010;
            break;
        case GL_HALF_FLOAT_OES:
            pMipSet->m_format = CMP_FORMAT_ARGB_32F;
            pMipSet->m_TextureDataType = TDT_ARGB;
            pMipSet->m_ChannelFormat = CF_Float32;
            break;
        default:
            if (KTX_CMips)
                KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) unsupported GL format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
            fclose(pFile);
            return -1;
        }
    }

    // Allocate MipSet header
    KTX_CMips->AllocateMipSet(pMipSet,
        pMipSet->m_ChannelFormat,
        pMipSet->m_TextureDataType,
        pMipSet->m_TextureType,
        fheader.pixelWidth,
        fheader.pixelHeight,
        1);

    // Determine buffer size and set Mip Set Levels 
    MipLevel *mipLevel = KTX_CMips->GetMipLevel(pMipSet, 0);
    pMipSet->m_nMipLevels = 1;
    KTX_CMips->AllocateMipLevelData(mipLevel, pMipSet->m_nWidth, pMipSet->m_nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType);

    // We have allocated a data buffer to fill get its referance
    BYTE* pData = (BYTE*)(mipLevel->m_pbData);
    
    fclose(pFile);
    pFile = NULL;
    pFile = fopen(pszFilename, "rb");
    if (pFile == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) loading file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    //skip key value data
    int imageSizeOffset = sizeof(KTX_header) + fheader.bytesOfKeyValueData;
    if (fseek(pFile, imageSizeOffset, SEEK_SET))
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) Seek past key/vals in KTX compressed bitmap file failed. Format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
        fclose(pFile);
        return -1;
    }

    //load image size
    UINT imageByteCount = 0;
    if (fread((void*)&imageByteCount, 1, 4, pFile) != 4)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) Read image size failed. Format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
        fclose(pFile);
        return -1;
    }

    //read image data
    const UINT bytesRead = fread(pData, 1, imageByteCount, pFile);
    if (bytesRead != imageByteCount)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) Read image data failed. Format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
        fclose(pFile);
        return -1;
    }

//without using libktx	
// Read the header

//	int components;
//	ktx_header header;
//
//	if (fread(&header, sizeof(ktx_header), 1, pFile) != 1)
//	{
//	   if (KTX_CMips)
//				KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) invalid KTX header. Filename = %s \n"), EL_Error, IDS_ERROR_NOT_KTX, pszFilename);
//	   fclose(pFile);
//	   return -1;
//	}
//
//	// ckeck the header
//	if (memcmp((uint8_t *) header.identifier, FileIdentifier, 12) != 0)
//	{
//	   if (KTX_CMips)
//				KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) invalid KTX identifier. Filename = %s \n"), EL_Error, IDS_ERROR_NOT_KTX, pszFilename);
//	   fclose(pFile);
//	   return -1;
//	}
//
//	if ((header.endianness != 0x04030201) && (header.endianness != 0x01020304))
//	{
//	   if (KTX_CMips)
//				KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) invalid KTX endianness. Filename = %s \n"), EL_Error, IDS_ERROR_NOT_KTX, pszFilename);
//	   fclose(pFile);
//	   return -1;
//	}
//
//	int switch_endianness = 0;
//
//	if (header.endianness == 0x01020304)
//	{
//		// reverse all header values for endness after the flag
//		header.gl_Type					= Endian_Conversion(header.gl_Type);				
//		header.gl_TypeSize				= Endian_Conversion(header.gl_TypeSize);			
//		header.gl_Format				= Endian_Conversion(header.gl_Format);			
//		header.gl_InternalFormat		= Endian_Conversion(header.gl_InternalFormat);	
//		header.gl_BaseInternalFormat	= Endian_Conversion(header.gl_BaseInternalFormat);
//		header.pixelWidth				= Endian_Conversion(header.pixelWidth);			
//		header.pixelHeight				= Endian_Conversion(header.pixelHeight);			
//		header.pixelDepth				= Endian_Conversion(header.pixelDepth);			
//		header.numberOfArrayElements	= Endian_Conversion(header.numberOfArrayElements);
//		header.numberOfFaces			= Endian_Conversion(header.numberOfFaces);		
//		header.numberOfMipmapLevels		= Endian_Conversion(header.numberOfMipmapLevels);	
//		header.bytesOfKeyValueData		= Endian_Conversion(header.bytesOfKeyValueData);	
//		switch_endianness				= 1;
//	}
//
//	// Skip Key value pairs
//	fseek(pFile, header.bytesOfKeyValueData, SEEK_CUR);
//
//
//	// the formats we support are:
//	// cartesian product of gl_type=(UNSIGNED_BYTE, UNSIGNED_SHORT, HALF_FLOAT, FLOAT) x gl_format=(RED, RG, RGB, RGBA, BGR, BGRA)
//	switch (header.gl_Format)
//	{
//	case GL_RED_EXT:
//		components = 1;
//		break;
//	case GL_RG_EXT:
//		components = 2;
//		break;
//	case GL_RGB:
//		components = 3;
//		break;
//	case GL_RGBA:
//		components = 4;
//		break;
////	case GL_BGR:
////		components = 3;
////		break;
//	case GL_BGRA_EXT:
//		components = 4;
//		break;
//	case GL_LUMINANCE:
//		components = 1;
//		break;
//	case GL_LUMINANCE_ALPHA:
//		components = 2;
//		break;
//	default:
//		if (KTX_CMips)
//			KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) unsupported GL format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, header.gl_Format);
//	   fclose(pFile);
//	   return -1;
//	};
//
//
//	// Although these are set up later, we include a default initializer to remove warnings
//
//	int bytes_per_component			= 1;	// bytes per component in the KTX file.
//	ChannelFormat channelformat		= CF_8bit;	
//	scanline_copy_method cm			= R8_TO_RGBA8;
//
//	switch (header.gl_Type)
//	{
//	case GL_UNSIGNED_BYTE:
//		{
//			bytes_per_component = 1;
//			switch (header.gl_Format)
//			{
//			case GL_RED_EXT:
//				cm = R8_TO_RGBA8;
//				break;
//			case GL_RG_EXT:
//				cm = RG8_TO_RGBA8;
//				break;
//			case GL_RGB:
//				cm = RGB8_TO_RGBA8;
//				break;
//			case GL_RGBA:
//				cm = RGBA8_TO_RGBA8;
//				break;
//		//	case GL_BGR:
//		//		cm = BGR8_TO_RGBA8;
//		//		break;
//			case GL_BGRA_EXT:
//				cm = BGRA8_TO_RGBA8;
//				break;
//			case GL_LUMINANCE:
//				cm = L8_TO_RGBA8;
//				break;
//			case GL_LUMINANCE_ALPHA:
//				cm = LA8_TO_RGBA8;
//				break;
//			}
//			break;
//		}
//	case GL_UNSIGNED_SHORT:
//		{
//			channelformat = CF_16bit;
//			bytes_per_component = 2;
//			switch (header.gl_Format)
//			{
//			case GL_RED_EXT:
//				cm = R16_TO_RGBA16F;
//				break;
//			case GL_RG_EXT:
//				cm = RG16_TO_RGBA16F;
//				break;
//			case GL_RGB:
//				cm = RGB16_TO_RGBA16F;
//				break;
//			case GL_RGBA:
//				cm = RGBA16_TO_RGBA16F;
//				break;
//		//	case GL_BGR:
//		//		cm = BGR16_TO_RGBA16F;
//		//		break;
//			case GL_BGRA_EXT:
//				cm = BGRA16_TO_RGBA16F;
//				break;
//			case GL_LUMINANCE:
//				cm = L16_TO_RGBA16F;
//				break;
//			case GL_LUMINANCE_ALPHA:
//				cm = LA16_TO_RGBA16F;
//				break;
//			}
//			break;
//		}
//	case GL_HALF_FLOAT_OES:
//		{
//			channelformat = CF_16bit;
//			bytes_per_component = 2;
//			switch (header.gl_Format)
//			{
//			case GL_RED_EXT:
//				cm = R16F_TO_RGBA16F;
//				break;
//			case GL_RG_EXT:
//				cm = RG16F_TO_RGBA16F;
//				break;
//			case GL_RGB:
//				cm = RGB16F_TO_RGBA16F;
//				break;
//			case GL_RGBA:
//				cm = RGBA16F_TO_RGBA16F;
//				break;
////			case GL_BGR:
////				cm = BGR16F_TO_RGBA16F;
////				break;
//			case GL_BGRA_EXT:
//				cm = BGRA16F_TO_RGBA16F;
//				break;
//			case GL_LUMINANCE:
//				cm = L16F_TO_RGBA16F;
//				break;
//			case GL_LUMINANCE_ALPHA:
//				cm = LA16F_TO_RGBA16F;
//				break;
//			}
//			break;
//		}
//	case GL_FLOAT:
//		{
//			channelformat = CF_16bit;
//			bytes_per_component = 4;
//			switch (header.gl_Format)
//			{
//			case GL_RED_EXT:
//				cm = R32F_TO_RGBA16F;
//				break;
//			case GL_RG_EXT:
//				cm = RG32F_TO_RGBA16F;
//				break;
//			case GL_RGB:
//				cm = RGB32F_TO_RGBA16F;
//				break;
//			case GL_RGBA:
//				cm = RGBA32F_TO_RGBA16F;
//				break;
//		//	case GL_BGR:
//		//		cm = BGR32F_TO_RGBA16F;
//		//		break;
//			case GL_BGRA_EXT:
//				cm = BGRA32F_TO_RGBA16F;
//				break;
//			case GL_LUMINANCE:
//				cm = L32F_TO_RGBA16F;
//				break;
//			case GL_LUMINANCE_ALPHA:
//				cm = LA32F_TO_RGBA16F;
//				break;
//			}
//			break;
//		}
//	default:
//		if (KTX_CMips)
//			KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) unsupported GL Type %x"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, header.gl_Type);
//	   fclose(pFile);
//	   return -1;
//	};
//	
//
//	int xsize = header.pixelWidth;
//	int ysize = header.pixelHeight;
//	int zsize = header.pixelDepth;
//
//	if (ysize == 0)
//		ysize = 1;
//	if (zsize == 0)
//		zsize = 1;
//
//	uint32_t specified_bytes_of_surface = 0;
//	size_t sb_read = fread(&specified_bytes_of_surface, 1, 4, pFile);
//	if (sb_read != 4)
//	{
//		if (KTX_CMips)
//			KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) file read error %d"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, sb_read);
//	   fclose(pFile);
//	   return -1;
//	}
//
//	if (switch_endianness)
//		specified_bytes_of_surface = ktx_u32_byterev(specified_bytes_of_surface);
//
//	// read the surface
//	uint32_t xstride = bytes_per_component * components * xsize;
//	uint32_t ystride = xstride * ysize;
//	uint32_t computed_bytes_of_surface = zsize * ystride;
//	if (computed_bytes_of_surface != specified_bytes_of_surface)
//	{
//		if (KTX_CMips)
//			KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) surface bytes %d != computed %d"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, specified_bytes_of_surface, computed_bytes_of_surface);
//	    fclose(pFile);
//	    return -1;
//	}
//
//
//	uint8_t *buf = (uint8_t *) malloc(specified_bytes_of_surface);
//	size_t bytes_read = fread(buf, 1, specified_bytes_of_surface, pFile);
//	fclose(pFile);
//
//	if (bytes_read != specified_bytes_of_surface)
//	{
//		free(buf);
//		if (KTX_CMips)
//			KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) Bytes read %d != specified bytes %d"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, bytes_read, specified_bytes_of_surface);
//	    fclose(pFile);
//	    return -1;
//	}
//
//	// perform an endianness swap on the surface if needed.
//	if (switch_endianness)
//	{
//		if (header.gl_TypeSize == 2)
//			switch_endianness2(buf, specified_bytes_of_surface);
//		if (header.gl_TypeSize == 4)
//			switch_endianness4(buf, specified_bytes_of_surface);
//	}
//
//
//	if(!KTX_CMips->AllocateMipSet(pMipSet, channelformat, TDT_ARGB, TT_2D, header.pixelWidth, header.pixelHeight, 1))
//	{
//		if (KTX_CMips)
//			KTX_CMips->PrintError("Error(0): KTX Plugin ID(5)\n");
//		return PE_Unknown;
//	}
//
//	// Allocate the permanent buffer and unpack the bitmap data into it
//	if(!KTX_CMips->AllocateMipLevelData(KTX_CMips->GetMipLevel(pMipSet, 0), header.pixelWidth, header.pixelHeight, channelformat, pMipSet->m_TextureDataType))
//	{
//		if (KTX_CMips)
//			KTX_CMips->PrintError("Error(0): KTX Plugin ID(6)\n");
//		return PE_Unknown;
//		}
//
//
//	pMipSet->m_dwFourCC		= 0;
//	pMipSet->m_dwFourCC2	= 0;
//	pMipSet->m_nMipLevels	= 1;
//	pMipSet->m_format		= CMP_FORMAT_ARGB_8888;
//
//    BYTE* pData = KTX_CMips->GetMipLevel(pMipSet,0)->m_pbData;
//    ASSERT(pData);
//    if(pData == NULL)
//    {
//      return PE_Unknown;
//    }
//
//	int  destLineSize = xsize * 4;
//	// Note we only support 2D for now so z loops only once
//	// Source is any size format but destination is always RGBA buffer either 8 bit or 16 bit)
//	for (int z = 0; z < 1; z++)
//	{
//		for (int y = 0; y < ysize; y++)
//		{
//			uint8_t *src = buf + (z * ystride) + (y * xstride);
//			copy_scanline(pData, src, xsize, cm);
//			// Set next output buffer location 4 bytes * width of image
//			pData += destLineSize;
//		}
//	}
//
//	free(buf);
	return 0;
}


int Plugin_KTX::TC_PluginFileSaveTexture(const TCHAR* pszFilename, MipSet* pMipSet)
{
    assert(pszFilename);
    assert(pMipSet);

    FILE* pFile = NULL;
    pFile = fopen(pszFilename, "wb");
    if (pFile == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

//using libktx
    KTX_texture_info textureinfo;
    KTX_image_info* inputMip = new KTX_image_info();

    UINT pDataLen = 0;
    BYTE* pData = NULL;
    BOOLEAN isCompressed = false;

    if (pMipSet->m_TextureType == TT_CubeMap)
        textureinfo.numberOfFaces = 6;
    else
        textureinfo.numberOfFaces = 1;

    textureinfo.numberOfArrayElements = 0;
    if (pMipSet->m_pMipLevelTable == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_ALLOCATEMIPSET, pszFilename);
        return -1;
    }

    MipLevel *miplevel = KTX_CMips->GetMipLevel(pMipSet, 0);
    if (miplevel == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_ALLOCATEMIPSET, pszFilename);
        return -1;
    }

    //int len = strlen((char*)KTX_CMips->GetMipLevel(pMipSet, 0)->m_pbData);
    //int siz = sizeof(BYTE*);
    inputMip->data = miplevel->m_pbData;
    int w = miplevel->m_nWidth;
    int h = miplevel->m_nHeight;

    switch (pMipSet->m_format)
    {
    //uncompressed format case
    case CMP_FORMAT_ARGB_8888 :
    case CMP_FORMAT_RGB_888 :
    case CMP_FORMAT_RG_8 :
    case CMP_FORMAT_R_8 :
        isCompressed = false;
        textureinfo.glType = GL_UNSIGNED_BYTE;
        textureinfo.glTypeSize = 1;
        break;
    case  CMP_FORMAT_ARGB_2101010 :      
        textureinfo.glType = GL_UNSIGNED_INT_2_10_10_10_REV_EXT;
        textureinfo.glTypeSize = 1;
        break;
    case  CMP_FORMAT_ARGB_16 :   
        textureinfo.glType = GL_UNSIGNED_SHORT;
        textureinfo.glTypeSize = 2;
        break;
    case  CMP_FORMAT_RG_16 :
        textureinfo.glType = GL_UNSIGNED_SHORT;
        textureinfo.glTypeSize = 2;
        break;
    case  CMP_FORMAT_R_16 :      
        textureinfo.glType = GL_UNSIGNED_SHORT;
        textureinfo.glTypeSize = 2;
        break;
    case  CMP_FORMAT_ARGB_16F :                 
    case  CMP_FORMAT_RG_16F :                   
    case  CMP_FORMAT_R_16F :                    
    case  CMP_FORMAT_ARGB_32F :                 
    case  CMP_FORMAT_RGB_32F :                  
    case  CMP_FORMAT_RG_32F :                   
    case  CMP_FORMAT_R_32F :
        textureinfo.glType = GL_HALF_FLOAT_OES;
        textureinfo.glTypeSize = 1;
        break;
    //compressed format case
    case  CMP_FORMAT_ATI1N :                   
    case  CMP_FORMAT_ATI2N :                   
    case  CMP_FORMAT_ATI2N_XY :                
    case  CMP_FORMAT_ATI2N_DXT5 :              
    case  CMP_FORMAT_ATC_RGB :                 
    case  CMP_FORMAT_ATC_RGBA_Explicit :       
    case  CMP_FORMAT_ATC_RGBA_Interpolated :   
    case  CMP_FORMAT_BC1 :                     
    case  CMP_FORMAT_BC2 :                     
    case  CMP_FORMAT_BC3 :                     
    case  CMP_FORMAT_BC4 :                     
    case  CMP_FORMAT_BC5 :                     
    case  CMP_FORMAT_BC6H :                    
    case  CMP_FORMAT_BC7 :                     
    case  CMP_FORMAT_DXT1 :                    
    case  CMP_FORMAT_DXT3 :                    
    case  CMP_FORMAT_DXT5 :                    
    case  CMP_FORMAT_DXT5_xGBR :               
    case  CMP_FORMAT_DXT5_RxBG :               
    case  CMP_FORMAT_DXT5_RBxG :               
    case  CMP_FORMAT_DXT5_xRBG :               
    case  CMP_FORMAT_DXT5_RGxB :               
    case  CMP_FORMAT_DXT5_xGxR :               
    case  CMP_FORMAT_ETC_RGB :                 
    case  CMP_FORMAT_ASTC :
    case  CMP_FORMAT_GT:
        isCompressed = true;
        textureinfo.glType = 0;
        textureinfo.glTypeSize = 1;
        textureinfo.glFormat = 0;
        break;
    //default case
    default:
        isCompressed = false;
        textureinfo.glType = GL_UNSIGNED_BYTE;
        textureinfo.glTypeSize = 1;
        break;
    }

    switch (pMipSet->m_TextureDataType)
    {
    case TDT_R:  //single component-- can be Luminance and Alpha case, here only cover R
    {
        inputMip->size = KTX_CMips->GetMipLevel(pMipSet, 0)->m_dwLinearSize;
        if (!isCompressed) 
        {
            textureinfo.glFormat = textureinfo.glBaseInternalFormat = GL_RED_EXT;
            textureinfo.glInternalFormat = GL_RED_EXT;
        }
        else
        {
            textureinfo.glBaseInternalFormat = GL_RED_EXT;
            textureinfo.glInternalFormat = GL_RED_EXT;
        }
    }
    break;
    case TDT_RG:  //two component
    {
        inputMip->size = KTX_CMips->GetMipLevel(pMipSet, 0)->m_dwLinearSize ;
        if (!isCompressed)
        {
            textureinfo.glFormat = textureinfo.glBaseInternalFormat = GL_RG_EXT;
            textureinfo.glInternalFormat = GL_RG_EXT; 
        }
        else
        {
            textureinfo.glBaseInternalFormat = GL_RG_EXT;
            textureinfo.glInternalFormat = GL_RG_EXT; //GL_COMPRESSED_RG not found in ANGLE
        }
    }
    break;
    case TDT_XRGB:  //normally 3 component
    {
        inputMip->size = KTX_CMips->GetMipLevel(pMipSet, 0)->m_dwLinearSize;
        if (!isCompressed)
        {
            textureinfo.glFormat = textureinfo.glBaseInternalFormat = GL_RGB;
            textureinfo.glInternalFormat = GL_RGB;
        }
        else
        {
            textureinfo.glBaseInternalFormat = GL_RGB;
            if (pMipSet->m_format == CMP_FORMAT_BC1 || pMipSet->m_format == CMP_FORMAT_DXT1)
                textureinfo.glInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            else
                textureinfo.glInternalFormat = GL_RGB; //other compressed format not found in qtangle header
        }
    }
    break;
    case TDT_ARGB:  //4 component
    {
        inputMip->size = KTX_CMips->GetMipLevel(pMipSet, 0)->m_dwLinearSize;
        if (!isCompressed)
        {
            textureinfo.glFormat = textureinfo.glBaseInternalFormat = GL_RGBA;
            textureinfo.glInternalFormat = GL_RGBA;
        }
        else
        {
            textureinfo.glBaseInternalFormat = GL_RGBA;
            switch (pMipSet->m_format)
            {
            case CMP_FORMAT_BC1:
            case CMP_FORMAT_DXT1:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                break;
            case CMP_FORMAT_BC2:
            case CMP_FORMAT_DXT3:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE;
                break;
            case CMP_FORMAT_BC3:
            case CMP_FORMAT_DXT5:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE;
                break;
            case CMP_FORMAT_BC7:
                textureinfo.glInternalFormat = RGB_BP_UNorm; 
                break;
            case  CMP_FORMAT_ATI1N:
                textureinfo.glInternalFormat = R_ATI1N_UNorm;
                break;
            case  CMP_FORMAT_ATI2N:
                textureinfo.glInternalFormat = R_ATI1N_SNorm;
                break;
            case  CMP_FORMAT_ATI2N_XY:
                textureinfo.glInternalFormat = RG_ATI2N_UNorm;
                break;
            case  CMP_FORMAT_ATI2N_DXT5:
                textureinfo.glInternalFormat = RG_ATI2N_SNorm;
                break;
            case  CMP_FORMAT_ATC_RGB:
                textureinfo.glInternalFormat = ATC_RGB_AMD;
                break;
            case  CMP_FORMAT_ATC_RGBA_Explicit:
                textureinfo.glInternalFormat = ATC_RGBA_EXPLICIT_ALPHA_AMD;
                break;
            case  CMP_FORMAT_ATC_RGBA_Interpolated:
                textureinfo.glInternalFormat = ATC_RGBA_INTERPOLATED_ALPHA_AMD;
                break;
            case  CMP_FORMAT_BC4:
                textureinfo.glInternalFormat = COMPRESSED_RED_RGTC1;
                break;
            case  CMP_FORMAT_BC5:
                textureinfo.glInternalFormat = COMPRESSED_RG_RGTC2;
                break;
            case  CMP_FORMAT_BC6H:
                textureinfo.glInternalFormat = RGB_BP_UNSIGNED_FLOAT;
                break;
            case  CMP_FORMAT_ASTC:
                textureinfo.glInternalFormat = COMPRESSED_RGBA_ASTC_4x4_KHR;
                break;
            case CMP_FORMAT_ETC_RGB:
                textureinfo.glInternalFormat = ETC1_RGB8_OES;
                break;
            case CMP_FORMAT_DXT5_xGBR:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_xGBR;
                break;
            case CMP_FORMAT_DXT5_RxBG:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_RxBG;
                break;
            case CMP_FORMAT_DXT5_RBxG:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_RBxG;
                break;
            case CMP_FORMAT_DXT5_xRBG:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_xRBG;
                break;
            case CMP_FORMAT_DXT5_RGxB:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_RGxB;
                break;
            case CMP_FORMAT_DXT5_xGxR:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_xGxR;
                break;
                
            }
        }
    }
    break;
    default:
        break;
    }
    
    textureinfo.pixelWidth = pMipSet->m_nWidth;
    textureinfo.pixelHeight = pMipSet->m_nHeight;
    textureinfo.pixelDepth = 0; //for 1D, 2D and cube texture , depth =0;

    //1D 
    if (pMipSet->m_nHeight == 1 && pData != NULL) {
        delete(pData);
        pData = NULL;
        pDataLen = 0;
    }

    textureinfo.numberOfMipmapLevels = pMipSet->m_nMipLevels;

    KTX_error_code save = ktxWriteKTXF(pFile, &textureinfo, pDataLen, pData, 1, inputMip);
    if (save == KTX_SUCCESS) {
        fclose(pFile);
    }
    else {
        if (KTX_CMips)
            KTX_CMips->PrintError(_T("Error(%d): KTX Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        fclose(pFile);
        return -1;
    }

//without using libktx
//	int x, y, z;
//	int i, j;
//
//	int xsize = pMipSet->m_nWidth;
//	int ysize = pMipSet->m_nHeight;
//	int zsize = 1;
//
//	int gl_format_of_channels[4] = { GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA };
//	int image_channels;
//	switch (pMipSet->m_format)
//	{
//		default:
//			image_channels = 4;	// Default ARGB
//	};
//
//		// Read the header
//	ktx_header header;
//	memset(&header, 0, sizeof(header));
//	
//	memcpy((void *)header.identifier, FileIdentifier, 12);
//	header.endianness	= 0x04030201;
//	header.gl_Type		= (pMipSet->m_ChannelFormat == CF_16bit) ? GL_HALF_FLOAT : GL_UNSIGNED_BYTE;
//	header.gl_TypeSize	= 0; // for now not saving compressed files
//	header.gl_Format	= gl_format_of_channels[image_channels - 1];
//	header.gl_InternalFormat = gl_format_of_channels[image_channels - 1];
//	header.gl_BaseInternalFormat = gl_format_of_channels[image_channels - 1];
//	header.pixelWidth = xsize;
//	header.pixelHeight = ysize;
//	header.pixelDepth = (zsize == 1) ? 0 : zsize;
//	header.numberOfArrayElements = 0;
//	header.numberOfFaces = 1;
//	header.numberOfMipmapLevels = 1;
//	header.bytesOfKeyValueData = 0;
//
//	WORD* pwData;
//	BYTE* pData;
//
//	// get data pointer
//	if (pMipSet->m_ChannelFormat == CF_16bit)
//	{
//		BYTE* pData = KTX_CMips->GetMipLevel(pMipSet,0)->m_pbData;
//		if(pData == NULL)
//		{
//			return PE_Unknown;
//		}
//	}
//	else
//	{
//		WORD* pwData = (WORD *)KTX_CMips->GetMipLevel(pMipSet,0)->m_pbData;
//		ASSERT(pwData);
//		if(pwData == NULL)
//		{
//			return PE_Unknown;
//		}
//	}
//	
//
//
//	
//	// collect image data to write
//	int bitness = (pMipSet->m_ChannelFormat == CF_16bit) ? 16: 8;
//	
//	BYTE ***row_pointers8 = NULL;
//	WORD ***row_pointers16 = NULL;
//
//	if (bitness == 8)
//	{
//		row_pointers8		= new BYTE **[zsize];
//		row_pointers8[0]	= new BYTE *[ysize * zsize];
//		row_pointers8[0][0] = new BYTE[xsize * ysize * zsize * image_channels + 3];
//
//		for (i = 1; i < zsize; i++)
//		{
//			row_pointers8[i]		= row_pointers8[0] + ysize * i;
//			row_pointers8[i][0]		= row_pointers8[0][0] + ysize * xsize * image_channels * i;
//		}
//		for (i = 0; i < zsize; i++)
//			for (j = 1; j < ysize; j++)
//				row_pointers8[i][j] = row_pointers8[i][0] + xsize * image_channels * j;
//
//		for (z = 0; z < zsize; z++)
//		{
//			for (y = 0; y < ysize; y++)
//			{
//				switch (image_channels)
//				{
//				case 1:		// single-component, treated as Luminance
//					for (x = 0; x < xsize; x++)
//					{
//						row_pointers8[z][y][x]			= *pData;
//						pData++;
//					}
//					break;
//				case 2:		// two-component, treated as Luminance-Alpha
//					for (x = 0; x < xsize; x++)
//					{
//						row_pointers8[z][y][2 * x]		= *pData;
//						pData +=3;
//						row_pointers8[z][y][2 * x + 1]	= *pData;
//						pData++;
//					}
//					break;
//				case 3:		// three-component, treated as RGB
//					for (x = 0; x < xsize; x++)
//					{
//						row_pointers8[z][y][3 * x]		= *pData++;
//						row_pointers8[z][y][3 * x + 1]	= *pData++;
//						row_pointers8[z][y][3 * x + 2]	= *pData++;
//					}
//					break;
//				case 4:		// four-component, treated as RGBA
//					for (x = 0; x < xsize; x++)
//					{
//						row_pointers8[z][y][4 * x]		= *pData++;
//						row_pointers8[z][y][4 * x + 1]	= *pData++;
//						row_pointers8[z][y][4 * x + 2]	= *pData++;
//						row_pointers8[z][y][4 * x + 3]	= *pData++;
//					}
//					break;
//				}
//			}
//		}
//	}
//	else						// if bitness == 16
//	{
//		row_pointers16			= new WORD **[zsize];
//		row_pointers16[0]		= new WORD *[ysize * zsize];
//		row_pointers16[0][0]	= new WORD[xsize * ysize * zsize * image_channels + 1];
//
//		for (i = 1; i < zsize; i++)
//		{
//			row_pointers16[i]		= row_pointers16[0] + ysize * i;
//			row_pointers16[i][0]	= row_pointers16[0][0] + ysize * xsize * image_channels * i;
//		}
//		for (i = 0; i < zsize; i++)
//			for (j = 1; j < ysize; j++)
//				row_pointers16[i][j] = row_pointers16[i][0] + xsize * image_channels * j;
//
//		for (z = 0; z < zsize; z++)
//		{
//			for (y = 0; y < ysize; y++)
//			{
//				switch (image_channels)
//				{
//				case 1:		// single-component, treated as Luminance
//					for (x = 0; x < xsize; x++)
//					{
//						row_pointers16[z][y][x] = *pwData++;
//					}
//					break;
//				case 2:		// two-component, treated as Luminance-Alpha
//					for (x = 0; x < xsize; x++)
//					{
//						row_pointers16[z][y][2 * x]		= *pwData;
//						pwData +=3;
//						row_pointers16[z][y][2 * x + 1] = *pwData;
//					}
//					break;
//				case 3:		// three-component, treated as RGB
//					for (x = 0; x < xsize; x++)
//					{
//						row_pointers16[z][y][3 * x]		= *pwData++;
//						row_pointers16[z][y][3 * x + 1] = *pwData++;
//						row_pointers16[z][y][3 * x + 2] = *pwData++;
//					}
//					break;
//				case 4:		// four-component, treated as RGBA
//					for (x = 0; x < xsize; x++)
//					{
//						row_pointers16[z][y][4 * x]		= *pwData++;
//						row_pointers16[z][y][4 * x + 1] = *pwData++;
//						row_pointers16[z][y][4 * x + 2] = *pwData++;
//						row_pointers16[z][y][4 * x + 3] = *pwData++;
//					}
//					break;
//				}
//			}
//		}
//	}
//
//	int retval = image_channels + (bitness == 16 ? 0x80 : 0);
//	UINT image_bytes = xsize * ysize * zsize * image_channels * (bitness / 8);
//	UINT image_write_bytes = (image_bytes + 3) & ~3;
//
//	FILE *wf = fopen(pszFilename, "wb");
//	if (wf)
//	{
//		void *dataptr = (bitness == 16) ? (void *)(row_pointers16[0][0]) : (void *)(row_pointers8[0][0]);
//
//		size_t expected_bytes_written = sizeof(ktx_header) + image_write_bytes + 4;
//		size_t hdr_bytes_written = fwrite(&header, 1, sizeof(ktx_header), wf);
//		size_t bytecount_bytes_written = fwrite(&image_bytes, 1, 4, wf);
//		size_t data_bytes_written = fwrite(dataptr, 1, image_write_bytes, wf);
//		fclose(wf);
//		if (hdr_bytes_written + bytecount_bytes_written + data_bytes_written != expected_bytes_written)
//			retval = -1;
//
//	}
//	else
//	{
//		retval = -1;
//	}
//
//	if (row_pointers8)
//	{
//		delete[]row_pointers8[0][0];
//		delete[]row_pointers8[0];
//		delete[]row_pointers8;
//	}
//	if (row_pointers16)
//	{
//		delete[]row_pointers16[0][0];
//		delete[]row_pointers16[0];
//		delete[]row_pointers16;
//	}

	fclose(pFile);
	return 0;
}



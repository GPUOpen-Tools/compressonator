// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4244)
//#include "ImfRgba.h"
#pragma warning(disable:4244)
#include "Texture.h"
#include "UtilFuncs.h"
//#include "Exports.h"
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define R_MASK_2101010 0x3ff00000
#define G_MASK_2101010 0x000ffc00
#define B_MASK_2101010 0x000003ff
#define A_MASK_2101010 0xc0000000

#define R_SHIFT_2101010 20
#define G_SHIFT_2101010 10
#define B_SHIFT_2101010  0
#define A_SHIFT_2101010 30

#define EXTRACT_BITS(value, mask, shift) ((value & mask) >> shift)

#define RGBA_OFFSET_A 24
#define RGBA_OFFSET_R 16
#define RGBA_OFFSET_G  8
#define RGBA_OFFSET_B  0

#define BYTE_MASK 0xff

#define CONVERT_TO_BYTE(w, s) static_cast<BYTE>((w >> s) & 0xff)
#define CONVERT_FLOAT_TO_BYTE(f) static_cast<BYTE>(((f) * 255.0) + 0.5)
#define CONVERT_BYTE_TO_FLOAT(b) (b) / 255.0f
#define CONVERT_WORD_TO_BYTE(w) static_cast<BYTE>((w >> 8) & 0xff)
#define CONVERT_BYTE_TO_WORD(b) ((b) | ((b) << 8))
#define CONVERT_DWORD_TO_BYTE(w) static_cast<BYTE>((w >> 24) & 0xff)
#define CONVERT_BYTE_TO_DWORD(b) ((b) | ((b) << 8) | ((b) << 16) | ((b) << 24))

DWORD GetChannelSize(ChannelFormat channelFormat) {
    switch(channelFormat) {
    case CF_8bit:
        return 1;
    case CF_Float16:
        return 2;
    case CF_Float32:
        return 3;
    case CF_16bit:
        return 2;
    case CF_2101010:
        return 1;
    case CF_32bit:
        return 4;
    default:
        ASSERT(0);
        return 0;
    }
}

DWORD GetChannelCount(TextureDataType textureDataType) {
    switch(textureDataType) {
    case TDT_R:
        return 1;
    case TDT_RG:
        return 2;
    default:
        return 4;
    }
}
DWORD GetPixelSize(const MipSet& mipset) {
    switch(mipset.m_ChannelFormat) {
    case CF_8bit:		// Fall-through
    case CF_2101010:
        return 4;
    case CF_Float16:	// Fall-through
    case CF_Float32:	// Fall-through
    case CF_16bit:		// Fall-through
    case CF_32bit:
        return (GetChannelSize(mipset.m_ChannelFormat) * GetChannelCount(mipset.m_TextureDataType));
    default:
        ASSERT(0);
        return 0;
    }
}

bool GetMipLevelPixelColorARGB(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, COLOR& color) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    DWORD dwPitch = ((8 * miplevel.m_nWidth) + 7) / 8;
    color.asDword = 0;
    switch(textureDataType) {
    case TDT_RG:
        color.rgba[1] = ((BYTE*) miplevel.m_pbData + (dwPitch * nYpos) + nXpos)[1];
    // Fall-through
    case TDT_R:
        color.rgba[2] = ((BYTE*) miplevel.m_pbData + (dwPitch * nYpos) + nXpos)[0];
        break;

    default:
        color = *((COLOR*) miplevel.m_pbData + (dwPitch * nYpos) + nXpos);
        break;
    }

    return true;
}

bool GetMipLevelPixelColorARGB(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, DWORD& dwColor) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    DWORD dwPitch = ((8 * miplevel.m_nWidth) + 7) / 8;
    switch(textureDataType) {
    case TDT_R:
        dwColor= (DWORD) (*((BYTE*) miplevel.m_pbData + (dwPitch * nYpos) + nXpos));
        break;

    case TDT_RG:
        dwColor = (DWORD) (*((WORD*) miplevel.m_pbData + (dwPitch * nYpos) + nXpos));
        break;

    default:
        dwColor = *((DWORD*) miplevel.m_pbData + (dwPitch * nYpos) + nXpos);
        break;
    }

    return true;
}

bool GetMipLevelPixelColorARGB(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, WORD color[4]) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    DWORD dwOffset = ((miplevel.m_nWidth * nYpos) + nXpos) * GetChannelCount(textureDataType);
    memset(color, 0, 4 * sizeof(color[0]));
    color[0] = miplevel.m_pwData[dwOffset++];
    if(textureDataType != TDT_R) {
        color[1] = miplevel.m_pwData[dwOffset++];
        if(textureDataType != TDT_RG) {
            color[2] = miplevel.m_pwData[dwOffset++];
            color[3] = miplevel.m_pwData[dwOffset++];
        }
    }

//#define ZIV 1
#ifdef ZIV
    color[0] = (color[0] >> 6);
    color[1] = (color[1] >> 6);
    color[2] = (color[2] >> 6);
    color[3] = (color[3] >> 10);
#endif // ZIV

    return true;
}

bool GetMipLevelPixelColorARGB(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, DWORD color[4]) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    DWORD dwOffset = ((miplevel.m_nWidth * nYpos) + nXpos) * GetChannelCount(textureDataType);
    memset(color, 0, 4 * sizeof(color[0]));
    color[0] = miplevel.m_pdwData[dwOffset++];
    if(textureDataType != TDT_R) {
        color[1] = miplevel.m_pdwData[dwOffset++];
        if(textureDataType != TDT_RG) {
            color[2] = miplevel.m_pdwData[dwOffset++];
            color[3] = miplevel.m_pdwData[dwOffset++];
        }
    }

    return true;
}

bool GetMipLevelPixelColorARGB(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, float color[4]) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    DWORD dwPitch = ((8 * miplevel.m_nWidth) + 7) / 8;
    DWORD dwColor = *((DWORD*) miplevel.m_pbData + (dwPitch * nYpos) + nXpos);
    memset(color, 0, 4 * sizeof(color[0]));
    color[0] = CONVERT_BYTE_TO_FLOAT((dwColor >> RGBA_OFFSET_R) & BYTE_MASK);
    if(textureDataType != TDT_R) {
        color[1] = CONVERT_BYTE_TO_FLOAT((dwColor >> RGBA_OFFSET_G) & BYTE_MASK);
        if(textureDataType != TDT_RG) {
            color[2] = CONVERT_BYTE_TO_FLOAT((dwColor >> RGBA_OFFSET_B) & BYTE_MASK);
            color[3] = CONVERT_BYTE_TO_FLOAT((dwColor >> RGBA_OFFSET_A) & BYTE_MASK);
        }
    }

    return true;
}

bool GetMipLevelPixelColorFloat32(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, COLOR& color) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    float* pData = miplevel.m_pfData + (((miplevel.m_nWidth * nYpos) + nXpos) * GetChannelCount(textureDataType));
    color.asDword = 0;
    color.rgba[2] = CONVERT_FLOAT_TO_BYTE(pData[0]);
    if(textureDataType != TDT_R) {
        color.rgba[1] = CONVERT_FLOAT_TO_BYTE(pData[1]);
        if(textureDataType != TDT_RG) {
            color.rgba[0] = CONVERT_FLOAT_TO_BYTE(pData[2]);
            color.rgba[3] = CONVERT_FLOAT_TO_BYTE(pData[3]);
        }
    }

    return true;
}

bool GetMipLevelPixelColorFloat32(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, float color[4]) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    float* pData = miplevel.m_pfData + (((miplevel.m_nWidth * nYpos) + nXpos) * GetChannelCount(textureDataType));
    switch(textureDataType) {
    case TDT_RG:
        color[1] = pData[1];
    // Fallthrough
    case TDT_R:
        color[0] = pData[0];
        color[2] = color[3] = 0.0;
        break;

    default:
        memcpy(color, pData, 4 * sizeof(float));
        break;
    }

    return true;
}

bool GetMipLevelPixelColorFloat16(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, COLOR& color) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    color.asDword = 0;
    Imf::Rgba* pData = (Imf::Rgba*) (miplevel.m_pfData + (((miplevel.m_nWidth * nYpos) + nXpos) * 2));
    color.rgba[2] = CONVERT_FLOAT_TO_BYTE(pData->r);
    if(textureDataType != TDT_R) {
        color.rgba[1] = CONVERT_FLOAT_TO_BYTE(pData->g);
        if(textureDataType != TDT_RG) {
            color.rgba[0] = CONVERT_FLOAT_TO_BYTE(pData->b);
            color.rgba[3] = CONVERT_FLOAT_TO_BYTE(pData->a);
        }
    }

    return true;
}

bool GetMipLevelPixelColorFloat16(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, float color[4]) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    memset(color, 0, 4 * sizeof(color[0]));
    half* pData = ((((half*) miplevel.m_pfData) + (((miplevel.m_nWidth * nYpos) + nXpos) * GetChannelCount(textureDataType))));
    color[0] = *pData++;
    if(textureDataType != TDT_R) {
        color[1] = *pData++;
        if(textureDataType != TDT_RG) {
            color[2] = *pData++;
            color[3] = *pData++;
        }
    }

    return true;
}

bool GetMipLevelPixelColorInt16(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, COLOR& color) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    color.asDword = 0;
    WORD* pData = miplevel.m_pwData + (((miplevel.m_nWidth * nYpos) + nXpos) * GetChannelCount(textureDataType));
    color.rgba[2] = CONVERT_WORD_TO_BYTE(pData[0]);
    if(textureDataType != TDT_R) {
        color.rgba[1] = CONVERT_WORD_TO_BYTE(pData[1]);
        if(textureDataType != TDT_RG) {
            color.rgba[0] = CONVERT_WORD_TO_BYTE(pData[2]);
            color.rgba[3] = CONVERT_WORD_TO_BYTE(pData[3]);
        }
    }

    return true;
}

bool GetMipLevelPixelColorInt32(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, COLOR& color) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    color.asDword = 0;
    DWORD* pData = miplevel.m_pdwData + (((miplevel.m_nWidth * nYpos) + nXpos) * GetChannelCount(textureDataType));
    color.rgba[2] = CONVERT_DWORD_TO_BYTE(pData[0]);
    if(textureDataType != TDT_R) {
        color.rgba[1] = CONVERT_DWORD_TO_BYTE(pData[1]);
        if(textureDataType != TDT_RG) {
            color.rgba[0] = CONVERT_DWORD_TO_BYTE(pData[2]);
            color.rgba[3] = CONVERT_DWORD_TO_BYTE(pData[3]);
        }
    }

    return true;
}

bool GetMipLevelPixelColor2101010(const MipLevel& miplevel, TextureDataType textureDataType, int nXpos, int nYpos, COLOR& color) {
    if(miplevel.m_pcData == NULL)
        return false;
    if(nXpos >= miplevel.m_nWidth || nXpos < 0)
        return false;
    if(nYpos >= miplevel.m_nHeight || nYpos < 0)
        return false;

    DWORD dwPitch = ((8 * miplevel.m_nWidth) + 7) / 8;
    DWORD dwColor = *((DWORD*) miplevel.m_pbData + (dwPitch * nYpos) + nXpos);
    color.rgba[2] = CONVERT_TO_BYTE(EXTRACT_BITS(dwColor, R_MASK_2101010, R_SHIFT_2101010), 2);
    if(textureDataType != TDT_R) {
        color.rgba[1] = CONVERT_TO_BYTE(EXTRACT_BITS(dwColor, G_MASK_2101010, G_SHIFT_2101010), 2);
        if(textureDataType != TDT_RG) {
            color.rgba[0] = CONVERT_TO_BYTE(EXTRACT_BITS(dwColor, B_MASK_2101010, B_SHIFT_2101010), 2);
            color.rgba[3] = CONVERT_TO_BYTE(EXTRACT_BITS(dwColor, A_MASK_2101010, A_SHIFT_2101010), 2);
        }
    }

    return true;
}

bool GetMipSetPixelColorARGB8888(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, COLOR& color) {
    if(nMipLevel >= mipset.m_nMipLevels)
        return false;

    if(mipset.m_ChannelFormat == CF_8bit) {
        switch(mipset.m_TextureDataType) {
        case TDT_XRGB: {
            if(GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color)) {
                color.rgba[3] = 0xff; // Alpha
                return true;
            } else
                return false;
        }
        case TDT_ARGB:
        case TDT_NORMAL_MAP:
            return GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);

        default:
            ASSERT(0);
            return false;
        }
    } else if(mipset.m_ChannelFormat == CF_2101010)
        return GetMipLevelPixelColor2101010(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);
    else if(mipset.m_ChannelFormat == CF_16bit)
        return GetMipLevelPixelColorInt16(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);
    else if(mipset.m_ChannelFormat == CF_32bit)
        return GetMipLevelPixelColorInt32(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);
    else if(mipset.m_ChannelFormat == CF_Float32)
        return GetMipLevelPixelColorFloat32(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);
    else if(mipset.m_ChannelFormat == CF_Float16)
        return GetMipLevelPixelColorFloat16(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);
    else {
        ASSERT(0);
        return false;
    }
}

bool GetMipSetPixelColorARGB16161616(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, WORD color[4]) {
    if(nMipLevel >= mipset.m_nMipLevels)
        return false;

    if(mipset.m_ChannelFormat == CF_16bit) {
        switch(mipset.m_TextureDataType) {
        case TDT_XRGB:
        case TDT_R:
        case TDT_RG: {
            if(GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color)) {
                color[3] = 0xffff; // Alpha
                return true;
            } else
                return false;
        }
        case TDT_ARGB:
        case TDT_NORMAL_MAP:
            return GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);

        default:
            ASSERT(0);
            return false;
        }
    }

    ASSERT(0);
    return false;
}

bool GetMipSetPixelColorARGB32(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, DWORD color[4]) {
    if(nMipLevel >= mipset.m_nMipLevels)
        return false;

    if(mipset.m_ChannelFormat == CF_32bit) {
        switch(mipset.m_TextureDataType) {
        case TDT_XRGB:
        case TDT_R:
        case TDT_RG: {
            if(GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color)) {
                color[3] = 0xffffffff; // Alpha
                return true;
            } else
                return false;
        }
        case TDT_ARGB:
        case TDT_NORMAL_MAP:
            return GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);

        default:
            ASSERT(0);
            return false;
        }
    }

    ASSERT(0);
    return false;
}

bool GetMipSetPixelColorARGB32F(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, float color[4]) {
    if(nMipLevel >= mipset.m_nMipLevels)
        return false;

    if(mipset.m_ChannelFormat == CF_8bit) {
        switch(mipset.m_TextureDataType) {
        case TDT_XRGB:
        case TDT_R:
        case TDT_RG: {
            if(GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color)) {
                color[3] = 1.0f; // Alpha
                return true;
            } else
                return false;
        }
        case TDT_ARGB:
        case TDT_NORMAL_MAP:
            return GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);

        default:
            ASSERT(0);
            return false;
        }
    } else if(mipset.m_ChannelFormat == CF_Float32) {
        return GetMipLevelPixelColorFloat32(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);
    } else if(mipset.m_ChannelFormat == CF_Float16) {
        return GetMipLevelPixelColorFloat16(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, color);
    }

    ASSERT(0);
    return false;
}

bool GetMipSetPixelColorARGB2101010(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, WORD color[4]) {
    if(nMipLevel >= mipset.m_nMipLevels)
        return false;

    if(mipset.m_ChannelFormat == CF_2101010) {
        DWORD dwColor;
        if(GetMipLevelPixelColorARGB(*TC_AppGetMipLevel(&mipset, nMipLevel, nFaceOrSlice), mipset.m_TextureDataType, nXpos, nYpos, dwColor)) {
            color[2] = (WORD) EXTRACT_BITS(dwColor, R_MASK_2101010, R_SHIFT_2101010);
            color[1] = (WORD) EXTRACT_BITS(dwColor, G_MASK_2101010, G_SHIFT_2101010);
            color[0] = (WORD) EXTRACT_BITS(dwColor, B_MASK_2101010, B_SHIFT_2101010);
            if(mipset.m_TextureDataType == TDT_XRGB)
                color[3] = 0x3; // Alpha
            else
                color[3] = (WORD) EXTRACT_BITS(dwColor, A_MASK_2101010, A_SHIFT_2101010);
            return true;
        } else
            return false;
    }

    assert(0);
    return false;
}
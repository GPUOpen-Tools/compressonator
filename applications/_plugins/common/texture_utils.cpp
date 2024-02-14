//=====================================================================
// Copyright (c) 2023-2024, Advanced Micro Devices, Inc. All rights reserved.
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

#include "texture_utils.h"

#include "atiformats.h"
#ifdef USE_LOSSLESS_COMPRESSION
#include "brotlig/brlg_sdk_wrapper.h"
#endif
#include "codec_common.h"

CMP_DWORD CalcBufferSize(CMP_FORMAT format, CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight)
{
    switch (format)
    {
    case CMP_FORMAT_RGBA_8888:
    case CMP_FORMAT_BGRA_8888:
    case CMP_FORMAT_ARGB_8888:
    case CMP_FORMAT_ABGR_8888:
    case CMP_FORMAT_RGBA_8888_S:
    case CMP_FORMAT_ARGB_8888_S:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * dwHeight));

    case CMP_FORMAT_RGBA_1010102:
    case CMP_FORMAT_ARGB_2101010:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * dwHeight));

    case CMP_FORMAT_BGR_888:
    case CMP_FORMAT_RGB_888:
    case CMP_FORMAT_RGB_888_S:
        return ((dwPitch) ? (dwPitch * dwHeight) : ((((dwWidth * 3) + 3) >> 2) * 4 * dwHeight));

    case CMP_FORMAT_RG_8:
    case CMP_FORMAT_RG_8_S:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 2 * dwHeight));

    case CMP_FORMAT_R_8:
    case CMP_FORMAT_R_8_S:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * dwHeight));

    case CMP_FORMAT_ARGB_16:
    case CMP_FORMAT_ABGR_16:
    case CMP_FORMAT_RGBA_16:
    case CMP_FORMAT_BGRA_16:
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(CMP_WORD) * dwHeight));

    case CMP_FORMAT_RG_16:
    case CMP_FORMAT_RG_16F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(CMP_WORD) * dwHeight));

    case CMP_FORMAT_R_16:
    case CMP_FORMAT_R_16F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(CMP_WORD) * dwHeight));

    case CMP_FORMAT_RGBE_32F:
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(float) * dwHeight));

    case CMP_FORMAT_BGR_32F:
    case CMP_FORMAT_RGB_32F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 3 * sizeof(float) * dwHeight));

    case CMP_FORMAT_RG_32F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 2 * sizeof(float) * dwHeight));

    case CMP_FORMAT_R_32F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 1 * sizeof(float) * dwHeight));

    case CMP_FORMAT_BINARY:
        return dwWidth * dwHeight;

    default:
        return CalcBufferSize(GetCodecType(format), dwWidth, dwHeight, nBlockWidth, nBlockHeight);
    }
}

CMP_DWORD CMP_API CMP_CalculateBufferSize(const CMP_Texture* texture)
{
    if (texture == NULL)
        return 0;

    if (texture->dwSize != sizeof(CMP_Texture))
        return 0;

    if (texture->dwWidth <= 0)
        return 0;

    if (texture->dwHeight <= 0)
        return 0;

    // Check format range is valid
    if (!CMP_IsValidFormat(texture->format))
        return 0;

#ifdef USE_LOSSLESS_COMPRESSION
    if (texture->format == CMP_FORMAT_BROTLIG)
    {
        CMP_DWORD numChannels = 1;

        // The only non-binary format we expect is a BCn format, so in those cases we just assume that
        // the size of the destination will never be bigger than a completely uncompressed 4 channel texture
        if (texture->transcodeFormat != CMP_FORMAT_BINARY)
            numChannels = 4;

        return (CMP_DWORD)BRLG::MaxCompressedSize(texture->dwWidth * texture->dwHeight * numChannels);
    }
#endif

    return CalcBufferSize(texture->format, texture->dwWidth, texture->dwHeight, texture->dwPitch, texture->nBlockWidth, texture->nBlockHeight);
}

CMP_ERROR CheckTexture(const CMP_Texture* pTexture, bool bSource)
{
    if (pTexture == NULL)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    if (pTexture->pData == NULL)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    if (pTexture->dwSize != sizeof(CMP_Texture))
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    if (pTexture->dwWidth <= 0)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    if (pTexture->dwHeight <= 0)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    if (!CMP_IsValidFormat(pTexture->format))
        return (bSource ? CMP_ERR_UNSUPPORTED_SOURCE_FORMAT : CMP_ERR_UNSUPPORTED_DEST_FORMAT);

    assert((pTexture->format != CMP_FORMAT_ARGB_8888 && pTexture->format != CMP_FORMAT_ARGB_2101010) || pTexture->dwPitch == 0 ||
           pTexture->dwPitch >= (pTexture->dwWidth * 4));

    assert((pTexture->format != CMP_FORMAT_RGBA_8888_S && pTexture->format != CMP_FORMAT_ARGB_2101010) || pTexture->dwPitch == 0 ||
           pTexture->dwPitch >= (pTexture->dwWidth * 4));

    if ((pTexture->format == CMP_FORMAT_ARGB_8888 || pTexture->format == CMP_FORMAT_ARGB_2101010) && pTexture->dwPitch != 0 &&
        pTexture->dwPitch < (pTexture->dwWidth * 4))
        return (bSource ? CMP_ERR_UNSUPPORTED_SOURCE_FORMAT : CMP_ERR_UNSUPPORTED_DEST_FORMAT);

    if ((pTexture->format == CMP_FORMAT_RGBA_8888_S || pTexture->format == CMP_FORMAT_ARGB_2101010) && pTexture->dwPitch != 0 &&
        pTexture->dwPitch < (pTexture->dwWidth * 4))
        return (bSource ? CMP_ERR_UNSUPPORTED_SOURCE_FORMAT : CMP_ERR_UNSUPPORTED_DEST_FORMAT);

    CMP_DWORD dwDataSize = CMP_CalculateBufferSize(pTexture);
    if (pTexture->dwDataSize < dwDataSize)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    return CMP_OK;
}
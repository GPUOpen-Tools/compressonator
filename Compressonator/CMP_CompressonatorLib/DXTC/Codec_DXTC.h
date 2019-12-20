//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
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
//  File Name:   Codec_DXTC.h
//  Description: interface for the CCodec_DXTC class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _CODEC_DXTC_H_INCLUDED_
#define _CODEC_DXTC_H_INCLUDED_

#include "Codec_Block_4x4.h"

#define DXTC_OFFSET_ALPHA 0
#define DXTC_OFFSET_RGB 2

class CCodec_DXTC : public CCodec_Block_4x4
{
public:
    CCodec_DXTC(CodecType codecType);
    virtual ~CCodec_DXTC();

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue);
    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue);
    virtual bool GetParameter(const CMP_CHAR* pszParamName, CMP_DWORD& dwValue);

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue);
    virtual bool GetParameter(const CMP_CHAR* pszParamName, CODECFLOAT& fValue);

protected:
    virtual CodecError CompressAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual CodecError CompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual CodecError CompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2], CODECFLOAT* pfChannelWeights = NULL, bool bDXT1 = false, bool bDXT1UseAlpha = false, CMP_BYTE nDXT1AlphaThreshold = 0);
    virtual CodecError CompressRGBABlock(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4], CODECFLOAT* pfChannelWeights = NULL);
    virtual CodecError CompressRGBABlock_ExplicitAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4], CODECFLOAT* pfChannelWeights = NULL);

    virtual CodecError CompressAlphaBlock_Fast(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual CodecError CompressExplicitAlphaBlock_Fast(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual CodecError CompressRGBBlock_Fast(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);
    virtual CodecError CompressRGBBlock_SuperFast(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);
    virtual CodecError CompressRGBABlock_Fast(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);
    virtual CodecError CompressRGBABlock_SuperFast(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);
    virtual CodecError CompressRGBABlock_ExplicitAlpha_Fast(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);
    virtual CodecError CompressRGBABlock_ExplicitAlpha_SuperFast(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);

    virtual CodecError CompressAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual CodecError CompressExplicitAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual CodecError CompressRGBBlock(CODECFLOAT rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2], CODECFLOAT* pfChannelWeights = NULL, bool bDXT1 = false, bool bDXT1UseAlpha = false, CODECFLOAT fDXT1AlphaThreshold = 0.0);
    virtual CodecError CompressRGBABlock(CODECFLOAT rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4], CODECFLOAT* pfChannelWeights = NULL);
    virtual CodecError CompressRGBABlock_ExplicitAlpha(CODECFLOAT rgbaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[4], CODECFLOAT* pfChannelWeights = NULL);

    virtual void DecompressAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual void DecompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual void DecompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2], bool bDXT1);
    virtual void DecompressRGBABlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);
    virtual void DecompressRGBABlock_ExplicitAlpha(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);

    virtual void DecompressAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual void DecompressExplicitAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2]);
    virtual void DecompressRGBBlock(CODECFLOAT rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2], bool bDXT1);
    virtual void DecompressRGBABlock(CODECFLOAT rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);
    virtual void DecompressRGBABlock_ExplicitAlpha(CODECFLOAT rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);

// Alpha decompression functions
    void GetCompressedAlphaRamp(CMP_BYTE alpha[8],CMP_DWORD compressedBlock[2]);
    void GetCompressedAlphaRamp(CODECFLOAT alpha[8],CMP_DWORD compressedBlock[2]);

// RGB compression functions
    CODECFLOAT* CalculateColourWeightings(CMP_BYTE block[BLOCK_SIZE_4X4X4]);
    CODECFLOAT* CalculateColourWeightings(CODECFLOAT block[BLOCK_SIZE_4X4X4]);

    void EncodeAlphaBlock(CMP_DWORD compressedBlock[2], CMP_BYTE nEndpoints[2], CMP_BYTE nIndices[BLOCK_SIZE_4X4]);

    bool m_bUseChannelWeighting;
    bool m_bUseAdaptiveWeighting;
    bool m_bUseFloat;
    bool m_b3DRefinement;
    bool m_bSwizzleChannels;

    CMP_BYTE m_nRefinementSteps;
    CMP_Speed m_nCompressionSpeed;

    CODECFLOAT m_fBaseChannelWeights[3];
    CODECFLOAT m_fChannelWeights[3];
};

#endif // !defined(_CODEC_DXTC_H_INCLUDED_)

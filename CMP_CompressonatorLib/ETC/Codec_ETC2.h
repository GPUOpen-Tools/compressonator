//===============================================================================
// Copyright (c) 2007-2018  Advanced Micro Devices, Inc. All rights reserved.
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
//
//  File Name:   Codec_ETC2.h
//  Description: implementation of the CCodec_ETC2 class
//
//////////////////////////////////////////////////////////////////////////////
//// etcpack v2.74
//// 
//// NO WARRANTY 
//// 
//// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE THE PROGRAM IS PROVIDED
//// "AS IS". ERICSSON MAKES NO REPRESENTATIONS OF ANY KIND, EXTENDS NO
//// WARRANTIES OR CONDITIONS OF ANY KIND; EITHER EXPRESS, IMPLIED OR
//// STATUTORY; INCLUDING, BUT NOT LIMITED TO, EXPRESS, IMPLIED OR
//// STATUTORY WARRANTIES OR CONDITIONS OF TITLE, MERCHANTABILITY,
//// SATISFACTORY QUALITY, SUITABILITY AND FITNESS FOR A PARTICULAR
//// PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
//// PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME
//// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. ERICSSON
//// MAKES NO WARRANTY THAT THE MANUFACTURE, SALE, OFFERING FOR SALE,
//// DISTRIBUTION, LEASE, USE OR IMPORTATION UNDER THE LICENSE WILL BE FREE
//// FROM INFRINGEMENT OF PATENTS, COPYRIGHTS OR OTHER INTELLECTUAL
//// PROPERTY RIGHTS OF OTHERS, AND THE VALIDITY OF THE LICENSE IS SUBJECT
//// TO YOUR SOLE RESPONSIBILITY TO MAKE SUCH DETERMINATION AND ACQUIRE
//// SUCH LICENSES AS MAY BE NECESSARY WITH RESPECT TO PATENTS, COPYRIGHT
//// AND OTHER INTELLECTUAL PROPERTY OF THIRD PARTIES.
//// 
//// FOR THE AVOIDANCE OF DOUBT THE PROGRAM (I) IS NOT LICENSED FOR; (II)
//// IS NOT DESIGNED FOR OR INTENDED FOR; AND (III) MAY NOT BE USED FOR;
//// ANY MISSION CRITICAL APPLICATIONS SUCH AS, BUT NOT LIMITED TO
//// OPERATION OF NUCLEAR OR HEALTHCARE COMPUTER SYSTEMS AND/OR NETWORKS,
//// AIRCRAFT OR TRAIN CONTROL AND/OR COMMUNICATION SYSTEMS OR ANY OTHER
//// COMPUTER SYSTEMS AND/OR NETWORKS OR CONTROL AND/OR COMMUNICATION
//// SYSTEMS ALL IN WHICH CASE THE FAILURE OF THE PROGRAM COULD LEAD TO
//// DEATH, PERSONAL INJURY, OR SEVERE PHYSICAL, MATERIAL OR ENVIRONMENTAL
//// DAMAGE. YOUR RIGHTS UNDER THIS LICENSE WILL TERMINATE AUTOMATICALLY
//// AND IMMEDIATELY WITHOUT NOTICE IF YOU FAIL TO COMPLY WITH THIS
//// PARAGRAPH.
//// 
//// IN NO EVENT WILL ERICSSON, BE LIABLE FOR ANY DAMAGES WHATSOEVER,
//// INCLUDING BUT NOT LIMITED TO PERSONAL INJURY, ANY GENERAL, SPECIAL,
//// INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF OR IN
//// CONNECTION WITH THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT
//// NOT LIMITED TO LOSS OF PROFITS, BUSINESS INTERUPTIONS, OR ANY OTHER
//// COMMERCIAL DAMAGES OR LOSSES, LOSS OF DATA OR DATA BEING RENDERED
//// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF
//// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS) REGARDLESS OF THE
//// THEORY OF LIABILITY (CONTRACT, TORT OR OTHERWISE), EVEN IF SUCH HOLDER
//// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//// 
//// (C) Ericsson AB 2005-2013. All Rights Reserved.
//// 

#ifndef _CODEC_ETC2_H_INCLUDED_
#define _CODEC_ETC2_H_INCLUDED_

#include "Common.h"
#include "Codec_Block_4x4.h"
#include "Compressonator_tc.h"
#include "CompressonatorXCodec.h"

#define ATC_OFFSET_ALPHA 0
#define ATC_OFFSET_RGB 2

#ifdef  USE_ETCPACK
#include "etcpack.h"
#include "etcpack_lib.h"
#endif

typedef char             int8;
typedef unsigned char   uint8;
typedef short           int16;
typedef unsigned short uint16;
typedef int             int32;
typedef unsigned int   uint32;
typedef unsigned long long  uint64;


#define SWIZZLE_DWORD(i) ((((i >> 24) & BYTE_MASK)) | (((i >> 16) & BYTE_MASK) << 8) | (((i >> 8) & BYTE_MASK) << 16) | ((i & BYTE_MASK) << 24))

#ifndef USE_ETCPACK
#define USE_CMP_ETC2
#endif

#define     CMP_CLAMP(ll,x,ul) (((x)<(ll)) ? (ll) : (((x)>(ul)) ? (ul) : (x)))

extern int   g_alphaTable[256][8];
extern int   cmp_alphaBase[16][4];
extern bool  g_alphaTableInitialized;
extern int   cmp_clamp(int val);
extern uint8 cmp_getbit(uint8 input, int frompos, int topos);

#ifdef USE_CMP_ETC2

extern void  cmp_decompressETC21BitAlpha(uint64 block, uint32 &pixel, uint32 x, uint32 y);
extern void  cmp_decompressETC2Pixel(uint64 block, uint32 &pixel, uint32 x, uint32 y);
extern void  cmp_decompressRGBABlockAlpha(CMP_BYTE* alphadata, CMP_BYTE* alphaimg);

extern void  cmp_setupAlphaTable();
extern void  cmp_compressBlockAlphaFast(uint8 * data, uint8* returnData);
extern void  cmp_compressBlockETC2FastPerceptual(uint8 *img, uint8 *imgdec, unsigned int &compressed1, unsigned int &compressed2);
extern void  cmp_compressBlockETC2Fast(uint8 *img, uint8* alphaimg, uint8 *imgdec, unsigned int &compressed1, unsigned int &compressed2);
#endif


enum IMG_DATA_FORMAT {
    IMG_DATA_FORMAT_INVALID = 0,
    IMG_DATA_FORMAT_ETC2_RGB,
    IMG_DATA_FORMAT_ETC2_RGBA,
    IMG_DATA_FORMAT_ETC2_R,
    IMG_DATA_FORMAT_ETC2_RG,
    IMG_DATA_FORMAT_ETC2_RGBA1
};


class CCodec_ETC2 : public CCodec_Block_4x4  
{
public:
    CCodec_ETC2(CodecType codecType);
    virtual ~CCodec_ETC2();

protected:
    bool m_fast = true;

    CodecError  CompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);
    void        DecompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);

    CodecError  CompressRGBA1Block(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);
    void        DecompressRGBA1Block(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]);

    CodecError  CompressRGBABlock(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);
    void        DecompressRGBABlock(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]);

    void        setupAlphaTable();
};
#endif // !defined(_CODEC_ATC_H_INCLUDED_)

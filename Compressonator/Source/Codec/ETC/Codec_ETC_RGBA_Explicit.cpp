//////////////////////////////////////////////////////////////////////////////
//
//  Advanced Micro Devices, Inc.
//  1 AMD Place
//  Sunnyvale, CA
//  USA 94088
//
//  File Name:   Codec_ETC.cpp
//  Description: implementation of the CCodec_ETC class
//
//  Copyright (c) 2004-2006 ATI Technologies Inc.
//
//  Developer:	Seth Sowerby
//  Email:		gputools.support@amd.com
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"  // napatel

#ifdef SUPPORT_ETC_ALPHA

#include "etcpack.h"
#include "Codec_ETC_RGBA_Explicit.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ETC_RGBA_Explicit::CCodec_ETC_RGBA_Explicit() :
CCodec_ETC(CT_ETC_RGBA_Explicit)
{

}

CCodec_ETC_RGBA_Explicit::~CCodec_ETC_RGBA_Explicit()
{

}

CodecError CCodec_ETC_RGBA_Explicit::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
	assert(bufferIn.GetWidth() == bufferOut.GetWidth());
	assert(bufferIn.GetHeight() == bufferOut.GetHeight());

	if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
		return CE_Unknown;

    readCompressParams();

	const AMD_TC_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
	const AMD_TC_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    AMD_TC_BYTE srcBlock[BLOCK_SIZE_4X4X4];
    AMD_TC_DWORD compressedBlock[4];
	for(AMD_TC_DWORD j = 0; j < dwBlocksY; j++)
	{
		for(AMD_TC_DWORD i = 0; i < dwBlocksX; i++)
		{
            bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, srcBlock);
            CompressRGBABlock_ExplicitAlpha(srcBlock, compressedBlock);
            bufferOut.WriteBlock(i*4, j*4, compressedBlock, 4);
        }
		if(pFeedbackProc)
		{
			float fProgress = 100.f * (j * dwBlocksX) / (dwBlocksX * dwBlocksY);
			if(pFeedbackProc(fProgress, pUser1, pUser2))
				return CE_Aborted;
		}
	}

	return CE_OK;
}

CodecError CCodec_ETC_RGBA_Explicit::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut)
{
	assert(bufferIn.GetWidth() == bufferOut.GetWidth());
	assert(bufferIn.GetHeight() == bufferOut.GetHeight());

	if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
		return CE_Unknown;

	const AMD_TC_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
	const AMD_TC_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    AMD_TC_DWORD compressedBlock[4];
    AMD_TC_BYTE destBlock[BLOCK_SIZE_4X4X4];
	for(AMD_TC_DWORD j = 0; j < dwBlocksY; j++)
	{
		for(AMD_TC_DWORD i = 0; i < dwBlocksX; i++)
		{
            bufferIn.ReadBlock(i*4, j*4, compressedBlock, 4);
            DecompressRGBABlock_ExplicitAlpha(destBlock, compressedBlock);
            bufferOut.WriteBlockRGBA(i*4, j*4, 4, 4, destBlock);
		}
	}

	return CE_OK;
}

#endif // SUPPORT_ETC_ALPHA

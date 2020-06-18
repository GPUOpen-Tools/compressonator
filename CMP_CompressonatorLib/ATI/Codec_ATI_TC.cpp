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

#ifdef SUPPORT_ATI_TC
#include "Codec_ATI_TC.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ATI_TC::CCodec_ATI_TC(CodecType codecType) :
CCodec_Block_4x4(codecType)
{

}

CCodec_ATI_TC::~CCodec_ATI_TC()
{

}

CodecError CCodec_ATI_TC::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
	assert(bufferIn.GetWidth() == bufferOut.GetWidth());
	assert(bufferIn.GetHeight() == bufferOut.GetHeight());

	if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
		return CE_Unknown;

	const ATI_TC_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
	const ATI_TC_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

	for(ATI_TC_DWORD j = 0; j < dwBlocksY; j++)
	{
		ATI_TC_DWORD compressedBlock[4];
		for(ATI_TC_DWORD i = 0; i < dwBlocksX; i++)
		{
/*			ATI_TC_BYTE cAlphaBlock[BLOCK_SIZE_4X4];
						bufferIn.ReadBlockR(i*4, j*4, 4, 4, cAlphaBlock);
						CompressAlphaBlock(cAlphaBlock, &compressedBlock[dwXOffset]);
			
						bufferIn.ReadBlockG(i*4, j*4, 4, 4, cAlphaBlock);
						CompressAlphaBlock(cAlphaBlock, &compressedBlock[dwYOffset]);
			
						bufferOut.WriteBlock(i*4, j*4, compressedBlock, 4);*/
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

CodecError CCodec_ATI_TC::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut)
{
	assert(bufferIn.GetWidth() == bufferOut.GetWidth());
	assert(bufferIn.GetHeight() == bufferOut.GetHeight());

	if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
		return CE_Unknown;

	const ATI_TC_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
	const ATI_TC_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

	for(ATI_TC_DWORD j = 0; j < dwBlocksY; j++)
	{
		for(ATI_TC_DWORD i = 0; i < dwBlocksX; i++)
		{
/*			ATI_TC_DWORD compressedBlock[4];
			bufferIn.ReadBlock(i*4, j*4, compressedBlock, 4);

			ATI_TC_BYTE alphaBlockR[BLOCK_SIZE_4X4];
			DecompressAlphaBlock(alphaBlockR, &compressedBlock[dwXOffset]);
			bufferOut.WriteBlockR(i*4, j*4, 4, 4, alphaBlockR);

			ATI_TC_BYTE alphaBlockG[BLOCK_SIZE_4X4];
			DecompressAlphaBlock(alphaBlockG, &compressedBlock[dwYOffset]);
			bufferOut.WriteBlockG(i*4, j*4, 4, 4, alphaBlockG);

			ATI_TC_BYTE alphaBlockB[BLOCK_SIZE_4X4];
			DeriveBlockB(alphaBlockR, alphaBlockG, alphaBlockB);
			bufferOut.WriteBlockB(i*4, j*4, 4, 4, alphaBlockB);

			ATI_TC_BYTE alphaBlockA[BLOCK_SIZE_4X4];
			memset(alphaBlockA, 0, sizeof(alphaBlockA));
			bufferOut.WriteBlockA(i*4, j*4, 4, 4, alphaBlockA);*/
		}
	}

	return CE_OK;
}

#endif // SUPPORT_ATI_TC

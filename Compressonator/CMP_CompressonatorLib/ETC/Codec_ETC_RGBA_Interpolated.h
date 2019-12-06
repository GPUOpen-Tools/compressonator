//////////////////////////////////////////////////////////////////////////////
//
//  Advanced Micro Devices, Inc.
//  1 AMD Place
//  Sunnyvale, CA
//  USA 94088
//
//  File Name:   Codec_ETC.h
//  Description: interface for the CCodec_ETC class
//
//  Copyright (c) 2004-2006 ATI Technologies Inc.
//
//  Developer:	Seth Sowerby
//  Email:		gputools.support@amd.com
//
//////////////////////////////////////////////////////////////////////////////

#if !defined(_Codec_ETC_RGBA_INTERP_H_INCLUDED_)
#define _Codec_ETC_RGBA_INTERP_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Codec_ETC.h"

class CCodec_ETC_RGBA_Interpolated : public CCodec_ETC
{
public:
	CCodec_ETC_RGBA_Interpolated();
	virtual ~CCodec_ETC_RGBA_Interpolated();

	virtual CodecError Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, DWORD_PTR pUser1 = NULL, DWORD_PTR pUser2 = NULL);
	virtual CodecError Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut);
};
#endif // !defined(_Codec_ETC_RGBA_INTERP_H_INCLUDED_)

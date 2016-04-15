/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: 2319952fa702d4c9a8e3c02868541b1d7cff9aa0 $ */

/*
Copyright (c) 2010 The Khronos Group Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and/or associated documentation files (the
"Materials"), to deal in the Materials without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Materials, and to
permit persons to whom the Materials are furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
unaltered in all copies or substantial portions of the Materials.
Any additions, deletions, or changes to the original source files
must be clearly indicated in accompanying documentation.

If only executable code is distributed, then the accompanying
documentation must state that "this software is based in part on the
work of the Khronos Group."

THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

#include "KHR/khrplatform.h"

/*
 * SwapEndian16: Swaps endianness in an array of 16-bit values
 */
void
_ktxSwapEndian16(khronos_uint16_t* pData16, int count)
{
	int i;
	for (i = 0; i < count; ++i)
	{
		khronos_uint16_t x = *pData16;
		*pData16++ = (x << 8) | (x >> 8);
	}
}

/*
 * SwapEndian32: Swaps endianness in an array of 32-bit values
 */
void 
_ktxSwapEndian32(khronos_uint32_t* pData32, int count)
{
	int i;
	for (i = 0; i < count; ++i)
	{
		khronos_uint32_t x = *pData32;
		*pData32++ = (x << 24) | ((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8) | (x >> 24);
	}
}



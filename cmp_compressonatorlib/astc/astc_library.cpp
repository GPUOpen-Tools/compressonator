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

#include "astc/astc_definitions.h"
#include "astc/astc_encode.h"
#include "astc/astc_decode.h"
#include "compressonator.h"


extern CMP_BOOL g_LibraryInitialized;
static ASTCBlockDecoder  g_Decoder;

// Need to remove these calls
int astc_codec_unlink(const char *filename) {
    int res = remove(filename);
    return res;
}

void astc_codec_internal_error(const char *filename, int linenum) {
    printf("Internal error: File=%s Line=%d\n", filename, linenum);
}

//
// Decode a block and write it to the output
//
//
//
extern "C" BC_ERROR CMP_DecodeASTCBlock( CMP_BYTE BlockWidth,
        CMP_BYTE BlockHeight,
        CMP_BYTE Bitness,
        BYTE *in,
        float out[][4]) {
    if(!g_LibraryInitialized) {
        return BC_ERROR_LIBRARY_NOT_INITIALIZED;
    }

    if( !in || !out ) {
        return BC_ERROR_INVALID_PARAMETERS;
    }

    g_Decoder.DecompressBlock(BlockWidth, BlockHeight, Bitness, out, in);
    return BC_ERROR_NONE;
}


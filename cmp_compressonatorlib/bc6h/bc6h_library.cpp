//===============================================================================
// Copyright (c) 2014-2016  Advanced Micro Devices, Inc. All rights reserved.
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

#include "bc7_definitions.h"
#include "bc6h_definitions.h"
#include "bc6h_encode.h"
#include "bc6h_decode.h"
#include "compressonator.h"


extern bool    g_LibraryInitialized;

static BC6HBlockDecoder  g_Decoder;

extern "C" BC_ERROR CMP_CreateBC6HEncoder(CMP_BC6H_BLOCK_PARAMETERS user_settings, BC6HBlockEncoder** encoder ) {
    if(!g_LibraryInitialized) {
        return BC_ERROR_LIBRARY_NOT_INITIALIZED;
    }

    if ( !encoder ) {
        return BC_ERROR_INVALID_PARAMETERS;
    }

    *encoder = new  BC6HBlockEncoder(user_settings);

    if ( !encoder ) {
        return BC_ERROR_OUT_OF_MEMORY;
    }

    return BC_ERROR_NONE;
}

//
// Submit a block for encoding
//
//
//
extern "C" BC_ERROR CMP_EncodeBC6HBlock( BC6HBlockEncoder* encoder, float  in[BC6H_BLOCK_PIXELS][MAX_DIMENSION_BIG], BYTE* out ) {
    if(!g_LibraryInitialized) {
        return BC_ERROR_LIBRARY_NOT_INITIALIZED;
    }

    if( !encoder || !in || !out ) {
        return BC_ERROR_INVALID_PARAMETERS;
    }

    encoder->CompressBlock( in, out );

    return BC_ERROR_NONE;
}


//
// Decode a block and write it to the output
//
//
//
extern "C" BC_ERROR CMP_DecodeBC6HBlock( BYTE *in, float  out[BC_BLOCK_PIXELS][MAX_DIMENSION_BIG] ) {
    if(!g_LibraryInitialized) {
        return BC_ERROR_LIBRARY_NOT_INITIALIZED;
    }

    if( !in || !out ) {
        return BC_ERROR_INVALID_PARAMETERS;
    }

    g_Decoder.DecompressBlock(out, in);
    return BC_ERROR_NONE;
}


//
// Destroys encoder object
//
//
//
extern "C" BC_ERROR CMP_DestroyBC6HEncoder( BC6HBlockEncoder* encoder ) {
    if(!g_LibraryInitialized) {
        return BC_ERROR_LIBRARY_NOT_INITIALIZED;
    }

    if( !encoder ) {
        return BC_ERROR_INVALID_PARAMETERS;
    }

    delete encoder;

    return BC_ERROR_NONE;
}


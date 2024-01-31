//===============================================================================
// Copyright (c) 2014-2024  Advanced Micro Devices, Inc. All rights reserved.
//===============================================================================
//
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
//////////////////////////////////////////////////////////////////////////////////

#ifndef _ASTC_ENCODE_H_
#define _ASTC_ENCODE_H_

#include <float.h>
#include "astc_definitions.h"
#include "astc_encode_kernel.h"

class ASTCBlockEncoder
{
public:
    ASTCBlockEncoder(){};

    ~ASTCBlockEncoder(){};

    // This routine compresses a block and returns the RMS error
    double CompressBlock_kernel(ASTC_Encoder::astc_codec_image* input_image, uint8_t* bp, int x, int y, int z, ASTC_Encoder::ASTC_Encode* ASTCEncode);

private:
    imageblock                m_pb;
    symbolic_compressed_block m_scb;
    physical_compressed_block m_pcb;
};

#endif

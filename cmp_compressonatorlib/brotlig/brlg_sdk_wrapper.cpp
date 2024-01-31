//===============================================================================
// Copyright (c) 2022-2024  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   brlg_sdk_wrapper.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "brlg_sdk_wrapper.h"

#include "BrotliG.h"
#include "BrotligCompute.h"
#include "common/BrotligConstants.h"

namespace BRLG
{

static char* g_errorString = nullptr;

static BROTLIG_DATA_FORMAT GetBrotliGFormat(CMP_FORMAT format)
{
    if (format == CMP_FORMAT_BC1)
        return BROTLIG_DATA_FORMAT_BC1;
    else if (format == CMP_FORMAT_BC2)
        return BROTLIG_DATA_FORMAT_BC2;
    else if (format == CMP_FORMAT_BC3)
        return BROTLIG_DATA_FORMAT_BC3;
    else if (format == CMP_FORMAT_BC4 || format == CMP_FORMAT_BC4_S)
        return BROTLIG_DATA_FORMAT_BC4;
    else if (format == CMP_FORMAT_BC5 || format == CMP_FORMAT_BC5_S)
        return BROTLIG_DATA_FORMAT_BC5;

    return BROTLIG_DATA_FORMAT_UNKNOWN;
}

bool IsPreconditionFormat(CMP_FORMAT format)
{
    return GetBrotliGFormat(format) != BROTLIG_DATA_FORMAT_UNKNOWN;
}

char* GetLastErrorStr()
{
    return g_errorString;
}

uint32_t MaxCompressedSize(uint32_t uncompressedSize)
{
    return BrotliG::MaxCompressedSize(uncompressedSize);
}

bool EncodeDataStream(const CMP_BYTE* inputData, uint32_t inputSize, CMP_BYTE* outputData, uint32_t* outputSize, uint32_t pageSize, EncodeParameters params)
{
    BrotliG::BrotligDataconditionParams brlgParams = {};

    brlgParams.precondition   = params.precondition;
    brlgParams.format         = GetBrotliGFormat(params.format);
    brlgParams.widthInPixels  = params.textureWidth;
    brlgParams.heightInPixels = params.textureHeight;
    brlgParams.numMipLevels   = params.numMipmapLevels;
    brlgParams.swizzle        = params.doSwizzle;
    brlgParams.delta_encode   = params.doDeltaEncode;

    if (brlgParams.format == BROTLIG_DATA_FORMAT_UNKNOWN)
        brlgParams.precondition = false;

    BROTLIG_ERROR result = BrotliG::Encode(inputSize, inputData, outputSize, outputData, pageSize, brlgParams, nullptr);
    return result == BROTLIG_ERROR::BROTLIG_OK;
}

bool DecodeDataStreamCPU(const CMP_BYTE* inputData, uint32_t inputSize, CMP_BYTE* outputData, uint32_t* outputSize)
{
    BROTLIG_ERROR result = BrotliG::DecodeCPU(inputSize, inputData, outputSize, outputData, nullptr);
    return result == BROTLIG_ERROR::BROTLIG_OK;
}

bool DecodeDataStreamGPU(const CMP_BYTE* inputData, uint32_t inputSize, CMP_BYTE* outputData, uint32_t* outputSize)
{
    BrotligCompute decoder;

    if (!outputSize)
    {
        g_errorString = "Output size is required for BrotliG GPU decoding";
        return false;
    }

    if (!decoder.Setup(BROTLIG_GPUD_DEFAULT_MAX_STREAMS_PER_LAUNCH, BROTLIG_GPUD_DEFAULT_NUM_GROUPS))
    {
        g_errorString = "Failed to initialize Brotli-G GPU Decoder";
        return false;
    }

    uint32_t outputId = decoder.AddOutputBuffer(outputData);

    if (!decoder.AddInput(inputData, inputSize, *outputSize, outputId))
    {
        g_errorString = "Failed to add input data to Brotli-G GPU Decoder";
        return false;
    }

    if (!decoder.Execute())
    {
        g_errorString = "Brotli-G GPU Decoder failed during execution";
        return false;
    }

    decoder.RemoveOutputBuffer(outputId);
    decoder.ClearInputs();

    return true;
}

}  // namespace BRLG
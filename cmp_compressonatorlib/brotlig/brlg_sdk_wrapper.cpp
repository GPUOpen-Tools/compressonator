//===============================================================================
// Copyright (c) 2022  Advanced Micro Devices, Inc. All rights reserved.
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

namespace BRLG {

static char* brlg_error_str = nullptr;

char* GetLastErrorStr()
{
    return brlg_error_str;
}

uint32_t MaxCompressedSize(uint32_t uncompressedSize)
{
    return BrotliG::MaxCompressedSize(uncompressedSize);
}

bool EncodeDataStream(const CMP_BYTE* inputData, uint32_t inputSize, CMP_BYTE* outputData, uint32_t* outputSize, uint32_t pageSize)
{
    BROTLIG_ERROR result = BrotliG::Encode(inputSize, inputData, outputSize, outputData, nullptr);
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
        brlg_error_str = "Output size is required for BrotliG GPU decoding";
        return false;
    }
    
    if (!decoder.Setup(BROTLIG_GPUD_DEFAULT_MAX_STREAMS_PER_LAUNCH, BROTLIG_GPUD_DEFAULT_NUM_GROUPS))
    {
        brlg_error_str = "Failed to initialize Brotli-G GPU Decoder";
        return false;
    }

    uint32_t outputId = decoder.AddOutputBuffer(outputData);

    if (!decoder.AddInput(inputData, inputSize, *outputSize, outputId))
    {
        brlg_error_str = "Failed to add input data to Brotli-G GPU Decoder";
        return false;
    }

    if (!decoder.Execute())
    {
        brlg_error_str = "Brotli-G GPU Decoder failed during execution";
        return false;
    }

    decoder.RemoveOutputBuffer(outputId);
    decoder.ClearInputs();

    return true;
}

}
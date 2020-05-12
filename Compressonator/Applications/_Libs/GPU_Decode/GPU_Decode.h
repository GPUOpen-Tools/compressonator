#ifndef H_GPU_DECOMP
#define H_GPU_DECOMP

//=====================================================================
// Copyright (c) 2016    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file GPU_Decode.h
//
//=====================================================================

#include "Compressonator.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
/// Converts the compressed source texture to the decompressed destination texture
/// \param[in] pSourceTexture A pointer to the source texture.
/// \param[in] pDestTexture A pointer to the destination texture.
/// \param[in] Type of GPU drivers to use for decode
/// \return    CMP_OK if successful, otherwise the error code.

    CMP_ERROR CMP_API CMP_DecompressTexture(
        const CMP_Texture* pSourceTexture,
              CMP_Texture* pDestTexture,
              CMP_GPUDecode GPUDecodeType
        );

//
/// CMP_InitializeDecompessLibrary - Initialize the DeCompression library based in GPU Driver support types
/// \return    CMP_OK if successful, otherwise the error code.
//
    CMP_ERROR CMP_APICMP_InitializeDecompessLibrary(CMP_GPUDecode GPUDecodeType);

//
/// CMP_ShutdownDecompessLibrary - Shutdown the DeCompression library
/// \return    CMP_OK if successful, otherwise the error code.
//
    CMP_ERROR CMP_API CMP_ShutdownDecompessLibrary();
#endif
#ifdef __cplusplus
};
#endif


#endif // !H_GPU_DECOMP


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
/// \file GPU_Decode.cpp
//
//=====================================================================

#ifdef _WIN32
#include "gpu_decodebase.h"
#include "gpu_decode.h"
#include "cmp_plugininterface.h"

#include <windows.h>

extern PluginManager    g_pluginManager;

PluginInterface_GPUDecode   *g_GPUDecode_plugin = NULL;

static CMP_GPUDecode DecodeType = GPUDecode_INVALID;

//
// CMP_InitializeDecompessLibrary - Initialize the DeCompression library based in GPU Driver support types
//
CMP_ERROR CMP_API CMP_InitializeDecompessLibrary(CMP_GPUDecode GPUDecodeType, CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback) {
    if (g_GPUDecode_plugin && (DecodeType == GPUDecodeType)) return CMP_OK;

    if (GPUDecodeType != DecodeType) {
        CMP_ShutdownDecompessLibrary();
    }

    switch (GPUDecodeType) {
    case GPUDecode_DIRECTX:
        g_GPUDecode_plugin = reinterpret_cast<PluginInterface_GPUDecode *>(g_pluginManager.GetPlugin("GPUDECODE", "DIRECTX"));
        break;
    case GPUDecode_OPENGL:
        g_GPUDecode_plugin = reinterpret_cast<PluginInterface_GPUDecode *>(g_pluginManager.GetPlugin("GPUDECODE", "OPENGL"));
        break;
    case GPUDecode_VULKAN:
        g_GPUDecode_plugin = reinterpret_cast<PluginInterface_GPUDecode *>(g_pluginManager.GetPlugin("GPUDECODE", "VULKAN"));
        break;
    default:
        return CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB;
    }

    if (g_GPUDecode_plugin) {
        if (g_GPUDecode_plugin->TC_Init(Width, Height, callback) != 0)
            return CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB;
    } else return CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB;

    return CMP_OK;
}


//
// CMP_ShutdownDecompessLibrary - Shutdown the DeCompression library
//
CMP_ERROR CMP_API CMP_ShutdownDecompessLibrary() {
    if (g_GPUDecode_plugin) {
        g_GPUDecode_plugin->TC_Close();
        delete g_GPUDecode_plugin;
        g_GPUDecode_plugin = NULL;
    }

    return CMP_OK;
}


CMP_ERROR CMP_API CMP_DecompressTexture(
    const CMP_Texture* pSourceTexture,
    CMP_Texture* pDestTexture,
    CMP_GPUDecode GPUDecodeType) {
    CMP_ERROR result;

    // This is temporary code we should move this into CLI and GUI
    result = CMP_InitializeDecompessLibrary(GPUDecodeType, pSourceTexture->dwWidth, pSourceTexture->dwHeight, NULL);
    if (result  != CMP_OK) return (result);

    if (g_GPUDecode_plugin) {
        result = g_GPUDecode_plugin->TC_Decompress(pSourceTexture, pDestTexture);
        if (result != CMP_OK) return (result);
    } else return CMP_ABORTED;


    return CMP_OK;
}
#endif

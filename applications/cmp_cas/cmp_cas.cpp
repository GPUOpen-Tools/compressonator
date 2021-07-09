//=============================================================================
// Copyright (c) 2021  Advanced Micro Devices, Inc. All rights reserved.
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
//==============================================================================


#include <Windows.h>
#include <atlbase.h>      // CComPtr
#include <d3d11.h>

#include <string>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include <cstdio>
#include <cassert>

#define A_CPU 1
#define USE_CMP_FIDELITY_FX_H
#include "common_def.h"
#include "ffx_cas.h"

#include "compressonator.h"

void PrintCommandLineSyntax()
{
    printf("Command line syntax:\n");
    printf("cmpcas.exe  [Options] SrcFile DstFile\n");
    printf("Options:\n");
    printf("  -scale <DstWidth> <DstHeight>\n");
    printf("  -sharpness <Value>\n");
    printf("   sharpness for CAS, between 0 (default) and 1.\n");
}

void ParseCommandLine(int argCount, char* args[], std::string& srcFilePath, std::string& dstFilePath, CMP_CFilterParams& params)
{
    int i = 0;

    // Options
    for (; i < argCount && args[i][0] == L'-'; ++i)
    {
        if (strcmp(args[i], "-scale") == 0 && i + 2 < argCount)
        {
            params.destWidth  = (uint32_t)std::stoi(args[++i]);
            params.destHeight = (uint32_t)std::stoi(args[++i]);
        }
        else if (strcmp(args[i], "-sharpness") == 0 && i + 1 < argCount)
        {
            params.fSharpness = (float)std::stof(args[++i]);
        }
        else
        {
            throw std::runtime_error("Unknown command line option.");
        }
    }

    // Files
    if ((argCount - i) % 2)
    {
        throw std::runtime_error("Invalid command line syntax.");
    }

    srcFilePath = args[i];
    dstFilePath = args[i + 1];
}

#include "pluginmanager.h"
#include "plugininterface.h"

int main(int argc, char* argv[])
{
    PluginManager g_pluginManager;
    g_pluginManager.getPluginList("/plugins", true);


    CMP_MipSet       srcMipSet;
    CMP_MipSet       destMipSet;
    CMP_ERROR        ressrc = CMP_ERR_GENERIC;
    CMP_ERROR        resdst = CMP_ERR_GENERIC;
    std::string      srcFilePath;
    std::string      dstFilePath;
    PluginInterface_Filters *cmp_cfx = NULL;

    try
    {
        if (argc <= 1)
        {
            PrintCommandLineSyntax();
            return 1;
        }

        CMP_CFilterParams pCFilterParams;
        ParseCommandLine(argc - 1, argv + 1, srcFilePath, dstFilePath, pCFilterParams );

        cmp_cfx = reinterpret_cast<PluginInterface_Filters*>(g_pluginManager.GetPlugin("FILTERS", "EFFECTS"));
        if (!cmp_cfx) {
            printf("Error Loading CAS Filter Effects plugin\n");
            return -1;
        }

        printf("Loading %s\n", srcFilePath.c_str());

        memset(&srcMipSet, 0, sizeof(CMP_MipSet));
        ressrc = CMP_LoadTexture(srcFilePath.c_str(), &srcMipSet);
        if (ressrc != CMP_OK) {
            throw("Loading %s\n", srcFilePath.c_str());
        }

        if (!CasSupportScaling((float)pCFilterParams.destWidth, (float)pCFilterParams.destHeight, (float)srcMipSet.m_nWidth, (float)srcMipSet.m_nHeight))
        {
            printf("WARNING: Scaling factor is greater than recommended %g.\n", CAS_AREA_LIMIT);
        }

        memset(&destMipSet, 0, sizeof(CMP_MipSet));
        destMipSet.m_format = srcMipSet.m_format;
        CMP_ERROR err       = CMP_CreateMipSet(&destMipSet, pCFilterParams.destWidth, pCFilterParams.destHeight, 1, CF_8bit, TT_2D);
        if (err != CMP_OK)
            throw("Unable to create target texture\n");

        if (cmp_cfx->TC_CFilter(&srcMipSet, &destMipSet, &pCFilterParams) != CMP_OK)
                throw("Unable to process Texture CAS\n");
 
        printf("Saving %s\n",dstFilePath.c_str());
        err = CMP_SaveTexture(dstFilePath.c_str(), &destMipSet);
        if (err != CMP_OK)
            throw("Error saving destination image %s\n", dstFilePath.c_str());

        CMP_FreeMipSet(&destMipSet);
        CMP_FreeMipSet(&srcMipSet);

        if (cmp_cfx)
            delete cmp_cfx;

        return 0;
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "ERROR: %s\n", ex.what());
        return -1;
    }

    if (ressrc == CMP_OK)
        CMP_FreeMipSet(&srcMipSet);
    if (resdst == CMP_OK)
        CMP_FreeMipSet(&destMipSet);
    if (cmp_cfx)
            delete cmp_cfx;
}

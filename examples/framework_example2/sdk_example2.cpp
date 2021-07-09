// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved
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

// Example2
// Sample application to process a image file using a HPC host with a BC7 encoder
//
// SDK files required for application:
//     compressonator.h
//     CMP_Framework_xx.lib  For static libs xx is either MD, MT or MDd or MTd,
//                      When using DLL's make sure the CMP_Framework_xx_DLL.dll is in exe path


#include <stdio.h>
#include <string>

#include "compressonator.h"

#ifdef _WIN32
#include <windows.h>
#include <time.h>
double timeStampsec() {
    static LARGE_INTEGER frequency;
    if (frequency.QuadPart == 0)
        QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return now.QuadPart / double(frequency.QuadPart);
}
#endif

bool g_bAbortCompression = false;   // If set true current compression will abort

//---------------------------------------------------------------------------
// Sample loop back code called for each compression block been processed
//---------------------------------------------------------------------------
bool CompressionCallback(CMP_FLOAT fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    (pUser1);
    (pUser2);

    std::printf("\rCompression progress = %2.0f  ", fProgress);

    return g_bAbortCompression;
}

int main(int argc, char* argv[]) {
    bool            bAbortCompression = false;

    if (argc < 4) {
        std::printf("Example2.exe SourceFile DestFile Quality\n");
        std::printf("This example shows how to compress a single image\n");
        std::printf("to a compression format using a quality setting with HPC BC7 encoding\n");
        std::printf("Quality is in the range of 0.0 to 1.0\n");
        std::printf("When using DLL builds make sure the  CMP_Framework_xx_DLL.dll is in exe path\n");
        std::printf("usage: Example2.exe ruby.dds ruby_bc7.dds 1.0\n");
        std::printf("this will generate a high quality compressed ruby file in BC7 format\n");
        return 0;
    }

    // please note the params are not checked for errors
    const char*     pszSourceFile = argv[1];
    const char*     pszDestFile   = argv[2];
    CMP_FLOAT fQuality;

    try {
        fQuality = std::stof(argv[3]);
        if (fQuality < 0.0f) {
            fQuality = 0.0f;
            std::printf("Warning: Quality setting is out of range using 0.0\n");
        }
        if (fQuality > 1.0f) {
            fQuality = 1.0f;
            std::printf("Warning: Quality setting is out of range using 1.0\n");
        }
    } catch (...) {
        std::printf("Error: Unable to process quality setting\n");
        return -1;
    }

    //=================================================================================
    // You can optionally use a 4th argv to set a destFormat other then the example BC7
    // just add additional encoder dll's is in the example binary path.
    // example: to use BC1 add CMP_BC1_MD.dll.
    // You can edit CopyFiles.bat to add additional libs to the correct build folders of this application.
    //
    // CMP_FORMAT      destFormat    = CMP_ParseFormat(argv[4]);
    //===================================================================================
    CMP_FORMAT      destFormat = CMP_FORMAT_BC7;

    //-------------------------------------------------------------------------------------------------------
    // Load the image, CMP_LoadTexture supports DDS, std_image and all compressonator runtime image plugins
    //-------------------------------------------------------------------------------------------------------
    CMP_MipSet MipSetIn;
    memset(&MipSetIn, 0, sizeof(CMP_MipSet));
    if (CMP_LoadTexture(pszSourceFile, &MipSetIn) != CMP_OK) {
        std::printf("Error: Loading source file!\n");
        return -1;
    }

    //----------------------------------------------------------------------------------------------------------
    // Set the target compression format and the host framework to use
    // For this example HPC is been used. HPC uses CPU based multi-threading that will:
    // auto detect CPU isa type and use the appropiate optimized instruction sets (SSEn, AVXn, ...)
    // Users can select the total number of threads assigned to CPU processor cores using KernalOptions::Threads
    // when using GPU: The texture must have width and height as a multiple of 4
    //-----------------------------------------------------------------------------------------------------------
    KernelOptions   kernel_options;
    memset(&kernel_options, 0, sizeof(KernelOptions));

    kernel_options.encodeWith   = CMP_HPC;      // Using HPC
    kernel_options.format       = destFormat;   // Set the format to process
    kernel_options.fquality     = fQuality;     // Set the quality of the result
    kernel_options.threads      = 0;            // Auto setting

    // Setup a results buffer for the processed file,
    // the content will be set after the source texture is processed
    CMP_MipSet MipSetCmp;
    memset(&MipSetCmp, 0, sizeof(CMP_MipSet));

    //----------------------------------------------------------------
    // Compress the source image
    //----------------------------------------------------------------
#ifdef _WIN32
    double process_start_time = timeStampsec();
#endif

    CMP_ERROR   cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp,kernel_options,CompressionCallback);
    if (cmp_status != CMP_OK) {
        CMP_FreeMipSet(&MipSetIn);
        std::printf("Error %d: Processing texture file\n",cmp_status);
        return -1;
    }


#ifdef _WIN32
    double process_end_time = timeStampsec();
#endif

    //--------------------------
    // Save the result to a file
    //--------------------------
    cmp_status = CMP_SaveTexture(pszDestFile, &MipSetCmp);

    //-----------------
    // Clean up buffers
    //-----------------
    CMP_FreeMipSet(&MipSetIn);
    CMP_FreeMipSet(&MipSetCmp);


    if (cmp_status != CMP_OK) {
        std::printf("Error %d: Saving processed file %s!\n",cmp_status,pszDestFile);
        return -1;
    }

#ifdef _WIN32
    std::printf("\nProcessed in %.3f seconds\n", process_end_time - process_start_time);
#else
    std::printf("Process Done \n");
#endif

    return 0;
}

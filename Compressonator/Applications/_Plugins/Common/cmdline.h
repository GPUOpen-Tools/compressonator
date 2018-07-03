//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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

#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif

#include <string>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "Compressonator.h"
#include "GPU_Decode/GPU_Decode.h"

#include "MIPS.h"

class CCmdLineParamaters
{
   public:
    // Set defaults
    CCmdLineParamaters()
    {
        SetDefault();
    };

    void SetDefault()
    {
        SourceFile           = "";
        DestFile             = "";
        DecompressFile       = "";
        use_WIC              = false;
        use_OCV              = false;
        use_WIC_out          = false;
        use_OCV_out          = false;
        use_noMipMaps        = false;
        use_Draco_Encode     = false;
        doMeshOptimize       = false;
        dwWidth              = 0;
        dwHeight             = 0;
        nMinSize             = 0;
        MipsLevel            = 1;
        silent               = false;
        noswizzle            = false;
        doswizzle            = false;
        doDecompress         = false;
        analysis             = false;
        diffImage            = false;
        BlockWidth           = 4;
        BlockHeight          = 4;
        BlockDepth           = 1;
        conversion_fDuration = 0;
        memset(&CompressOptions, 0, sizeof(CompressOptions));
        CompressOptions.dwSize            = sizeof(CompressOptions);
        CompressOptions.nCompressionSpeed = (CMP_Speed)CMP_Speed_Normal;
        CompressOptions.dwnumThreads      = 8;

        CompressOptions.iCmpLevel    = CMP_MESH_COMP_LEVEL;
        CompressOptions.iPosBits     = CMP_MESH_POS_BITS;
        CompressOptions.iTexCBits    = CMP_MESH_TEXC_BITS;
        CompressOptions.iNormalBits  = CMP_MESH_NORMAL_BITS;
        CompressOptions.iGenericBits = CMP_MESH_GENERIC_BITS;

#ifdef USE_3DMESH_OPTIMIZE
        CompressOptions.iVcacheSize     = CMP_MESH_VCACHE_SIZE_DEFAULT;
        CompressOptions.iVcacheFIFOSize = CMP_MESH_VCACHEFIFO_SIZE_DEFAULT;
        CompressOptions.fOverdrawACMR   = CMP_MESH_OVERDRAW_ACMR_DEFAULT;
        CompressOptions.iSimplifyLOD    = CMP_MESH_SIMPLIFYMESH_LOD_DEFAULT;
        CompressOptions.bVertexFetch    = CMP_MESH_OPTVFETCH_DEFAULT;
#endif

        CompressOptions.fquality       = AMD_CODEC_QUALITY_DEFAULT;  // This default value is also ref by code in SDK Compress.cpp
        CompressOptions.fInputDefog    = AMD_CODEC_DEFOG_DEFAULT;
        CompressOptions.fInputExposure = AMD_CODEC_EXPOSURE_DEFAULT;
        CompressOptions.fInputKneeLow  = AMD_CODEC_KNEELOW_DEFAULT;
        CompressOptions.fInputKneeHigh = AMD_CODEC_KNEEHIGH_DEFAULT;
        CompressOptions.fInputGamma    = AMD_CODEC_GAMMA_DEFAULT;
        CompressOptions.dwmodeMask     = 0xCF;  // If you reset this default: seach for comments with dwmodeMask and change the values also
        DestFormat                     = CMP_FORMAT_Unknown;
        SourceFormat                   = CMP_FORMAT_Unknown;
    }

   public:
    std::string         SourceFile;            //
    std::string         DestFile;              //
    std::string         DiffFile;              // Diff image file name
    std::string         DecompressFile;        //
    CMP_FORMAT          SourceFormat;          //
    CMP_FORMAT          DestFormat;            //
    CMP_CompressOptions CompressOptions;       //
    CMP_DWORD           dwWidth;               //
    CMP_DWORD           dwHeight;              //
    double              conversion_fDuration;  // Total Performance time
    int                 MipsLevel;             //
    int                 nMinSize;              //
    bool                doDecompress;          //
    bool                noswizzle;             //
    bool                doswizzle;             //
    bool                silent;                //
    bool                analysis;              //  run analysis
    bool                diffImage;             //  generate diff image
    bool imageprops;        //  print image properties (i.e. image name, path, file size, image size, image width, height, miplevel and format)
    bool showperformance;   //
    bool noprogressinfo;    //
    bool doMeshOptimize;    //  mesh optimization
    bool use_Draco_Encode;  //  draco compression
    bool use_noMipMaps;     //  use of image loads based on Open CV Components in place of raw image plugins for write to file
    bool use_WIC;           //  use of image loads based on Windows Imagaing Components in place of raw image plugins for read from file
    bool use_OCV;           //  use of image loads based on Open CV Components in place of raw image plugins  for read from file
    bool use_WIC_out;       //  use of image loads based on Windows Imagaing Components in place of raw image plugins  for write to file
    bool use_OCV_out;       //  use of image loads based on Open CV Components in place of raw image plugins for write to file
    int  BlockWidth;        // Width (xdim) in pixels of the Compression Block that is to be processed default for ASTC is 4
    int  BlockHeight;       // Height (ydim)in pixels of the Compression Block that is to be processed default for ASTC is 4
    int  BlockDepth;        // Depth  (zdim)in pixels of the Compression Block that is to be processed default for ASTC is 1
};

extern void               PrintInfo(const char* Format, ...);
extern bool               ParseParams(int argc, CMP_CHAR* argv[]);
extern int                ProcessCMDLine(CMP_Feedback_Proc pFeedbackProc, MipSet* userMips);
extern CCmdLineParamaters g_CmdPrams;
extern int                GetNumberOfCores(wchar_t* envp[]);
extern bool               SouceAndDestCompatible(CCmdLineParamaters g_CmdPrams);
extern int                DecompressCMDLine(CMP_Feedback_Proc pFeedbackProc, MipSet* userMips);

#endif

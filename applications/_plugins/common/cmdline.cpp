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

#ifdef _MSC_VER
#pragma warning(disable : 4996)            // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

#include "cmdline.h"

#include "atiformats.h"
#include "texture.h"
#include "textureio.h"
#include "pluginmanager.h"
#include "plugininterface.h"
#include "tc_plugininternal.h"
#include "version.h"
#include "misc.h"
#include "cmp_fileio.h"

#include <string>
#include <algorithm>

#ifdef USE_MESH_CLI
#include <gltf/tiny_gltf2.h>
#include <gltf/cmp_gltfcommon.h>
#include "modeldata.h"
#include "utilfuncs.h"

using namespace tinygltf2;

#if (LIB_BUILD_MESHCOMPRESSOR == 1)
#ifdef _DEBUG
#pragma comment(lib, "CMP_MeshCompressor_MDd.lib")
#else
#pragma comment(lib, "CMP_MeshCompressor_MD.lib")
#endif
#endif

#ifdef USE_MESHOPTIMIZER
#include "mesh_optimizer.h"
#else
#include "mesh_tootle.h"
#endif
#endif

#ifdef USE_CMP_TRANSCODE
#include "cmp_transcoder/transcoders.h"
#endif

// #define SHOW_PROCESS_MEMORY

#define USE_SWIZZLE

#ifndef _WIN32
#include <fcntl.h> /* For O_RDWR */
#include <stb_image.h>
#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include <unistd.h> /* For open(), creat() */
#endif

#ifdef SHOW_PROCESS_MEMORY
#include "windows.h"
#include "psapi.h"
#endif

#include <sstream>

using namespace std;

CCmdLineParamaters g_CmdPrams;

static inline void helper_erase_all(std::string& str, const char* pErase)
{
    size_t pos = str.find(pErase);
    while (pos != std::string::npos)
    {
        str.erase(pos, 1);
        pos = str.find(pErase);
    }
}

static inline void helper_to_upper(std::string& str)
{
    for (char& i : str)
        i = toupper(i);
}

string DefaultDestination(string SourceFile, CMP_FORMAT DestFormat, string DestFileExt, CMP_BOOL suffixDestFormat)
{
    string    DestFile = "";
    string    file_name = CMP_GetJustFileName(SourceFile);
    string    file_ext  = CMP_GetJustFileExt(SourceFile);

    DestFile.append(file_name);

    if (suffixDestFormat)
    {
        DestFile.append("_");
        file_ext.erase(std::remove(file_ext.begin(), file_ext.end(), '.'), file_ext.end());
        DestFile.append(file_ext);
        if ((DestFormat != CMP_FORMAT_BROTLIG) && (DestFormat != CMP_FORMAT_Unknown))
        {
            DestFile.append("_");
            DestFile.append(GetFormatDesc(DestFormat));
        }
    }
    else
        file_ext.erase(std::remove(file_ext.begin(), file_ext.end(), '.'), file_ext.end());

    if (DestFileExt.find('.') != std::string::npos)
    {
        DestFile.append(DestFileExt);
    }
    else
    {
#if (OPTION_BUILD_ASTC == 1)
        if (DestFormat == CMP_FORMAT_ASTC)
            DestFile.append(".astc");
        else
#endif
        if (DestFormat == CMP_FORMAT_BROTLIG)
            DestFile.append(".brlg");
        else
        {
            DestFile.append(".dds");
        }
    }

    return DestFile;
}

// check if file is glTF format extension
bool fileIsGLTF(string SourceFile)
{
    string    file_ext  = CMP_GetJustFileExt(SourceFile);
    helper_to_upper(file_ext);
    return (file_ext.compare(".GLTF") == 0);
}

// check if file is OBJ format extension
bool fileIsOBJ(string SourceFile)
{
    string file_ext = CMP_GetJustFileExt(SourceFile);
    helper_to_upper(file_ext);
    return (file_ext.compare(".OBJ") == 0);
}

// check if file is DRC (draco compressed OBJ file) format extension
bool fileIsDRC(string SourceFile)
{
    string file_ext = CMP_GetJustFileExt(SourceFile);
    helper_to_upper(file_ext);
    return (file_ext.compare(".DRC") == 0);
}

bool fileIsModel(string SourceFile)
{
    return (fileIsGLTF(SourceFile) || fileIsOBJ(SourceFile) || fileIsDRC(SourceFile));
}

static inline bool IsProcessingBRLG(const CCmdLineParamaters& params)
{
    std::string fileExt = CMP_GetJustFileExt(params.SourceFile);
    helper_to_upper(fileExt);

    return params.CompressOptions.DestFormat == CMP_FORMAT_BROTLIG || fileExt.compare(".BRLG") == 0;
}

CMP_GPUDecode DecodeWith(const char* strParameter)
{
    if (strcmp(strParameter, "DirectX") == 0)
        return GPUDecode_DIRECTX;
    else if (strcmp(strParameter, "DXC") == 0)
        return GPUDecode_OPENGL;
    else if (strcmp(strParameter, "OpenGL") == 0)
        return GPUDecode_OPENGL;
    else if (strcmp(strParameter, "OGL") == 0)
        return GPUDecode_OPENGL;
    else if (strcmp(strParameter, "Vulkan") == 0)
        return GPUDecode_VULKAN;
    else if (strcmp(strParameter, "VLK") == 0)
        return GPUDecode_VULKAN;
    else
        return GPUDecode_INVALID;
}

CMP_Compute_type EncodeWith(const char* strParameter)
{
    if (strcmp(strParameter, "CPU") == 0)
        return CMP_CPU;
    else if (strcmp(strParameter, "HPC") == 0)
        return CMP_HPC;
    else if (strcmp(strParameter, "GPU") == 0)
        return CMP_GPU_HW;
    else if (strcmp(strParameter, "OCL") == 0)
        return CMP_GPU_OCL;
    else if (strcmp(strParameter, "DXC") == 0)
        return CMP_GPU_DXC;
    else if (strcmp(strParameter, "VLK") == 0)
        return CMP_GPU_VLK;
    else
        return CMP_UNKNOWN;
}

bool ProcessSingleFlags(const char* strCommand)
{
    bool isSet = false;

    if ((strcmp(strCommand, "-nomipmap") == 0))
    {
        g_CmdPrams.use_noMipMaps = true;
        isSet                    = true;
        if (g_CmdPrams.MipsLevel > 1)
            throw "Both nomipmap and miplevels are set!";
    }
#ifdef USE_MESH_DRACO_EXTENSION
    else if ((strcmp(strCommand, "-draco") == 0))
    {
        g_CmdPrams.use_Draco_Encode = true;
        isSet                       = true;
    }
#endif
    else if ((strcmp(strCommand, "-silent") == 0))
    {
        g_CmdPrams.silent = true;
        isSet             = true;
    }
    else if ((strcmp(strCommand, "-performance") == 0))
    {
        g_CmdPrams.showperformance = true;
        isSet                      = true;
    }
    else if ((strcmp(strCommand, "-noprogress") == 0))
    {
        g_CmdPrams.noprogressinfo = true;
        isSet                     = true;
    }
    else if ((strcmp(strCommand, "-noswizzle") == 0))
    {
        g_CmdPrams.noswizzle = true;
        isSet                = true;
    }
    else if ((strcmp(strCommand, "-doswizzle") == 0))
    {
        g_CmdPrams.doswizzle = true;
        isSet                = true;
    }
    else if ((strcmp(strCommand, "-analysis") == 0) || (strcmp(strCommand, "-Analysis") == 0))
    {
        g_CmdPrams.analysis = true;
        isSet               = true;
    }
    else if ((strcmp(strCommand, "-diff_image") == 0))
    {
        g_CmdPrams.diffImage = true;
        isSet                = true;
    }
    else if ((strcmp(strCommand, "-imageprops") == 0))
    {
        g_CmdPrams.imageprops = true;
        isSet                 = true;
    }
#ifdef USE_3DMESH_OPTIMIZE
    else if ((strcmp(strCommand, "-meshopt") == 0))
    {
        g_CmdPrams.doMeshOptimize = true;
        isSet                     = true;
    }
#endif
    else if (strcmp(strCommand, "-compress-gltf-images") == 0)
    {
        g_CmdPrams.compressImagesFromGLTF = true;
        isSet                             = true;
    }
    else if ((strcmp(strCommand, "-log") == 0))
    {
        g_CmdPrams.logcsvformat     = false;
        g_CmdPrams.logresultsToFile = true;
        g_CmdPrams.logresults       = true;
        isSet                       = true;
        g_CmdPrams.LogProcessResultsFile.assign(LOG_PROCESS_RESULTS_FILE_TXT);
    }
    else if ((strcmp(strCommand, "-logcsv") == 0))
    {
        g_CmdPrams.logcsvformat     = true;
        g_CmdPrams.logresultsToFile = true;
        g_CmdPrams.logresults       = true;
        isSet                       = true;
        g_CmdPrams.LogProcessResultsFile.assign(LOG_PROCESS_RESULTS_FILE_CSV);
    }
    else if (strcmp(strCommand, "-UseGPUDecompress") == 0)
    {
        g_CmdPrams.CompressOptions.bUseGPUDecompress = true;
        isSet                                        = true;
    }
#ifndef __linux__
    else if ((strcmp(strCommand, "-GenGPUMipMaps") == 0))
    {
        g_CmdPrams.CompressOptions.genGPUMipMaps = true;
        isSet                                    = true;
    }
#endif
#ifndef __linux__
    else if ((strcmp(strCommand, "-UseSRGBFrames") == 0))
    {
        g_CmdPrams.CompressOptions.useSRGBFrames = true;
        isSet                                    = true;
    }
#endif
    else if (strcmp(strCommand, "-UseMangledFileNames") == 0)
    {
        g_CmdPrams.mangleFileNames = true;
        isSet = true;
    }

    return isSet;
}

bool ProcessCMDLineOptions(const char* strCommand, const char* strParameter)
{
    try
    {
        // ====================================
        // Prechecks prior to sending to Codec
        // ====================================

        // Special Case for ASTC - improve this remove in next revision
        // This command param is passed down to ASTC Codex
        // We are doing an early capture to get some dimensions for ASTC file Save
#if (OPTION_BUILD_ASTC == 1)
        if (strcmp(strCommand, "-BlockRate") == 0)
        {
            if (strchr(strParameter, 'x') != NULL)
            {
                int dimensions = sscanf(strParameter, "%dx%dx", &g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight);
                if (dimensions < 2)
                    throw "Command Parameter is invalid";
                else
                {
                    //astc_find_closest_blockxy_2d(&g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight, 0);
                }
            }
            else
            {
                float m_target_bitrate = static_cast<float>(atof(strParameter));
                if (m_target_bitrate > 0)
                    find_closest_blockdim_2d(m_target_bitrate, &g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight, 0);
                else
                    throw "Command Parameter is invalid";
            }
        }
        else 
#endif
        if (strcmp(strCommand, "-Quality") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No Quality value specified";
            }
            float value = std::stof(strParameter);
            if ((value < 0) || (value > 1.0))
            {
                throw "Quality value should be in range of 0 to 1.0";
            }
            g_CmdPrams.CompressOptions.fquality = value;
        }
#ifdef ENABLE_MAKE_COMPATIBLE_API
        else if (strcmp(strCommand, "-InExposure") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No input exposure value specified";
            }
            float value = std::stof(strParameter);
            if ((value < -10) || (value > 10))
            {
                throw "Input Exposure value should be in range of -10 to 10";
            }
            g_CmdPrams.CompressOptions.fInputExposure = value;
        }
        else if (strcmp(strCommand, "-InDefog") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No input defog value specified";
            }
            float value = std::stof(strParameter);
            if ((value < 0) || (value > 0.01))
            {
                throw "Input Defog value should be in range of 0.0000 to 0.0100";
            }
            g_CmdPrams.CompressOptions.fInputDefog = value;
        }
        else if (strcmp(strCommand, "-InKneeLow") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No input kneelow value specified";
            }
            float value = std::stof(strParameter);
            if ((value < -3) || (value > 3))
            {
                throw "Input Knee Low value should be in range of -3 to 3";
            }
            g_CmdPrams.CompressOptions.fInputKneeLow = value;
        }
        else if (strcmp(strCommand, "-InKneeHigh") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No input kneehigh value specified";
            }
            float value = std::stof(strParameter);
            if ((value < 3.5) || (value > 7.5))
            {
                throw "Input Knee High value should be in range of 3.5 to 7.5";
            }
            g_CmdPrams.CompressOptions.fInputKneeHigh = value;
        }
        else if (strcmp(strCommand, "-Gamma") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No gamma value specified";
            }
            float value = std::stof(strParameter);
            if ((value < 1.0) || (value > 2.6))
            {
                throw "Gamma supported is in range of 1.0 to 2.6";
            }
            g_CmdPrams.CompressOptions.fInputGamma = value;
        }
        else if (strcmp(strCommand, "-FilterGamma") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No filter gamma value specified";
            }
            float value = std::stof(strParameter);
            if ((value < 1.0) || (value > 2.6))
            {
                throw "Filter gamma supported is in range of 1.0 to 2.6";
            }
            g_CmdPrams.CompressOptions.fInputFilterGamma = value;
        }
#endif
        else if (strcmp(strCommand, "-WeightR") == 0)
        {
            if (!g_CmdPrams.CompressOptions.bUseChannelWeighting)
            {
                throw "Please enable \"-UseChannelWeighting 1\" first before setting weight for color channel";
            }

            if (strlen(strParameter) == 0)
            {
                throw "No WeightR value specified";
            }
            float value = std::stof(strParameter);
            if ((value < 0) || (value > 1.0))
            {
                throw "WeightR value should be in range of 0 to 1.0";
            }
            g_CmdPrams.CompressOptions.fWeightingRed = value;
        }
        else if (strcmp(strCommand, "-WeightG") == 0)
        {
            if (!g_CmdPrams.CompressOptions.bUseChannelWeighting)
            {
                throw "Please enable \"-UseChannelWeighting 1\" first before setting weight for color channel";
            }
            if (strlen(strParameter) == 0)
            {
                throw "No WeightG value specified";
            }
            float value = std::stof(strParameter);
            if ((value < 0) || (value > 1.0))
            {
                throw "WeightG value should be in range of 0 to 1.0";
            }
            g_CmdPrams.CompressOptions.fWeightingGreen = value;
        }
        else if (strcmp(strCommand, "-WeightB") == 0)
        {
            if (!g_CmdPrams.CompressOptions.bUseChannelWeighting)
            {
                throw "Please enable \"-UseChannelWeighting 1\" first before setting weight for color channel";
            }
            if (strlen(strParameter) == 0)
            {
                throw "No WeightB value specified";
            }
            float value = std::stof(strParameter);
            if ((value < 0) || (value > 1.0))
            {
                throw "WeightB value should be in range of 0 to 1.0";
            }
            g_CmdPrams.CompressOptions.fWeightingBlue = value;
        }
        else if (strcmp(strCommand, "-UseChannelWeighting") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No UseChannelWeighting value specified (default is 0:off; 1:on)";
            }

            float value = std::stof(strParameter);
            if ((value < 0) || (value > 1))
            {
                throw "UseChannelWeighting value should be 1 or 0";
            }
            g_CmdPrams.CompressOptions.bUseChannelWeighting = bool(value);
        }
        else if (strcmp(strCommand, "-AlphaThreshold") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No Alpha threshold value specified";
            }

            int value = std::stoi(strParameter);
            if ((value < 1) || (value > 255))
            {
                throw "Alpha threshold value should be in range of 1 to 255";
            }
            g_CmdPrams.CompressOptions.nAlphaThreshold = value;
             g_CmdPrams.CompressOptions.bDXT1UseAlpha = true;
        }
        else if (strcmp(strCommand, "-DXT1UseAlpha") == 0) // Legacy will be removed! use AlphaThreshold > 0 to set
        {
            if (strlen(strParameter) == 0)
            {
                throw "No DXT1UseAlpha value specified (default is 0:off; 1:on)";
            }
            int value = std::stoi(strParameter);
            if ((value < 0) || (value > 1))
            {
                throw "DXT1UseAlpha value should be 1 or 0";
            }
            g_CmdPrams.CompressOptions.bDXT1UseAlpha   = bool(value);
        }
        else if (strcmp(strCommand, "-RefineSteps") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No RefineSteps value specified (To enable set value to 1 or 2)";
            }
            int value = std::stoi(strParameter);
            if ((value < 1) || (value > 2))
            {
                throw "RefineSteps valid range is 1 or 2";
            }
            g_CmdPrams.CompressOptions.bUseRefinementSteps = true;
            g_CmdPrams.CompressOptions.nRefinementSteps     = value;
        }
        else if (strcmp(strCommand, "-DecodeWith") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No API specified (set either OpenGL or DirectX (Default is OpenGL).";
            }

            g_CmdPrams.CompressOptions.nGPUDecode = DecodeWith((char*)strParameter);

            if (g_CmdPrams.CompressOptions.nGPUDecode == GPUDecode_INVALID)
            {
                throw "Unknown API format specified.";
            }

            g_CmdPrams.CompressOptions.bUseGPUDecompress = true;
        }
#ifdef USE_3DMESH_OPTIMIZE
        else if (strcmp(strCommand, "-optVCacheSize") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No hardware vertices cache size specified.";
            }

            int value = std::stoi(strParameter);
            if (value < 1)
            {
                throw "Hardware vertices cache size must be a positive value.";
            }
            g_CmdPrams.CompressOptions.iVcacheSize = value;
        }
        else if (strcmp(strCommand, "-optVCacheFIFOSize") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No hardware vertices cache(FIFO replacement policy) size specified.";
            }

            int value = std::stoi(strParameter);
            if (value < 1)
            {
                throw "Hardware vertices cache(FIFO replacement policy) size should be > 1.";
            }
            g_CmdPrams.CompressOptions.iVcacheFIFOSize = value;
        }
        else if (strcmp(strCommand, "-optOverdrawACMRThres") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No ACMR threshold value specified for overdraw optimization.";
            }

            float value = std::stof(strParameter);
            if ((value < 1) || (value > 3))
            {
                throw "ACMR (Average Cache Miss Ratio) should be in the range of 1 - 3.";
            }
            g_CmdPrams.CompressOptions.fOverdrawACMR = value;
        }
        else if (strcmp(strCommand, "-simplifyMeshLOD") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No LOD (Level of Details) specified for mesh simplication.";
            }

            int value = std::stoi(strParameter);
            if (value < 1)
            {
                throw "LOD (Level of Details) for mesh simplication should be > 1.";
            }
            g_CmdPrams.CompressOptions.iSimplifyLOD = value;
        }
        else if (strcmp(strCommand, "-optVFetch") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No boolean value (1 or 0) specified to enable vertex fetch optimization or not.";
            }

            int value = std::stoi(strParameter);
            if ((value < 0) || (value > 1))
            {
                throw "Optimize vertex fetch value should be 1(enabled) or 0(disabled).";
            }
            g_CmdPrams.CompressOptions.bVertexFetch = bool(value);
        }
#endif
        else if (strcmp(strCommand, "-EncodeWith") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No API specified.";
            }

            g_CmdPrams.CompressOptions.nEncodeWith = EncodeWith((char*)strParameter);

            if (g_CmdPrams.CompressOptions.nEncodeWith == CMP_Compute_type::CMP_UNKNOWN)
            {
                g_CmdPrams.CompressOptions.nEncodeWith    = CMP_Compute_type::CMP_CPU;
                g_CmdPrams.CompressOptions.bUseCGCompress = false;
                PrintInfo("Unknown EncodeWith format specified. CPU will be used!");
            }
            else if ((g_CmdPrams.CompressOptions.nEncodeWith == CMP_Compute_type::CMP_CPU)
                     //|| (g_CmdPrams.CompressOptions.nEncodeWith == CMP_Compute_type::CMP_HPC)
            )
                g_CmdPrams.CompressOptions.bUseCGCompress = false;
            else
                g_CmdPrams.CompressOptions.bUseCGCompress = true;
        }
        else if (strcmp(strCommand, "-decomp") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no destination file specified";
            }
            g_CmdPrams.DecompressFile = (char*)strParameter;
            if (g_CmdPrams.DestFile.length() == 0)
                g_CmdPrams.DestFile = g_CmdPrams.DecompressFile;
            g_CmdPrams.doDecompress = true;
        }
#ifdef USE_MESH_DRACO_EXTENSION
#ifdef USE_MESH_DRACO_SETTING
        else if (strcmp(strCommand, "-dracolvl") == 0)
        {  //draco compression level
            if (strlen(strParameter) == 0)
            {
                throw "No draco compression level specified";
            }
            int value = std::stoi(strParameter);
            if ((value < 0) || (value > 10))
            {
                throw "draco compression level supported is in range of 0 to 10";
            }
            g_CmdPrams.CompressOptions.iCmpLevel = value;
        }
        else if (strcmp(strCommand, "-qpos") == 0)
        {  //quantization bit for positon
            if (strlen(strParameter) == 0)
            {
                throw "No quantization bits for position specified";
            }
            int value = std::stoi(strParameter);
            if ((value < 1) || (value > 30))
            {
                throw "quantization bits for position supported is in range of 1 to 30";
            }
            g_CmdPrams.CompressOptions.iPosBits = value;
        }
        else if (strcmp(strCommand, "-qtexc") == 0)
        {  //quantization bit for texture coordinates
            if (strlen(strParameter) == 0)
            {
                throw "No quantization bits for texture coordinates specified";
            }
            int value = std::stoi(strParameter);
            if ((value < 1) || (value > 30))
            {
                throw "quantization bits for texture coordinates supported is in range of 1 to 30";
            }
            g_CmdPrams.CompressOptions.iTexCBits = value;
        }
        else if (strcmp(strCommand, "-qnorm") == 0)
        {  //quantization bit for normal
            if (strlen(strParameter) == 0)
            {
                throw "No quantization bits for normal specified";
            }
            int value = std::stoi(strParameter);
            if ((value < 1) || (value > 30))
            {
                throw "quantization bits for normal supported is in range of 1 to 30";
            }
            g_CmdPrams.CompressOptions.iNormalBits = value;
        }
        else if (strcmp(strCommand, "-qgen") == 0)
        {  //quantization bit for generic
            if (strlen(strParameter) == 0)
            {
                throw "No quantization bits for generic (i.e. tangent) specified";
            }
            int value = std::stoi(strParameter);
            if ((value < 1) || (value > 30))
            {
                throw "quantization bits for generic (i.e. tangent) supported is in range of 1 to 30";
            }
            g_CmdPrams.CompressOptions.iGenericBits = value;
        }
#endif
#endif
        else if (strcmp(strCommand, "-logfile") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no log file specified";
            }
            g_CmdPrams.logcsvformat     = false;
            g_CmdPrams.logresults       = true;
            g_CmdPrams.logresultsToFile = true;
            g_CmdPrams.LogProcessResultsFile.assign((char*)strParameter);
        }
        else if (strcmp(strCommand, "-logcsvfile") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no log file specified";
            }
            g_CmdPrams.logcsvformat     = true;
            g_CmdPrams.logresults       = true;
            g_CmdPrams.logresultsToFile = true;
            g_CmdPrams.LogProcessResultsFile.assign((char*)strParameter);
        }
#ifdef CMP_ENABLE_LEGACY_OPTIONS
        else if (strcmp(strCommand, "-fs") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no format specified";
            }

            g_CmdPrams.CompressOptions.SourceFormat = CMP_ParseFormat((char*)strParameter);

            if (g_CmdPrams.CompressOptions.SourceFormat == CMP_FORMAT_Unknown)
            {
                throw "unknown format specified";
            }
        }
#endif
        else if ((strcmp(strCommand, "-ff") == 0) ||  //    FileFilter used for collecting list of source files in a given source Dir
                 (strcmp(strCommand, "-fx") == 0))    //and FileOutExt used for file output extension at given output Dir
        {  
            if (strlen(strParameter) == 0)
            {
                throw "no file filter specified";
            }

            std::string filterParameter = strParameter;
            std::transform(filterParameter.begin(), filterParameter.end(), filterParameter.begin(), ::toupper);

            std::string supportedExtensions = "DDS;KTX;KTX2;TGA;EXR;PNG;BMP;HDR;JPG;TIFF;TIF;PPM;BRLG";

            std::istringstream filterStream(filterParameter);

            std::string currentFilter;
            int filter_num = 0;
            while (getline(filterStream, currentFilter, ','))
            {
                filter_num++;
                if (supportedExtensions.find(currentFilter) == std::string::npos)
                {
                    char err[128];
                    sprintf(err, "[%s] file filter is not supported", currentFilter.c_str());
                    throw(err);
                }
                else if (filter_num > 1 && strcmp(strCommand, "-fx") == 0)
                {
                    throw "Too many arguments passed to -fx option. Only one file extension is supported for the output files.";
                }
            }

            if (strcmp(strCommand, "-ff") == 0)
            {
                g_CmdPrams.FileFilter = filterParameter;
            }
            else if (strcmp(strCommand, "-fx") == 0)
            {
                // change to lowercase and add dot
                std::transform(filterParameter.begin(), filterParameter.end(), filterParameter.begin(), ::tolower);
                g_CmdPrams.FileOutExt = "." + filterParameter;
            }
        }
        else if (strcmp(strCommand, "-fd") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no format specified";
            }

            g_CmdPrams.CompressOptions.DestFormat = CMP_ParseFormat((char*)strParameter);

            if (g_CmdPrams.CompressOptions.DestFormat == CMP_FORMAT_Unknown)
            {
                std::string tempParam(strParameter);

                // For the near future, this is a message to let users know that ASTC is no longer supported by default
                if (tempParam == "ASTC" || tempParam == "astc")
                    throw "ASTC format no longer supported";
                else
                    throw "unknown format specified";
            }
        }
        else if ((strcmp(strCommand, "-miplevels") == 0))
        {
            if (strlen(strParameter) == 0)
            {
                throw "no level is specified";
            }

            if (g_CmdPrams.use_noMipMaps == true)
                throw "Both miplevels and nomipmap are set!";

            try
            {
                g_CmdPrams.MipsLevel = std::stoi(strParameter);

                if ((g_CmdPrams.MipsLevel <= 0) || ((g_CmdPrams.MipsLevel > 20)))
                    throw "Invalid miplevel value specified, valid range is [1 to 20]";
            }
            catch (std::exception)
            {
                throw "conversion failed for miplevel value";
            }
        }
        else if ((strcmp(strCommand, "-mipsize") == 0))
        {
            if (strlen(strParameter) == 0)
            {
                throw "no size is specified";
            }

            try
            {
                g_CmdPrams.nMinSize = std::stoi(strParameter);
            }
            catch (std::exception)
            {
                throw "conversion failed for mipsize value";
            }

            g_CmdPrams.MipsLevel = 2;
        }
        else if (strcmp(strCommand, "-r") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no read codec framework is specified";
            }

            if (strcmp(strParameter, "ocv") == 0)
                g_CmdPrams.use_OCV = true;
            else
            {
                throw "unsupported codec framework is specified";
            }
        }
        else if (strcmp(strCommand, "-w") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no read codec framework is specified";
            }

            if (strcmp(strParameter, "ocv") == 0)
                g_CmdPrams.use_OCV_out = true;
            else
            {
                throw "unsupported codec framework is specified";
            }
        }
        else if ((strcmp(strCommand, "-version") == 0) || (strcmp(strCommand, "-v") == 0))
        {
            printf("version %d.%d.%d\n", VERSION_MAJOR_MAJOR, VERSION_MAJOR_MINOR, VERSION_MINOR_MAJOR);
            exit(0);
        }
        else if (strcmp(strCommand, "-NumThreads") == 0)
        {
            if (strlen(strParameter) == 0)
                throw "Number of threads not specified.";

            g_CmdPrams.CompressOptions.dwnumThreads = std::stoi(strParameter);

            // Add to the cmd set because codecs expect a NumThreads parameter to exist
            strcpy(g_CmdPrams.CompressOptions.CmdSet[g_CmdPrams.CompressOptions.NumCmds].strCommand, "NumThreads");
            strcpy(g_CmdPrams.CompressOptions.CmdSet[g_CmdPrams.CompressOptions.NumCmds].strParameter, strParameter);

            g_CmdPrams.CompressOptions.NumCmds++;
        }
        else
        {
            if ((strlen(strParameter) > 0) || (strCommand[0] == '-'))
            {
                // List of command the codecs use.
                // A call back loop should be used command for all codecs that can validate the seetings
                // For now we list all of the commands here to check. Prior to passing down to Codecs!
                if ((strcmp(strCommand, "-Quality") == 0) || (strcmp(strCommand, "-ModeMask") == 0) ||
                    (strcmp(strCommand, "-PatternRec") == 0) || (strcmp(strCommand, "-ColourRestrict") == 0) || (strcmp(strCommand, "-AlphaRestrict") == 0) ||
                    (strcmp(strCommand, "-AlphaThreshold") == 0) || (strcmp(strCommand, "-ImageNeedsAlpha") == 0) || (strcmp(strCommand, "-UseSSE2") == 0) ||
                    (strcmp(strCommand, "-DXT1UseAlpha") == 0) || (strcmp(strCommand, "-WeightR") == 0) || (strcmp(strCommand, "-WeightG") == 0) ||
                    (strcmp(strCommand, "-WeightB") == 0) || (strcmp(strCommand, "-3DRefinement") == 0) || (strcmp(strCommand, "-UseAdaptiveWeighting") == 0) ||
                    (strcmp(strCommand, "-UseChannelWeighting") == 0) || (strcmp(strCommand, "-RefinementSteps") == 0) || (strcmp(strCommand, "-PageSize") == 0) ||
                    (strcmp(strCommand, "-ForceFloatPath") == 0) || (strcmp(strCommand, "-CompressionSpeed") == 0) ||
                    (strcmp(strCommand, "-SwizzleChannels") == 0) || (strcmp(strCommand, "-CompressionSpeed") == 0) ||
                    (strcmp(strCommand, "-Performance") == 0) || (strcmp(strCommand, "-MultiThreading") == 0))
                {
                    // Reserved for future dev: command options passed down to codec levels
                    const char* str;
                    // strip leading - or --
                    str = strCommand;
                    if (strCommand[0] == '-')
                        str++;
                    if (strCommand[1] == '-')
                        str++;

                    if (strlen(str) > AMD_MAX_CMD_STR)
                    {
                        throw "Command option is invalid";
                    }

                    if (strlen(strParameter) > AMD_MAX_CMD_PARAM)
                    {
                        throw "Command Parameter is invalid";
                    }

                    strcpy(g_CmdPrams.CompressOptions.CmdSet[g_CmdPrams.CompressOptions.NumCmds].strCommand, str);
                    strcpy(g_CmdPrams.CompressOptions.CmdSet[g_CmdPrams.CompressOptions.NumCmds].strParameter, strParameter);

                    g_CmdPrams.CompressOptions.NumCmds++;
                }
                else
                    throw "Command option is invalid";
            }
            else
            {
                // Flags or Source and destination files specified
                if ((g_CmdPrams.SourceFile.length() == 0) && (g_CmdPrams.SourceFileList.size() == 0))
                {
                    CMP_PATHTYPES sourcePathType = CMP_PathType(strCommand);

                    if (sourcePathType == CMP_PATHTYPES::CMP_PATH_IS_FILE && CMP_FileExists(strCommand))
                        g_CmdPrams.SourceFile = strCommand;
                    else
                    {
                        // Assuming it was meant to be a directory

                        const std::string directory(CMP_GetFullPath(strCommand));

                        CMP_GetFilesInDirectory(directory, g_CmdPrams.SourceFileList, g_CmdPrams.FileFilter);

                        // feature for recursive  folder scans to get souce list to process, feature for v4.2
                        // g_CmdPrams.SourceDir = directory;
                        // CMP_GetAllDirFilesList(directory, g_CmdPrams.SourceFileList, g_CmdPrams.FileFilter);

                        if (g_CmdPrams.SourceFileList.size() > 0)
                        {
                            // Set the first file in the new list to SourceFile and delete it from the list
                            g_CmdPrams.SourceFile = g_CmdPrams.SourceFileList[0].c_str();
                            g_CmdPrams.SourceFileList.erase(g_CmdPrams.SourceFileList.begin());
                        }
                        else
                            throw "No files to process in source dir";
                    }
                }
                else  // now check for destination once source file or file list has been added
                if ((g_CmdPrams.DestFile.length() == 0) && (g_CmdPrams.SourceFile.length() || g_CmdPrams.SourceFileList.size()))
                {
                    CMP_PATHTYPES destPathType = CMP_PathType(strCommand);

                    // Check if destination is a file
                    if (destPathType == CMP_PATHTYPES::CMP_PATH_IS_FILE && !CMP_DirExists(strCommand))
                    {
                        g_CmdPrams.DestFile = strCommand;

                        string file_extension = CMP_GetJustFileExt(strCommand);

                        // User did not supply a destination extension default
                        if (file_extension.length() == 0)
                        {
                            if (g_CmdPrams.DestFile.length() == 0) 
                            {
                                g_CmdPrams.DestFile =
                                    DefaultDestination(g_CmdPrams.SourceFile, g_CmdPrams.CompressOptions.DestFormat, g_CmdPrams.FileOutExt, g_CmdPrams.mangleFileNames);
                                PrintInfo("Destination texture file was not supplied: Defaulting to %s\n", g_CmdPrams.DestFile.c_str());
                            }
                            else
                            {
#if (OPTION_BUILD_ASTC == 1)
                                if (g_CmdPrams.CompressOptions.DestFormat == CMP_FORMAT_ASTC)
                                    g_CmdPrams.DestFile.append(".astc");
                                else
#endif
                                if (g_CmdPrams.CompressOptions.DestFormat == CMP_FORMAT_BROTLIG)
                                    g_CmdPrams.DestFile.append(".brlg");
                                else
                                    g_CmdPrams.DestFile.append(".dds");
                            }
                        }
                    }
                    else
                    {
                        // Processing a Directory
                        const std::string directory = CMP_GetFullPath(strCommand);

                        if (directory.length() > 0)
                        {
                            // check if dir exist if not create it!
                            if (!CMP_DirExists(directory))
                            {
                                if (!CMP_CreateDir(directory))
                                    throw "Unable to create destination dir";
                                // check and wait for system to generate a valid dir,
                                // typically this should not happen on local a dir
                                int delayloop = 0;
                                while (!CMP_DirExists(directory) && (delayloop < 5))
                                {
#ifdef _WIN32
                                    Sleep(100);
#else
                                    usleep(100000);
#endif
                                    delayloop++;
                                }
                                if (delayloop == 5)
                                    throw "Unable to create destination dir";
                            }
                            g_CmdPrams.DestDir = directory;
                            std::string destFileName;
                            
#ifdef USE_LOSSLESS_COMPRESSION_BINARY
                            // we keep the dest file name empty when decompressing binary data so that we can use the original file name
                            if (CMP_GetJustFileExt(g_CmdPrams.SourceFile) != ".brlg" && CMP_GetJustFileExt(g_CmdPrams.SourceFile) != ".BRLG")
#endif
                            {
                                //since DestFile is empty we need to create one from the source file
                                destFileName = DefaultDestination(g_CmdPrams.SourceFile, g_CmdPrams.CompressOptions.DestFormat, g_CmdPrams.FileOutExt, g_CmdPrams.mangleFileNames);
                            }

                            g_CmdPrams.DestFile = directory + "/" + destFileName;
                        }
                        else
                        {
                            throw "Unable to create destination dir or file path is invalid";
                        }
                    }
                }
                else
                {
                    throw "unknown source, destination file or dir path specified";
                }
            }
        }
    }  // Try code

    catch (const char* str)
    {
        PrintInfo("Option [%s] : %s\n\n", strCommand, str);
        return false;
    }
    catch (std::exception& e)
    {
        PrintInfo("Option [%s] : %s\n\n", strCommand, e.what());
        return false;
    }

    return true;
}

/*
 This function can be called using one of these examples:
 from a main(), it will parse the parmameters and act on them.

    main(int argc,  char* argv[])
    ParseParams(argc,argv);

 or from code

     char *argv[5];
     argv[0]="foo.exe";
     argv[1]="-x";
     argv[2]="myfile";
     argv[3]="-f";
     argv[4]="myflag";
     ParseParams(5,argv);
*/

bool ParseParams(int argc, CMP_CHAR* argv[])
{
    try
    {
        g_CmdPrams.SetDefault();

#ifdef USE_WITH_COMMANDLINE_TOOL1
        if (argc == 1)
        {
            PrintUsage();
        }
#endif

        std::string strCommand;
        std::string strParameter;
        std::string strTemp;

        for (int i = 1; i < argc; i++)
        {
            strTemp = argv[i];

            //
            // Cmd line options can be -option value
            //
            strCommand = strTemp;
            if ((strTemp[0] == '-') && (i <= (argc - 1)))
            {
                if (!ProcessSingleFlags(strCommand.c_str()))
                {
                    i++;
                    if (i < argc)
                        strParameter = argv[i];
                    else
                        strParameter = "";
                    if (!ProcessCMDLineOptions(strCommand.c_str(), strParameter.c_str()))
                    {
                        throw "Invalid Command";
                    }

                    // Check odd cases
                    if (g_CmdPrams.use_noMipMaps && g_CmdPrams.CompressOptions.genGPUMipMaps)
                        throw "Invalid commands set for combined nomipmap and GenGPUMipMaps";
                }
            }
            else
            {
                if (!ProcessCMDLineOptions(strCommand.c_str(), ""))
                {
                    throw "Invalid Command";
                }
            }

        }  // for loop
    }
    catch (const char* str)
    {
        PrintInfo("%s\n", str);
        return false;
    }

    return true;
}

bool SouceAndDestCompatible(CCmdLineParamaters CmdPrams)
{
    return true;
}

/*
class MyCMIPS : CMIPS
{
   public:
    void PrintInfo(const char* Format, ...);
};

void MyCMIPS::PrintInfo(const char* Format, ...)
{
    char buff[128];
    // define a pointer to save argument list
    va_list args;
    // process the arguments into our debug buffer
    va_start(args, Format);
    vsprintf(buff, Format, args);
    va_end(args);

    printf(buff);
}
*/

extern PluginManager g_pluginManager;  // Global plugin manager instance
extern bool          g_bAbortCompression;
extern CMIPS*        g_CMIPS;  // Global MIPS functions shared between app and all IMAGE plugins

MipSet g_MipSetIn;
MipSet g_MipSetCmp;
MipSet g_MipSetOut;
int    g_MipLevel  = 1;
float  g_fProgress = -1;

bool CompressionCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    if (g_fProgress != fProgress)
    {
        UNREFERENCED_PARAMETER(pUser1);
        UNREFERENCED_PARAMETER(pUser2);

        if ((g_CmdPrams.noprogressinfo) || (g_CmdPrams.silent))
            return g_bAbortCompression;
        if (g_MipLevel > 1)
            PrintInfo("\rProcessing progress   = %3.0f MipLevel = %2dn", fProgress, g_MipLevel);
        else
            PrintInfo("\rProcessing progress   = %3.0f", fProgress);
        g_fProgress = fProgress; 
    }

    return g_bAbortCompression;
}

bool TC_PluginCodecSupportsFormat(const MipSet* pMipSet)
{
    return (pMipSet->m_ChannelFormat == CF_8bit || pMipSet->m_ChannelFormat == CF_16bit || pMipSet->m_ChannelFormat == CF_2101010 ||
            pMipSet->m_ChannelFormat == CF_32bit || pMipSet->m_ChannelFormat == CF_Float16 || pMipSet->m_ChannelFormat == CF_Float32);
}

void LogErrorToCSVFile(AnalysisErrorCodeType error)
{
    // Used in test automation and results validation
    if (g_CmdPrams.logcsvformat)
    {
        CMP_ANALYSIS_DATA analysisData = {0};
        analysisData.SSIM              = -1;  // Set data content is invalid and not processed
        analysisData.errCode           = error;
        ProcessResults(g_CmdPrams, analysisData);
    }
}

static void DeallocateMipSet(MipSet* mipSet)
{
    if (!mipSet)
        return;
    
    if (mipSet->m_pMipLevelTable)
    {
        g_CMIPS->FreeMipSet(mipSet);
        mipSet->m_pMipLevelTable = NULL;
    }

    if (mipSet->m_pReservedData)
    {
        free(mipSet->m_pReservedData);
        mipSet->m_pReservedData = NULL;
    }
}

void cleanup(bool Delete_gMipSetIn, bool SwizzleMipSetIn)
{
#ifdef _WIN32
    SetDllDirectory(NULL);
#endif
    if (Delete_gMipSetIn)
        DeallocateMipSet(&g_MipSetIn);
    else if (SwizzleMipSetIn)
        SwizzleMipSet(&g_MipSetIn);

    DeallocateMipSet(&g_MipSetCmp);
    DeallocateMipSet(&g_MipSetOut);
}

// mesh optimization process
// only support case glTF->glTF, case obj->obj
bool OptimizeMesh(std::string SourceFile, std::string DestFile)
{
    if (!(CMP_FileExists(SourceFile)))
    {
        PrintInfo("Error: Source Model Mesh File is not found.\n");
        return false;
    }

    void*       modelDataIn  = nullptr;
    void*       modelDataOut = nullptr;
    CMP_GLTFCommon* gltfdata     = nullptr;

    // load model
    string file_extension = CMP_GetJustFileExt(SourceFile);
    helper_to_upper(file_extension);

    helper_erase_all(file_extension, ".");

    PluginInterface_3DModel_Loader* plugin_loader;
    plugin_loader = reinterpret_cast<PluginInterface_3DModel_Loader*>(g_pluginManager.GetPlugin("3DMODEL_LOADER", (char*)file_extension.c_str()));

    if (plugin_loader)
    {
        plugin_loader->TC_PluginSetSharedIO(g_CMIPS);
        void* msgHandler = NULL;

        int result = plugin_loader->LoadModelData(g_CmdPrams.SourceFile.c_str(), "", &g_pluginManager, msgHandler, &CompressionCallback);
        if (result != 0)
        {
            PrintInfo("[Mesh Optimization] Failed to load file: %s\n", SourceFile.c_str());
            return false;
        }
        if (fileIsGLTF(g_CmdPrams.SourceFile))
        {
            gltfdata = (CMP_GLTFCommon*)plugin_loader->GetModelData();
            if (gltfdata)
            {
                if (gltfdata->m_meshBufferData.m_meshData.size() > 0 && gltfdata->m_meshBufferData.m_meshData[0].vertices.size() > 0)
                    modelDataIn = (void*)&(gltfdata->m_meshBufferData);
                else
                {
                    modelDataIn = nullptr;
                    PrintInfo("[Mesh Optimization] Error in processing mesh. Mesh data format size is not supported.\n");
                    return false;
                }
            }
        }
        else
            modelDataIn = plugin_loader->GetModelData();
    }
    else
    {
        PrintInfo("[Mesh Optimization] Loading file error.: %s\n.", SourceFile.c_str());
        if (plugin_loader)
        {
            delete plugin_loader;
            plugin_loader = nullptr;
        }
        return false;
    }

#ifdef USE_3DMESH_OPTIMIZE
    if (!g_CmdPrams.silent) PrintInfo("Processing: Mesh Optimization...\n");
    // perform mesh optimization
    PluginInterface_Mesh* plugin_Mesh = NULL;

    plugin_Mesh = reinterpret_cast<PluginInterface_Mesh*>(g_pluginManager.GetPlugin("MESH_OPTIMIZER", "TOOTLE_MESH"));

    if (plugin_Mesh)
    {
        if (plugin_Mesh->Init() == 0)
        {
            plugin_Mesh->TC_PluginSetSharedIO(g_CMIPS);
            MeshSettings meshSettings;
            meshSettings.bOptimizeOverdraw = (g_CmdPrams.CompressOptions.fOverdrawACMR > 0);
            if (meshSettings.bOptimizeOverdraw)
                meshSettings.nOverdrawACMRthreshold = (float)g_CmdPrams.CompressOptions.fOverdrawACMR;
            meshSettings.bOptimizeVCache = (g_CmdPrams.CompressOptions.iVcacheSize > 0);
            if (meshSettings.bOptimizeVCache)
                meshSettings.nCacheSize = g_CmdPrams.CompressOptions.iVcacheSize;
            meshSettings.bOptimizeVCacheFifo = (g_CmdPrams.CompressOptions.iVcacheFIFOSize > 0);
            if (meshSettings.bOptimizeVCacheFifo)
                meshSettings.nCacheSize = g_CmdPrams.CompressOptions.iVcacheFIFOSize;
            meshSettings.bOptimizeVFetch = g_CmdPrams.CompressOptions.bVertexFetch;
            meshSettings.bRandomizeMesh  = false;
            meshSettings.bSimplifyMesh   = (g_CmdPrams.CompressOptions.iSimplifyLOD > 0);
            if (meshSettings.bSimplifyMesh)
                meshSettings.nlevelofDetails = g_CmdPrams.CompressOptions.iSimplifyLOD;

            try
            {
                modelDataOut = plugin_Mesh->ProcessMesh(modelDataIn, (void*)&meshSettings, NULL, &CompressionCallback);
            }
            catch (std::exception& e)
            {
                PrintInfo("[Mesh Optimization] Error: %s\n.", e.what());
                if (plugin_Mesh)
                    plugin_Mesh->CleanUp();
                return false;
            }
        }
    }

    //save model
    if (modelDataOut)
    {
        std::vector<CMP_Mesh>*          optimized   = ((std::vector<CMP_Mesh>*)modelDataOut);
        PluginInterface_3DModel_Loader* plugin_save = NULL;

        string destfile_extension = CMP_GetJustFileExt(DestFile);
        helper_to_upper(destfile_extension);

        helper_erase_all(destfile_extension, ".");

        plugin_save = reinterpret_cast<PluginInterface_3DModel_Loader*>(g_pluginManager.GetPlugin("3DMODEL_LOADER", (char*)destfile_extension.c_str()));
        if (plugin_save)
        {
            plugin_save->TC_PluginSetSharedIO(g_CMIPS);

            int result = 0;
            if (fileIsGLTF(g_CmdPrams.DestFile))
            {
                if (gltfdata)
                {
                    CMP_GLTFCommon optimizedGltf;
                    optimizedGltf.buffersData                 = gltfdata->buffersData;
                    optimizedGltf.isBinFile                   = gltfdata->isBinFile;
                    optimizedGltf.j3                          = gltfdata->j3;
                    optimizedGltf.m_CommonLoadTime            = gltfdata->m_CommonLoadTime;
                    optimizedGltf.m_distance                  = gltfdata->m_distance;
                    optimizedGltf.m_filename                  = gltfdata->m_filename;
                    optimizedGltf.m_meshes                    = gltfdata->m_meshes;
                    optimizedGltf.m_path                      = gltfdata->m_path;
                    optimizedGltf.m_scenes                    = gltfdata->m_scenes;
                    optimizedGltf.m_meshBufferData.m_meshData = *optimized;

                    if (plugin_save->SaveModelData(g_CmdPrams.DestFile.c_str(), &optimizedGltf) == -1)
                    {
                        PrintInfo("[Mesh Optimization] Failed to save optimized gltf data.\n");
                        plugin_Mesh->CleanUp();
                        if (plugin_save)
                        {
                            delete plugin_save;
                            plugin_save = nullptr;
                        }
                        return false;
                    }
                }
                else
                {
                    PrintInfo("[Mesh Optimization] Failed to save optimized gltf data.\n");
                    plugin_Mesh->CleanUp();
                    if (plugin_save)
                    {
                        delete plugin_save;
                        plugin_save = nullptr;
                    }
                    return false;
                }
            }
            else
            {
                if (plugin_save->SaveModelData(g_CmdPrams.DestFile.c_str(), &((*optimized)[0])) != -1)
                {
                    if (!g_CmdPrams.silent) PrintInfo("[Mesh Optimization] Success in saving optimized obj data.\n");
                }
                else
                {
                    PrintInfo("[Mesh Optimization] Failed to save optimized obj data.\n");
                    plugin_Mesh->CleanUp();
                    if (plugin_save)
                    {
                        delete plugin_save;
                        plugin_save = nullptr;
                    }
                    return false;
                }
            }

            if (result != 0)
            {
                PrintInfo("[Mesh Optimization] Error in saving mesh file.\n");
                plugin_Mesh->CleanUp();
                if (plugin_save)
                {
                    delete plugin_save;
                    plugin_save = nullptr;
                }
                return false;
            }
            else
            {
                if (!g_CmdPrams.silent) PrintInfo("[Mesh Optimization] Saving %s done.\n", g_CmdPrams.DestFile.c_str());
            }
        }
        else
        {
            PrintInfo("[Mesh Optimization]Saving: File format not supported.\n");
            plugin_Mesh->CleanUp();
            if (plugin_save)
            {
                delete plugin_save;
                plugin_save = nullptr;
            }
            return false;
        }

        plugin_Mesh->CleanUp();
        if (plugin_save)
        {
            delete plugin_save;
            plugin_save = nullptr;
        }
    }
#endif
    return true;
}

// mesh draco compression/decompression
bool CompressDecompressMesh(std::string SourceFile, std::string DestFile)
{
    if (!(CMP_FileExists(SourceFile)))
    {
        PrintInfo("Error: Source Model Mesh File is not found.\n");
        return false;
    }

    // Case: glTF -> glTF handle both compression and decompression
    if (fileIsGLTF(SourceFile))
    {
        if (fileIsGLTF(DestFile))
        {
            std::string err;
            Model       model;
            TinyGLTF    loader;
            TinyGLTF    saver;

            //clean up draco mesh buffer
#ifdef USE_MESH_DRACO_EXTENSION
            model.dracomeshes.clear();
#endif
            std::string srcFile = SourceFile;
            std::string dstFile = DestFile;
            // Check if mesh optimization was done if so then source is optimized file
            if (g_CmdPrams.doMeshOptimize)
            {
                srcFile = DestFile;
                if (!CMP_FileExists(srcFile))
                {
                    PrintInfo("Error: Source Model Mesh File is not found.\n");
                    return false;
                }
                std::size_t dotPos = srcFile.rfind('.');
                dstFile            = srcFile.substr(0, dotPos) + "_drc.glTF";
            }

            bool ret = loader.LoadASCIIFromFile(&model, &err, srcFile, g_CmdPrams.use_Draco_Encode);
            if (!err.empty())
            {
                PrintInfo("Error processing gltf source:[%s] file [%s]\n", srcFile.c_str(), err.c_str());
                return false;
            }
            if (!ret)
            {
                PrintInfo("Failed in loading glTF file : [%s].\n", srcFile.c_str());
                return false;
            }
            else
            {
                if (!g_CmdPrams.silent) PrintInfo("Success in loading glTF file : [%s].\n", srcFile.c_str());
            }

            bool is_draco_src = false;
#ifdef USE_MESH_DRACO_EXTENSION
            if (model.dracomeshes.size() > 0)
            {
                is_draco_src = true;
            }
#endif
            err.clear();

            if (g_CmdPrams.compressImagesFromGLTF)
            {
                std::string dstFolder = dstFile;
                auto        pos       = dstFolder.rfind("\\");
                if (pos == std::string::npos)
                {
                    pos = dstFolder.rfind("/");
                }
                if (pos != std::string::npos)
                {
                    dstFolder = dstFolder.substr(0, pos + 1);
                }
                size_t originalImages = model.images.size();
                for (unsigned i = 0; i < model.images.size(); ++i)
                {
                    std::string input = model.images[i].uri;
                    if (!g_CmdPrams.silent) PrintInfo("Processing '%s'\n", input.c_str());
                    if (input.empty())
                    {
                        PrintInfo("Error: Compressonator can only compress separate images with glTF!\n");
                        return false;
                    }
                    std::string output = dstFolder + input;
                    output.replace(output.rfind('.'), 1, "_");
                    output += ".dds";

                    std::string imgSrcDir = "";
                    auto        pos       = srcFile.rfind("\\");
                    if (pos == std::string::npos)
                    {
                        pos = srcFile.rfind("/");
                    }
                    if (pos != std::string::npos)
                    {
                        imgSrcDir = srcFile.substr(0, pos + 1);
                    }

                    std::string imgDestDir = dstFolder;
                    pos                    = output.rfind("\\");
                    if (pos == std::string::npos)
                    {
                        pos = output.rfind("/");
                    }
                    if (pos != std::string::npos)
                    {
                        imgDestDir = output.substr(0, pos + 1);
                    }

                    if (CMP_CreateDir(imgDestDir))
                    {
                        MipSet inMips{};
                        memset(&inMips, 0, sizeof(CMP_MipSet));
                        int ret = AMDLoadMIPSTextureImage((imgSrcDir + input).c_str(), &inMips, false, &g_pluginManager);

                        if (inMips.m_nMipLevels < g_CmdPrams.MipsLevel && !g_CmdPrams.use_noMipMaps)
                        {
                            CMP_INT requestLevel = g_CmdPrams.MipsLevel;
                            CMP_INT nMinSize     = CMP_CalcMinMipSize(inMips.m_nHeight, inMips.m_nWidth, requestLevel);
                            CMP_CFilterParams CFilterParam = {};
                            CFilterParam.dwMipFilterOptions = 0;
                            CFilterParam.nFilterType        = 0;
                            CFilterParam.nMinSize           = nMinSize;
                            CFilterParam.fGammaCorrection   = g_CmdPrams.CompressOptions.fInputFilterGamma;
                            CMP_GenerateMIPLevelsEx(&inMips, &CFilterParam);
                        }

                        CMP_MipSet mipSetCmp;
                        memset(&mipSetCmp, 0, sizeof(CMP_MipSet));

                        KernelOptions kernel_options;
                        memset(&kernel_options, 0, sizeof(KernelOptions));

                        kernel_options.format     = g_CmdPrams.CompressOptions.DestFormat;
                        kernel_options.fquality   = g_CmdPrams.CompressOptions.fquality;
                        kernel_options.threads    = g_CmdPrams.CompressOptions.dwnumThreads;
                        kernel_options.height     = inMips.dwHeight;
                        kernel_options.width      = inMips.dwWidth;
                        kernel_options.encodeWith = g_CmdPrams.CompressOptions.nEncodeWith;

                        auto cmp_status = CMP_ProcessTexture(&inMips, &mipSetCmp, kernel_options, CompressionCallback);
                        if (cmp_status == CMP_ERR_FAILED_HOST_SETUP)
                        {
                            g_CmdPrams.CompressOptions.nEncodeWith = CMP_Compute_type::CMP_CPU;
                            kernel_options.encodeWith              = g_CmdPrams.CompressOptions.nEncodeWith;
                            memset(&mipSetCmp, 0, sizeof(CMP_MipSet));
                            cmp_status = CMP_ProcessTexture(&inMips, &mipSetCmp, kernel_options, CompressionCallback);
                        }

                        if (cmp_status != CMP_OK)
                        {
                            PrintInfo("Error: Something went wrong while compressing image!\n");
                            return false;
                        }

                        ret = AMDSaveMIPSTextureImage(output.c_str(), &mipSetCmp, false, g_CmdPrams.CompressOptions);
                        if (ret != 0)
                        {
                            PrintInfo("Error: Something went wrong while saving compressed image!\n");
                            return false;
                        }
                    }

                    if (!CMP_FileExists(dstFolder + input))
                    {
                        std::string src = imgSrcDir;
                        src.append(input);
                        std::string dst = dstFolder;
                        dst.append(input);
                        CMP_FileCopy(src, dst);
                    }
                }
            }

            ret = saver.WriteGltfSceneToFile(&model, &err, dstFile, g_CmdPrams.CompressOptions, is_draco_src, g_CmdPrams.use_Draco_Encode);

            if (!err.empty())
            {
                PrintInfo("Error processing gltf destination:[%s] file [%s]\n", dstFile.c_str(), err.c_str());
                return false;
            }
            if (!ret)
            {
                PrintInfo("Failed to save glTF file %s\n", dstFile.c_str());
                return false;
            }
            else
            {
                if (!g_CmdPrams.silent) PrintInfo("Success in writting glTF file : [%s].\n", dstFile.c_str());
            }
        }
        else
        {
            PrintInfo("Destination file must also be a gltf file\n");
            return false;
        }
        return true;
    }

    // Case: obj -> drc compression
    // Case: drc -> obj decompression
    //PluginInterface_3DModel_Loader* m_plugin_loader_drc = NULL;

#if (LIB_BUILD_MESHCOMPRESSOR)
        PluginInterface_Mesh* plugin_MeshComp;
    plugin_MeshComp = reinterpret_cast<PluginInterface_Mesh*>(g_pluginManager.GetPlugin("MESH_COMPRESSOR", "DRACO"));

    if (plugin_MeshComp)
    {
        if (plugin_MeshComp->Init() == 0)
        {
            //showProgressDialog("Process Mesh Data");
            plugin_MeshComp->TC_PluginSetSharedIO(g_CMIPS);

            CMP_DracoOptions DracoOptions;
            DracoOptions.is_point_cloud               = false;
            DracoOptions.use_metadata                 = false;
            DracoOptions.compression_level            = g_CmdPrams.CompressOptions.iCmpLevel;
            DracoOptions.pos_quantization_bits        = g_CmdPrams.CompressOptions.iPosBits;
            DracoOptions.tex_coords_quantization_bits = g_CmdPrams.CompressOptions.iTexCBits;
            DracoOptions.normals_quantization_bits    = g_CmdPrams.CompressOptions.iNormalBits;
            DracoOptions.generic_quantization_bits    = g_CmdPrams.CompressOptions.iGenericBits;

            // Check if mesh optimization was done if so then source is optimized file
            if (g_CmdPrams.doMeshOptimize)
            {
                DracoOptions.input = DestFile;
                if (fileIsOBJ(DracoOptions.input))
                    DracoOptions.output = DestFile + ".drc";
            }
            else
            {
                DracoOptions.input = SourceFile;
                //obj->obj
                if (fileIsOBJ(SourceFile) && fileIsOBJ(DestFile))
                {
                    //this obj->obj case only support for encode, a new encode.drc will be created
                    if (g_CmdPrams.use_Draco_Encode)
                        DracoOptions.output = DestFile + ".drc";
                    else
                    {
                        PrintInfo("Error: please use -draco option to perform encode on the source obj file.\n");
                        return false;
                    }
                }

                //drc->obj or obj->drc
                else if (fileIsDRC(SourceFile) || fileIsDRC(DestFile))
                    DracoOptions.output = DestFile;
            }

            if (!g_CmdPrams.silent) PrintInfo("Processing: Mesh Compression/Decompression...\n");

            void* modelDataOut = nullptr;
            void* modelDataIn  = nullptr;

            PluginInterface_3DModel_Loader* m_plugin_loader_drc;
            m_plugin_loader_drc = reinterpret_cast<PluginInterface_3DModel_Loader*>(g_pluginManager.GetPlugin("3DMODEL_LOADER", "DRC"));

            if (m_plugin_loader_drc)
            {
                m_plugin_loader_drc->TC_PluginSetSharedIO(g_CMIPS);

                int result;
                if (fileIsOBJ(DracoOptions.input))
                {
                    if (result = m_plugin_loader_drc->LoadModelData("OBJ", NULL, &g_pluginManager, &DracoOptions, &CompressionCallback) != 0)
                    {
                        if (result != 0)
                        {
                            PrintInfo("[Mesh Compression] Error Loading Model Data.\n");
                            plugin_MeshComp->CleanUp();
                            return false;
                        }
                    }
                }
                else if (fileIsDRC(DracoOptions.input))
                {
                    if (result =
                            m_plugin_loader_drc->LoadModelData(DracoOptions.input.c_str(), NULL, &g_pluginManager, &DracoOptions, &CompressionCallback) != 0)
                    {
                        if (result != 0)
                        {
                            PrintInfo("[Mesh Compression] Error Loading Model Data.\n");
                            plugin_MeshComp->CleanUp();
                            return false;
                        }
                    }
                    plugin_MeshComp->CleanUp();
                    return true;
                }

                modelDataIn = m_plugin_loader_drc->GetModelData();

                try
                {
                    if (modelDataIn)
                        modelDataOut = plugin_MeshComp->ProcessMesh(modelDataIn, (void*)&DracoOptions, NULL, &CompressionCallback);
                }
                catch (std::exception& e)
                {
                    PrintInfo("[Mesh Compression] Error: %s . Please try another setting.\n", e.what());
                    plugin_MeshComp->CleanUp();
                    return false;
                }

                if (!modelDataOut)
                {
                    PrintInfo("[Mesh Compression] Error in processing mesh.\n");
                    plugin_MeshComp->CleanUp();
                    return false;
                }
            }
        }
        else
        {
            PrintInfo("[Mesh Compression] Error in init mesh plugin.\n");
            plugin_MeshComp->CleanUp();
            return false;
        }
    }
    else
    {
        PrintInfo("[Mesh Compression] Error in loading mesh compression plugin.\n");
        return false;
    }
#endif

    return true;
}

//cmdline only
bool GenerateAnalysis(std::string SourceFile, std::string DestFile)
{
    if (!(CMP_FileExists(SourceFile)))
    {
        PrintInfo("Error: Source Image File is not found.\n");
        return false;
    }

    if (!(CMP_FileExists(DestFile)))
    {
        PrintInfo("Error: Destination Image File is not found.\n");
        return false;
    }

    // Note this feature is also provided by -log option
    PluginInterface_Analysis* Plugin_Analysis;
    int                       testpassed = 0;
    Plugin_Analysis                      = reinterpret_cast<PluginInterface_Analysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        if (g_CmdPrams.diffImage)
        {
            g_CmdPrams.DiffFile = DestFile;
            int lastindex       = (int)g_CmdPrams.DiffFile.find_last_of(".");
            g_CmdPrams.DiffFile = g_CmdPrams.DiffFile.substr(0, lastindex);
            g_CmdPrams.DiffFile.append("_diff.bmp");
        }
        else
        {
            g_CmdPrams.DiffFile = "";
        }

        string results_file = "";
        if (g_CmdPrams.analysis)
        {
            results_file = DestFile;
            int index    = (int)results_file.find_last_of("/");
            results_file = results_file.substr(0, (index + 1));
            results_file.append("Analysis_Result.xml");
        }

        testpassed = Plugin_Analysis->TC_ImageDiff(
            SourceFile.c_str(), DestFile.c_str(), g_CmdPrams.DiffFile.c_str(), (char*)results_file.c_str(), NULL, &g_pluginManager, NULL);
        delete Plugin_Analysis;
        if (testpassed != 0)
        {
            return false;
        }
    }
    else
    {
        printf("Error: Plugin for image analysis is not loaded\n");
        return false;
    }

    return true;
}

//cmdline only: print image properties (i.e. image name, path, file size, image size, image width, height, miplevel and format)
bool OutputImageProps(const std::string& filePath)
{
    MipSet mipSet = {};

    if (!(CMP_FileExists(filePath)))
    {
        PrintInfo("Error: Image File is not found.\n");
        return false;
    }

    // file name
    {
        std::string fileName = CMP_GetFileName(filePath);
        PrintInfo("File Name: %s\n", fileName.c_str());
    }

    // full path
    PrintInfo("File Full Path: %s\n", CMP_GetFullPath(filePath).c_str());

    // file size
    uintmax_t fileSize = CMP_GetFileSize(filePath);

    if (fileSize > 1024000)
    {
        fileSize /= 1024000;
        PrintInfo("File Size: %ju MB\n", fileSize);
    }
    else if (fileSize > 1024)
    {
        fileSize /= 1024;
        PrintInfo("File Size: %ju KB\n", fileSize);
    }
    else
    {
        PrintInfo("File Size: %ju Bytes\n", fileSize);
    }

    // load image into mipset
    if (AMDLoadMIPSTextureImage(filePath.c_str(), &mipSet, g_CmdPrams.use_OCV, &g_pluginManager) != 0)
    {
        PrintInfo("Error: reading image, data type not supported.\n");
        return false;
    }

    //image size

    CMIPS CMips;
    MipLevel* mipLevel = CMips.GetMipLevel(&mipSet, 0, 0);
    uintmax_t imagesize = mipLevel->m_dwLinearSize;

    if (imagesize > 1024000)
    {
        imagesize /= 1024000;
        PrintInfo("Image Size: %ju MB\n", imagesize);
    }
    else if (imagesize > 1024)
    {
        imagesize /= 1024;
        PrintInfo("Image Size: %ju KB\n", imagesize);
    }
    else
    {
        PrintInfo("Image Size: %ju Bytes\n", imagesize);
    }

    //width, height
    PrintInfo("Image Width: %u px\n", mipSet.m_nWidth);
    PrintInfo("Image Height: %u px\n", mipSet.m_nHeight);

    //miplevel, format
    int numMipLevels = mipSet.m_nMipLevels;

    PrintInfo("Mip Levels: %u\n", numMipLevels);

    if (mipSet.m_format != CMP_FORMAT_Unknown)
        PrintInfo("Format: %s\n", GetFormatDesc(mipSet.m_format));
    else
    {
        switch (mipSet.m_ChannelFormat)
        {
        case CF_8bit:  //< 8-bit integer data.
            PrintInfo("Channel Format: 8-bit integer data\n");
            break;
        case CF_Float16:  //< 16-bit float data.
            PrintInfo("Channel Format: 16-bit float data\n");
            break;
        case CF_Float32:  //< 32-bit float data.
            PrintInfo("Channel Format: 32-bit float data\n");
            break;
        case CF_Compressed:  //< Compressed data.
            PrintInfo("Channel Format: Compressed data\n");
            break;
        case CF_16bit:  //< 16-bit integer data.
            PrintInfo("Channel Format: 16-bit integer data\n");
            break;
        case CF_1010102:
        case CF_2101010:  //< 10-bit integer data in the color channels & 2-bit integer data in the alpha channel.
            PrintInfo("Channel Format: 10-bit integer data in the color channels & 2-bit integer data in the alpha channel\n");
            break;
        case CF_32bit:  //< 32-bit integer data.
            PrintInfo("Channel Format: 32-bit integer data\n");
            break;
        case CF_Float9995E:  //< 32-bit partial precision float.
            PrintInfo("Channel Format: 9995E 32-bit partial precision float\n");
            break;
        case CF_YUV_420:  //< YUV Chroma formats
            PrintInfo("Channel Format: YUV 420 Chroma formats\n");
            break;
        case CF_YUV_422:  //< YUV Chroma formats
            PrintInfo("Channel Format: YUV 422 Chroma formats\n");
            break;
        case CF_YUV_444:  //< YUV Chroma formats
            PrintInfo("Channel Format: YUV 444 Chroma formats\n");
            break;
        case CF_YUV_4444:  //< YUV Chroma formats
            PrintInfo("Channel Format: YUV 4444 Chroma formats\n");
            break;
        };
    }

    DeallocateMipSet(&mipSet);

    return true;
}

void LocalPrintF(char* buff)
{
// Issue #89 Pull Request
#ifdef __clang__
#pragma clang diagnostic ignored "-Wformat-security"  // warning : warning: format string is not a string literal
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-security"  // warning : format string is not a string literal (potentially insecure)
#endif
    printf(buff);
}

#include "common_kerneldef.h"
#include "compute_base.h"

bool SVMInitCodec(KernelOptions* options)
{
    CMP_FORMAT format = options->format;
    switch (format)
    {
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
    case CMP_FORMAT_BC7:
#if (OPTION_BUILD_ASTC == 1)
    case CMP_FORMAT_ASTC:
#endif
    {
        unsigned char* src = (unsigned char*)options->data;
        unsigned char* dst = (unsigned char*)options->dataSVM;
        memcpy(dst, src, options->size);
        return true;
    }
    break;
    }
    return false;
}

//#endif

double timeStampsec()
{
#ifdef _WIN32
    static LARGE_INTEGER frequency;
    if (frequency.QuadPart == 0)
        QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return now.QuadPart / double(frequency.QuadPart);
#else
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec / 1000000000.0;
#endif
}

bool CMP_GenerateMipLevelData(CMP_FORMAT format, int w, int h, int level, int nFaceOrSlice, MipSet* pMipSetOut)
{
    //========================
    // Compressed Destination
    //========================
    CMP_Texture destGPUMipTexture;
    destGPUMipTexture.dwSize     = sizeof(CMP_Texture);
    destGPUMipTexture.dwPitch    = 0;
    destGPUMipTexture.format     = format;
    destGPUMipTexture.dwWidth    = w;
    destGPUMipTexture.dwHeight   = h;
    destGPUMipTexture.dwDataSize = CMP_CalculateBufferSize(&destGPUMipTexture);

    pMipSetOut->m_format   = format;
    pMipSetOut->dwDataSize = destGPUMipTexture.dwDataSize;
    pMipSetOut->dwWidth    = w;
    pMipSetOut->dwHeight   = h;

    MipLevel* pGPUOutMipLevel = g_CMIPS->GetMipLevel(pMipSetOut, level, nFaceOrSlice);
    if (!g_CMIPS->AllocateCompressedMipLevelData(pGPUOutMipLevel, w, h, destGPUMipTexture.dwDataSize))
    {
        return false;
    }

    return true;
}

CMP_ERROR CMP_ConvertMipTextureCGP(MipSet* pMipSetIn, MipSet* pMipSetOut, CMP_CompressOptions* pCompressOptions, CMP_Feedback_Proc pFeedbackProc)
{
    assert(pMipSetIn);
    assert(pMipSetOut);
    assert(pCompressOptions);

    // -------------
    // Output
    // -------------
    memset(pMipSetOut, 0, sizeof(CMP_MipSet));
    pMipSetOut->m_Flags   = MS_FLAG_Default;
    pMipSetOut->m_format  = pCompressOptions->DestFormat;
    pMipSetOut->m_nHeight = pMipSetIn->m_nHeight;
    pMipSetOut->m_nWidth  = pMipSetIn->m_nWidth;

    //=====================================================
    // Case Uncompressed Source to Compressed Destination
    //=====================================================
    // Allocate compression data
    pMipSetOut->m_ChannelFormat = CF_Compressed;
    pMipSetOut->m_nMaxMipLevels = pMipSetIn->m_nMaxMipLevels;
    pMipSetOut->m_nMipLevels    = 1;  // this is overwriiten depending on input.
    pMipSetOut->m_nBlockWidth   = 4;  // Update is required for other sizes.
    pMipSetOut->m_nBlockHeight  = 4;  // - need to fix pMipSetIn m_nBlock settings for this to work
    pMipSetOut->m_nDepth        = pMipSetIn->m_nDepth;
    pMipSetOut->m_TextureType   = pMipSetIn->m_TextureType;
    pMipSetOut->m_nIterations   = 0;

    if (!g_CMIPS->AllocateMipSet(
            pMipSetOut, pMipSetOut->m_ChannelFormat, TDT_ARGB, pMipSetOut->m_TextureType, pMipSetIn->m_nWidth, pMipSetIn->m_nHeight, pMipSetOut->m_nDepth))
    {  // depthsupport, what should nDepth be set as here?
        return CMP_ERR_MEM_ALLOC_FOR_MIPSET;
    }

    CMP_Texture srcTexture = {};
    srcTexture.dwSize = sizeof(srcTexture);
    int DestMipLevel  = pMipSetIn->m_nMipLevels;

    pMipSetOut->m_nMipLevels = DestMipLevel;

    CMP_BOOL isGPUEncoding = !((pCompressOptions->nEncodeWith == CMP_Compute_type::CMP_CPU) || (pCompressOptions->nEncodeWith == CMP_Compute_type::CMP_HPC));
    pCompressOptions->format_support_hostEncoder =
        isGPUEncoding || (pCompressOptions->nEncodeWith == CMP_Compute_type::CMP_HPC);  // HPC Encoder is supported as static plugin
    CGU_BOOL dataProcessed     = false;
    CGU_BOOL contineProcessing = true;

    // When using the GPU we will allow the HW to optionally generate and process miplevels.
    // but only if the Source has no MipLevels, in that case GPU will cycle through each source mip level for processing
    CGU_BOOL genGPUMipMaps = pCompressOptions->genGPUMipMaps;
    // If generating GPU mipmaps using HW allocate IO data, do this only once as this loop also processes cubemap faces
    bool genOntimeDestMipMaps = false;
    if (pCompressOptions->nEncodeWith == CMP_Compute_type::CMP_GPU_HW)
    {
        genOntimeDestMipMaps = genGPUMipMaps;
    }

    for (int nMipLevel = 0; (nMipLevel < DestMipLevel) && contineProcessing; nMipLevel++)
    {
        for (int nFaceOrSlice = 0; (nFaceOrSlice < CMP_MaxFacesOrSlices(pMipSetIn, nMipLevel)) && contineProcessing; nFaceOrSlice++)
        {
            //=====================
            // Uncompressed source
            //======================
            MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(pMipSetIn, nMipLevel, nFaceOrSlice);

            //=======================================
            // For GPU skip non-divisable by 4 pixels
            //=======================================
            if (isGPUEncoding && ((pInMipLevel->m_nWidth % 4) || (pInMipLevel->m_nHeight % 4)))
            {
                // adjust final output miplevel
                if (!g_CmdPrams.silent) PrintInfo("GPU Process Warning! MIP level %d not processed", nMipLevel);
                if (!g_CmdPrams.silent) PrintInfo("MIP level width (%d) or height (%d) not divisible by 4!. CPU will be used\n", pInMipLevel->m_nWidth, pInMipLevel->m_nHeight);
                // pMipSetOut->m_nMipLevels = nMipLevel;
                // contineProcessing        = false;
                pCompressOptions->format_support_hostEncoder = false;
                isGPUEncoding                                = false;
                // continue;
            }

            dataProcessed           = true;
            srcTexture.dwPitch      = 0;
            srcTexture.nBlockWidth  = pMipSetIn->m_nBlockWidth;
            srcTexture.nBlockHeight = pMipSetIn->m_nBlockHeight;
            srcTexture.nBlockDepth  = pMipSetIn->m_nBlockDepth;
            srcTexture.format       = pMipSetIn->m_format;
            srcTexture.dwWidth      = pInMipLevel->m_nWidth;
            srcTexture.dwHeight     = pInMipLevel->m_nHeight;
            srcTexture.pData        = pInMipLevel->m_pbData;
            srcTexture.dwDataSize   = CMP_CalculateBufferSize(&srcTexture);

            // Temporary setting!
            pMipSetIn->dwWidth    = pInMipLevel->m_nWidth;
            pMipSetIn->dwHeight   = pInMipLevel->m_nHeight;
            pMipSetIn->pData      = pInMipLevel->m_pbData;
            pMipSetIn->dwDataSize = CMP_CalculateBufferSize(&srcTexture);

            //========================
            // Compressed Destination
            //========================
            CMP_Texture destTexture = {};
            destTexture.dwSize       = sizeof(destTexture);
            destTexture.dwWidth      = pInMipLevel->m_nWidth;
            destTexture.dwHeight     = pInMipLevel->m_nHeight;
            destTexture.dwPitch      = 0;
            destTexture.nBlockWidth  = pMipSetIn->m_nBlockWidth;
            destTexture.nBlockHeight = pMipSetIn->m_nBlockHeight;
            destTexture.format       = pCompressOptions->DestFormat;
            destTexture.transcodeFormat = pMipSetIn->m_format;
            destTexture.dwDataSize   = CMP_CalculateBufferSize(&destTexture);

            pMipSetOut->m_format        = pCompressOptions->DestFormat;
            pMipSetOut->dwDataSize      = CMP_CalculateBufferSize(&destTexture);
            pMipSetOut->dwWidth         = pInMipLevel->m_nWidth;
            pMipSetOut->dwHeight        = pInMipLevel->m_nHeight;
            pMipSetOut->m_atmiplevel    = nMipLevel;
            pMipSetOut->m_atfaceorslice = nFaceOrSlice;

            MipLevel* pOutMipLevel = g_CMIPS->GetMipLevel(pMipSetOut, nMipLevel, nFaceOrSlice);
            if (!g_CMIPS->AllocateCompressedMipLevelData(pOutMipLevel, destTexture.dwWidth, destTexture.dwHeight, destTexture.dwDataSize))
            {
                return CMP_ERR_MEM_ALLOC_FOR_MIPSET;
            }

            destTexture.pData = pOutMipLevel->m_pbData;
            pMipSetOut->pData = pOutMipLevel->m_pbData;

            //========================
            // Process ConvertTexture
            //========================
            if (pCompressOptions->format_support_hostEncoder)
            {
                //------------------------------------------------
                // Initializing the Host Framework
                // if it fails revert to CPU version of the codec
                //------------------------------------------------
                KernelOptions kernel_options;
                memset(&kernel_options, 0, sizeof(KernelOptions));
                kernel_options.format        = pCompressOptions->DestFormat;
                kernel_options.encodeWith    = pCompressOptions->nEncodeWith;
                kernel_options.fquality      = pCompressOptions->fquality;
                kernel_options.threads       = pCompressOptions->dwnumThreads;
                kernel_options.getPerfStats  = pCompressOptions->getPerfStats;
                kernel_options.getDeviceInfo = pCompressOptions->getDeviceInfo;
                kernel_options.genGPUMipMaps = genGPUMipMaps;
                kernel_options.useSRGBFrames = pCompressOptions->useSRGBFrames;
                // New BC15 settings
                kernel_options.bc15.useAlphaThreshold  = pCompressOptions->bDXT1UseAlpha;
                kernel_options.bc15.alphaThreshold     = pCompressOptions->nAlphaThreshold;
                kernel_options.bc15.useAdaptiveWeights = pCompressOptions->bUseAdaptiveWeighting;
                kernel_options.bc15.useChannelWeights  = pCompressOptions->bUseChannelWeighting;
                kernel_options.bc15.channelWeights[0]  = pCompressOptions->fWeightingRed;
                kernel_options.bc15.channelWeights[1]  = pCompressOptions->fWeightingGreen;
                kernel_options.bc15.channelWeights[2]  = pCompressOptions->fWeightingBlue;
                kernel_options.bc15.useRefinementSteps = pCompressOptions->bUseRefinementSteps;
                kernel_options.bc15.refinementSteps    = pCompressOptions->nRefinementSteps;

                do
                {
                    ComputeOptions options;
                    options.force_rebuild = false;  // set this to true if you want the shader source code  to be allways compiled!

                    //===============================================================================
                    // Initalize the  Pipeline that will be used for the codec to run on HPC or GPU
                    //===============================================================================
                    if (CMP_CreateComputeLibrary(&g_MipSetIn, &kernel_options, g_CMIPS) != CMP_OK)
                    {
                        if (!g_CmdPrams.silent) PrintInfo("Warning! CPU will be used for compression\n");
                        pCompressOptions->format_support_hostEncoder = false;
                        break;
                    }

                    // Init Compute Codec info IO
                    if ((g_CMIPS->PrintLine == NULL) && (PrintStatusLine != NULL))
                    {
                        g_CMIPS->PrintLine = PrintStatusLine;
                    }

                    // Set any addition feature as needed for the Host
                    if (CMP_SetComputeOptions(&options) != CMP_OK)
                    {
                        PrintInfo("Failed to setup (HPC, SPMD or GPU) host options\n");
                        return CMP_ERR_FAILED_HOST_SETUP;
                    }

                    if (genOntimeDestMipMaps)
                    {
                        // Generate MipMap images : Using CPU Box Filter, note however these miplevel images will be overritten by GPU generated images
                        // CMP_GenerateMIPLevels(pMipSetIn, 4);

                        // Now setup destination miplevel storage that will be filled in by GPU based mipmaping
                        CMP_INT levels = CMP_CalcMaxMipLevel(pMipSetIn->m_nWidth, pMipSetIn->m_nHeight, true);

                        if (levels > pMipSetIn->m_nMaxMipLevels)
                            levels = pMipSetIn->m_nMaxMipLevels;

                        if ((pCompressOptions->miplevels < levels) && (pCompressOptions->miplevels >= 1))
                        {
                            levels = pCompressOptions->miplevels;
                        }

                        kernel_options.miplevels = levels;
                        pMipSetOut->m_nMipLevels = levels;

                        for (CMP_INT level = 1; level < levels; level++)
                        {
                            CMP_INT w = pMipSetIn->m_nWidth >> level;
                            CMP_INT h = pMipSetIn->m_nHeight >> level;
                            if (!CMP_GenerateMipLevelData(pMipSetOut->m_format, w, h, level, nFaceOrSlice, pMipSetOut))
                                return CMP_ERR_MEM_ALLOC_FOR_MIPSET;
                            if (genGPUMipMaps)
                                pMipSetOut->m_nIterations++;
                        }
                    }

                    //==========================
                    // Print info about input
                    //==========================
                    // NOTE: This is duplicated in CMP_ConvertMipTexture
                    if (pCompressOptions->m_PrintInfoStr)
                    {
                        char buff[256];
#ifdef USE_LOSSLESS_COMPRESSION_BINARY
                        snprintf(buff, sizeof(buff), "Source file size = %d Bytes\n", srcTexture.dwDataSize);
#else
                        snprintf(buff,
                                sizeof(buff),
                                "\nSource file size = %d Bytes, width = %d px  height = %d px\n",
                                srcTexture.dwDataSize,
                                srcTexture.dwWidth,
                                srcTexture.dwHeight);
#endif

                        pCompressOptions->m_PrintInfoStr(buff);
                    }

                    // Do the compression
                    if (CMP_CompressTexture(&kernel_options, *pMipSetIn, *pMipSetOut, pFeedbackProc) != CMP_OK)
                    {
                        if (!g_CmdPrams.silent) PrintInfo("Warning: Target device or format is not supported or failed to build. CPU will be used\n");
                        pCompressOptions->format_support_hostEncoder = false;
                        break;
                    }
                    else
                        pMipSetOut->m_nIterations++;

                    if (kernel_options.getPerfStats)
                    {
                        // Get Preformance Stats
                        if (CMP_GetPerformanceStats(&kernel_options.perfStats) == CMP_OK)
                        {
                            pCompressOptions->perfStats = kernel_options.perfStats;
                        }
                        else
                            if (!g_CmdPrams.silent) PrintInfo("Warning: Target device or format is not supported or failed to build. CPU will be used\n");
                    }

                    if (kernel_options.getDeviceInfo)
                    {
                        // Get Device Info
                        if (CMP_GetDeviceInfo(&kernel_options.deviceInfo) == CMP_OK)
                        {
                            pCompressOptions->deviceInfo = kernel_options.deviceInfo;
                        }
                        else
                            if (!g_CmdPrams.silent) PrintInfo("Warning: Target device or format is not supported or failed to build. CPU will be used\n");
                    }

                    //==========================
                    // Print info about output
                    //==========================
                    // NOTE: This is duplicated in CMP_ConvertMipTexture
                    if (pCompressOptions->m_PrintInfoStr && destTexture.dwDataSize > 0 && destTexture.format != CMP_FORMAT_BINARY)
                    {
                        char buff[256];
                        snprintf(buff,sizeof(buff),
                                    "\rDestination file size = %d Bytes   Resulting compression ratio = %2.2f:1\n",
                                    destTexture.dwDataSize,
                                    srcTexture.dwDataSize / (float)destTexture.dwDataSize);

                        pCompressOptions->m_PrintInfoStr(buff);
                    }

                    //===============================================================================
                    // Close the Pipeline with option to cache as needed
                    //===============================================================================
                    CMP_DestroyComputeLibrary(false);

                } while (0);
            }

            // SPMD/GPU failed run CPU version
            if (!pCompressOptions->format_support_hostEncoder)
            {
                isGPUEncoding = false;
                if (CMP_ConvertTexture(&srcTexture, &destTexture, pCompressOptions, pFeedbackProc) != CMP_OK)
                {
                    PrintInfo("Error in compressing destination texture\n");
                    return CMP_ERR_CMP_DESTINATION;
                }
                else
                    pMipSetOut->m_nIterations++;
            }
        }

        // Check if GPU was used for MipMap Generation, This indicates that All processed compressed miplevels were generated
        // Need to exit the outter processing loop
        if (genGPUMipMaps && isGPUEncoding)
            contineProcessing = false;
    }

    //if (pFeedbackProc)
    //    pFeedbackProc(100, NULL, NULL);

    if (isGPUEncoding && !dataProcessed)
    {
        PrintInfo("Error source file is not processed, check source width & height is divisable by 4!\n");
        return CMP_ERR_CMP_DESTINATION;
    }

    CMP_Format2FourCC(pCompressOptions->DestFormat, pMipSetOut);

    return CMP_OK;
}

// ToDo replace with plugin scan, qt checks and src dest format checks.
bool SupportedFileTypes(std::string fileExt)
{
//#if (OPTION_BUILD_ASTC == 1)
//    char* supportedTypes[20] = {"DDS",  "KTX", "KTX2", "BMP", "PNG", "JPEG", "JPG", "EXR", "TGA",  "TIF",
//                                "TIFF", "OBJ", "GLTF", "PBM", "PGM", "PPM",  "XBM", "XPM", "ASTC", "DRC"};
//    for (int i = 0; i < 20; i++)
//#else
//    // Adding BRLG, BrotliG 
//    char* supportedTypes[20] = {"DDS",  "KTX", "KTX2", "BMP", "PNG", "JPEG", "JPG", "EXR", "TGA",  "TIF",
//                                "TIFF", "OBJ", "GLTF", "PBM", "PGM", "PPM",  "XBM", "XPM", "DRC", "BRLG"};
//    for (int i = 0; i < 20; i++)
//#endif
//    {
//        if (fileExt.compare(supportedTypes[i]) == 0)
//            return true;
//    }

    vector<string> supportedTypes = {
        "DDS", "KTX", "KTX2", "BMP", "PNG", "JPEG", "JPG", "EXR", "TGA", "TIF", "TIFF", "OBJ", "GLTF", "PBM", "PGM", "PPM", "XBM", "XPM", "DRC"
        };
#if (OPTION_BUILD_ASTC == 1)
    supportedTypes.push_back("ASTC");
#endif
#if (USE_LOSSLESS_COMPRESSION == 1)
    supportedTypes.push_back("BRLG");
#endif

    for (int i = 0; i < supportedTypes.size(); i++)
    {
       if (fileExt.compare(supportedTypes[i]) == 0)
           return true;
    }

    return false;
}

void LogToResults(CCmdLineParamaters& prams, char* str)
{
    if (prams.logresultsToFile)
    {
#ifdef _WIN32
        FILE* fp;
        //errno_t err =
        fopen_s(&fp, prams.LogProcessResultsFile.c_str(), "a");
#else
        FILE* fp = fopen(prams.LogProcessResultsFile.c_str(), "a");
#endif
        if (fp)
        {
            fprintf(fp, "%s", str);
            fclose(fp);
        }
    }
}

void ProcessResults(CCmdLineParamaters& prams, CMP_ANALYSIS_DATA& analysisData)
{
    if (prams.logresultsToFile)
    {
        bool newfile = false;
        // Check for file existance first!
        if (!CMP_FileExists(prams.LogProcessResultsFile))
            newfile = true;

#ifdef _WIN32
        FILE* fp;
        fopen_s(&fp, prams.LogProcessResultsFile.c_str(), "a");
#else
        FILE* fp = fopen(prams.LogProcessResultsFile.c_str(), "a");
#endif
        if (fp)
        {
            // Write Header info
            if (newfile)
            {
                fprintf(fp, "CompressonatorCLI Performance Log v1.3\n\n");
                fprintf(fp, "Negative values are errors in measurement Sets ErrCode > 0 else 0 for none\n");
                fprintf(fp, "For images with no errors MSE= 0 PSNR=255 and SSIM= 1.0\n");
                fprintf(fp, "Transcoded images         MSE= 0 PSNR=255 and SSIM=-2.0\n");
                fprintf(fp, "No image data generated   MSE=-1 PSNR= -1 and SSIM=-1.0 with ErrCode set\n\n");

                if (prams.logcsvformat)
                {
                    fprintf(fp,
                            "Source,Height,Width,LinearSize(MB),Destination,ProcessedTo,Iteration,Duration(s),Using,Quality,KPerf(ms),MTx/"
                            "s,MSE,PSNR,SSIM,TotalTime(s),ErrCode\n");
                }
            }

            //
            // Gather data to print
            //
            char        buffer[1024];
            std::string str_source      = prams.SourceFile;
            std::string str_destination = prams.DestFile;
            std::string str_height;
            std::string str_width;
            std::string str_linearsize;
            std::string str_processedto;
            std::string str_compress_nIterations;
            std::string str_compress_fDuration;
            std::string str_encodewith;
            std::string str_fquality;
            std::string str_processed_to;
            std::string str_perf;
            std::string str_mtx;
            std::string str_mse;
            std::string str_psnr;
            std::string str_ssim;
            std::string str_duration;

            // if (prams.compute_setup_fDuration > 0) to remove!
            // {
            //    //"Compute Setup: %.3f seconds\n", g_CmdPrams.compute_setup_fDuration
            // }

            //====================================
            // EncodeWith option
            //====================================
            if (prams.CompressOptions.bUseCGCompress)
            {
                if (prams.CompressOptions.nEncodeWith != CMP_Compute_type::CMP_CPU)
                {
                    if (!prams.CompressOptions.format_support_hostEncoder)
                    {
                        str_encodewith.append("[CPU] ");
                    }
                    str_encodewith.append(GetEncodeWithDesc(prams.CompressOptions.nEncodeWith));
                }
                else
                    str_encodewith.append("[CPU]");
            }
            else
            {
                if (prams.CompressOptions.bUseGPUDecompress)
                {
                    switch (prams.CompressOptions.nGPUDecode)
                    {
                    case GPUDecode_DIRECTX:
                        str_encodewith.append("DirectX");
                        break;
                    case GPUDecode_VULKAN:
                        str_encodewith.append("Vulkan");
                        break;
                    case GPUDecode_OPENGL:
                    default:
                        str_encodewith.append("OpenGL");
                        break;
                    }
                }
                else
                    str_encodewith.append("CPU");
            }

            //======================================
            // Source Height , Width , Linear size
            //======================================
            snprintf(buffer, 1024, "%d", prams.dwHeight);
            str_height = buffer;

            snprintf(buffer, 1024, "%d", prams.dwWidth);
            str_width = buffer;

            snprintf(buffer, 1024, "%2.3f", (float)prams.dwDataSize / 1000000.0f);
            str_linearsize = buffer;

            //======================================
            // Processed To info
            //======================================
            snprintf(buffer, 1024, "%s", GetFormatDesc(prams.CompressOptions.DestFormat));
            str_processedto = buffer;
            snprintf(buffer, 1024, "%2i", prams.compress_nIterations);
            str_compress_nIterations = buffer;
            snprintf(buffer, 1024, "%.3f", prams.compress_fDuration);
            str_compress_fDuration = buffer;
            snprintf(buffer, 1024, "%.2f", g_CmdPrams.CompressOptions.fquality);
            str_fquality = buffer;

            //======================================
            // Performance Data
            //======================================
            snprintf(buffer, 1024, "%3.3f", prams.CompressOptions.perfStats.m_computeShaderElapsedMS * 1000);
            str_perf = buffer;
            snprintf(buffer, 1024, "%3.3f", prams.CompressOptions.perfStats.m_CmpMTxPerSec);
            str_mtx = buffer;

            //======================================
            // Quality Data
            //======================================
            if (analysisData.PSNR <= 0)
            {
                analysisData.PSNR = 256;
            }

            if (analysisData.SSIM != -1)
            {
                snprintf(buffer, 1024, "%.4f", analysisData.MSE);
                str_mse = buffer;
                snprintf(buffer, 1024, "%.2f", analysisData.PSNR);
                str_psnr = buffer;
                snprintf(buffer, 1024, "%.4f", analysisData.SSIM);
                str_ssim = buffer;
            }
            else
            {
                str_mse  = "-1";
                str_psnr = "-1";
                str_ssim = "-1";
            }

            snprintf(buffer, 1024, "%.3f", prams.conversion_fDuration);
            str_duration = buffer;

            if (prams.logcsvformat)
            {
                fprintf(fp,
                        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",
                        str_source.c_str(),
                        str_height.c_str(),
                        str_width.c_str(),
                        str_linearsize.c_str(),
                        str_destination.c_str(),
                        str_processedto.c_str(),
                        str_compress_nIterations.c_str(),
                        str_compress_fDuration.c_str(),
                        str_encodewith.c_str(),
                        str_fquality.c_str(),
                        str_perf.c_str(),
                        str_mtx.c_str(),
                        str_mse.c_str(),
                        str_psnr.c_str(),
                        str_ssim.c_str(),
                        str_duration.c_str(),
                        analysisData.errCode);
            }
            else
            {
                fprintf(fp,
                        "Source       : %s, Height %s, Width %s, Linear size %s MB\n",
                        str_source.c_str(),
                        str_height.c_str(),
                        str_width.c_str(),
                        str_linearsize.c_str());
                fprintf(fp, "Destination  : %s\n", str_destination.c_str());
                fprintf(fp,
                        "Processed to : %-10s with %s iteration(s) in %s seconds\n",
                        str_processedto.c_str(),
                        str_compress_nIterations.c_str(),
                        str_compress_fDuration.c_str());
                fprintf(fp, "Using        : %s\n", str_encodewith.c_str());
                fprintf(fp, "Quality      : %s\n", str_fquality.c_str());
                fprintf(fp, "KPerf(ms)    : %s\n", str_perf.c_str());
                fprintf(fp, "MTx/s        : %s\n", str_mtx.c_str());
                fprintf(fp, "MSE          : %s\n", str_mse.c_str());
                fprintf(fp, "PSNR         : %s\n", str_psnr.c_str());
                fprintf(fp, "SSIM         : %s\n", str_ssim.c_str());
                fprintf(fp, "Total time(s): %s\n", str_duration.c_str());
            }
            fclose(fp);
        }
    }
    else
    {
        if (analysisData.PSNR <= 0)
        {
            analysisData.PSNR = 256;
        }

        if (analysisData.SSIM != -1)
        {
            prams.MSE  = analysisData.MSE;
            prams.PSNR = analysisData.PSNR;
            prams.SSIM = analysisData.SSIM;
        }
    }
}

void PrintInfoStr(const char* InfoStr)
{
    if (!g_CmdPrams.silent) PrintInfo(InfoStr);
}

int Process_Lossless_Compressed_To_UncompressedFile(const std::string& sourcePath, const std::string& destPath, MipSet* pMipSetIn, CMP_Feedback_Proc pFeedbackProc)
{
    int processResult = 0;

    std::string destFilePath = destPath;
    std::string destFileName = CMP_GetFileName(destPath);

    //===========================================================================
    // no destination provided, check mipset extra info for destination file name
    //===========================================================================
    if (destFileName.size() == 0)
    {
        BRLG_ExtraInfo* brlgInfo = (BRLG_ExtraInfo*)pMipSetIn->m_pReservedData;

        if (brlgInfo)
        {
            destFileName = brlgInfo->fileName;
            if (destPath.size() > 0)
                destFilePath = destPath + "/" + destFileName;
            else
                destFilePath = destFileName;
        }
        else
        {
            LogErrorToCSVFile(ANALYSIS_FAILED_FILESAVE);
            PrintInfo("Error: loading BRLG extra info failed for %s.\n", sourcePath.c_str());
            return -1;
        }
    }

    pMipSetIn->m_format = CMP_FORMAT_BROTLIG;

    MipSet decompressedMipSet = {};
    decompressedMipSet.m_Flags  = MS_FLAG_Default;
    decompressedMipSet.m_format = CMP_FORMAT_BINARY;

    g_CmdPrams.CompressOptions.DestFormat = CMP_FORMAT_BINARY;
    g_CmdPrams.DestFile = destFilePath;

    CMP_ERROR cmp_status = CMP_ConvertMipTexture((CMP_MipSet*)pMipSetIn, (CMP_MipSet*)&decompressedMipSet, &g_CmdPrams.CompressOptions, pFeedbackProc);

    if (AMDSaveMIPSTextureImage(destFilePath.c_str(), &decompressedMipSet, false, g_CmdPrams.CompressOptions) != 0)
    {
        LogErrorToCSVFile(ANALYSIS_FAILED_FILESAVE);
        PrintInfo("Error: Saving '%s' failed\n", destFilePath.c_str());
        return -1;
    }

    return processResult;
}

int Process_Lossless_Uncompressed_To_CompressedFile(std::string SourceFile, std::string DestFile, MipSet* pMipSetIn, CMP_Feedback_Proc pFeedbackProc)
{
    int processResult = 0;
    MipSet MipSetCmp;

    memset(&MipSetCmp, 0, sizeof(MipSet));
    MipSetCmp.m_Flags  = MS_FLAG_Default;
    if (g_CmdPrams.CompressOptions.DestFormat == CMP_FORMAT_BROTLIG)
    {
        MipSetCmp.m_format = CMP_FORMAT_BROTLIG;
    }

    CMP_ERROR cmp_status = CMP_ConvertMipTexture((CMP_MipSet*)pMipSetIn, (CMP_MipSet*)&MipSetCmp, &g_CmdPrams.CompressOptions, pFeedbackProc);

    MipSetCmp.m_TextureDataType = pMipSetIn->m_TextureDataType;
    
    bool isLocalMipSet = false;

    if (pMipSetIn->m_pReservedData != NULL)
    {
        // user suppiled extra info 
        MipSetCmp.m_pReservedData = pMipSetIn->m_pReservedData;
    }
    else
    {
        std::string FileNameExt = CMP_GetFileNameAndExt(SourceFile);
        if (FileNameExt.size() < BRLG_MAX_FILE_NAME_SIZE)
        {
            isLocalMipSet             = true;
            BRLG_ExtraInfo* extraInfo = (BRLG_ExtraInfo*)calloc(1, sizeof(BRLG_ExtraInfo));
            if (extraInfo)
            {
                memcpy(extraInfo->fileName, FileNameExt.c_str(), FileNameExt.size());
                MipSetCmp.m_pReservedData = extraInfo;
            }
        }
    }

    // no destination file supplied create one from source file name
    if ((DestFile.size() == 0) && (SourceFile.size() > 0))
    {
        DestFile = DefaultDestination(SourceFile, CMP_FORMAT_BROTLIG, g_CmdPrams.FileOutExt, g_CmdPrams.mangleFileNames);
        if (!g_CmdPrams.silent) PrintInfo("Destination file was not supplied: Defaulting to %s\n", DestFile.c_str());
        g_CmdPrams.DestFile += DestFile;
    }

    if (AMDSaveMIPSTextureImage(DestFile.c_str(), &MipSetCmp, false, g_CmdPrams.CompressOptions) != 0)
    {
        if (isLocalMipSet)
            free(MipSetCmp.m_pReservedData);
        LogErrorToCSVFile(ANALYSIS_FAILED_FILESAVE);
        PrintInfo("Error: Saving image '%s' failed. Write permission denied or format is unsupported for the file extension.\n", g_CmdPrams.DestFile.c_str());
        return -1;
    }

    if (isLocalMipSet)
        free(MipSetCmp.m_pReservedData);

    return processResult;
}


int ProcessCMDLine(CMP_Feedback_Proc pFeedbackProc, MipSet* p_userMipSetIn)
{
    int    processResult = 0;
    double frequency, conversion_loopStartTime = {0}, conversion_loopEndTime = {0}, compress_loopStartTime = {0}, compress_loopEndTime = {0},
                      decompress_loopStartTime = {0}, decompress_loopEndTime = {0};
    double ssim_sum              = 0.0;
    double psnr_sum              = 0.0;
    double process_time_sum      = 0.0;
    int    total_processed_items = 0;

    // These flags indicate if the source and destination files are compressed
    bool SourceFormatIsCompressed       = false;
    bool DestinationFormatIsCompressed  = false;
    bool TranscodeBits    = false;
    bool MidwayDecompress = false;
    bool PostCompress     = false;

    CMP_FORMAT  SaveTempFormat;
    CMP_FORMAT  SaveDestFormat;
    std::string SaveDestName;

    //  With a user suppiled Mip Map dont delete it on exit
    bool Delete_gMipSetIn = false;

    // flags if an user suppiled Mip Map input was swizzled during compression / decompression
    bool SwizzledMipSetIn = false;

    // Currently active input and output mips buffers
    // These point to the allocated g_MipSetxxx buffers
    // depending on the users requirement for input and output file formats
    MipSet* p_MipSetIn  = 0;
    MipSet* p_MipSetOut = 0;

    std::vector<std::string> processedFileList;

#ifdef SHOW_PROCESS_MEMORY
    PROCESS_MEMORY_COUNTERS memCounter1;
    PROCESS_MEMORY_COUNTERS memCounter2;

#ifdef SHOW_PROCESS_MEMORY
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter1, sizeof(memCounter1));
#endif

#endif

    bool                      MoreSourceFiles = false;
    PluginInterface_Analysis* Plugin_Analysis = NULL;

    if (g_CmdPrams.logresults)
    {
        // Check for vaild -log usage
        if ((!fileIsModel(g_CmdPrams.SourceFile)) && (!fileIsModel(g_CmdPrams.DestFile)))
        {
            //int             testpassed = 0;
            Plugin_Analysis = reinterpret_cast<PluginInterface_Analysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
        }
        else
        {
            if (!g_CmdPrams.silent) PrintInfo("Warning: -log is only valid for Images, option is turned off!\n");
            g_CmdPrams.logresults = false;
        }
    }

    // Check if print status line has been assigned
    // if not get it a default to printf
    if (PrintStatusLine == NULL)
    {
        PrintStatusLine = &LocalPrintF;
    }

    // Fix to output view to look the same as v3.1 print info for calls to CMP_ConvertMipTexture
    g_CmdPrams.CompressOptions.m_PrintInfoStr = PrintInfoStr;

    do
    {
        // Initailize stats data and defaults for repeated use in do while()!
        g_CmdPrams.compress_nIterations                       = 0;
        g_CmdPrams.decompress_nIterations                     = 0;
        g_CmdPrams.CompressOptions.format_support_hostEncoder = false;

        // TODO: In many ways BRLG has a different control flow to other types of processing, so maybe it could
        // be taken out of this monolithic if statement?

        if (IsProcessingBRLG(g_CmdPrams) || 
            (!fileIsModel(g_CmdPrams.SourceFile) && !fileIsModel(g_CmdPrams.DestFile)))
        {
            //============
            // Do Analysis
            //============
            if (g_CmdPrams.analysis || g_CmdPrams.diffImage)
            {
                if (!(GenerateAnalysis(g_CmdPrams.SourceFile, g_CmdPrams.DestFile)))
                {
                    LogErrorToCSVFile(ANALYSIS_IMAGE_TESTFAILED);
                    PrintInfo("Error: Image Analysis Failed\n");
                    return -1;
                }
                return 0;
            }

            //===============
            // Do ImageProps
            //===============
            if (g_CmdPrams.imageprops)
            {
                if (!(OutputImageProps(g_CmdPrams.SourceFile)))
                {
                    LogErrorToCSVFile(ANALYSIS_RETRIEVE_IMAGE_PROPERTIES);
                    PrintInfo("Error: Failed to retrieve image properties\n");
                    return -1;
                }
                return 0;
            }

            std::string DestExt;
            std::string SrcExt;
            CMP_FORMAT  destFormat;
            CMP_FORMAT  srcFormat;
            CMP_FORMAT  cmpformat;
            bool        losslessProcessing = false;

            memset(&g_MipSetIn, 0, sizeof(MipSet));

            SrcExt = CMP_GetFilePathExtension(g_CmdPrams.SourceFile);
            helper_to_upper(SrcExt);

            //=====================
            // Set the destination
            //=====================
            if (IsProcessingBRLG(g_CmdPrams))
            {
                destFormat = CMP_FORMAT_BROTLIG;
            }
            else
            {
                if ((g_CmdPrams.DestFile.size() == 0) && (g_CmdPrams.SourceFile.size() > 0))  
                {
                    g_CmdPrams.DestFile = DefaultDestination(g_CmdPrams.SourceFile, g_CmdPrams.CompressOptions.DestFormat, g_CmdPrams.FileOutExt, g_CmdPrams.mangleFileNames);
                    if (!g_CmdPrams.silent) PrintInfo("Destination file was not supplied: Defaulting to %s\n", g_CmdPrams.DestFile.c_str());
                }

                std::string FileName(g_CmdPrams.DestFile);
                DestExt = CMP_GetFilePathExtension(FileName);
                helper_to_upper(DestExt);

                if (!SupportedFileTypes(DestExt))
                {
                    LogErrorToCSVFile(ANALYSIS_DESTINATION_TYPE_NOT_SUPPORTED);
                    PrintInfo("Error: Destination file type is not supported\n");
                    processResult = -2;
                    continue;
                }

                // Check if destination folder exists
                std::string fullPath;
                fullPath = CMP_GetPath(FileName);
                if (fullPath.size() > 0)
                {  // not at current dir
                    if (!CMP_DirExists(fullPath))
                    {
                        LogErrorToCSVFile(ANALYSIS_DESTINATION_TYPE_NOT_SUPPORTED);
                        PrintInfo("Error: Destination folder does not exist for: %s\n", fullPath.c_str());
                        return -2;
                    }
                }

                // Try some known format to attach by supported file ext types
                if (g_CmdPrams.CompressOptions.DestFormat == CMP_FORMAT_Unknown)
                {
#if (OPTION_BUILD_ASTC == 1)
                    if (DestExt.compare("ASTC") == 0)
                        g_CmdPrams.CompressOptions.DestFormat = CMP_FORMAT_ASTC;
                    else
#endif
#ifdef USE_BASIS
                        if (DestExt.compare("BASIS") == 0)
                        g_CmdPrams.CompressOptions.DestFormat = CMP_FORMAT_BASIS;
                    else
#endif
                        if (DestExt.compare("EXR") == 0)
                        g_CmdPrams.CompressOptions.DestFormat = CMP_FORMAT_RGBA_16F;
                    else if (DestExt.compare("BMP") == 0)
                        g_CmdPrams.CompressOptions.DestFormat = CMP_FORMAT_ARGB_8888;
                    else if (DestExt.compare("PNG") == 0)
                        g_CmdPrams.CompressOptions.DestFormat = CMP_FORMAT_ARGB_8888;
                    else if (DestExt.compare("BRLG") == 0)
                        g_CmdPrams.CompressOptions.DestFormat = CMP_FORMAT_BROTLIG;
                }

                destFormat = g_CmdPrams.CompressOptions.DestFormat;

                // Determine if destination file is to be compressed
                DestinationFormatIsCompressed = CMP_IsCompressedFormat(destFormat);

#if (OPTION_BUILD_ASTC == 1)
                if (destFormat != CMP_FORMAT_ASTC)
                {
                    // Check for valid format to destination for ASTC
                    if (DestExt.compare("ASTC") == 0)
                    {
                        LogErrorToCSVFile(ANALYSIS_ASTC_DESTINATION_TYPE_NOT_SUPPORTED);
                        PrintInfo("Error: destination file type only supports ASTC compression format\n");
                        return -1;
                    }
                }
                else
                {
                    // Check for valid format to destination for ASTC
                    if (!((DestExt.compare("ASTC") == 0) || (DestExt.compare("KTX") == 0) || (DestExt.compare("KTX2") == 0)))
                    {
                        LogErrorToCSVFile(ANALYSIS_ASTC_DESTINATION_FILE_FORMAT_NOTSET);
                        PrintInfo("Error: destination file type for ASTC must be set to .astc or .ktx, .ktx2\n");
                        return -1;
                    }

                    //==========================================================
                    // Determine if MIP mapping is set for invalid file formats
                    //==========================================================
                    if ((g_CmdPrams.MipsLevel > 1) && (DestExt.compare("ASTC") == 0))
                    {
                        LogErrorToCSVFile(ANALYSIS_ASTC_MIPMAP_DESTINATION_NOT_SUPPORTED);
                        PrintInfo("Error: destination file type for ASTC must be set to .ktx or .ktx2 for miplevel support\n");
                        return -1;
                    }
                }
#endif
            }

            // Has user set quality settings, if not use a default
            if ((g_CmdPrams.CompressOptions.fquality > 0.00f) && (g_CmdPrams.CompressOptions.fquality < 0.01f))
            {
                // set default max quality for fast processing codecs
                if ((destFormat != CMP_FORMAT_BC7) && (destFormat != CMP_FORMAT_BC6H) && (destFormat != CMP_FORMAT_BC6H_SF))
                {
                    g_CmdPrams.CompressOptions.fquality = 1.0f;
                }
                else
                {  // set default low quality for slow process codecs
                    g_CmdPrams.CompressOptions.fquality = AMD_CODEC_QUALITY_DEFAULT;
                }
            }

            conversion_loopStartTime = timeStampsec();

            //=====================
            // User provided MipSet
            //=====================
            bool loadedCMPFormatedData = false;
            if (p_userMipSetIn) // provided by GUI or user
            {
                memcpy(&g_MipSetIn, p_userMipSetIn, sizeof(MipSet));
                g_MipSetIn.m_pMipLevelTable = p_userMipSetIn->m_pMipLevelTable;
                Delete_gMipSetIn            = false;
            }
            else // CLI
            {
                // ===============================================
                // INPUT IMAGE Swizzling options for DXT formats
                // ===============================================
                // Did User set no swizzle
                if (g_CmdPrams.noswizzle)
                    g_MipSetIn.m_swizzle = false;

                if (g_CmdPrams.doswizzle)
                    g_MipSetIn.m_swizzle = true;

                if (IsProcessingBRLG(g_CmdPrams))
                {
                    losslessProcessing  = true;
                    if (SrcExt.compare("BRLG") == 0)
                        g_MipSetIn.m_format = CMP_FORMAT_BROTLIG;
                    else
                    {
                        loadedCMPFormatedData = false;
                        g_MipSetIn.m_format   = CMP_FORMAT_BINARY;

                        //TODO: for textures check for source type if none texture then use Binary else load using CMP format
                        //if (AMDLoadMIPSTextureImage(g_CmdPrams.SourceFile.c_str(), &g_MipSetIn, g_CmdPrams.use_OCV, &g_pluginManager) == 0)
                        //    loadedCMPFormatedData = true;
                    }
                }

                //========================
                // Load source file
                //======================== 
                if (!p_userMipSetIn && !loadedCMPFormatedData)  // Load MipSet from file, Destination is lossless then load source as a binary into MipSet
                {
                    Delete_gMipSetIn = true;
                    if (!g_CmdPrams.silent) PrintInfo("Processing source     : %s\n", g_CmdPrams.SourceFile.c_str());
                    if (AMDLoadMIPSTextureImage(g_CmdPrams.SourceFile.c_str(), &g_MipSetIn, g_CmdPrams.use_OCV, &g_pluginManager) != 0)
                    {
                        LogErrorToCSVFile(ANALYSIS_FAILED_FILELOAD);
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        PrintInfo(
                            "Error: Could not load source file %s\n\tDouble check file name to make sure it exists, or make sure directory name ends with "
                            "a "
                            "slash.\n",
                            g_CmdPrams.SourceFile.c_str());
                        return -1;
                    }
                }  // load mipset from file

                //===============================================
                // Get initial source data sizes, for logging
                //===============================================
                {
                    MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(&g_MipSetIn, 0, 0);
                    g_CmdPrams.dwHeight   = pInMipLevel->m_nHeight;
                    g_CmdPrams.dwWidth    = pInMipLevel->m_nWidth;
                    g_CmdPrams.dwDataSize = pInMipLevel->m_dwLinearSize;
                }

                //--------------------------------------------
                // Set user specification for Block sizes
                //---------------------------------------------
                g_MipSetIn.m_nBlockWidth  = g_CmdPrams.BlockWidth;
                g_MipSetIn.m_nBlockHeight = g_CmdPrams.BlockHeight;
                g_MipSetIn.m_nBlockDepth  = g_CmdPrams.BlockDepth;
                if (g_CmdPrams.use_noMipMaps)
                {
                    g_MipSetIn.m_Flags = MS_FLAG_DisableMipMapping;
                }

                // check if CubeMap is supported in destination file
                if (g_MipSetIn.m_TextureType == TT_CubeMap)
                {
                    if (!(DestExt.compare("DDS") == 0 || DestExt.compare("KTX") == 0 || DestExt.compare("KTX2") == 0))
                    {
                        LogErrorToCSVFile(ANALYSIS_CUBEMAP_NOTSUPPORTED);
                        PrintInfo("Error: Cube Maps is not supported in destination file.\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }
                }
            }
            
            //========================
            // Set the source format
            //========================
            {
                // User setting overrides file setting in this case
                if (g_CmdPrams.CompressOptions.SourceFormat != CMP_FORMAT_Unknown)
                    srcFormat = g_CmdPrams.CompressOptions.SourceFormat;
                else
                    srcFormat = g_MipSetIn.m_format;

                // Load MIPS did not return a format try to set one
                if (srcFormat == CMP_FORMAT_Unknown)
                {
                    g_MipSetIn.m_format = GetFormat(&g_MipSetIn);
                    if (g_MipSetIn.m_format == CMP_FORMAT_Unknown)
                    {
                        LogErrorToCSVFile(ANALYSIS_UNSUPPORTED_IMAGE_FORMAT);
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        PrintInfo("Error: unsupported input image file format\n");
                        return -1;
                    }

                    srcFormat = g_MipSetIn.m_format;
                }

                // Determine if Source file Is Compressed
                SourceFormatIsCompressed = CMP_IsCompressedFormat(srcFormat);
            }

            //====================================
            // At this point only MipSetIn is set
            //====================================
            frequency = timeStampsec();
            
            //===============================
            // Lossless Processing of source
            //===============================
            if (losslessProcessing)
            {
                if (g_CmdPrams.MipsLevel > 1 || g_CmdPrams.nMinSize > 0 || g_CmdPrams.CompressOptions.genGPUMipMaps)
                    if (!g_CmdPrams.silent) PrintInfo("WARNING: The Brotli-G codec does not support generating mipmaps, so mipmap options will be ignored.\n");
                
                if (g_CmdPrams.CompressOptions.bUseCGCompress || (g_CmdPrams.CompressOptions.nEncodeWith != CMP_Compute_type::CMP_UNKNOWN && g_CmdPrams.CompressOptions.nEncodeWith != CMP_Compute_type::CMP_CPU))
                    if (!g_CmdPrams.silent) PrintInfo("WARNING: Brotli-G currently only supports CPU encoding.\n");

                if (g_CmdPrams.FileOutExt.size() > 0 && g_CmdPrams.FileOutExt != ".brlg")
                {
                    LogErrorToCSVFile(ANALYSIS_DESTINATION_TYPE_NOT_SUPPORTED);
                    PrintInfo("Error: Invalid -fx value '%s' for Brotli-G compression, only 'brlg' is supported.\n", g_CmdPrams.FileOutExt.c_str());
                    processResult = -2;
                }
                else if (SrcExt.compare("BRLG") == 0)
                {
                    // decompression
                    processResult = Process_Lossless_Compressed_To_UncompressedFile(g_CmdPrams.SourceFile, g_CmdPrams.DestFile, &g_MipSetIn, pFeedbackProc);
                }
                else
                {
                    //compression
                    
                    std::string fileName = CMP_GetFileName(g_CmdPrams.DestFile);
                    std::string fileExt = CMP_GetFileExtension(fileName.c_str(), false, true);

                    if (fileName.size() == 0 || fileExt == "BRLG")
                        processResult = Process_Lossless_Uncompressed_To_CompressedFile(g_CmdPrams.SourceFile, g_CmdPrams.DestFile, &g_MipSetIn, pFeedbackProc);
                    else
                    {
                        LogErrorToCSVFile(ANALYSIS_DESTINATION_TYPE_NOT_SUPPORTED);
                        PrintInfo("Error: Invalid destination file type '%s' for Brotli-G compression\n", fileExt.c_str());
                        processResult = -2;
                    }

                }
            }
            //===============================
            // Lossy Processing of source
            //===============================
            else
            {
                //=====================================================
                // Check for Transcode Compressed to Compressed 
                // else Unsupported conversion
                // ====================================================
                if (SourceFormatIsCompressed && DestinationFormatIsCompressed)
                {
                    bool SourceFormatIsBCN = g_MipSetIn.m_format == CMP_FORMAT_BC1 || g_MipSetIn.m_format == CMP_FORMAT_BC2 ||
                                            g_MipSetIn.m_format == CMP_FORMAT_BC3 || g_MipSetIn.m_format == CMP_FORMAT_BC4 ||
                                            g_MipSetIn.m_format == CMP_FORMAT_BC4_S || g_MipSetIn.m_format == CMP_FORMAT_BC5 ||
                                            g_MipSetIn.m_format == CMP_FORMAT_BC5_S || g_MipSetIn.m_format == CMP_FORMAT_BC6H ||
                                            g_MipSetIn.m_format == CMP_FORMAT_BC6H_SF || g_MipSetIn.m_format == CMP_FORMAT_BC7;

                    bool DestFormatIsBCN = destFormat == CMP_FORMAT_BC1 || destFormat == CMP_FORMAT_BC2 || destFormat == CMP_FORMAT_BC3 ||
                                        destFormat == CMP_FORMAT_BC4 || destFormat == CMP_FORMAT_BC4_S || destFormat == CMP_FORMAT_BC5 ||
                                        destFormat == CMP_FORMAT_BC5_S || destFormat == CMP_FORMAT_BC6H || destFormat == CMP_FORMAT_BC6H_SF ||
                                        destFormat == CMP_FORMAT_BC7;

                    if (SourceFormatIsBCN && DestFormatIsBCN && (g_MipSetIn.m_format != destFormat))
                    {
    #ifdef USE_BASIS
                        //============================================================
                        // Process Transcode Compress source using BASIS
                        //============================================================
                        if (CMP_TranscodeFormat)
                        {
                            if (!g_CmdPrams.silent) PrintInfo("\nTranscoding %s to %s\n", GetFormatDesc(g_MipSetIn.m_format), GetFormatDesc(g_MipSetCmp.m_format));
                            if (GTCTranscode(&g_MipSetIn, &g_MipSetCmp, g_pluginManager))
                            {
                                g_MipSetOut.m_nMipLevels = p_MipSetIn->m_nMipLevels;

                                p_MipSetOut = &g_MipSetCmp;
                                //-------------------------------------------------------------
                                // Save the new compressed format
                                //-------------------------------------------------------------
                                p_MipSetOut->m_nBlockWidth  = g_CmdPrams.BlockWidth;
                                p_MipSetOut->m_nBlockHeight = g_CmdPrams.BlockHeight;
                                p_MipSetOut->m_nBlockDepth  = g_CmdPrams.BlockDepth;

                                if (AMDSaveMIPSTextureImage(g_CmdPrams.DestFile.c_str(), &g_MipSetCmp, g_CmdPrams.use_OCV_out, g_CmdPrams.CompressOptions) != 0)
                                {
                                    PrintInfo("Error: saving image failed, write permission denied or format is unsupported for the file extension.\n");
                                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                    return -1;
                                }
                            }
                        }
    #endif
                        // temporarily change cmd parameters to the uncompressed destination format
                        SaveDestFormat = g_CmdPrams.CompressOptions.DestFormat;
                        SaveDestName   = g_CmdPrams.DestFile;

                        g_CmdPrams.CompressOptions.DestFormat = destFormat;
                        g_CmdPrams.DestFile = "transcode_temp.dds";

                        //===================================================
                        // flag a Decompress followed by a compress process
                        //===================================================
                        MidwayDecompress = true;
                        PostCompress     = true;
                    }
                    else
                    {
                        LogErrorToCSVFile(ANALYSIS_TRANSCODE_SRC_TO_DST_NOT_SUPPORTED);
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        if (g_MipSetIn.m_format == destFormat)
                            PrintInfo("Transcoding Error: Both compressed source and destination are the same format\n");
                        else
                            PrintInfo("Transcoding Error: Compressed source and compressed destination selection is not supported\n");
                        return -1;
                    }
                }
    
                //=====================================================
                // Perform swizzle
                // ===================================================
                {
                    if (!p_userMipSetIn)
                    {
                        if (g_MipSetIn.m_swizzle)
                            SwizzleMipSet(&g_MipSetIn);
                    }
                }
    
                CMP_BOOL isGPUEncoding = !((g_CmdPrams.CompressOptions.nEncodeWith == CMP_Compute_type::CMP_CPU)     ||
                                            (g_CmdPrams.CompressOptions.nEncodeWith == CMP_Compute_type::CMP_UNKNOWN) ||
                                            (g_CmdPrams.CompressOptions.nEncodeWith == CMP_Compute_type::CMP_HPC));
    
                //======================================================
                // Determine if MIP mapping is required
                // if so generate the MIP levels for the source file
                //=======================================================
                {
                    if (g_MipSetIn.m_ChannelFormat == CF_Compressed)
                    {
                        // the extra check is added so that the error will not appear in the GUI
                        // this is important because the GUI will automatically add the -miplevels flag on any image with mipmap levels
                        // even if the user didn't set it, so we don't want to show an error for something the user didn't do
                        if (!p_userMipSetIn && g_CmdPrams.MipsLevel > 1 && !g_CmdPrams.use_noMipMaps)
                            if (!g_CmdPrams.silent) PrintInfo("Mipmap generation is not supported for compressed images so it will be skipped.\n");
                    }
                    else if (g_CmdPrams.MipsLevel > 1 && !g_CmdPrams.use_noMipMaps)
                    {
                        int nMinSize;

                        // Precheck user setting against image size
                        CMP_INT nHeight  = g_MipSetIn.m_nHeight;
                        CMP_INT nWidth   = g_MipSetIn.m_nWidth;
                        CMP_INT maxLevel = CMP_CalcMaxMipLevel(nWidth, nHeight, (g_CmdPrams.CompressOptions.genGPUMipMaps || isGPUEncoding));

                        // User has two option to specify MIP levels
                        if (g_CmdPrams.nMinSize > 0)
                        {
                            nMinSize = g_CmdPrams.nMinSize;

                            if (!g_CmdPrams.silent) PrintInfo("Generating mipmaps for minimum size %d...\n", nMinSize);
                        }
                        else
                        {
                            if (maxLevel < g_CmdPrams.MipsLevel)
                            {
                                if (!g_CmdPrams.silent) PrintInfo("Warning miplevels %d is larger then required, value is autoset to use %d\n", g_CmdPrams.MipsLevel, maxLevel);
                            }
                            else
                                maxLevel = g_CmdPrams.MipsLevel;

                            nMinSize = CMP_CalcMinMipSize(g_MipSetIn.m_nHeight, g_MipSetIn.m_nWidth, maxLevel);

                            if (!g_CmdPrams.silent) PrintInfo("Generating %d mipmap levels for source image...\n", maxLevel);
                        }
                        CMP_CFilterParams CFilterParam = {};
                        CFilterParam.dwMipFilterOptions = 0;
                        CFilterParam.nFilterType        = 0;
                        CFilterParam.nMinSize           = nMinSize;
                        CFilterParam.fGammaCorrection   = g_CmdPrams.CompressOptions.fInputFilterGamma;
                        CMP_GenerateMIPLevelsEx((CMP_MipSet*)&g_MipSetIn, &CFilterParam);
                    }
                    else if (g_CmdPrams.CompressOptions.genGPUMipMaps)
                    {
                        // check ranges for GPU use
                        CMP_INT maxlevel = CMP_CalcMaxMipLevel(g_MipSetIn.m_nHeight, g_MipSetIn.m_nWidth, true);
                        if (maxlevel < g_CmdPrams.MipsLevel)
                        {
                            if (!g_CmdPrams.silent) PrintInfo("Warning miplevels %d is larger then required, value is autoset to use %d\n", g_CmdPrams.MipsLevel, maxlevel);
                            g_CmdPrams.MipsLevel = maxlevel;
                        }
                    }
                }

                //--------------------------------
                // Setup Compressed Mip Set
                //--------------------------------
                cmpformat = destFormat;
                {
                    memset(&g_MipSetCmp, 0, sizeof(MipSet));
                    g_MipSetCmp.m_Flags  = MS_FLAG_Default;
                    g_MipSetCmp.m_format = destFormat;

                    // -------------
                    // Output
                    // -------------
                    memset(&g_MipSetOut, 0, sizeof(MipSet));

                    //----------------------------------
                    // Now set data sets pointers for processing
                    //----------------------------------
                    p_MipSetIn  = &g_MipSetIn;
                    p_MipSetOut = &g_MipSetOut;
                }
    
                //=====================================================
                // Case Uncompressed Source to Compressed Destination
                //
                // Example: BMP -> DDS  with -fd Compression flag
                //
                //=====================================================
                if (!g_CmdPrams.silent) PrintInfo("Processing destination: %s\n", g_CmdPrams.DestFile.c_str());
                if ((!SourceFormatIsCompressed) && (DestinationFormatIsCompressed))
                {
                    compress_loopStartTime                   = timeStampsec();
                    g_CmdPrams.CompressOptions.getPerfStats  = true;
                    g_CmdPrams.CompressOptions.getDeviceInfo = true;

                    //--------------------------------------------
                    // V3.1.9000+  new SDK interface using MipSets
                    //--------------------------------------------
                    CMP_ERROR cmp_status;

                    // Use CGP only if it is not a transcoder format
                    if (g_CmdPrams.CompressOptions.bUseCGCompress
    #ifdef USE_BASIS
                        && (g_MipSetCmp.m_format != CMP_FORMAT_BASIS)
    #endif
                        && (g_MipSetCmp.m_format != CMP_FORMAT_BROTLIG))
                    {
                        // Use HPC, GPU Interfaces

                        // Use this for internal CPU verion code tests
                        // KernelOptions   kernel_options;
                        // memset(&kernel_options, 0, sizeof(KernelOptions));
                        //
                        // kernel_options.encodeWith  = (unsigned int)(CMP_HPC);
                        // kernel_options.data_type    = CMP_FORMAT_BC7;
                        // kernel_options.fquality     = 0.05;
                        // kernel_options.threads      = 8;
                        // cmp_status = CMP_ProcessTexture(&g_MipSetIn,&g_MipSetCmp,kernel_options,pFeedbackProc);

                        // Save info for -log
                        if (g_CmdPrams.logresultsToFile)
                        {
                            MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(&g_MipSetIn, 0, 0);
                            g_CmdPrams.dwHeight   = pInMipLevel->m_nHeight;
                            g_CmdPrams.dwWidth    = pInMipLevel->m_nWidth;
                            g_CmdPrams.dwDataSize = pInMipLevel->m_dwLinearSize;
                        }

                        // set for kernel options use, incase of GPU processing
                        g_CmdPrams.CompressOptions.miplevels = g_CmdPrams.MipsLevel;

                        // Process the texture
                        cmp_status = CMP_ConvertMipTextureCGP(&g_MipSetIn, &g_MipSetCmp, &g_CmdPrams.CompressOptions, pFeedbackProc);

                        g_CmdPrams.compress_nIterations = g_MipSetCmp.m_nIterations;
                    }
                    else
                    {
                        // use Compressonator SDK : This is only CPU based. Check if user set GPU, give warning msg
                        if (isGPUEncoding)
                            if (!g_CmdPrams.silent) PrintInfo("Warning! GPU Encoding with this codec is not supported. CPU will be used for compression\n");
                        cmp_status = CMP_ConvertMipTexture((CMP_MipSet*)&g_MipSetIn, (CMP_MipSet*)&g_MipSetCmp, &g_CmdPrams.CompressOptions, pFeedbackProc);
                        g_CmdPrams.compress_nIterations = g_MipSetCmp.m_nIterations;
                    }
        
                    if (cmp_status != CMP_OK)
                    {
                        LogErrorToCSVFile(ANALYSIS_COMPRESSING_TEXTURE);
                        PrintInfo("Error %d: Compressing Texture\n", cmp_status);
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }
        
                    compress_loopEndTime = timeStampsec();

                    // set m_dwFourCC format for default DDS file save, we can check the ext of the destination
                    // but in some cases the FourCC maybe used on other file types!
                    CMP_Format2FourCC(destFormat, &g_MipSetCmp);
                }
    
                //==============================================
                // Save to file destination buffer if
                // Uncompressed file to compressed file format
                //==============================================
                if ((!SourceFormatIsCompressed) && (DestinationFormatIsCompressed) &&
                    (IsDestinationUnCompressed((const char*)g_CmdPrams.DestFile.c_str()) == false))
                {
                    //-------------------------------------------------------------
                    // Set user specification for ASTC Block sizes that was used!
                    //-------------------------------------------------------------
                    g_MipSetCmp.m_nBlockWidth  = g_CmdPrams.BlockWidth;
                    g_MipSetCmp.m_nBlockHeight = g_CmdPrams.BlockHeight;
                    g_MipSetCmp.m_nBlockDepth  = g_CmdPrams.BlockDepth;

                    // pass the file data pointer to the output mip set for saving
                    if (destFormat == CMP_FORMAT_BROTLIG)
                    {
                        if (g_MipSetIn.m_pReservedData != NULL)
                            g_MipSetCmp.m_pReservedData = g_MipSetIn.m_pReservedData;
                        else
                        {
                            std::string FileNameExt = CMP_GetFileNameAndExt(g_CmdPrams.SourceFile);
                            if (FileNameExt.size() < BRLG_MAX_FILE_NAME_SIZE)
                            {
                                BRLG_ExtraInfo* extraInfo = (BRLG_ExtraInfo*)calloc(1, sizeof(BRLG_ExtraInfo));
                                memcpy(extraInfo->fileName, FileNameExt.c_str(), FileNameExt.size());
                                g_MipSetCmp.m_pReservedData = extraInfo;
                            }
                        }
                    }

                    if (g_MipSetIn.m_format == CMP_FORMAT_BROTLIG)
                    {
                        g_MipSetCmp.m_format = CMP_FORMAT_BINARY;
                    }

#ifdef USE_WITH_COMMANDLINE_TOOL
                    if (!g_CmdPrams.silent) PrintInfo("\n");
#endif
                    if (AMDSaveMIPSTextureImage(g_CmdPrams.DestFile.c_str(), &g_MipSetCmp, g_CmdPrams.use_OCV_out, g_CmdPrams.CompressOptions) != 0)
                    {
                        LogErrorToCSVFile(ANALYSIS_FAILED_FILESAVE);
                        PrintInfo("Error: Saving image '%s' failed. Write permission denied or format is unsupported for the file extension.\n",
                                g_CmdPrams.DestFile.c_str());
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }

                    // User requested a DECOMPRESS file also
                    // Set a new destinate and flag a Midway Decompress
                    if (g_CmdPrams.doDecompress)
                    {
                        // Clean the Mipset if any was set
                        if (g_MipSetOut.m_pMipLevelTable)
                        {
                            g_CMIPS->FreeMipSet(&g_MipSetOut);
                        }
                        g_CmdPrams.DestFile = g_CmdPrams.DecompressFile;
                        MidwayDecompress    = true;
                    }
                }
    
                //=====================================================
                // Case UnCompressed Source to UnCompressed Destination
                // Transcoding file formats
                //
                // Case example: BMP -> BMP with -fd uncompression flag
                //
                //=====================================================
                if ((!SourceFormatIsCompressed) && (!DestinationFormatIsCompressed))
                {
                    TranscodeBits = true;
                    // Check if source and destinatation types are supported for transcoding
                    if ((g_MipSetOut.m_TextureType == TT_2D) && (g_MipSetIn.m_TextureType == TT_CubeMap))
                    {
                        LogErrorToCSVFile(ANALYSIS_CUBEMAP_TRANSCODE_NOTSUPPORTED);
                        PrintInfo("Error: Transcoding Cube Maps is not supported.\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }
                }
    
                //=====================================================
                // Case uncompressed Source to UnCompressed Destination
                // with mid way compression
                //
                // Case example: BMP -> BMP with -fd compression flag
                //
                //=====================================================
                if ((!SourceFormatIsCompressed) && (DestinationFormatIsCompressed) && (IsDestinationUnCompressed((const char*)g_CmdPrams.DestFile.c_str()) == true))
                {
                    MidwayDecompress = true;
                    // Prepare for an uncompress request on destination
                    p_MipSetIn = &g_MipSetCmp;
                    srcFormat  = g_MipSetCmp.m_format;
                }
    
                //=====================================================
                // Case Compressed Source to UnCompressed Destination
                // Example(s):
                //                DDS - BMP  with no -fd flag
                //                BMP - BMP  with no -fd flag(s)
                //                BMP - BMP  with    -fd flag(s)
                //
                //=====================================================
                if (MidwayDecompress)
                {
                    if (!g_CmdPrams.silent) PrintInfo("Processed image is been decompressed to new target format!\n");
                }
    
                if (((SourceFormatIsCompressed) && (!DestinationFormatIsCompressed)) || (TranscodeBits) || (MidwayDecompress))
                {
                    compress_loopStartTime = timeStampsec();

                    if (SourceFormatIsCompressed)
                    {
                        // BMP is saved as CMP_FORMAT_ARGB_8888
                        // EXR is saved as CMP_FORMAT_ARGB_32F
                        switch (srcFormat)
                        {
                        case CMP_FORMAT_BC6H:
                        case CMP_FORMAT_BC6H_SF:
                            destFormat                  = CMP_FORMAT_RGBA_16F;
                            g_MipSetOut.m_ChannelFormat = CF_Float16;
                            break;
                        case CMP_FORMAT_BC4_S:
                        case CMP_FORMAT_BC5_S:
                            destFormat                    = CMP_FORMAT_RGBA_8888_S;
                            g_MipSetOut.m_TextureDataType = TDT_XRGB;
                            break;
                        case CMP_FORMAT_BROTLIG:
                            destFormat = g_MipSetIn.m_transcodeFormat;
                            break;
                        default:
                            destFormat = CMP_FORMAT_ARGB_8888;
                            break;
                        }
                    }
                    else
                    {
                        if (MidwayDecompress)
                        {
                            // Need to determine a target format.
                            // Based on file extension.
                            switch (srcFormat)
                            {
                            case CMP_FORMAT_BC6H:
                            case CMP_FORMAT_BC6H_SF:
                                destFormat                  = CMP_FORMAT_RGBA_16F;
                                g_MipSetOut.m_ChannelFormat = CF_Float16;
                                break;
                            case CMP_FORMAT_BC4_S:
                            case CMP_FORMAT_BC5_S:
                                destFormat                    = CMP_FORMAT_RGBA_8888_S;
                                g_MipSetOut.m_TextureDataType = TDT_XRGB;
                                break;
                            //TODO: add a case for CMP_FORMAT_BRLG?
                            default:
                                destFormat = FormatByFileExtension((const char*)g_CmdPrams.DestFile.c_str(), &g_MipSetOut);
                                break;
                            }
                        }
                    }

                    // Something went wrong in determining destination format
                    // Just default it!
                    if (destFormat == CMP_FORMAT_Unknown)
                        destFormat = CMP_FORMAT_ARGB_8888;

                    SaveTempFormat = destFormat;

                    g_MipSetOut.m_ChannelFormat  = GetChannelFormat(destFormat);
                    g_MipSetOut.m_format         = destFormat;
                    g_MipSetOut.m_isDeCompressed = srcFormat != CMP_FORMAT_Unknown ? srcFormat : CMP_FORMAT_MAX;

                    // set m_TextureDataType Correct to Format Type
                    switch (destFormat)
                    {
                    case CMP_FORMAT_RGB_888:
                        g_MipSetOut.m_TextureDataType = TDT_RGB;
                        break;
                    case CMP_FORMAT_RGBA_8888_S:
                        // skip already set
                        break;
                    case CMP_FORMAT_BROTLIG:
                    case CMP_FORMAT_BINARY:
                        g_MipSetOut.m_TextureDataType = TDT_8;
                        break;
                    default:
                        g_MipSetOut.m_TextureDataType = TDT_ARGB;
                        break;
                    }

                    // Allocate output MipSet
                    if (!g_CMIPS->AllocateMipSet(&g_MipSetOut,
                                                g_MipSetOut.m_ChannelFormat,
                                                g_MipSetOut.m_TextureDataType,
                                                p_MipSetIn->m_TextureType,
                                                p_MipSetIn->m_nWidth,
                                                p_MipSetIn->m_nHeight,
                                                p_MipSetIn->m_nDepth))
                    {  // depthsupport, what should nDepth be set as here?
                        LogErrorToCSVFile(ANALYSIS_MEMORY_ERROR2);
                        PrintInfo("Memory Error(2): allocating MIPSet Output buffer\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }

                    g_MipSetOut.m_CubeFaceMask = p_MipSetIn->m_CubeFaceMask;

                    MipLevel* pCmpMipLevel    = g_CMIPS->GetMipLevel(p_MipSetIn, 0);
                    int       nMaxFaceOrSlice = CMP_MaxFacesOrSlices(p_MipSetIn, 0);
                    int       nWidth          = pCmpMipLevel->m_nWidth;
                    int       nHeight         = pCmpMipLevel->m_nHeight;
                    //
                    CMP_BYTE* pMipData = g_CMIPS->GetMipLevel(p_MipSetIn, 0, 0)->m_pbData;

                    bool          use_GPUDecode = g_CmdPrams.CompressOptions.bUseGPUDecompress;
                    CMP_GPUDecode DecodeWith    = g_CmdPrams.CompressOptions.nGPUDecode;

                    decompress_loopStartTime = timeStampsec();

                    for (int nFaceOrSlice = 0; nFaceOrSlice < nMaxFaceOrSlice; nFaceOrSlice++)
                    {
                        int nMipWidth  = nWidth;
                        int nMipHeight = nHeight;

                        for (int nMipLevel = 0; nMipLevel < p_MipSetIn->m_nMipLevels; nMipLevel++)
                        {
                            MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(p_MipSetIn, nMipLevel, nFaceOrSlice);
                            if (!pInMipLevel)
                            {
                                LogErrorToCSVFile(ANALYSIS_MEMORY_ERROR3);
                                PrintInfo("Memory Error(3): allocating MIPSet Output Cmp level buffer\n");
                                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                return -1;
                            }

                            // Valid Mip Level ?
                            if (pInMipLevel->m_pbData)
                                pMipData = pInMipLevel->m_pbData;

                            if (!g_CMIPS->AllocateMipLevelData(g_CMIPS->GetMipLevel(&g_MipSetOut, nMipLevel, nFaceOrSlice),
                                                            nMipWidth,
                                                            nMipHeight,
                                                            g_MipSetOut.m_ChannelFormat,
                                                            g_MipSetOut.m_TextureDataType))
                            {
                                LogErrorToCSVFile(ANALYSIS_MEMORY_ERROR4);
                                PrintInfo("Memory Error(4): allocating MIPSet Output level buffer\n");
                                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                return -1;
                            }

                            //----------------------------
                            // Compressed source
                            //-----------------------------
                            CMP_Texture srcTexture     = {};
                            srcTexture.dwSize          = sizeof(srcTexture);
                            srcTexture.dwWidth         = nMipWidth;
                            srcTexture.dwHeight        = nMipHeight;
                            srcTexture.dwPitch         = 0;
                            srcTexture.nBlockHeight    = p_MipSetIn->m_nBlockHeight;
                            srcTexture.nBlockWidth     = p_MipSetIn->m_nBlockWidth;
                            srcTexture.nBlockDepth     = p_MipSetIn->m_nBlockDepth;
                            srcTexture.format          = srcFormat;
                            srcTexture.transcodeFormat = p_MipSetIn->m_transcodeFormat;

                            if (srcTexture.format == CMP_FORMAT_BROTLIG)
                            {
                                srcTexture.dwDataSize = pInMipLevel->m_dwLinearSize;
                            }

                            srcTexture.dwDataSize = CMP_CalculateBufferSize(&srcTexture);

                            srcTexture.pData = pMipData;

                            g_CmdPrams.dwHeight   = srcTexture.dwHeight;
                            g_CmdPrams.dwWidth    = srcTexture.dwWidth;
                            g_CmdPrams.dwDataSize = srcTexture.dwDataSize;

                            //-----------------------------
                            // Uncompressed Destination
                            //-----------------------------
                            CMP_Texture destTexture  = {};
                            destTexture.dwSize       = sizeof(destTexture);
                            destTexture.dwWidth      = nMipWidth;
                            destTexture.dwHeight     = nMipHeight;
                            destTexture.dwPitch      = 0;
                            destTexture.nBlockHeight = p_MipSetIn->m_nBlockHeight;
                            destTexture.nBlockWidth  = p_MipSetIn->m_nBlockWidth;
                            destTexture.nBlockDepth  = p_MipSetIn->m_nBlockDepth;
                            destTexture.format       = destFormat;
                            destTexture.dwDataSize   = CMP_CalculateBufferSize(&destTexture);
                            destTexture.pData        = g_CMIPS->GetMipLevel(&g_MipSetOut, nMipLevel, nFaceOrSlice)->m_pbData;
                            if (!g_CmdPrams.silent)
                            {
                                if ((destFormat != CMP_FORMAT_BROTLIG) && (g_MipSetIn.m_format != CMP_FORMAT_BROTLIG))
                                    PrintInfo("\rProcessing destination     MipLevel %2d FaceOrSlice %2d", nMipLevel + 1, nFaceOrSlice + 1);
                            }
    #ifdef _WIN32
                            if (/*(srcTexture.dwDataSize > destTexture.dwDataSize) || */ (IsBadWritePtr(destTexture.pData, destTexture.dwDataSize)))
                            {
                                LogErrorToCSVFile(ANALYSIS_MEMORY_ERROR5);
                                PrintInfo("Memory Error(5): Destination image must be compatible with source\n");
                                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                return -1;
                            }
    #else
                            int nullfd = open("/dev/random", O_WRONLY);
                            if (write(nullfd, destTexture.pData, destTexture.dwDataSize) < 0)
                            {
                                LogErrorToCSVFile(ANALYSIS_MEMORY_ERROR5);
                                PrintInfo("Memory Error(5): Destination image must be compatible with source\n");
                                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                return -1;
                            }
                            close(nullfd);
    #endif
                            g_fProgress = -1;

                            if (use_GPUDecode && (srcTexture.format != CMP_FORMAT_BROTLIG))
                            {
    #ifdef _WIN32
    #if (OPTION_BUILD_ASTC == 1)
                                if (srcTexture.format == CMP_FORMAT_ASTC)
                                {
                                    LogErrorToCSVFile(ANALYSIS_ATSC_TRANCODE_WITH_GPU_NOT_SUPPORTED);
                                    PrintInfo("Destination Error: ASTC decompressed with GPU is not supported. Please view ASTC compressed images using CPU.\n");
                                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                    return -1;
                                }
    #endif

                                if (CMP_DecompressTexture(&srcTexture, &destTexture, DecodeWith) != CMP_OK)
                                {
                                    LogErrorToCSVFile(ANALYSIS_DECOMPRESSING_SOURCE);
                                    PrintInfo("Error in Destination source file\n");
                                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                    return -1;
                                }
    #else
                                PrintInfo("GPU Decompress is not supported in linux.\n");
                                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                return -1;
    #endif
                            }
                            else
                            {
                                if (CMP_ConvertTexture(&srcTexture, &destTexture, &g_CmdPrams.CompressOptions, pFeedbackProc) != CMP_OK)
                                {
                                    LogErrorToCSVFile(ANALYSIS_ERROR_COMPRESSING_DESTINATION_TEXTURE);
                                    PrintInfo("Error in compressing destination texture\n");
                                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                    return -1;
                                }
                            }

                            g_CmdPrams.decompress_nIterations++;

                            pMipData += srcTexture.dwDataSize;
                            nMipWidth  = (nMipWidth > 1) ? (nMipWidth >> 1) : 1;
                            nMipHeight = (nMipHeight > 1) ? (nMipHeight >> 1) : 1;
                        }
                    }

                    compress_loopEndTime   = timeStampsec();
                    decompress_loopEndTime = timeStampsec();

                    g_MipSetOut.m_nMipLevels = p_MipSetIn->m_nMipLevels;

                    p_MipSetOut = &g_MipSetOut;

                    // ===============================================
                    // INPUT IMAGE Swizzling options for DXT formats
                    // ===============================================
                    //if (!use_GPUDecode)
                    //    g_MipSetOut.m_swizzle = KeepSwizzle(srcFormat);

                    //-----------------------
                    // User swizzle overrides
                    //-----------------------
                    // Did User set no swizzle
                    if (g_CmdPrams.noswizzle)
                        g_MipSetOut.m_swizzle = false;

                    // Did User set do swizzle
                    if (g_CmdPrams.doswizzle)
                        g_MipSetOut.m_swizzle = true;

                    //=================================
                    // Save to file destination buffer
                    //==================================

                    //-------------------------------------------------------------
                    // Set user specification for block sizes that was used!
                    //-------------------------------------------------------------
                    p_MipSetOut->m_nBlockWidth  = g_CmdPrams.BlockWidth;
                    p_MipSetOut->m_nBlockHeight = g_CmdPrams.BlockHeight;
                    p_MipSetOut->m_nBlockDepth  = g_CmdPrams.BlockDepth;

#ifdef USE_WITH_COMMANDLINE_TOOL
                    if (destFormat != CMP_FORMAT_BROTLIG)
                        if (!g_CmdPrams.silent) PrintInfo("\n");
#endif

#ifdef USE_LOSSLESS_COMPRESSION_BINARY
                    if ((destFormat == CMP_FORMAT_BROTLIG) || (p_MipSetIn->m_format == CMP_FORMAT_BROTLIG))
                    {
                        p_MipSetOut->m_pReservedData = p_MipSetIn->m_pReservedData;
                        p_MipSetIn->m_pReservedData  = 0;
                        // Decoding
                        if (p_MipSetIn->m_format == CMP_FORMAT_BROTLIG)
                            p_MipSetOut->m_format = CMP_FORMAT_BINARY;
                    }
#endif

                    if (AMDSaveMIPSTextureImage(g_CmdPrams.DestFile.c_str(), p_MipSetOut, g_CmdPrams.use_OCV_out, g_CmdPrams.CompressOptions) != 0)
                    {
                        LogErrorToCSVFile(ANALYSIS_FAILED_FILESAVE);
                        PrintInfo("Error: saving image failed, write permission denied or format is unsupported for the file extension.\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }
                }
            } // lossy processing

            conversion_loopEndTime = timeStampsec();

            // Reports and Status
            {
                g_CmdPrams.compress_fDuration = compress_loopEndTime - compress_loopStartTime;
                if (g_CmdPrams.compress_fDuration < 0.001)
                    g_CmdPrams.compress_fDuration = 0.0;

                g_CmdPrams.decompress_fDuration = decompress_loopEndTime - decompress_loopStartTime;
                if (g_CmdPrams.decompress_fDuration < 0.001)
                    g_CmdPrams.decompress_fDuration = 0.0;

                g_CmdPrams.conversion_fDuration = conversion_loopEndTime - conversion_loopStartTime;
                if (g_CmdPrams.conversion_fDuration < 0.001)
                    g_CmdPrams.conversion_fDuration = 0.0;

                if ((!g_CmdPrams.silent) && (g_CmdPrams.showperformance))
                {
#ifdef USE_WITH_COMMANDLINE_TOOL
                    if (!g_CmdPrams.silent) PrintInfo("\r");
#endif

                    if (g_CmdPrams.compress_nIterations)
                        if (!g_CmdPrams.silent) PrintInfo("Compressed to %s with %i iteration(s) in %.3f seconds\n",
                                  GetFormatDesc(cmpformat),
                                  g_CmdPrams.compress_nIterations,
                                  g_CmdPrams.compress_fDuration);

                    if (g_CmdPrams.decompress_nIterations)
                        if (!g_CmdPrams.silent) PrintInfo("Processed to %s with %i iteration(s) in %.3f seconds\n",
                                  GetFormatDesc(destFormat),
                                  g_CmdPrams.decompress_nIterations,
                                  g_CmdPrams.decompress_fDuration);

                    if (!g_CmdPrams.silent) PrintInfo("Total time taken (includes file I/O): %.3f seconds\n", g_CmdPrams.conversion_fDuration);
                }
            }
        }
        else // Model Processing
        {
#ifdef USE_MESH_CLI
            //Mesh Optimization
#ifdef USE_3DMESH_OPTIMIZE
            if (g_CmdPrams.doMeshOptimize)
            {
                if ((fileIsGLTF(g_CmdPrams.SourceFile) && fileIsGLTF(g_CmdPrams.DestFile)) ||
                    (fileIsOBJ(g_CmdPrams.SourceFile) && fileIsOBJ(g_CmdPrams.DestFile)))
                {
                    if (!(OptimizeMesh(g_CmdPrams.SourceFile, g_CmdPrams.DestFile)))
                    {
                        LogErrorToCSVFile(ANALYSIS_MESH_OPTIMIZATION_FAILED);
                        PrintInfo("Error: Mesh Optimization Failed.\n");
                        return -1;
                    }
                }
                else
                {
                    LogErrorToCSVFile(ANALYSIS_MESH_OPTIMIZATION_TYPE_FAILED);
                    PrintInfo("Error: Mesh Optimization Failed. Only glTF->glTF, obj->obj are supported in mesh optimization.\n");
                    return -1;
                }
            }
#endif

            // Mesh Compression and Decompression
            if (!(g_CmdPrams.doMeshOptimize && !g_CmdPrams.use_Draco_Encode))
            {  // skip mesh decompression for case only meshopt turn on: CompressonatorCLI.exe -meshopt source.gltf/obj dest.gltf/obj
                if ((fileIsGLTF(g_CmdPrams.SourceFile) && fileIsGLTF(g_CmdPrams.DestFile))
#ifdef _WIN32
                    || (fileIsOBJ(g_CmdPrams.SourceFile) && fileIsDRC(g_CmdPrams.DestFile)) ||
                    (fileIsDRC(g_CmdPrams.SourceFile) && fileIsOBJ(g_CmdPrams.DestFile)) || (fileIsOBJ(g_CmdPrams.SourceFile) && fileIsOBJ(g_CmdPrams.DestFile))
#endif
                )
                {
                    if (!(CompressDecompressMesh(g_CmdPrams.SourceFile, g_CmdPrams.DestFile)))
                    {
                        LogErrorToCSVFile(ANALYSIS_MESH_COMPRESSION_FAILED);
                        PrintInfo("Error: Mesh Compression Failed.\n");
                        return -1;
                    }
                }
                else
                {
#ifdef _WIN32
                    PrintInfo(
                        "Note: Mesh Compression Failed. Only glTF->glTF, obj->drc(compression), drc->obj(decompression), obj->obj(optimize first then "
                        "compress to .drc file) are supported in mesh compression/decompression.\n");
#else
                    PrintInfo("Note: Mesh Compression Failed. Only glTF->glTF are supported in mesh compression/decompression.\n");
#endif
                    LogErrorToCSVFile(ANALYSIS_MESH_COMPRESSION_FAILED);
                    return -1;
                }
            }
#endif
        }

        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);

#ifdef SHOW_PROCESS_MEMORY
        bool result2 = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter2, sizeof(memCounter2));
#endif

        if (g_CmdPrams.logresults)
        {
            if (!g_CmdPrams.silent)
            {
#ifdef USE_WITH_COMMANDLINE_TOOL
                PrintInfo("\rlog results                                           ");
#endif
            }

            CMP_ANALYSIS_DATA analysisData = {0};
            analysisData.SSIM              = -1;  // Set data content is invalid and not processed

            if (Plugin_Analysis)
            {
                // Diff File
                {
                    g_CmdPrams.DiffFile = g_CmdPrams.DestFile;
                    int lastindex       = (int)g_CmdPrams.DiffFile.find_last_of(".");
                    g_CmdPrams.DiffFile = g_CmdPrams.DiffFile.substr(0, lastindex);
                    g_CmdPrams.DiffFile.append("_diff.bmp");
                }  // Image Diff

                // Analysis only for when compresing images and not ( Lossless or LDR vs HDR )
                bool CMP_HDR    = CMP_IsHDR(g_MipSetCmp.m_format);
                bool IN_HDR     = CMP_IsHDR(g_MipSetIn.m_format);
                bool IS_LOSSLSS = CMP_IsLossless(g_MipSetCmp.m_format);
                bool LDR_HDR    = !IN_HDR && CMP_HDR;
                bool HDR_LDR    = IN_HDR && !CMP_HDR;

                if ((!IS_LOSSLSS) && (!LDR_HDR) && (!HDR_LDR))
                {
                    if (Plugin_Analysis->TC_ImageDiff(g_CmdPrams.SourceFile.c_str(),
                                                      g_CmdPrams.DestFile.c_str(),
                                                      "",  // g_CmdPrams.DiffFile.c_str(),  Skip image diff for now , it takes too long to process
                                                      "",
                                                      &analysisData,
                                                      &g_pluginManager,
                                                      NULL) != 0)
                    {
                        analysisData.SSIM = -2;
                    }
                }  // Analysis report
                else
                    analysisData.SSIM = -1;
            }

            ProcessResults(g_CmdPrams, analysisData);

            if ((analysisData.SSIM != -1) && ((analysisData.SSIM != -2)))
            {
                psnr_sum += analysisData.PSNR;
                ssim_sum += analysisData.SSIM;
                process_time_sum += g_CmdPrams.compress_fDuration;
                total_processed_items++;  // used to track number of processed items and used for avg of Process Time, SSIM and PSNR stats.
            }
            // Reset single use items
            g_CmdPrams.compute_setup_fDuration = 0;
        }

        //==============================================================
        // Get a new set of source and destination files for processing
        //==============================================================
        if (PostCompress)
        {
            MidwayDecompress                        = false;
            PostCompress                            = false;
            MoreSourceFiles                         = true;
            g_CmdPrams.SourceFile                   = g_CmdPrams.DestFile;
            g_CmdPrams.DestFile                     = SaveDestName;
            g_CmdPrams.CompressOptions.SourceFormat = SaveTempFormat;
            g_CmdPrams.CompressOptions.DestFormat   = SaveDestFormat;
            
            // allow app to load mipset for transcode_temp.dds
            p_userMipSetIn = NULL;
        }
        else if (g_CmdPrams.SourceFileList.size() > 0)
        {
            // TODO: This and the directory stuff in ProcessCMDLineOptions needs to be reworked and simplified

            MoreSourceFiles = true;
            // Set the first file in list to SourceFile and delete it from the list
            g_CmdPrams.SourceFile = g_CmdPrams.SourceFileList[0].c_str();
            g_CmdPrams.SourceFileList.erase(g_CmdPrams.SourceFileList.begin());

            processedFileList.push_back(CMP_GetFileName(g_CmdPrams.DestFile));

            std::string destFileName = "";
            
            if (g_CmdPrams.CompressOptions.SourceFormat != CMP_FORMAT_BROTLIG && g_CmdPrams.CompressOptions.DestFormat != CMP_FORMAT_BINARY)
            {
                destFileName = DefaultDestination(g_CmdPrams.SourceFile, g_CmdPrams.CompressOptions.DestFormat, g_CmdPrams.FileOutExt, g_CmdPrams.mangleFileNames);
            }

            // if a previous file name exists, force a mangled file name
            if (!g_CmdPrams.mangleFileNames)
            {
                if (std::find(processedFileList.begin(), processedFileList.end(), destFileName) != processedFileList.end())
                {
                    destFileName = DefaultDestination(g_CmdPrams.SourceFile, g_CmdPrams.CompressOptions.DestFormat, g_CmdPrams.FileOutExt, true);
                }
            }

            if (g_CmdPrams.DestDir.empty())
                g_CmdPrams.DestFile = destFileName;
            else
                g_CmdPrams.DestFile = g_CmdPrams.DestDir + "/" + destFileName;

        }
        else
            MoreSourceFiles = false;

        if (!g_CmdPrams.silent)
        {
            if ((g_CmdPrams.logresults) && (!g_CmdPrams.logresultsToFile))
            {
                // check if any data was calculated
                if (g_CmdPrams.SSIM > 0)
                {
                    PrintInfo("MSE %.2f PSRN %.2f SSIM %.4f\n", g_CmdPrams.MSE, g_CmdPrams.PSNR, g_CmdPrams.SSIM);
                }
            }

            PrintInfo("Done Processing\n\n");
        }

    } while (MoreSourceFiles);

    processedFileList.clear();

    //===================
    // Final Log Summary
    //===================
    if (g_CmdPrams.logresults)
    {
        if (Plugin_Analysis)
            delete Plugin_Analysis;

        if (total_processed_items > 1)
        {
            char buff[128];
            snprintf(buff,
                     sizeof(buff),
                     "Average      : PSNR: %.2f  SSIM: %.4f  Time %.3f Sec for %d item(s) \n",
                     psnr_sum / total_processed_items,
                     ssim_sum / total_processed_items,
                     process_time_sum / total_processed_items,
                     total_processed_items);
            LogToResults(g_CmdPrams, buff);
        }

        if (!g_CmdPrams.logcsvformat)
            LogToResults(g_CmdPrams, "--------------\n");
    }

    return processResult;
}


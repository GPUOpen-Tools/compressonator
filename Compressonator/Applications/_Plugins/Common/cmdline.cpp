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

#include "cmdline.h"
#include "ATIFormats.h"
#include "Compressonator.h"
#include "Texture.h"
#include "TextureIO.h"
#include "MIPS.h"
#include "PluginManager.h"
#include "PluginInterface.h"
#include "TC_PluginInternal.h"
#include "Version.h"

#include <ImfStandardAttributes.h>
#include <ImathBox.h>
#include <ImfArray.h>
#include <ImfRgba.h>

#ifdef USE_MESH_CLI
#include <boost/algorithm/string.hpp>
#include "gltf/tiny_gltf2.h"
#include "GltfCommon.h"
#include "ModelData.h"
#include "UtilFuncs.h"
using namespace tinygltf2;
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

// #define SHOW_PROCESS_MEMORY
// #define USE_COMPUTE
#define USE_SWIZZLE

#ifndef _WIN32
#include <stdarg.h>
#include <fcntl.h>  /* For O_RDWR */
#include <unistd.h> /* For open(), creat() */
#include <time.h>
#endif

#ifdef SHOW_PROCESS_MEMORY
#include "windows.h"
#include "psapi.h"
#endif

CCmdLineParamaters g_CmdPrams;

string DefaultDestination(string SourceFile, CMP_FORMAT DestFormat)
{
    string                  DestFile = "";
    boost::filesystem::path fp(SourceFile);
    string                  file_name = fp.stem().string();
    DestFile.append(file_name);
    DestFile.append("_");
    string file_ext = fp.extension().string();
    file_ext.erase(std::remove(file_ext.begin(), file_ext.end(), '.'), file_ext.end());
    DestFile.append(file_ext);
    DestFile.append("_");
    DestFile.append(GetFormatDesc(DestFormat));
    DestFile.append(".DDS");
    return DestFile;
}

#ifdef USE_MESH_CLI
// check if file is glTF format extension
bool fileIsGLTF(string SourceFile)
{
    boost::filesystem::path fp(SourceFile);
    string                  file_ext = fp.extension().string();
    boost::algorithm::to_upper(file_ext);
    return (file_ext.compare(".GLTF") == 0);
}

// check if file is OBJ format extension
bool fileIsOBJ(string SourceFile)
{
    boost::filesystem::path fp(SourceFile);
    string                  file_ext = fp.extension().string();
    boost::algorithm::to_upper(file_ext);
    return (file_ext.compare(".OBJ") == 0);
}

// check if file is DRC (draco compressed OBJ file) format extension
bool fileIsDRC(string SourceFile)
{
    boost::filesystem::path fp(SourceFile);
    string                  file_ext = fp.extension().string();
    boost::algorithm::to_upper(file_ext);
    return (file_ext.compare(".DRC") == 0);
}

bool fileIsModel(string SourceFile)
{
    return (fileIsGLTF(SourceFile) || fileIsOBJ(SourceFile) || fileIsDRC(SourceFile));
}
#endif

#ifdef USE_WITH_COMMANDLINE_TOOL

int GetNumberOfCores(wchar_t* envp[])
{
    int i, cores = 1;

    for (i = 0; envp[i] != NULL; i++)
    {
        //wprintf(L"var = %ws\n", envp[i]);
        cores = wcsncmp(envp[i], L"NUMBER_OF_PROCESSORS", 20);
        if (cores == 0)
        {
            wchar_t* p_envp;
            size_t   equal = wcscspn(envp[i], L"=");
            if ((equal > 0) && (equal < wcslen(envp[i])))
            {
                p_envp = envp[i] + equal + 1;
                wchar_t num[16];
                wcsncpy_s(num, p_envp, 16);
                cores = _wtoi(num);
                return cores > 0 ? cores : 1;
            }
            break;
        }
    }
    return 1;
}

#else  // Code is shared with GUI
#endif

CMP_GPUDecode DecodeWith(const char* strParameter)
{
    if (strcmp(strParameter, "DirectX") == 0)
        return GPUDecode_DIRECTX;
    else if (strcmp(strParameter, "OGL") == 0)
        return GPUDecode_OPENGL;
    else if (strcmp(strParameter, "OpenGL") == 0)
        return GPUDecode_OPENGL;
    else if (strcmp(strParameter, "Vulkan") == 0)
        return GPUDecode_VULKAN;
    else
        return GPUDecode_INVALID;
}

CMP_Compute_type EncodeWith(const char* strParameter)
{
    if (strcmp(strParameter, "DX") == 0)
        return Compute_DIRECTX;
    else if (strcmp(strParameter, "DirectX") == 0)
        return Compute_DIRECTX;
    else if (strcmp(strParameter, "CL") == 0)
        return Compute_OPENCL;
    else if (strcmp(strParameter, "OpenCL") == 0)
        return Compute_OPENCL;
    else if (strcmp(strParameter, "VK") == 0)
        return Compute_VULKAN;
    else if (strcmp(strParameter, "Vulkan") == 0)
        return Compute_VULKAN;
    else
        return Compute_INVALID;
}

bool ProcessSingleFlags(const char* strCommand)
{
    bool isset = false;
    if ((strcmp(strCommand, "-nomipmap") == 0))
    {
        g_CmdPrams.use_noMipMaps = true;
        isset                    = true;
    }
#ifdef USE_MESH_DRACO_EXTENSION
    else if ((strcmp(strCommand, "-draco") == 0))
    {
        g_CmdPrams.use_Draco_Encode = true;
        isset                       = true;
    }
#endif
    else if ((strcmp(strCommand, "-silent") == 0))
    {
        g_CmdPrams.silent = true;
        isset             = true;
    }
    else if ((strcmp(strCommand, "-performance") == 0))
    {
        g_CmdPrams.showperformance = true;
        isset                      = true;
    }
    else if ((strcmp(strCommand, "-noprogress") == 0))
    {
        g_CmdPrams.noprogressinfo = true;
        isset                     = true;
    }
    else if ((strcmp(strCommand, "-noswizzle") == 0))
    {
        g_CmdPrams.noswizzle = true;
        isset                = true;
    }
    else if ((strcmp(strCommand, "-doswizzle") == 0))
    {
        g_CmdPrams.doswizzle = true;
        isset                = true;
    }
    else if ((strcmp(strCommand, "-analysis") == 0) || (strcmp(strCommand, "-Analysis") == 0))
    {
        g_CmdPrams.analysis = true;
        isset               = true;
    }
    else if ((strcmp(strCommand, "-diff_image") == 0))
    {
        g_CmdPrams.diffImage = true;
        isset                = true;
    }
    else if ((strcmp(strCommand, "-imageprops") == 0))
    {
        g_CmdPrams.imageprops = true;
        isset                 = true;
    }
#ifdef USE_3DMESH_OPTIMIZE
    else if ((strcmp(strCommand, "-meshopt") == 0))
    {
        g_CmdPrams.doMeshOptimize = true;
        isset                     = true;
    }
#endif
#ifdef USE_COMPUTE
    else if ((strcmp(strCommand, "-UseGPUCompress") == 0))
    {
        g_CmdPrams.CompressOptions.bUseGPUCompress = true;
        isset                                      = true;
    }
#endif
    else if (strcmp(strCommand, "-UseGPUDecompress") == 0)
    {
        g_CmdPrams.CompressOptions.bUseGPUDecompress = true;
        isset                                        = true;
    }
    return isset;
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
        if (strcmp(strCommand, "-BlockRate") == 0)
        {
            if (strchr(strParameter, 'x') != NULL)
            {
                int dimensions = sscanf(strParameter, "%dx%dx", &g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight);
                if (dimensions < 2)
                    throw "Command Parameter is invalid";
                else
                {
                    astc_find_closest_blockxy_2d(&g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight, 0);
                }
            }
            else
            {
                float m_target_bitrate = static_cast<float>(atof(strParameter));
                if (m_target_bitrate > 0)
                    astc_find_closest_blockdim_2d(m_target_bitrate, &g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight, 0);
                else
                    throw "Command Parameter is invalid";
            }
        }
        else if (strcmp(strCommand, "-Quality") == 0)
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
            int value = std::stof(strParameter);
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
            int value = std::stof(strParameter);
            if ((value < 1) || (value > 255))
            {
                throw "Alpha threshold value should be in range of 1 to 255";
            }
            g_CmdPrams.CompressOptions.nAlphaThreshold = value;
        }
        else if (strcmp(strCommand, "-DXT1UseAlpha") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No DXT1UseAlpha value specified (default is 0:off; 1:on)";
            }
            int value = std::stof(strParameter);
            if ((value < 0) || (value > 1))
            {
                throw "DXT1UseAlpha value should be 1 or 0";
            }
            g_CmdPrams.CompressOptions.bDXT1UseAlpha   = bool(value);
            g_CmdPrams.CompressOptions.nAlphaThreshold = 128;  //default to 128
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

            int value = std::stof(strParameter);
            if ((value < 0) || (value > 1))
            {
                throw "Optimize vertex fetch value should be 1(enabled) or 0(disabled).";
            }
            g_CmdPrams.CompressOptions.bVertexFetch = bool(value);
        }
#endif
#ifdef USE_COMPUTE
        else if (strcmp(strCommand, "-EncodeWith") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No API specified (set either OpenCL or DirectX (Default is OpenCL).";
            }

            g_CmdPrams.CompressOptions.nComputeWith = EncodeWith((char*)strParameter);

            if (g_CmdPrams.CompressOptions.nComputeWith == Compute_INVALID)
            {
                throw "Unknown API format specified.";
            }

            g_CmdPrams.CompressOptions.bUseGPUCompress = true;
        }
#endif
        else if (strcmp(strCommand, "-decomp") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no destination file specified";
            }
            g_CmdPrams.DecompressFile = (char*)strParameter;
            g_CmdPrams.doDecompress   = true;
        }
#ifdef USE_MESH_DRACO_EXTENSION
        else if (strcmp(strCommand, "-dracolvl") == 0)  //draco compression level
        {
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
        else if (strcmp(strCommand, "-qpos") == 0)  //quantization bit for positon
        {
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
        else if (strcmp(strCommand, "-qtexc") == 0)  //quantization bit for texture coordinates
        {
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
        else if (strcmp(strCommand, "-qnorm") == 0)  //quantization bit for normal
        {
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
        else if (strcmp(strCommand, "-qgen") == 0)  //quantization bit for generic
        {
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
        else if (strcmp(strCommand, "-fs") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no format specified";
            }

            g_CmdPrams.SourceFormat = ParseFormat((char*)strParameter);

            if (g_CmdPrams.SourceFormat == CMP_FORMAT_Unknown)
            {
                throw "unknown format specified";
            }
        }
        else if (strcmp(strCommand, "-fd") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no format specified";
            }

            g_CmdPrams.DestFormat = ParseFormat((char*)strParameter);

            if (g_CmdPrams.DestFormat == CMP_FORMAT_Unknown)
            {
                throw "unknown format specified";
            }
        }
        else if ((strcmp(strCommand, "-miplevels") == 0))
        {
            if (strlen(strParameter) == 0)
            {
                throw "no level is specified";
            }

            try
            {
                g_CmdPrams.MipsLevel = boost::lexical_cast<int>(strParameter);
            }
            catch (boost::bad_lexical_cast)
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
                g_CmdPrams.nMinSize = boost::lexical_cast<int>(strParameter);
            }
            catch (boost::bad_lexical_cast)
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
        }
        else
        {
            if (strlen(strParameter) > 0)
            {
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
            {  // Flags or Source and destination files specified

                if (g_CmdPrams.SourceFile.length() == 0)
                {
                    g_CmdPrams.SourceFile = strCommand;
                }
                else if (g_CmdPrams.DestFile.length() == 0)
                {
                    g_CmdPrams.DestFile   = strCommand;
                    string file_extension = boost::filesystem::extension(strCommand);
                    // User did not supply a destination extension default
                    if (file_extension.length() == 0)
                    {
                        if (g_CmdPrams.DestFile.length() == 0)
                        {
                            g_CmdPrams.DestFile = DefaultDestination(g_CmdPrams.SourceFile, g_CmdPrams.DestFormat);
                            PrintInfo("Destination Texture file was not supplied: Defaulting to %s\n", g_CmdPrams.DestFile.c_str());
                        }
                        else
                            g_CmdPrams.DestFile.append(".DDS");
                    }
                }
                else
                {
                    throw "unknown source or destination file";
                }
            }
        }

    }  // Try code

    catch (const char* str)
    {
        PrintInfo("Option [%s] : %s\n\n", strCommand, str);
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
            if ((strTemp[0] == '-') && (i < (argc - 1)))
            {
                if (!ProcessSingleFlags(strCommand.c_str()))
                {
                    i++;
                    strParameter = argv[i];
                    if (!ProcessCMDLineOptions(strCommand.c_str(), strParameter.c_str()))
                    {
                        throw "Invalid Command";
                    }
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
            PrintInfo("\rProcessing progress = %2.0f  MipLevel = %2d", fProgress, g_MipLevel);
        else
            PrintInfo("\rProcessing progress = %2.0f", fProgress);

        g_fProgress = fProgress;
    }

    return g_bAbortCompression;
}

int CalcMinMipSize(int nHeight, int nWidth, int MipsLevel)
{
    while (MipsLevel > 0)
    {
        nWidth  = max(nWidth >> 1, 1);
        nHeight = max(nHeight >> 1, 1);
        MipsLevel--;
    }

    if (nWidth > nHeight)
        return (nHeight);
    return (nWidth);
}

bool TC_PluginCodecSupportsFormat(const MipSet* pMipSet)
{
    return (pMipSet->m_ChannelFormat == CF_8bit || pMipSet->m_ChannelFormat == CF_16bit || pMipSet->m_ChannelFormat == CF_2101010 ||
            pMipSet->m_ChannelFormat == CF_32bit || pMipSet->m_ChannelFormat == CF_Float16 || pMipSet->m_ChannelFormat == CF_Float32);
}

void cleanup(bool Delete_gMipSetIn, bool SwizzleMipSetIn)
{
#ifdef _WIN32
    SetDllDirectory(NULL);
#endif
    if (Delete_gMipSetIn)
    {
        if (g_MipSetIn.m_pMipLevelTable)
        {
            g_CMIPS->FreeMipSet(&g_MipSetIn);
            g_MipSetIn.m_pMipLevelTable = NULL;
        }
    }
    else
    {
        if (SwizzleMipSetIn)
        {
            SwizzleMipMap(&g_MipSetIn);
        }
    }

    if (g_MipSetCmp.m_pMipLevelTable)
    {
        g_CMIPS->FreeMipSet(&g_MipSetCmp);
        g_MipSetCmp.m_pMipLevelTable = NULL;
    }

    if (g_MipSetOut.m_pMipLevelTable)
    {
        g_CMIPS->FreeMipSet(&g_MipSetOut);
        g_MipSetOut.m_pMipLevelTable = NULL;
    }

    if (g_CMIPS)
    {
        free(g_CMIPS);
        g_CMIPS = NULL;
    }
}

// mesh optimization process
// only support case glTF->glTF, case obj->obj
bool OptimizeMesh(std::string SourceFile, std::string DestFile)
{
    if (!(boost::filesystem::exists(SourceFile)))
    {
        PrintInfo("Error: Source Model Mesh File is not found.\n");
        return false;
    }

    void*       modelDataIn  = nullptr;
    void*       modelDataOut = nullptr;
    GLTFCommon* gltfdata     = nullptr;

    // load model
    string file_extension = boost::filesystem::extension(SourceFile);
    boost::algorithm::to_upper(file_extension);
    boost::erase_all(file_extension, ".");
    PluginInterface_3DModel_Loader* plugin_loader;
    plugin_loader = reinterpret_cast<PluginInterface_3DModel_Loader*>(g_pluginManager.GetPlugin("3DMODEL_LOADER", (char*)file_extension.c_str()));

    if (plugin_loader)
    {
        plugin_loader->TC_PluginSetSharedIO(g_CMIPS);
        void* msgHandler = NULL;

        int result;
        if (result = plugin_loader->LoadModelData(g_CmdPrams.SourceFile.c_str(), "", &g_pluginManager, msgHandler, &CompressionCallback) != 0)
        {
            if (result != 0)
                throw("Error Loading Model Data");
        }
        if (fileIsGLTF(g_CmdPrams.SourceFile))
        {
            gltfdata = (GLTFCommon*)plugin_loader->GetModelData();
            if (gltfdata)
            {
                if (gltfdata->m_meshBufferData.m_meshData[0].vertices.size() > 0)
                    modelDataIn = (void*)&(gltfdata->m_meshBufferData);
                else
                {
                    modelDataIn = nullptr;
                    PrintInfo("[Mesh Optimization] Error in processing mesh. Mesh data format size is not supported.");
                    return false;
                }
            }
        }
        else
            modelDataIn = plugin_loader->GetModelData();
    }
    else
    {
        PrintInfo("[Mesh Optimization] Loading file error.: %s\n.", SourceFile);
        if (plugin_loader)
        {
            delete plugin_loader;
            plugin_loader = nullptr;
        }
        return false;
    }

    PrintInfo("Processing: Mesh Optimization...\n");
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
        std::vector<CMP_Mesh>*          optimized          = ((std::vector<CMP_Mesh>*)modelDataOut);
        PluginInterface_3DModel_Loader* plugin_save        = NULL;
        string                          destfile_extension = boost::filesystem::extension(DestFile);
        boost::algorithm::to_upper(destfile_extension);
        boost::erase_all(destfile_extension, ".");
        plugin_save =
            reinterpret_cast<PluginInterface_3DModel_Loader*>(g_pluginManager.GetPlugin("3DMODEL_LOADER", (char*)destfile_extension.c_str()));
        if (plugin_save)
        {
            plugin_save->TC_PluginSetSharedIO(g_CMIPS);

            int result = 0;
            if (fileIsGLTF(g_CmdPrams.DestFile))
            {
                if (gltfdata)
                {
                    GLTFCommon optimizedGltf;
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
                    PrintInfo("[Mesh Optimization] Success in saving optimized obj data.\n");
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
                PrintInfo("[Mesh Optimization] Error in saving mesh file.");
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
                PrintInfo("[Mesh Optimization] Saving %s done.\n", g_CmdPrams.DestFile.c_str());
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

    return true;
}

// mesh draco compression/decompression
bool CompressDecompressMesh(std::string SourceFile, std::string DestFile)
{
    if (!(boost::filesystem::exists(SourceFile)))
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
            model.dracomeshes.clear();

            std::string srcFile = SourceFile;
            std::string dstFile = DestFile;
            // Check if mesh optimization was done if so then source is optimized file
            if (g_CmdPrams.doMeshOptimize)
            {
                srcFile = DestFile;
                if (!(boost::filesystem::exists(srcFile)))
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
                PrintInfo("Success in loading glTF file : [%s].\n", srcFile.c_str());
            }

            bool is_draco_src = false;
#ifdef USE_MESH_DRACO_EXTENSION
            if (model.dracomeshes.size() > 0)
            {
                is_draco_src = true;
            }
#endif
            err.clear();

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
                PrintInfo("Success in writting glTF file : [%s].\n", dstFile.c_str());
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
    PluginInterface_3DModel_Loader* m_plugin_loader_drc = NULL;

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

            PrintInfo("Processing: Mesh Compression/Decompression...\n");

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
                    if (result = m_plugin_loader_drc->LoadModelData("OBJ", NULL, &DracoOptions, nullptr, &CompressionCallback) != 0)
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
                            m_plugin_loader_drc->LoadModelData(DracoOptions.input.c_str(), NULL, &DracoOptions, nullptr, &CompressionCallback) != 0)
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

    return true;
}

//cmdline only
bool GenerateAnalysis(std::string SourceFile, std::string DestFile)
{
    if (!(boost::filesystem::exists(SourceFile)))
    {
        PrintInfo("Error: Source Image File is not found.\n");
        return false;
    }

    if (!(boost::filesystem::exists(DestFile)))
    {
        PrintInfo("Error: Destination Image File is not found.\n");
        return false;
    }

    PluginInterface_Analysis* Plugin_Analysis;
    int                       testpassed = 0;
    Plugin_Analysis                      = reinterpret_cast<PluginInterface_Analysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        if (g_CmdPrams.diffImage)
        {
            g_CmdPrams.DiffFile = DestFile;
            int lastindex       = g_CmdPrams.DiffFile.find_last_of(".");
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
            int index    = results_file.find_last_of("/");
            results_file = results_file.substr(0, (index + 1));
            results_file.append("Analysis_Result.xml");
        }

        testpassed = Plugin_Analysis->TC_ImageDiff(SourceFile.c_str(), DestFile.c_str(), g_CmdPrams.DiffFile.c_str(), (char*)results_file.c_str(),
                                                   &g_pluginManager, NULL);
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
bool GenerateImageProps(std::string ImageFile)
{
    if (!(boost::filesystem::exists(ImageFile)))
    {
        PrintInfo("Error: Image File is not found.\n");
        return false;
    }

    // file name
    PrintInfo("File Name: %s\n", ImageFile.c_str());

    // full path
    boost::filesystem::path p(ImageFile);
    boost::filesystem::path fullpath = boost::filesystem::absolute(p);
    PrintInfo("File Full Path: %s\n", fullpath.generic_string().c_str());

    // file size
    uintmax_t filesize = boost::filesystem::file_size(ImageFile);

    if (filesize > 1024000)
    {
        filesize /= 1024000;
        PrintInfo("File Size: %ju MB\n", filesize);
    }
    else if (filesize > 1024)
    {
        filesize /= 1024;
        PrintInfo("File Size: %ju KB\n", filesize);
    }
    else
    {
        PrintInfo("File Size: %ju Bytes\n", filesize);
    }

    // load image into mipset
    if (AMDLoadMIPSTextureImage(ImageFile.c_str(), &g_MipSetIn, g_CmdPrams.use_OCV, &g_pluginManager) != 0)
    {
        PrintInfo("Error: reading image, data type not supported.\n");
        return false;
    }

    if (&g_MipSetIn)
    {
        //image size

        CMIPS     CMips;
        MipLevel* pInMipLevel = CMips.GetMipLevel(&g_MipSetIn, 0, 0);
        uintmax_t imagesize   = pInMipLevel->m_dwLinearSize;

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
        PrintInfo("Image Width: %u px\n", g_MipSetIn.m_nWidth);
        PrintInfo("Image Height: %u px\n", g_MipSetIn.m_nHeight);

        //miplevel, format
        int miplevel = g_MipSetIn.m_nMipLevels;

        PrintInfo("Mip Levels: %u\n", miplevel);
        PrintInfo("Format: %s\n", GetFormatDesc(g_MipSetIn.m_format));
    }

    return true;
}

void LocalPrintF(char* buff)
{
    printf(buff);
}

#ifdef USE_COMPUTE
#include "Common_Kernel.h"
#include "Compute_Lib\Compute_Base.h"

#ifdef _DEBUG
#pragma comment(lib, "Compute_lib_MDd.lib")
#else
#pragma comment(lib, "Compute_lib_MD.lib")
#endif

bool SVMInitCodec(KernalOptions* options)
{
    CMP_FORMAT format = (CMP_FORMAT)options->data_type;
    switch (format)
    {
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
    case CMP_FORMAT_BC7:
    case CMP_FORMAT_ASTC:
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

#endif

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

int ProcessCMDLine(CMP_Feedback_Proc pFeedbackProc, MipSet* p_userMipSetIn)
{
    double frequency, conversion_loopStartTime = {0}, conversion_loopEndTime = {0}, compress_loopStartTime = {0}, compress_loopEndTime = {0},
                      decompress_loopStartTime = {0}, decompress_loopEndTime = {0};
    int compress_nIterations = 0, decompress_nIterations = 0;

    // These flags indicate if the source and destination files are compressed
    bool SourceFormatIsCompressed      = false;
    bool DestinationFileIsCompressed   = false;
    bool DestinationFormatIsCompressed = false;

    bool TranscodeBits    = false;
    bool MidwayDecompress = false;

    //  With a user suppiled Mip Map dont delete it on exit
    bool Delete_gMipSetIn = false;

    // flags if an user suppiled Mip Map input was swizzled during compression / decompression
    bool SwizzledMipSetIn = false;

    // Currently active input and output mips buffers
    // These point to the allocated g_MipSetxxx buffers
    // depending on the users requirement for input and output file formats
    MipSet* p_MipSetIn;
    MipSet* p_MipSetOut;

#ifdef SHOW_PROCESS_MEMORY
    PROCESS_MEMORY_COUNTERS memCounter1;
    PROCESS_MEMORY_COUNTERS memCounter2;

#ifdef SHOW_PROCESS_MEMORY
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter1, sizeof(memCounter1));
#endif

#endif
    if ((!fileIsModel(g_CmdPrams.SourceFile)) && (!fileIsModel(g_CmdPrams.DestFile)))
    {
        // Check if print status line has been assigned
        // if not get it a default to printf
        if (PrintStatusLine == NULL)
            PrintStatusLine = &LocalPrintF;

        if (g_CmdPrams.analysis || g_CmdPrams.diffImage)
        {
            if (!(GenerateAnalysis(g_CmdPrams.SourceFile, g_CmdPrams.DestFile)))
            {
                PrintInfo("Error: Image Analysis Failed\n");
                return -1;
            }
            return 0;
        }

        if (g_CmdPrams.imageprops)
        {
            if (!(GenerateImageProps(g_CmdPrams.SourceFile)))
            {
                PrintInfo("Error: Failed to retrieve image properties\n");
                return -1;
            }
            return 0;
        }

        //QueryPerformanceFrequency(&frequency);
        frequency = timeStampsec();

        // ==========================
        // Mip Settings Class
        // ==========================
        g_CMIPS = (CMIPS*)new (MyCMIPS);

        // ---------
        // Input
        // ---------
        CMP_FORMAT srcFormat;
        memset(&g_MipSetIn, 0, sizeof(MipSet));

        //--------------------------------------------
        // Set user specification for ASTC Block sizes
        //---------------------------------------------
        g_MipSetIn.m_nBlockWidth  = g_CmdPrams.BlockWidth;
        g_MipSetIn.m_nBlockHeight = g_CmdPrams.BlockHeight;
        g_MipSetIn.m_nBlockDepth  = g_CmdPrams.BlockDepth;
        if (g_CmdPrams.use_noMipMaps)
        {
            g_MipSetIn.m_Flags = MS_FLAG_DisableMipMapping;
        }

        //===========================
        // Set the destination format
        //===========================
        CMP_FORMAT destFormat;
        destFormat = g_CmdPrams.DestFormat;

        // Determin if destinationfile is to be Compressed
        DestinationFormatIsCompressed = CompressedFormat(destFormat);

        //========================
        // Set the source format
        //========================
        //----------------
        // Read Input file
        //----------------
        if (p_userMipSetIn)  //for GUI
        {
            memcpy(&g_MipSetIn, p_userMipSetIn, sizeof(MipSet));
            // Data in DXTn Files are expected to be in BGRA as input to CMP_ConvertTexture
            // Data in ASTC BC6 BC7 etc - expect data to be RGBA as input to CMP_ConvertTexture
            g_MipSetIn.m_swizzle        = KeepSwizzle(destFormat);
            g_MipSetIn.m_pMipLevelTable = p_userMipSetIn->m_pMipLevelTable;

            Delete_gMipSetIn = false;
        }
        else  //for CLI
        {
            Delete_gMipSetIn = true;

            // ===============================================
            // INPUT IMAGE Swizzling options for DXT formats
            // ===============================================
#ifdef USE_SWIZZLE
            if (!g_CmdPrams.CompressOptions.bUseGPUCompress)
                g_MipSetIn.m_swizzle = KeepSwizzle(destFormat);
#endif

            // Flag to image loader that data is going to be compressed
            // and that we want data to be in ARGB format
            // g_MipSetIn.m_swizzle = (bool)DestinationFormatIsCompressed;
            //
            //-----------------------
            // User swizzle overrides
            //-----------------------
            // Did User set no swizzle
            if (g_CmdPrams.noswizzle)
                g_MipSetIn.m_swizzle = false;

            // Did User set do swizzle
            if (g_CmdPrams.doswizzle)
                g_MipSetIn.m_swizzle = true;

            //---------------------------------------
            // Set user specification for Block sizes
            //----------------------------------------
            if (AMDLoadMIPSTextureImage(g_CmdPrams.SourceFile.c_str(), &g_MipSetIn, g_CmdPrams.use_OCV, &g_pluginManager) != 0)
            {
                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                PrintInfo("Error: loading image, data type not supported.\n");
                return -1;
            }
        }

        if (g_CmdPrams.showperformance)
            conversion_loopStartTime = timeStampsec();
        //QueryPerformanceCounter(&conversion_loopStartTime);

        // User setting overrides file setting in this case
        if (g_CmdPrams.SourceFormat != CMP_FORMAT_Unknown)
            srcFormat = g_CmdPrams.SourceFormat;
        else
            srcFormat = g_MipSetIn.m_format;

        // Load MIPS did not return a format try to set one
        if (srcFormat == CMP_FORMAT_Unknown)
        {
            g_MipSetIn.m_format = GetFormat(&g_MipSetIn);
            if (g_MipSetIn.m_format == CMP_FORMAT_Unknown)
            {
                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                PrintInfo("Error: unsupported input image file format\n");
                return -1;
            }

            srcFormat = g_MipSetIn.m_format;
        }

        // Determin if Source file Is Compressed
        SourceFormatIsCompressed = CompressedFormat(srcFormat);

#ifndef ENABLE_MAKE_COMPATIBLE_API
        if ((FloatFormat(srcFormat) && !FloatFormat(destFormat)) || (!FloatFormat(srcFormat) && FloatFormat(destFormat)))
        {
            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
            PrintInfo("Error: Processing floating point format <-> non-floating point format is not supported\n");
            return -1;
        }
#endif
        //=====================================================
        // Unsupported conversion !
        // ====================================================
        if (SourceFormatIsCompressed && DestinationFormatIsCompressed)
        {
            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
            PrintInfo("Error: Processing compressed source and compressed destination is not supported\n");
            return -1;
        }

        //=====================================================
        // Perform swizzle
        // ===================================================
        if (p_userMipSetIn)  //for GUI
        {
#ifdef USE_SWIZZLE
            if (g_MipSetIn.m_swizzle && !g_CmdPrams.CompressOptions.bUseGPUCompress)
            {
                SwizzleMipMap(&g_MipSetIn);
                SwizzledMipSetIn = true;
            }
#endif
        }
        else
        {
            if (g_MipSetIn.m_swizzle)
                SwizzleMipMap(&g_MipSetIn);
        }

        //======================================================
        // Determine if MIP mapping is required
        // if so generate the MIP levels for the source file
        //=======================================================
        if (((g_CmdPrams.MipsLevel > 1) && (g_MipSetIn.m_nMipLevels == 1)) && (!g_CmdPrams.use_noMipMaps))
        {
            PluginInterface_Filters* plugin_Filter;
            plugin_Filter = reinterpret_cast<PluginInterface_Filters*>(g_pluginManager.GetPlugin("FILTERS", "BOXFILTER"));
            if (plugin_Filter)
            {
                int nMinSize;

                // User has two option to specify MIP levels
                if (g_CmdPrams.nMinSize > 0)
                    nMinSize = g_CmdPrams.nMinSize;
                else
                    nMinSize = CalcMinMipSize(g_MipSetIn.m_nHeight, g_MipSetIn.m_nWidth, g_CmdPrams.MipsLevel);

                plugin_Filter->TC_GenerateMIPLevels(&g_MipSetIn, nMinSize);
                delete plugin_Filter;
            }
            else
            {
                PrintInfo("Error Loading: BoxFilter plugin for MIP Level Generation\n");
                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                return -1;
            }
        }

        // --------------------------------
        // Setup Compressed Mip Set
        // --------------------------------
        CMP_FORMAT cmpformat = CMP_FORMAT_Unknown;
        memset(&g_MipSetCmp, 0, sizeof(MipSet));
        g_MipSetCmp.m_Flags = MS_FLAG_Default;

        // -------------
        // Output
        // -------------
        memset(&g_MipSetOut, 0, sizeof(MipSet));

        //----------------------------------
        // Now set data sets pointers for processing
        //----------------------------------
        p_MipSetIn  = &g_MipSetIn;
        p_MipSetOut = &g_MipSetOut;

        //=====================================================
        // Case Uncompressed Source to Compressed Destination
        //
        // Example: BMP -> DDS  with -fd Compression flag
        //
        //=====================================================
        if ((!SourceFormatIsCompressed) && (DestinationFormatIsCompressed))
        {
            // Allocate compression
            g_MipSetCmp.m_ChannelFormat = CF_Compressed;
            g_MipSetCmp.m_nMaxMipLevels = g_MipSetIn.m_nMaxMipLevels;
            g_MipSetCmp.m_nMipLevels    = 1;  // this is overwriiten depending on input.
            if (!g_CMIPS->AllocateMipSet(&g_MipSetCmp, CF_8bit, TDT_ARGB, TT_2D, g_MipSetIn.m_nWidth, g_MipSetIn.m_nHeight, 1))
            {
                PrintInfo("Memory Error(1): allocating MIPSet Compression buffer\n");
                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                return -1;
            }

            g_MipSetCmp.m_nHeight = g_MipSetIn.m_nHeight;
            g_MipSetCmp.m_nWidth  = g_MipSetIn.m_nWidth;
            g_MipSetCmp.m_format  = destFormat;
            Format2FourCC(destFormat, &g_MipSetCmp);

            CMP_Texture srcTexture;
            srcTexture.dwSize = sizeof(srcTexture);
            int DestMipLevel  = g_MipSetIn.m_nMipLevels;

            if (g_CmdPrams.showperformance)
                compress_loopStartTime = timeStampsec();

            g_MipSetCmp.m_nMipLevels = DestMipLevel;

            for (int nMipLevel = 0; nMipLevel < DestMipLevel; nMipLevel++)
            {
                g_MipLevel = nMipLevel + 1;

                for (int nFaceOrSlice = 0; nFaceOrSlice < MaxFacesOrSlices(&g_MipSetIn, nMipLevel); nFaceOrSlice++)
                {
                    //=====================
                    // Uncompressed source
                    //======================
                    MipLevel* pInMipLevel   = g_CMIPS->GetMipLevel(&g_MipSetIn, nMipLevel, nFaceOrSlice);
                    srcTexture.dwWidth      = pInMipLevel->m_nWidth;
                    srcTexture.dwHeight     = pInMipLevel->m_nHeight;
                    srcTexture.dwPitch      = 0;
                    srcTexture.nBlockWidth  = g_MipSetIn.m_nBlockWidth;
                    srcTexture.nBlockHeight = g_MipSetIn.m_nBlockHeight;
                    srcTexture.nBlockDepth  = g_MipSetIn.m_nBlockDepth;
                    srcTexture.format       = srcFormat;
                    srcTexture.pData        = NULL;

                    //========================
                    // Compressed Destination
                    //========================
                    CMP_Texture destTexture;
                    destTexture.dwSize       = sizeof(destTexture);
                    destTexture.dwWidth      = pInMipLevel->m_nWidth;
                    destTexture.dwHeight     = pInMipLevel->m_nHeight;
                    destTexture.dwPitch      = 0;
                    destTexture.nBlockWidth  = g_CmdPrams.BlockWidth;
                    destTexture.nBlockHeight = g_CmdPrams.BlockHeight;
                    destTexture.format       = destFormat;
                    destTexture.dwDataSize   = CMP_CalculateBufferSize(&destTexture);

                    srcTexture.pData      = pInMipLevel->m_pbData;
                    srcTexture.dwDataSize = CMP_CalculateBufferSize(&srcTexture);

                    PrintInfo("Source Texture size = %d Bytes, width = %d px  height = %d px\n", srcTexture.dwDataSize, srcTexture.dwWidth,
                              srcTexture.dwHeight);

                    if (destTexture.dwDataSize > 0)
                        PrintInfo("Destination Texture size = %d Bytes   Resulting compression ratio = %2.2f:1\n", destTexture.dwDataSize,
                                  srcTexture.dwDataSize / (float)destTexture.dwDataSize);

                    MipLevel* pOutMipLevel = g_CMIPS->GetMipLevel(&g_MipSetCmp, nMipLevel, nFaceOrSlice);
                    if (!g_CMIPS->AllocateCompressedMipLevelData(pOutMipLevel, destTexture.dwWidth, destTexture.dwHeight, destTexture.dwDataSize))
                    {
                        PrintInfo("Memory Error(1): allocating MIPSet compression level data buffer\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);

                        return -1;
                    }

                    destTexture.pData = pOutMipLevel->m_pbData;
                    g_fProgress       = -1;

                    //========================
                    // Process ConvertTexture
                    //========================
#ifdef USE_COMPUTE
                    if (g_CmdPrams.CompressOptions.bUseGPUCompress)
                    {
                        bool          format_support_gpu = true;
                        KernalOptions kernel_options;
                        memset(&kernel_options, 0, sizeof(KernalOptions));
                        PluginInterface_Compute* plugin_compute = NULL;

                        kernel_options.data_type    = (unsigned int)(g_CmdPrams.DestFormat);
                        kernel_options.Compute_type = (unsigned int)(g_CmdPrams.CompressOptions.nComputeWith);

                        plugin_compute =
                            reinterpret_cast<PluginInterface_Compute*>(g_pluginManager.GetPlugin("COMPUTE", GetFormatDesc(g_CmdPrams.DestFormat)));

                        // Plugin was not found!
                        if (plugin_compute == NULL)
                        {
                            PrintInfo("Failed to load %s compute plugin\n", GetFormatDesc(g_CmdPrams.DestFormat));
                            PrintInfo("CPU will be used for compression.\n");
                            format_support_gpu = false;
                        }
                        else
                        {
                            do
                            {
                                CMP_Compute_Base::ComputeOptions options;
                                options.force_rebuild = true;

                                TC_PluginVersion PluginVersion;
                                plugin_compute->TC_PluginGetVersion(&PluginVersion);
                                printf("Using %s Compute plugin V%d.%d\n", GetFormatDesc(g_CmdPrams.DestFormat), PluginVersion.dwPluginVersionMajor,
                                       PluginVersion.dwPluginVersionMinor);

                                if (plugin_compute->TC_Init(&srcTexture, &kernel_options) != 0)
                                {
                                    PrintInfo("Failed to init compute plugin\nCPU will be used for compression.\n");
                                    format_support_gpu = false;
                                    break;
                                }

                                if (CMP_InitializeCompressLibrary(g_CmdPrams.CompressOptions.nComputeWith, &srcTexture, &kernel_options) != CMP_OK)
                                {
                                    PrintInfo("Failed to init compute plugin\nCPU will be used for compression.\n");
                                    format_support_gpu = false;
                                    break;
                                }

                                if (CMP_SetComputeOptions(&options) != CMP_OK)
                                {
                                    PrintInfo("Failed to set compute options\n");
                                    return -1;
                                }

                                char opencl_source[512];
                                sprintf_s(opencl_source, 512, "%s", plugin_compute->TC_ComputeSourceFile());
                                if (CMP_CompressTexture(opencl_source, &kernel_options, srcTexture, destTexture) != CMP_OK)
                                {
                                    PrintInfo("Failed to run compute plugin\nCPU will be used for compression.\n");
                                    format_support_gpu = false;
                                    break;
                                }

                                CMP_ShutdownComputeLibrary();
                            } while (0);
                        }

                        if (!format_support_gpu)
                        {
                            g_MipSetIn.m_swizzle = KeepSwizzle(destFormat);
                            if (g_MipSetIn.m_swizzle)
                                SwizzleMipMap(&g_MipSetIn);
                            if (CMP_ConvertTexture(&srcTexture, &destTexture, &g_CmdPrams.CompressOptions, pFeedbackProc, NULL, NULL) != CMP_OK)
                            {
                                PrintInfo("Error in compressing destination texture\n");
                                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                return -1;
                            }
                        }
                    }
                    else
#endif
                    {
                        if (CMP_ConvertTexture(&srcTexture, &destTexture, &g_CmdPrams.CompressOptions, pFeedbackProc, NULL, NULL) != CMP_OK)
                        {
                            PrintInfo("Error in compressing destination texture\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                            return -1;
                        }
                    }

                    if (g_CmdPrams.showperformance)
                        compress_nIterations++;
                }
            }

            if (g_CmdPrams.showperformance)
                compress_loopEndTime = timeStampsec();

            srcFormat = destFormat;

            // for printing info about what compression was used
            cmpformat = destFormat;

            if (pFeedbackProc)
                pFeedbackProc(100, NULL, NULL);
        }

        //==============================================
        // Save to file destination buffer if
        // Uncomprssed file to Compressed File format
        //==============================================

        if ((!SourceFormatIsCompressed) && (DestinationFormatIsCompressed) &&
            (IsDestinationUnCompressed((const char*)g_CmdPrams.DestFile.c_str()) == false)

        )
        {
            //-------------------------------------------------------------
            // Set user specification for ASTC Block sizes that was used!
            //-------------------------------------------------------------
            g_MipSetCmp.m_nBlockWidth  = g_CmdPrams.BlockWidth;
            g_MipSetCmp.m_nBlockHeight = g_CmdPrams.BlockHeight;
            g_MipSetCmp.m_nBlockDepth  = g_CmdPrams.BlockDepth;

            if (AMDSaveMIPSTextureImage(g_CmdPrams.DestFile.c_str(), &g_MipSetCmp, g_CmdPrams.use_OCV_out) != 0)
            {
                PrintInfo("Error: saving image failed, write permission denied or format is unsupported.\n");
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
        // Case Compressed Source to UnCompressed Destination
        // Transcoding file formats
        //
        // Case example: BMP -> BMP with -fd uncompression flag
        //
        //=====================================================
        if ((!SourceFormatIsCompressed) && (!DestinationFormatIsCompressed))
        {
            TranscodeBits = true;
        }

        //=====================================================
        // Case uncompressed Source to UnCompressed Destination
        // with mid way compression
        //
        // Case example: BMP -> BMP with -fd compression flag
        //
        //=====================================================
        if ((!SourceFormatIsCompressed) && (DestinationFormatIsCompressed) &&
            (IsDestinationUnCompressed((const char*)g_CmdPrams.DestFile.c_str()) == true))
        {
            MidwayDecompress = true;
            // Prepare for an uncompress request on destination
            p_MipSetIn = &g_MipSetCmp;
        }

        //=====================================================
        // Case Compressed Source to UnCompressed Destination
        // Example(s):
        //                DDS - BMP  with no -fd flag
        //                BMP - BMP  with no -fd flag(s)
        //                BMP - BMP  with    -fd flag(s)
        //
        //=====================================================
        if (((SourceFormatIsCompressed) && (!DestinationFormatIsCompressed)) || (TranscodeBits) || (MidwayDecompress))
        {
            g_MipSetOut.m_TextureDataType = TDT_ARGB;

            if (SourceFormatIsCompressed)
            {
                // BMP is saved as CMP_FORMAT_ARGB_8888
                // EXR is saved as CMP_FORMAT_ARGB_32F
                switch (srcFormat)
                {
                case CMP_FORMAT_BC6H:
                case CMP_FORMAT_BC6H_SF:
                    destFormat                  = CMP_FORMAT_ARGB_16F;
                    g_MipSetOut.m_ChannelFormat = CF_Float16;
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
                    // Need to determin a target format.
                    // Based on file extension.
                    switch (srcFormat)
                    {
                    case CMP_FORMAT_BC6H:
                    case CMP_FORMAT_BC6H_SF:
                        destFormat                  = CMP_FORMAT_ARGB_16F;
                        g_MipSetOut.m_ChannelFormat = CF_Float16;
                        break;
                    default:
                        destFormat = FormatByFileExtension((const char*)g_CmdPrams.DestFile.c_str(), &g_MipSetOut);
                        break;
                    }
                }
            }

            // Something went wrone in determining destination format
            // Just default it!
            if (destFormat == CMP_FORMAT_Unknown)
                destFormat = CMP_FORMAT_ARGB_8888;

            if (destFormat == CMP_FORMAT_ARGB_32F)
                g_MipSetOut.m_ChannelFormat = CF_Float32;
            else if (destFormat == CMP_FORMAT_ARGB_16F)
                g_MipSetOut.m_ChannelFormat = CF_Float16;

            g_MipSetOut.m_format         = destFormat;
            g_MipSetOut.m_isDeCompressed = srcFormat != CMP_FORMAT_Unknown ? srcFormat : CMP_FORMAT_MAX;

            // Allocate output MipSet
            if (!g_CMIPS->AllocateMipSet(&g_MipSetOut, g_MipSetOut.m_ChannelFormat, g_MipSetOut.m_TextureDataType, p_MipSetIn->m_TextureType,
                                         p_MipSetIn->m_nWidth, p_MipSetIn->m_nHeight, p_MipSetIn->m_nDepth))
            {
                PrintInfo("Memory Error(2): allocating MIPSet Output buffer\n");
                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                return -1;
            }

            g_MipSetOut.m_CubeFaceMask = p_MipSetIn->m_CubeFaceMask;

            MipLevel* pCmpMipLevel    = g_CMIPS->GetMipLevel(p_MipSetIn, 0);
            int       nMaxFaceOrSlice = MaxFacesOrSlices(p_MipSetIn, 0);
            int       nWidth          = pCmpMipLevel->m_nWidth;
            int       nHeight         = pCmpMipLevel->m_nHeight;
            //
            CMP_BYTE* pMipData = g_CMIPS->GetMipLevel(p_MipSetIn, 0, 0)->m_pbData;

            bool          use_GPUDecode = g_CmdPrams.CompressOptions.bUseGPUDecompress;
            CMP_GPUDecode DecodeWith    = g_CmdPrams.CompressOptions.nGPUDecode;

            if (g_CmdPrams.showperformance)
                decompress_loopStartTime = timeStampsec();
            //QueryPerformanceCounter(&decompress_loopStartTime);

            for (int nFaceOrSlice = 0; nFaceOrSlice < nMaxFaceOrSlice; nFaceOrSlice++)
            {
                int nMipWidth  = nWidth;
                int nMipHeight = nHeight;

                for (int nMipLevel = 0; nMipLevel < p_MipSetIn->m_nMipLevels; nMipLevel++)
                {
                    MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(p_MipSetIn, nMipLevel, nFaceOrSlice);
                    if (!pInMipLevel)
                    {
                        PrintInfo("Memory Error(2): allocating MIPSet Output Cmp level buffer\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }

                    // Valid Mip Level ?
                    if (pInMipLevel->m_pbData)
                        pMipData = pInMipLevel->m_pbData;

                    if (!g_CMIPS->AllocateMipLevelData(g_CMIPS->GetMipLevel(&g_MipSetOut, nMipLevel, nFaceOrSlice), nMipWidth, nMipHeight,
                                                       g_MipSetOut.m_ChannelFormat, g_MipSetOut.m_TextureDataType))
                    {
                        PrintInfo("Memory Error(2): allocating MIPSet Output level buffer\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }

                    //----------------------------
                    // Compressed source
                    //-----------------------------
                    CMP_Texture srcTexture;
                    srcTexture.dwSize       = sizeof(srcTexture);
                    srcTexture.dwWidth      = nMipWidth;
                    srcTexture.dwHeight     = nMipHeight;
                    srcTexture.dwPitch      = 0;
                    srcTexture.nBlockHeight = p_MipSetIn->m_nBlockHeight;
                    srcTexture.nBlockWidth  = p_MipSetIn->m_nBlockWidth;
                    srcTexture.nBlockDepth  = p_MipSetIn->m_nBlockDepth;
                    srcTexture.format       = srcFormat;
                    srcTexture.dwDataSize   = CMP_CalculateBufferSize(&srcTexture);
                    srcTexture.pData        = pMipData;

                    //-----------------------------
                    // Uncompressed Destination
                    //-----------------------------
                    CMP_Texture destTexture;
                    destTexture.dwSize     = sizeof(destTexture);
                    destTexture.dwWidth    = nMipWidth;
                    destTexture.dwHeight   = nMipHeight;
                    destTexture.dwPitch    = 0;
                    destTexture.format     = destFormat;
                    destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
                    destTexture.pData      = g_CMIPS->GetMipLevel(&g_MipSetOut, nMipLevel, nFaceOrSlice)->m_pbData;
                    if (!g_CmdPrams.silent)
                    {
                        if ((nMipLevel > 1) || (nFaceOrSlice > 1))
                            PrintInfo("\rProcessing destination     MipLevel %2d FaceOrSlice %2d", nMipLevel + 1, nFaceOrSlice);
                        else
                            PrintInfo("\rProcessing destination    ");
                    }
#ifdef _WIN32
                    if (/*(srcTexture.dwDataSize > destTexture.dwDataSize) || */ (IsBadWritePtr(destTexture.pData, destTexture.dwDataSize)))
                    {
                        PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }
#else
                    int nullfd = open("/dev/random", O_WRONLY);
                    if (write(nullfd, destTexture.pData, destTexture.dwDataSize) < 0)
                    {
                        PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
                    }
                    close(nullfd);
#endif
                    g_fProgress = -1;

                    if (use_GPUDecode)
                    {
#ifdef _WIN32
#ifndef DISABLE_TESTCODE
                        if (srcTexture.format == CMP_FORMAT_ASTC)
                        {
                            PrintInfo(
                                "Decompress Error: ASTC decompressed with GPU is not supported yet. Please view ASTC compressed images using CPU.\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                            return -1;
                        }

                        if (CMP_DecompressTexture(&srcTexture, &destTexture, DecodeWith) != CMP_OK)
                        {
                            PrintInfo("Error in decompressing source texture\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                            return -1;
                        }
#endif
#else
                        PrintInfo("GPU Decompress is not supported in linux yet.\n");
                        cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                        return -1;
#endif
                    }
                    else
                    {
                        if (CMP_ConvertTexture(&srcTexture, &destTexture, &g_CmdPrams.CompressOptions, pFeedbackProc, NULL, NULL) != CMP_OK)
                        {
                            PrintInfo("Error(2) in compressing destination texture\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                            return -1;
                        }
                    }

                    if (g_CmdPrams.showperformance)
                        decompress_nIterations++;

                    pMipData += srcTexture.dwDataSize;
                    nMipWidth  = (nMipWidth > 1) ? (nMipWidth >> 1) : 1;
                    nMipHeight = (nMipHeight > 1) ? (nMipHeight >> 1) : 1;
                }
            }

            if (g_CmdPrams.showperformance)
                decompress_loopEndTime = timeStampsec();
            //QueryPerformanceCounter(&decompress_loopEndTime);

            g_MipSetOut.m_nMipLevels = p_MipSetIn->m_nMipLevels;

            p_MipSetOut = &g_MipSetOut;

            // ===============================================
            // INPUT IMAGE Swizzling options for DXT formats
            // ===============================================
            if (!use_GPUDecode)
                g_MipSetOut.m_swizzle = KeepSwizzle(srcFormat);

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
            // Set user specification for ASTC Block sizes that was used!
            //-------------------------------------------------------------
            p_MipSetOut->m_nBlockWidth  = g_CmdPrams.BlockWidth;
            p_MipSetOut->m_nBlockHeight = g_CmdPrams.BlockHeight;
            p_MipSetOut->m_nBlockDepth  = g_CmdPrams.BlockDepth;

            if (AMDSaveMIPSTextureImage(g_CmdPrams.DestFile.c_str(), p_MipSetOut, g_CmdPrams.use_OCV_out) != 0)
            {
                PrintInfo("Error: saving image failed, write permission denied or format is unsupported.\n");
                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                return -1;
            }
        }

        if (g_CmdPrams.showperformance)
            conversion_loopEndTime = timeStampsec();
        //QueryPerformanceCounter(&conversion_loopEndTime);

        if ((!g_CmdPrams.silent) && (g_CmdPrams.showperformance))
        {
            double compress_fDuration = compress_loopEndTime - compress_loopStartTime;

            double decompress_fDuration = decompress_loopEndTime - decompress_loopStartTime;

            g_CmdPrams.conversion_fDuration = conversion_loopEndTime - conversion_loopStartTime;

#ifdef USE_WITH_COMMANDLINE_TOOL
            PrintInfo("\r");
#endif

            if (compress_nIterations)
                PrintInfo("Compressed to %s with %i iteration(s) in %.3f seconds\n", GetFormatDesc(cmpformat), compress_nIterations,
                          compress_fDuration);

            if (decompress_nIterations)
                PrintInfo("DeCompressed to %s with %i iteration(s) in %.3f seconds\n", GetFormatDesc(destFormat), decompress_nIterations,
                          decompress_fDuration);

            PrintInfo("Total time taken (includes file I/O): %.3f seconds\n", g_CmdPrams.conversion_fDuration);
        }
    }
    else
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
                    PrintInfo("Error: Mesh Optimization Failed.\n");
                    return -1;
                }
            }
            else
            {
                PrintInfo("Error: Mesh Optimization Failed. Only glTF->glTF, obj->obj are supported in mesh optimization.\n");
                return -1;
            }
        }
#endif

        // Mesh Compression and Decompression
        if (!(g_CmdPrams.doMeshOptimize &&
              !g_CmdPrams
                   .use_Draco_Encode))  // skip mesh decompression for case only meshopt turn on: CompressonatorCLI.exe -meshopt source.gltf/obj dest.gltf/obj
        {
            if ((fileIsGLTF(g_CmdPrams.SourceFile) && fileIsGLTF(g_CmdPrams.DestFile)) ||
                (fileIsOBJ(g_CmdPrams.SourceFile) && fileIsDRC(g_CmdPrams.DestFile)) ||
                (fileIsDRC(g_CmdPrams.SourceFile) && fileIsOBJ(g_CmdPrams.DestFile)) ||
                (fileIsOBJ(g_CmdPrams.SourceFile) && fileIsOBJ(g_CmdPrams.DestFile)))
            {
                if (!(CompressDecompressMesh(g_CmdPrams.SourceFile, g_CmdPrams.DestFile)))
                {
                    PrintInfo("Error: Mesh Compression Failed.\n");
                    return -1;
                }
            }
            else
            {
                PrintInfo(
                    "Error: Mesh Compression Failed. Only glTF->glTF, obj->drc(compression), drc->obj(decompression), obj->obj(optimize first then "
                    "compress to .drc file) are supported in mesh compression/decompression.\n");
                return -1;
            }
        }
#endif
    }

    if (!g_CmdPrams.silent)
    {
#ifdef USE_WITH_COMMANDLINE_TOOL
        PrintInfo("\rDone                                                  \n");
#else
        PrintInfo("Done\n");
#endif
    }

    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);

#ifdef SHOW_PROCESS_MEMORY
    bool result2 = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter2, sizeof(memCounter2));
#endif

    return 0;
}

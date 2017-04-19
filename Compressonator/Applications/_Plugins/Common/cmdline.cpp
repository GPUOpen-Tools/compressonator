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
#include <imfrgba.h>

// #define SHOW_PROCESS_MEMORY
// #define USE_COMPUTE
#define USE_SWIZZLE


#ifdef SHOW_PROCESS_MEMORY
#include "windows.h"
#include "psapi.h"
#endif

CCmdLineParamaters g_CmdPrams;

#ifdef USE_WITH_COMMANDLINE_TOOL

void AboutCompressonator()
{
    printf( "------------------------------------------------\n");
    // current build release
    if ((VERSION_MINOR_MAJOR > 0))
        printf("CompressonatorCLI V%d.%d.%d Copyright AMD 2016\n", VERSION_MAJOR_MAJOR, VERSION_MAJOR_MINOR, VERSION_MINOR_MAJOR);
    else
    {
        // Keep track of Customer patches from last release to current
        // This is what is shown when you build the exe outside of the automated Build System (such as Jenkins)
        printf("CompressonatorCLI V2.5.0  SP1 Copyright AMD 2016 - 2017\n");
    }
    printf( "------------------------------------------------\n");
    printf( "\n");
}

void PrintUsage()
{
    AboutCompressonator();
    
    printf("Usage: CompressonatorCLI.exe [options] SourceFile DestFile\n\n");
    printf("MipMap options:\n\n");
    printf("-nomipmap                 Turns off Mipmap generation\n");
    printf("-mipsize    <size>        The size in pixels used to determine\n");
    printf("                          how many mip levels to generate\n");
    printf("-miplevels  <Level>       Sets Mips Level for output,\n");
    printf("                          (mipSize overides this option): default is 1\n");
    printf("Compression options:\n\n");
    printf("-fs <format>    Optionally specifies the source texture format to use\n");
    printf("-fd <format>    Specifies the destination texture format to use\n");
    printf("-decomp <filename>   If the destination  file is compressed optionally\n");
    printf("                     decompress it\n");
    printf("                     to the specified file. Note the destination  must\n");
    printf("                     be compatible \n");
    printf("                     with the sources format,decompress formats are typically\n");
    printf("                     set to ARGB_8888 or ARGB_32F\n");
#ifdef USE_COMPUTE
    printf("-UseGPUCompress      By default compression is done using CPU\n");
    printf("                     when set, OpenCL will be used by default, this can be \n");
    printf("                     changed to DirectX using EcodeWith setting\n");
    printf("-EncodeWith          Sets OpenCL or DirectX for GPU compress\n");
    printf("                     Default is OpenCL, UseGPUCompress is implied when\n");
    printf("                     this option is set\n");
#endif
    printf("-UseGPUDecompress    By default decompression is done using CPU\n");
    printf("                     when set OpenGL will be used by default, this can be \n");
    printf("                     changed to DirectX or Vulkan using DecodeWith setting\n");
    printf("-DecodeWith          Sets OpenGL, DirectX or Vulkan for GPU decompress\n");
    printf("                     Default is OpenGL, UseGPUDecompress is implied when\n");
    printf("                     this option is set\n");
    printf("-doswizzle           Swizzle the source images Red and Blue channels\n");
    printf("\n");
    printf("The following is a list of channel formats\n");
    printf("ARGB_16        ARGB format with 16-bit fixed channels\n");
    printf("ARGB_16F       ARGB format with 16-bit floating-point channels\n");
    printf("ARGB_32F       ARGB format with 32-bit floating-point channels\n");
    printf("ARGB_2101010   ARGB format with 10-bit fixed channels for color\n");
    printf("               and a 2-bit fixed channel for alpha\n");
    printf("ARGB_8888      ARGB format with 8-bit fixed channels\n");
    printf("R_8            Single component format with 8-bit fixed channels\n");
    printf("R_16           Single component format with 16-bit fixed channels\n");
    printf("R_16F          Two component format with 32-bit floating-point channels\n");
    printf("R_32F          Single component with 32-bit floating-point channels\n");
    printf("RG_8           Two component format with 8-bit fixed channels\n");
    printf("RG_16          Two component format with 16-bit fixed channels\n");
    printf("RG_16F         Two component format with 16-bit floating-point channels\n");
    printf("RG_32F         Two component format with 32-bit floating-point channels\n");
    printf("RGB_888        RGB format with 8-bit fixed channels\n");
    printf("\n");
    printf("The following is a list of compression formats\n");
    printf("ASTC           Adaptive Scalable Texture Compression\n");
    printf("ATC_RGB                 Compressed RGB format\n");
    printf("ATC_RGBA_Explicit       ARGB format with explicit alpha\n");
    printf("ATC_RGBA_Interpolated   ARGB format with interpolated alpha\n");
    printf("ATI1N          Single component compression format using the same\n");
    printf("               technique as DXT5 alpha. Four bits per pixel\n");
    printf("ATI2N          Two component compression format using the same\n");
    printf("               technique as DXT5 alpha. Designed for compression object\n");
    printf("               space normal maps. Eight bits per pixel\n");
    printf("ATI2N_XY       Two component compression format using the same technique\n");
    printf("               as DXT5 alpha. The same as ATI2N but with the channels swizzled.\n"); 
    printf("               Eight bits per pixel\n");
    printf("ATI2N_DXT5     An ATI2N like format using DXT5. Intended for use on GPUs that\n");
    printf("               do not natively support ATI2N. Eight bits per pixel\n");
    printf("BC1            Four component opaque (or 1-bit alpha) compressed texture\n");
    printf("               format. Four bit per pixel\n");
    printf("BC2            Four component compressed texture format with explicit\n");
    printf("               alpha.  Eight bits per pixel\n");
    printf("BC3            Four component compressed texture format with interpolated\n");
    printf("               alpha.  Eight bits per pixel\n");
    printf("BC4            Single component compressed texture format for Microsoft\n");
    printf("BC5            Two component compressed texture format for Microsoft\n");
    printf("BC6H           High-Dynamic Range  compression format\n");
    printf("BC7            High-quality compression of RGB and RGBA data\n\n");
    printf("DXT1           An opaque (or 1-bit alpha) DXTC compressed texture format.\n");
    printf("               Four bits per pixel\n");
    printf("DXT3           DXTC compressed texture format with explicit alpha.\n");
    printf("               Eight bits per pixel\n");
    printf("DXT5           DXTC compressed texture format with interpolated alpha.\n");
    printf("               Eight bits per pixel\n");
    printf("DXT5_xGBR      DXT5 with the red component swizzled into the alpha channel.\n");
    printf("               Eight bits per pixel\n");
    printf("DXT5_RxBG      Swizzled DXT5 format with the green component swizzled\n");
    printf("               into the alpha channel. Eight bits per pixel\n");
    printf("DXT5_RBxG      Swizzled DXT5 format with the green component swizzled\n");
    printf("               into the alpha channel & the blue component swizzled into\n");
    printf("               the green channel. Eight bits per pixel\n");
    printf("DXT5_xRBG      Swizzled DXT5 format with the green component swizzled\n");
    printf("               into the alpha channel & the red component swizzled into the\n");
    printf("               green channel. Eight bits per pixel\n");
    printf("DXT5_RGxB      Swizzled DXT5 format with the blue component swizzled\n");
    printf("               into the alpha channel. Eight bits per pixel\n");
    printf("DXT5_xGxR      Two-component swizzled DXT5 format with the red component\n");
    printf("               swizzled into the alpha channel & the green component in the\n");
    printf("               green channel. Eight bits per pixel\n");
    printf("ETC_RGB        Ericsson Texture Compression - Compressed RGB format.\n");
    printf("ETC2_RGB       Ericsson Texture Compression - Compressed RGB format.\n");
    printf("\n");
    printf("<codec options>: Reference  documentation for range of values\n\n");
    printf("-UseChannelWeighting <value> Use channel weightings\n");
    printf("-WeightR <value>             The weighting of the Red or X Channel\n");
    printf("-WeightG <value>             The weighting of the Green or Y Channel\n");
    printf("-WeightB <value>             The weighting of the Blue or Z Channel\n");
    printf("-AlphaThreshold <value>      The alpha threshold to use when compressing\n");
    printf("                             to DXT1 & BC1 with DXT1UseAlpha\n");
    printf("                             Texels with an alpha value less than the threshold\n");
    printf("                             are treated as transparent\n");
    printf("                             value is in the range of 0 to 255, default is 128\n");
    printf("-BlockRate <value>           ASTC 2D only - sets block size or bit rate\n");
    printf("                             value can be a bit per pixel rate from 0.0 to 9.9\n");
    printf("                             or can be a combination of x and y axes with paired\n");
    printf("                             values of 4,5,6,8,10 or 12 from 4x4 to 12x12\n");
    printf("-DXT1UseAlpha <value>        Encode single-bit alpha data.\n");
    printf("                             Only valid when compressing to DXT1 & BC1\n");
    printf("-Exposure <value>            BC6 only: Sets the resulting exposure of compressed Images\n");
    printf("                             default is 0.95, lower values produce darker images,\n");
    printf("                             higher values produce brighter images\n");
    printf("-CompressionSpeed <value>    The trade-off between compression speed & quality\n");
    printf("                             This setting is not used in BC6H and BC7\n");
    printf("-Signed <value>              Used for BC6H only, Default BC6H format disables\n");
    printf("                             use of a sign bit in the 16-bit floating point\n");
    printf("                             channels, with a value set to 1 BC6H format will\n");
    printf("                             use a sign bit\n");
    printf("-NumThreads <value>          Number of threads to initialize for BC6H or BC7\n");
    printf("                             encoding (Max up to 128). Default set to 8\n");
    printf("-Quality <value>             Sets quality of encoding for BC7\n");
    printf("-Performance <value>         Sets performance of encoding for BC7\n");
    printf("-ColourRestrict <value>      This setting is a quality tuning setting for BC7\n");
    printf("                             which may be necessary for convenience in some\n");
    printf("                             applications\n");
    printf("-AlphaRestrict <value>       This setting is a quality tuning setting for BC7\n");
    printf("                             which may be necessary for some textures\n");
    printf("-ModeMask <value>            Mode to set BC7 to encode blocks using any of 8\n");
    printf("                             different block modes in order to obtain the\n");
    printf("                             highest quality\n");
    printf("\n\n");
    printf("Output options:\n\n");
    printf("-silent                      Disable print messages\n");
    printf("-performance                 Shows various performance stats\n");
    printf("-noprogress                  Disables showing of compression progress messages\n");
    printf("\n\n");
    printf("Example compression:\n\n");
    printf("CompressonatorCLI.exe -fd ASTC image.bmp result.astc \n");
    printf("CompressonatorCLI.exe -fd ASTC -BlockRate 0.8 image.bmp result.astc\n");
    printf("CompressonatorCLI.exe -fd ASTC -BlockRate 12x12 image.bmp result.astc\n");
    printf("CompressonatorCLI.exe -fd BC7  image.bmp result.dds \n");
    printf("CompressonatorCLI.exe -fd BC7  -NumTheads 16 image.bmp result.dds\n");
    printf("CompressonatorCLI.exe -fd BC6H image.exr result.dds\n\n");
    printf("Example decompression from compressed image using CPU:\n\n");
    printf("CompressonatorCLI.exe  result.dds image.bmp\n\n");
    printf("Example decompression from compressed image using GPU:\n\n");
    printf("CompressonatorCLI.exe  -UseGPUDecompress result.dds image.bmp\n\n");
    printf("Example compression with decompressed result (Useful for qualitative analysis):\n\n");
    printf("CompressonatorCLI.exe -fd BC7  image.bmp result.bmp\n");
    printf("CompressonatorCLI.exe -fd BC6H image.exr result.exr\n");
}

int GetNumberOfCores(wchar_t *envp[ ])
{
    int i,cores=1;

    for (i = 0; envp[i] != NULL; i++)
    {
        //wprintf(L"var = %ws\n", envp[i]);
        cores = wcsncmp(envp[i],L"NUMBER_OF_PROCESSORS",20);
        if (cores == 0)
        {
            wchar_t *p_envp; 
            size_t equal = wcscspn (envp[i],L"=");
            if ((equal > 0) && (equal < wcslen(envp[i])))
            {
                p_envp = envp[i] + equal + 1;
                wchar_t num[16];
                wcsncpy_s(num,p_envp,16);
                cores = _wtoi(num);
                return cores > 0?cores:1;
            }
            break;
        }
    }
    return 1;
}

int RunInfo()
{
    PrintUsage();
    return 0;
}

#else // Code is shared with GUI 
#endif

CMP_GPUDecode DecodeWith(const  char *strParameter)
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

CMP_Compute_type EncodeWith(const  char *strParameter)
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

bool isFloat(CMP_FORMAT format)
{
    // determin of the swizzle flag needs to be turned on!
    switch (format)
    {
    case CMP_FORMAT_BC6H       :
    case CMP_FORMAT_BC6H_SF    :
    case CMP_FORMAT_ARGB_16F   :            
    case CMP_FORMAT_ABGR_16F   :        
    case CMP_FORMAT_RGBA_16F   :        
    case CMP_FORMAT_BGRA_16F   :        
    case CMP_FORMAT_RG_16F     :       
    case CMP_FORMAT_R_16F      :        
    case CMP_FORMAT_ARGB_32F   :        
    case CMP_FORMAT_ABGR_32F   :        
    case CMP_FORMAT_RGBA_32F   :   
    case CMP_FORMAT_RGBE_32F   :
    case CMP_FORMAT_BGRA_32F   :        
    case CMP_FORMAT_RGB_32F    :        
    case CMP_FORMAT_BGR_32F    :        
    case CMP_FORMAT_RG_32F     :        
    case CMP_FORMAT_R_32F      :
        return true;
        break;
    default:
        break;
    }
    return false;
}

bool ProcessSingleFlags(const  char *strCommand)
{
    bool isset = false;
    if ((strcmp(strCommand,"-nomipmap") == 0))
    {
        g_CmdPrams.use_noMipMaps = true;
        isset = true;
    }
    else
    if ((strcmp(strCommand,"-silent") == 0))
    {
        g_CmdPrams.silent = true;
        isset = true;
    }
    else
    if ((strcmp(strCommand,"-performance") == 0))
    {
        g_CmdPrams.showperformance = true;
        isset = true;
    }
    else
    if ((strcmp(strCommand,"-noprogress") == 0))
    {
        g_CmdPrams.noprogressinfo = true;
        isset = true;
    }
    else
    if ((strcmp(strCommand,"-noswizzle") == 0))
    {
        g_CmdPrams.noswizzle = true;
        isset = true;
    }
    else
    if ((strcmp(strCommand,"-doswizzle") == 0))
    {
        g_CmdPrams.doswizzle = true;
        isset = true;
    }
    else
    if ((strcmp(strCommand, "-analysis") == 0))
    {
        g_CmdPrams.analysis = true;
        isset = true;
    }
#ifdef USE_COMPUTE
    else
    if ((strcmp(strCommand, "-UseGPUCompress") == 0))
    {
        g_CmdPrams.CompressOptions.bUseGPUCompress = true;
        isset = true;
    }
#endif
    else
    if (strcmp(strCommand, "-UseGPUDecompress") == 0)
    {
        g_CmdPrams.CompressOptions.bUseGPUDecompress = true;
        isset = true;
    }
    return isset;
}


bool ProcessCMDLineOptions(const  char *strCommand, const char *strParameter)
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

            if (strchr(strParameter, '.') != NULL)
            {
                float m_target_bitrate = static_cast < float >(atof(strParameter));
                astc_find_closest_blockdim_2d(m_target_bitrate, &g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight, 0);
            }
            else
            {
                int dimensions = sscanf(strParameter, "%dx%dx", &g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight);
                if (dimensions < 2) throw "Command Parameter is invalid";
                else{
                    astc_find_closest_blockxy_2d(&g_CmdPrams.BlockWidth, &g_CmdPrams.BlockHeight, 0);
                }
            }
        }
        else
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
        else
        if (strcmp(strCommand, "-WeightR") == 0)
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
        else
        if (strcmp(strCommand, "-WeightG") == 0)
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
        else
        if (strcmp(strCommand, "-WeightB") == 0)
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
        else
        if (strcmp(strCommand, "-UseChannelWeighting") == 0)
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
        else
        if (strcmp(strCommand, "-AlphaThreshold") == 0)
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
        else
        if (strcmp(strCommand, "-DXT1UseAlpha") == 0)
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
            g_CmdPrams.CompressOptions.bDXT1UseAlpha = bool(value);
            g_CmdPrams.CompressOptions.nAlphaThreshold = 128;  //default to 128
        }
        else
        if (strcmp(strCommand, "-DecodeWith") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No API specified (set either OpenGL or DirectX (Default is OpenGL).";
            }

            g_CmdPrams.CompressOptions.nGPUDecode= DecodeWith((char *)strParameter);

            if (g_CmdPrams.CompressOptions.nGPUDecode == GPUDecode_INVALID)
            {
                throw "Unknown API format specified.";
            }

            g_CmdPrams.CompressOptions.bUseGPUDecompress = true;
        }
#ifdef USE_COMPUTE
        else
        if (strcmp(strCommand, "-EncodeWith") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "No API specified (set either OpenCL or DirectX (Default is OpenCL).";
            }

            g_CmdPrams.CompressOptions.nComputeWith = EncodeWith((char *)strParameter);

            if (g_CmdPrams.CompressOptions.nComputeWith == Compute_INVALID)
            {
                throw "Unknown API format specified.";
            }

            g_CmdPrams.CompressOptions.bUseGPUCompress = true;
        }
#endif
        else
        if (strcmp(strCommand, "-decomp") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no destination file specified";
            }
            g_CmdPrams.DecompressFile = (char *)strParameter;
            g_CmdPrams.doDecompress = true;
        }
        else
        if (strcmp(strCommand,"-fs") == 0 )
        {
            if (strlen(strParameter) == 0)
            {
                throw "no format specified";
            }

            g_CmdPrams.SourceFormat = ParseFormat((char *)strParameter);

            if (g_CmdPrams.SourceFormat == CMP_FORMAT_Unknown)
            {
                throw "unknown format specified";
            }
        }
        else
        if (strcmp(strCommand,"-fd") == 0 )
        {
            if (strlen(strParameter) == 0)
            {
                throw "no format specified";
            }

            g_CmdPrams.DestFormat = ParseFormat((char *)strParameter);

            if (g_CmdPrams.DestFormat == CMP_FORMAT_Unknown)
            {
                throw "unknown format specified";
            }
        }
        else
        if ((strcmp(strCommand,"-miplevels") == 0))
        {
            if (strlen(strParameter) == 0)
            {
                throw "no level is specified";
            }

            try {
                g_CmdPrams.MipsLevel = boost::lexical_cast<int>(strParameter);
            } catch (boost::bad_lexical_cast) {
                throw "conversion failed for miplevel value";
            }

        }
        else
        if ((strcmp(strCommand,"-mipsize") == 0))
        {
            if (strlen(strParameter) == 0)
            {
                throw "no size is specified";
            }

            try {
                g_CmdPrams.nMinSize = boost::lexical_cast<int>(strParameter);
            } catch (boost::bad_lexical_cast) {
                throw "conversion failed for mipsize value";
            }

            g_CmdPrams.MipsLevel = 2;
        }
        else
        if (strcmp(strCommand,"-r") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no read codec framework is specified";
            }
        
            if (strcmp(strParameter,"ocv") == 0) g_CmdPrams.use_OCV = true;
            else
            {
                throw "unsupported codec framework is specified";
            }
        }
        else
        if (strcmp(strCommand,"-w") == 0)
        {
            if (strlen(strParameter) == 0)
            {
                throw "no read codec framework is specified";
            }
        
            if (strcmp(strParameter,"ocv") == 0) g_CmdPrams.use_OCV_out = true;
            else
            {
                throw "unsupported codec framework is specified";
            }
        }
        else
        {
            if (strlen(strParameter) > 0)
            {
                const char *str;
                // strip leading - or --
                str = strCommand;
                if (strCommand[0] == '-') str++;
                if (strCommand[1] == '-') str++;
                
                if (strlen(str) > AMD_MAX_CMD_STR)
                {
                    throw "Command option is invalid";
                }

                if (strlen(strParameter) > AMD_MAX_CMD_PARAM)
                {
                    throw "Command Parameter is invalid";
                }

                strcpy_s(g_CmdPrams.CompressOptions.CmdSet[g_CmdPrams.CompressOptions.NumCmds].strCommand, str);
                strcpy_s(g_CmdPrams.CompressOptions.CmdSet[g_CmdPrams.CompressOptions.NumCmds].strParameter, strParameter);

                g_CmdPrams.CompressOptions.NumCmds++;

            }
            else
            {   // Flags or Source and destination files specified

                if (g_CmdPrams.SourceFile.length() == 0)
                {
                    g_CmdPrams.SourceFile = strCommand;
                }
                else 
                if (g_CmdPrams.DestFile.length()  == 0)
                {
                    g_CmdPrams.DestFile = strCommand;
                    string file_extension = boost::filesystem::extension(strCommand);
                    // User did not supply a destination extension default to KTX
                    if (file_extension.length() == 0)
                    {
                        g_CmdPrams.DestFile.append(".DDS");
                    }
                }
                else
                {
                    throw "unknown source or destination file";
                }
            }
        }
            
    } // Try code

    catch (char *str)
    {
        PrintInfo("Option [%s] : %s\n\n",strCommand,str);
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
        if(argc == 1)
        {
            PrintUsage();
        }
        #endif

        std::string strCommand;
        std::string strParameter;
        std::string strTemp;

        for(int i = 1; i < argc; i++)
        {
            strTemp = argv[i];

            //
            // Cmd line options can be -option value
            //
            strCommand = strTemp;
            if ((strTemp[0] == '-') && (i < (argc-1))) 
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
            

        } // for loop

    }
    catch (char *str)
    {
        PrintInfo("%s\n",str);
        return false;
    }

    return true;
}

bool SouceAndDestCompatible(CCmdLineParamaters CmdPrams)
{
    return true;
}

class MyCMIPS:CMIPS
{
public:
    void PrintInfo(const char* Format, ... );
};

void MyCMIPS::PrintInfo(const char* Format, ... )
{
    char buff[128];
    // define a pointer to save argument list
    va_list args;
    // process the arguments into our debug buffer
    va_start(args, Format);
    vsprintf_s(buff,Format,args);
    va_end(args);

    printf(buff);
}

extern PluginManager    g_pluginManager;            // Global plugin manager instance
extern bool             g_bAbortCompression;
extern CMIPS*           g_CMIPS;                            // Global MIPS functions shared between app and all IMAGE plugins

MipSet            g_MipSetIn;
MipSet            g_MipSetCmp;
MipSet            g_MipSetOut;
int               g_MipLevel = 1;
float             g_fProgress = -1;



bool CompressionCallback(float fProgress, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
   if (g_fProgress != fProgress)
   {
        UNREFERENCED_PARAMETER(pUser1);
        UNREFERENCED_PARAMETER(pUser2);

       if ((g_CmdPrams.noprogressinfo) || (g_CmdPrams.silent)) return g_bAbortCompression;
   
       if (g_MipLevel > 1)
            PrintInfo("\rCompression progress = %2.0f  MipLevel = %2d",fProgress,g_MipLevel);
       else
            PrintInfo("\rCompression progress = %2.0f",fProgress);

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

    if (nWidth > nHeight) return (nHeight);
    return (nWidth);
}


bool _cdecl TC_PluginCodecSupportsFormat(const MipSet* pMipSet)
{
    return (pMipSet->m_ChannelFormat == CF_8bit     ||
            pMipSet->m_ChannelFormat == CF_16bit    ||
            pMipSet->m_ChannelFormat == CF_2101010  ||
            pMipSet->m_ChannelFormat == CF_32bit    ||
            pMipSet->m_ChannelFormat == CF_Float16  ||
            pMipSet->m_ChannelFormat == CF_Float32);
}

void cleanup(bool Delete_gMipSetIn,bool SwizzleMipSetIn)
{
    SetDllDirectory(NULL);

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


void swap_Bytes(BYTE *src, int width, int height,int offset)
{
    int  i, j;
    BYTE b;

    for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
            b        = *src;         // hold 1st byte
            *src     = *(src+2);     // move 1st to offsetrd 
            *(src+2) = b;            // save offset to 1st 
            src      = src+offset;   // move to next set of bytes
        }
    }

}

void SwizzleMipMap(MipSet *pMipSet)
{
        CMP_DWORD dwWidth;
        CMP_DWORD dwHeight;
        CMP_BYTE     *pData;

        for(int nMipLevel=0; nMipLevel<pMipSet->m_nMipLevels; nMipLevel++)
        {        
            for(int nFaceOrSlice=0; nFaceOrSlice<MaxFacesOrSlices(pMipSet, nMipLevel); nFaceOrSlice++)
            {
                    //=====================
                    // Uncompressed source
                    //======================
                    MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(pMipSet, nMipLevel, nFaceOrSlice);
                    dwWidth     = pInMipLevel->m_nWidth;
                    dwHeight    = pInMipLevel->m_nHeight;
                    pData       = pInMipLevel->m_pbData;

                    // Swizzle to RGBA format when compressing from uncompressed DDS file! This is a Patch for now.
                    // may want to try this patch on other file types BMP & PNG to move swizzle out to main code.
                        switch (pMipSet->m_TextureDataType)
                        {
                            case TDT_ARGB: swap_Bytes(pInMipLevel->m_pbData, dwWidth, dwHeight, 4); break;
                            case TDT_XRGB: swap_Bytes(pInMipLevel->m_pbData, dwWidth, dwHeight, 3); break;
                            default: break;
                        }
            }
        }

}

// Determine if RGB channel to BGA can be done or skipped
// for special cases of compressed formats.

bool KeepSwizzle(CMP_FORMAT destformat)
{
    // determin of the swizzle flag needs to be turned on!
    switch (destformat)
    {
            case CMP_FORMAT_BC4:         
            case CMP_FORMAT_ATI1N:        // same as BC4    
            case CMP_FORMAT_ATI2N:        // same as BC4    
            case CMP_FORMAT_BC5:         
            case CMP_FORMAT_ATI2N_XY:    // same as BC5    
            case CMP_FORMAT_ATI2N_DXT5:    // same as BC5    
            case CMP_FORMAT_BC1:         
            case CMP_FORMAT_DXT1:        // same as BC1     
            case CMP_FORMAT_BC2:         
            case CMP_FORMAT_DXT3:        // same as BC2     
            case CMP_FORMAT_BC3:         
            case CMP_FORMAT_DXT5:        // same as BC3     
            case CMP_FORMAT_ATC_RGB:
            case CMP_FORMAT_ATC_RGBA_Explicit:
            case CMP_FORMAT_ATC_RGBA_Interpolated:
            return true;    
            break;
            default:
                break;
    }

    return false;
}


bool FormatIsFloat(CMP_FORMAT InFormat)
{
    switch (InFormat)
    {
    case CMP_FORMAT_ARGB_16F   :
    case CMP_FORMAT_ABGR_16F   :
    case CMP_FORMAT_RGBA_16F   :
    case CMP_FORMAT_BGRA_16F   :
    case CMP_FORMAT_RG_16F     :
    case CMP_FORMAT_R_16F      :
    case CMP_FORMAT_ARGB_32F   :
    case CMP_FORMAT_ABGR_32F   :
    case CMP_FORMAT_RGBA_32F   :
    case CMP_FORMAT_BGRA_32F   :
    case CMP_FORMAT_RGB_32F    :
    case CMP_FORMAT_BGR_32F    :
    case CMP_FORMAT_RG_32F     :
    case CMP_FORMAT_R_32F      :
    case CMP_FORMAT_BC6H       :
    case CMP_FORMAT_BC6H_SF    :
    case CMP_FORMAT_RGBE_32F   :
                                {
                                        return true;
                                }
                                break;
    default:
                                break;
    }

    return false;
}

#ifdef MAKE_FORMAT_COMPATIBLE
extern float half_conv_float(unsigned short in);
extern inline float knee(double x, double f);
extern float findKneeF(float x, float y);
void Float2Byte(CMP_BYTE cBlock[], FLOAT* fBlock, CMP_Texture& srcTexture)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(cBlock);
    assert(fBlock);
    assert(&srcTexture);

    if (cBlock && fBlock)
    {
        float r, g, b, a;
        float kl = Imath::Math<float>::pow(2.f, 0);
        float f = findKneeF(Imath::Math<float>::pow(2.f, 5) - kl, Imath::Math<float>::pow(2.f, 3.5f) - kl);
        int i = 0;
        for (int y = 0; y < srcTexture.dwHeight; y++) {
            for (int x = 0; x < srcTexture.dwWidth; x++) {

                r = *fBlock;
                fBlock++;
                g = *fBlock;
                fBlock++;
                b = *fBlock;
                fBlock++;
                a = *fBlock;
                fBlock++;
                BYTE r_b, g_b, b_b, a_b;


                //  1) Compensate for fogging by subtracting defog
                //     from the raw pixel values.
                // We assume a defog of 0


                //  2) Multiply the defogged pixel values by
                //     2^(exposure + 2.47393).
                const float exposeScale = Imath::Math<float>::pow(2, 3.47393f);
                r = r * exposeScale;
                g = g * exposeScale;
                b = b * exposeScale;
                a = a * exposeScale;

                //  3) Values that are now 1.0 are called "middle gray".
                //     If defog and exposure are both set to 0.0, then
                //     middle gray corresponds to a raw pixel value of 0.18.
                //     In step 6, middle gray values will be mapped to an
                //     intensity 3.5 f-stops below the display's maximum
                //     intensity.

                //  4) Apply a knee function.  The knee function has two
                //     parameters, kneeLow and kneeHigh.  Pixel values
                //     below 2^kneeLow are not changed by the knee
                //     function.  Pixel values above kneeLow are lowered
                //     according to a logarithmic curve, such that the
                //     value 2^kneeHigh is mapped to 2^3.5.  (In step 6,
                //     this value will be mapped to the the display's
                //     maximum intensity.)
                if (r > kl) {
                    r = kl + knee(r - kl, f);
                }
                if (g > kl) {
                    g = kl + knee(g - kl, f);
                }
                if (b > kl) {
                    b = kl + knee(b - kl, f);
                }
                if (a > kl) {
                    a = kl + knee(a - kl, f);
                }

                //  5) Gamma-correct the pixel values, according to the
                //     screen's gamma.  (We assume that the gamma curve
                //     is a simple power function.)
                r = Imath::Math<float>::pow(r, 0.4545);
                g = Imath::Math<float>::pow(g, 0.4545);
                b = Imath::Math<float>::pow(b, 0.4545);
                a = Imath::Math<float>::pow(a, 2.2);

                //  6) Scale the values such that middle gray pixels are
                //     mapped to a frame buffer value that is 3.5 f-stops
                //     below the display's maximum intensity. (84.65 if
                //     the screen's gamma is 2.2)
                r *= 84.65;
                g *= 84.65;
                b *= 84.65;
                a *= 84.65;

                r_b = Imath::clamp<float>(r, 0.f, 255.f);
                g_b = Imath::clamp<float>(g, 0.f, 255.f);
                b_b = Imath::clamp<float>(b, 0.f, 255.f);
                a_b = Imath::clamp<float>(a, 0.f, 255.f);
                cBlock[i] = r_b;
                i++;
                cBlock[i] = g_b;
                i++;
                cBlock[i] = b_b;
                i++;
                cBlock[i] = a_b;
                i++;
            }
            
        }

    }
}

void Byte2Float(FLOAT* fBlock, CMP_BYTE* cBlock, CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(fBlock);
    assert(cBlock);
    assert(dwBlockSize);
    if (fBlock && cBlock && dwBlockSize)
    {
        half temp;
        for (CMP_DWORD i = 0; i < dwBlockSize; i++)
        {
            temp = FLOAT(cBlock[i]);
            fBlock[i] = temp.bits();
        }
    }
}
#endif

//http://www.ludicon.com/castano/blog/2016/09/lightmap-compression-in-the-witness/
#ifdef RGBM_BC7     
inline int ftoi_round(float f) {
    return _mm_cvt_ss2si(_mm_set_ss(f));
}

template <typename T>
inline T clamp(const T & x, const T & a, const T & b)
{
    return min(max(x, a), b);
}
inline float saturate(float f) {
    return clamp(f, 0.0f, 1.0f);
}
inline float square(float f) { return f * f; }
#endif

bool MakeFormatCompatible(CMP_Texture& srcTexture, MipLevel* pInMipLevel, CMP_FORMAT destFormat)
{
    bool src  = FormatIsFloat(srcTexture.format);
    bool dest = FormatIsFloat(destFormat);

#ifdef MAKE_FORMAT_COMPATIBLE
    if (src == dest)
    {
        srcTexture.pData = pInMipLevel->m_pbData;
        srcTexture.dwDataSize = CMP_CalculateBufferSize(&srcTexture);
        return true;
    }
    CMP_DWORD size = srcTexture.dwWidth * srcTexture.dwHeight;

    if (src && !dest)
    {
        FLOAT* pfData = pInMipLevel->m_pfData;
        CMP_BYTE *byteData = new CMP_BYTE[size*4];
  
#ifdef RGBM_BC7
        float r, g, b, a;
        float threshold = 0.95;
        float bestError = FLT_MAX;
        float bestM;
        int i = 0;
        int j = 0;
        for (int x = 0; x<size; x++)
        {
                r = pfData[i];
                i++;
                g = pfData[i];
                i++;
                b = pfData[i];
                i++;
                a = pfData[i];
                i++;
                for (int m = 0; m < 256; m++) {
                    // Decode M
                    float M = float(m) / 255.0f * (1 - threshold) + threshold;

                    // Encode RGB.
                    int R = ftoi_round(255.0f * saturate(r / M));
                    int G = ftoi_round(255.0f * saturate(g / M));
                    int B = ftoi_round(255.0f * saturate(b / M));

                    // Decode RGB.
                    float dr = (float(R) / 255.0f) * M;
                    float dg = (float(G) / 255.0f) * M;
                    float db = (float(B) / 255.0f) * M;

                    // Measure error.
                    float error = square(r - dr) + square(g - dg) + square(b - db);

                    if (error < bestError) {
                        bestError = error;
                        bestM = M;
                    }
                }

                byteData[j] = ftoi_round(255.0f * saturate(r / bestM));
                j++;
                byteData[j] = ftoi_round(255.0f * saturate(g / bestM));
                j++;
                byteData[j] = ftoi_round(255.0f * saturate(b / bestM));
                j++;
                byteData[j] = ftoi_round(255.0f * saturate(a / bestM));
                j++;
            
        }
#else
        Float2Byte(byteData, pfData, srcTexture);
#endif

        srcTexture.pData = (CMP_BYTE*)byteData;
        srcTexture.format = CMP_FORMAT_ARGB_8888;    
        srcTexture.dwDataSize = size*4;

        src = dest;
    }

    else if (!src && dest)
    {
        CMP_BYTE *pbData = pInMipLevel->m_pbData;
        FLOAT *floatData = new FLOAT[size * 4];
        Byte2Float(floatData, pbData, size*4);
        srcTexture.pData = (CMP_BYTE*)floatData;
        srcTexture.format = CMP_FORMAT_ARGB_32F;
        srcTexture.dwDataSize = size * sizeof(float)*4;
    
        src = dest;
    }
#else
    srcTexture.pData = pInMipLevel->m_pbData;
    srcTexture.dwDataSize = CMP_CalculateBufferSize(&srcTexture);
#endif
    return (src==dest);
}


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

    PluginInterface_Analysis *Plugin_Analysis;
    int testpassed = 0;
    Plugin_Analysis = reinterpret_cast<PluginInterface_Analysis *>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        g_CmdPrams.DiffFile = DestFile;
        int lastindex = g_CmdPrams.DiffFile.find_last_of(".");
        g_CmdPrams.DiffFile = g_CmdPrams.DiffFile.substr(0, lastindex);
        g_CmdPrams.DiffFile.append("_diff.bmp");

        string results_file = "";
        results_file = DestFile;
        int index = results_file.find_last_of("/");
        results_file = results_file.substr(0, (index+1));
        results_file.append("Analysis_Result.xml");

        testpassed = Plugin_Analysis->TC_ImageDiff(SourceFile.c_str(), DestFile.c_str(), g_CmdPrams.DiffFile.c_str(), (char*)results_file.c_str(), &g_pluginManager, NULL);
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

void LocalPrintF(char *buff)
{
    printf(buff);
}

#ifdef USE_COMPUTE  
#include "Common_Kernel.h"
#include "Compute_Lib\Compute_Base.h"

#ifdef _DEBUG
#pragma comment(lib,"Compute_lib_MDd.lib")
#else
#pragma comment(lib,"Compute_lib_MD.lib")
#endif

bool SVMInitCodec(KernalOptions *options)
{
    CMP_FORMAT format = (CMP_FORMAT)options->data_type;
    switch (format)
    {
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
    case CMP_FORMAT_BC7:
    case CMP_FORMAT_ASTC:
    {
        unsigned char *src = (unsigned char *)options->data;
        unsigned char *dst = (unsigned char *)options->dataSVM;
        memcpy(dst,src, options->size);
        return true;
    }
    break;
    }
    return false;
}

#endif
//
// Used exclusively by the GUI app 
// ToDo : Remove this code and try to use ProcessCMDLine
MipSet* DecompressMIPSet(MipSet *MipSetIn, CMP_GPUDecode decodeWith, Config *configSetting)
{
    // validate MipSet is Compressed
    if (!MipSetIn->m_compressed) return NULL;
    if (MipSetIn->m_format == CMP_FORMAT_ASTC && !(configSetting->useCPU))
    {
        configSetting->errMessage = "ASTC decompressed with GPU is not supported yet. Please view ASTC compressed image using CPU (under Settings->Application Options.)\n";
        PrintInfo("Decompress Error: ASTC decompressed with GPU is not supported yet. Please view ASTC compressed image using CPU (under Settings->Application Options.)\n");
        return NULL;
    }

    // Compress Options
    bool silent = true;
    CMP_CompressOptions    CompressOptions;
    memset(&CompressOptions,0,sizeof(CMP_CompressOptions));
    CompressOptions.dwnumThreads = 8;
    CMIPS m_CMIPS;
    
    MipSet    *MipSetOut = new MipSet();
    memset(MipSetOut, 0, sizeof(MipSet));

    MipSetOut->m_TextureDataType = TDT_ARGB;
    MipSetOut->m_swizzle         = false;
    MipSetOut->m_CubeFaceMask    = MipSetIn->m_CubeFaceMask;
    MipSetOut->m_Flags           = MipSetIn->m_Flags;
    MipSetOut->m_nDepth          = MipSetIn->m_nDepth;
    MipSetOut->m_nMaxMipLevels   = MipSetIn->m_nMaxMipLevels;
    MipSetOut->m_nHeight         = MipSetIn->m_nHeight;
    MipSetOut->m_nWidth          = MipSetIn->m_nWidth;

    // BMP is saved as CMP_FORMAT_ARGB_8888
    // EXR is saved as CMP_FORMAT_ARGB_16F
    switch (MipSetIn->m_format)
    {
        case CMP_FORMAT_BC6H:
        case CMP_FORMAT_BC6H_SF:
            MipSetOut->m_format = CMP_FORMAT_ARGB_16F;
            MipSetOut->m_ChannelFormat = CF_Float16;
            break;
        default:
            MipSetOut->m_format = CMP_FORMAT_ARGB_8888;
            break;
     }
    
    // Allocate output MipSet
    if (!m_CMIPS.AllocateMipSet(MipSetOut,
            MipSetOut->m_ChannelFormat,
            MipSetOut->m_TextureDataType,
            MipSetIn->m_TextureType,
            MipSetIn->m_nWidth,
            MipSetIn->m_nHeight,
            MipSetIn->m_nDepth))
    {
        configSetting->errMessage = "Memory Error(2): allocating MIPSet Output buffer.";
        PrintInfo("Memory Error(2): allocating MIPSet Output buffer\n");
        m_CMIPS.FreeMipSet(MipSetOut);
        delete MipSetOut;
        MipSetOut = NULL;
        return NULL;
    }

    MipLevel* pCmpMipLevel = m_CMIPS.GetMipLevel(MipSetIn, 0);
    int nMaxFaceOrSlice = MaxFacesOrSlices(MipSetIn, 0);
    int nWidth = pCmpMipLevel->m_nWidth;
    int nHeight = pCmpMipLevel->m_nHeight;

    BYTE* pMipData = m_CMIPS.GetMipLevel(MipSetIn, 0, 0)->m_pbData;

    for (int nFaceOrSlice = 0; nFaceOrSlice<nMaxFaceOrSlice; nFaceOrSlice++)
    {
            int nMipWidth = nWidth;
            int nMipHeight = nHeight;

            for (int nMipLevel = 0; nMipLevel<MipSetIn->m_nMipLevels; nMipLevel++)
            {
                MipLevel* pInMipLevel = m_CMIPS.GetMipLevel(MipSetIn, nMipLevel, nFaceOrSlice);
                if (!pInMipLevel)
                {
                    configSetting->errMessage = "Memory Error(2): allocating MIPSet Output Cmp level buffer";
                    PrintInfo("Memory Error(2): allocating MIPSet Output Cmp level buffer\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }

                // Valid Mip Level ?
                if (pInMipLevel->m_pbData)
                    pMipData = pInMipLevel->m_pbData;

                if (!m_CMIPS.AllocateMipLevelData(m_CMIPS.GetMipLevel(MipSetOut, nMipLevel, nFaceOrSlice), nMipWidth,
                    nMipHeight, MipSetOut->m_ChannelFormat, MipSetOut->m_TextureDataType))
                {
                    configSetting->errMessage = "Memory Error(2): allocating MIPSet Output level buffer.";
                    PrintInfo("Memory Error(2): allocating MIPSet Output level buffer\n");
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }

                //----------------------------
                // Compressed source 
                //-----------------------------
                CMP_Texture srcTexture;
                srcTexture.dwSize        = sizeof(srcTexture);
                srcTexture.dwWidth       = nMipWidth;
                srcTexture.dwHeight      = nMipHeight;
                srcTexture.dwPitch       = 0;
                srcTexture.nBlockWidth   = MipSetIn->m_nBlockWidth;
                srcTexture.nBlockHeight  = MipSetIn->m_nBlockHeight;
                srcTexture.nBlockDepth   = MipSetIn->m_nBlockDepth;
                srcTexture.format        = MipSetIn->m_format;
                srcTexture.dwDataSize    = CMP_CalculateBufferSize(&srcTexture);
                srcTexture.pData         = pMipData;

                //-----------------------------
                // Uncompressed Destination
                //-----------------------------
                CMP_Texture destTexture;
                destTexture.dwSize       = sizeof(destTexture);
                destTexture.dwWidth      = nMipWidth;
                destTexture.dwHeight     = nMipHeight;
                destTexture.dwPitch      = 0;
                destTexture.nBlockWidth  = MipSetOut->m_nBlockWidth;
                destTexture.nBlockHeight = MipSetOut->m_nBlockHeight;
                destTexture.nBlockDepth  = MipSetOut->m_nBlockDepth;
                destTexture.format       = MipSetOut->m_format;
                destTexture.dwDataSize   = CMP_CalculateBufferSize(&destTexture);
                destTexture.pData        = m_CMIPS.GetMipLevel(MipSetOut, nMipLevel, nFaceOrSlice)->m_pbData;

                if (!silent)
                {
                    if ((nMipLevel > 1) || (nFaceOrSlice > 1))
                        PrintInfo("\rProcessing destination MipLevel %2d FaceOrSlice %2d", nMipLevel + 1, nFaceOrSlice);
                    else
                        PrintInfo("\rProcessing destination    ");
                }

                try
                {

                    if ((IsBadReadPtr(srcTexture.pData, srcTexture.dwDataSize)))
                    {
                        configSetting->errMessage = "Memory Error(2): Source image cannot be accessed.";
                        PrintInfo("Memory Error(2): Source image\n");
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }

                    if (/*(srcTexture.dwDataSize > destTexture.dwDataSize) ||*/ (IsBadWritePtr(destTexture.pData, destTexture.dwDataSize)))
                    {
                        configSetting->errMessage = "Memory Error(2): Destination image must be compatible with source.";
                        PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }

                    // Return values of the CMP_ calls should be checked for failures
                    CMP_ERROR res;
                    if (configSetting->useCPU)
                    {
                        res = CMP_ConvertTexture(&srcTexture, &destTexture, &CompressOptions, NULL, NULL, NULL);
                        if (res != CMP_OK)
                        {
                            configSetting->errMessage = "Compress Failed with Error " + res;
                            PrintInfo("Compress Failed with Error %d\n", res);
                            m_CMIPS.FreeMipSet(MipSetOut);
                            delete MipSetOut;
                            MipSetOut = NULL;
                            return NULL;
                        }
                    }
                    else
                    {
                        CMP_ERROR res;
                        res = CMP_DecompressTexture(&srcTexture, &destTexture, decodeWith);
                        if (res == CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE)
                        {
                            configSetting->errMessage = "Error: ASTC gpu decode only target AMD Vega 10 GPU.\n";
                            PrintInfo("Error: ASTC gpu decode only target AMD Vega 10 GPU.\n");
                            m_CMIPS.FreeMipSet(MipSetOut);
                            delete MipSetOut;
                            MipSetOut = NULL;
                            return NULL;
                        }
                        else if (res != CMP_OK)
                        {
                            configSetting->errMessage = "Decompress Failed with Error " + res;
                            PrintInfo("Decompress Failed with Error %d\n",res);
                            m_CMIPS.FreeMipSet(MipSetOut);
                            delete MipSetOut;
                            MipSetOut = NULL;
                            return NULL;
                        }
                    }
                    

                }
                catch (std::exception& e)
                {
                    PrintInfo(e.what());
                    m_CMIPS.FreeMipSet(MipSetOut);
                    delete MipSetOut;
                    MipSetOut = NULL;
                    return NULL;
                }

                pMipData    += srcTexture.dwDataSize;

                nMipWidth    = (nMipWidth>1) ? (nMipWidth >> 1) : 1;
                nMipHeight    = (nMipHeight>1) ? (nMipHeight >> 1) : 1;
            }
    }

    MipSetOut->m_nMipLevels = MipSetIn->m_nMipLevels;

    return MipSetOut;
}


int ProcessCMDLine(CMP_Feedback_Proc pFeedbackProc, MipSet *p_userMipSetIn)
{
    LARGE_INTEGER   frequency,
                    conversion_loopStartTime    = {0},
                    conversion_loopEndTime      = {0},
                    compress_loopStartTime      = {0},
                    compress_loopEndTime        = {0},
                    decompress_loopStartTime    = {0},
                    decompress_loopEndTime      = {0};
    int             compress_nIterations        =0,
                    decompress_nIterations      =0;

    // These flags indicate if the source and destination files are compressed
    bool            SourceFormatIsCompressed        = false;
    bool            DestinationFileIsCompressed     = false;
    bool            DestinationFormatIsCompressed   = false;

    bool            TranscodeBits                   = false;
    bool            MidwayDecompress                = false;

    //  With a user suppiled Mip Map dont delete it on exit
    bool            Delete_gMipSetIn            = false;

    // flags if an user suppiled Mip Map input was swizzled during compression / decompression
    bool            SwizzledMipSetIn            = false;

    // Currently active input and output mips buffers
    // These point to the allocated g_MipSetxxx buffers 
    // depending on the users requirement for input and output file formats
    MipSet            *p_MipSetIn;
    MipSet            *p_MipSetOut;

#ifdef SHOW_PROCESS_MEMORY
    PROCESS_MEMORY_COUNTERS memCounter1;
    PROCESS_MEMORY_COUNTERS memCounter2;

#ifdef SHOW_PROCESS_MEMORY
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter1, sizeof(memCounter1));
#endif

#endif


        // Check if print status line has been assigned
        // if not get it a default to printf
        if (PrintStatusLine == NULL)
            PrintStatusLine = &LocalPrintF;

        
        if (g_CmdPrams.analysis)
        {
            if (!(GenerateAnalysis(g_CmdPrams.SourceFile, g_CmdPrams.DestFile)))
            {
                PrintInfo("Error: Image Analysis Failed\n");
                return -1;
            }
            return 0;
        }

        QueryPerformanceFrequency(&frequency);

        // ==========================
        // Mip Settings Class
        // ==========================
        g_CMIPS = (CMIPS*) new(MyCMIPS);

        // ---------
        // Input
        // ---------
        CMP_FORMAT   srcFormat;
        memset(&g_MipSetIn,0,sizeof(MipSet));

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
        CMP_FORMAT   destFormat;
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
            g_MipSetIn.m_swizzle = KeepSwizzle(destFormat); 
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

            if (AMDLoadMIPSTextureImage(g_CmdPrams.SourceFile.c_str(), &g_MipSetIn, g_CmdPrams.use_OCV) != 0)
            {
                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                PrintInfo("Error: loading image, data type not supported.\n");
                return -1;
            }
        }

        if (g_CmdPrams.showperformance)
            QueryPerformanceCounter(&conversion_loopStartTime);
      
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

#ifndef MAKE_FORMAT_COMPATIBLE
        if ((isFloat(srcFormat) && !isFloat(destFormat)) || (!isFloat(srcFormat) && isFloat(destFormat)))
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
            PluginInterface_Filters *plugin_Filter;
            plugin_Filter = reinterpret_cast<PluginInterface_Filters *>(g_pluginManager.GetPlugin("FILTERS", "BOXFILTER"));
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
        CMP_FORMAT   cmpformat = CMP_FORMAT_Unknown;
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
               g_MipSetCmp.m_ChannelFormat      = CF_Compressed;
               g_MipSetCmp.m_nMaxMipLevels      = g_MipSetIn.m_nMaxMipLevels; 
               g_MipSetCmp.m_nMipLevels         = 1;    // this is overwriiten depending on input.
               if (!g_CMIPS->AllocateMipSet(&g_MipSetCmp, CF_8bit, TDT_ARGB, TT_2D, g_MipSetIn.m_nWidth, g_MipSetIn.m_nHeight, 1))
               {
                    PrintInfo("Memory Error(1): allocating MIPSet Compression buffer\n");
                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                    return -1;
               }

               g_MipSetCmp.m_nHeight    = g_MipSetIn.m_nHeight;
               g_MipSetCmp.m_nWidth     = g_MipSetIn.m_nWidth;
               g_MipSetCmp.m_format     = destFormat;
               Format2FourCC(destFormat,&g_MipSetCmp);

               CMP_Texture srcTexture;
               srcTexture.dwSize = sizeof(srcTexture);
               int DestMipLevel = g_MipSetIn.m_nMipLevels;

               if (g_CmdPrams.showperformance)
                   QueryPerformanceCounter(&compress_loopStartTime);

               for(int nMipLevel=0; nMipLevel<DestMipLevel; nMipLevel++)
               {        
                    g_MipLevel = nMipLevel+1;

                    for (int nFaceOrSlice = 0; nFaceOrSlice < MaxFacesOrSlices(&g_MipSetIn, nMipLevel); nFaceOrSlice++)
                    {

                        //=====================
                        // Uncompressed source
                        //======================
                        MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(&g_MipSetIn, nMipLevel, nFaceOrSlice);
                        srcTexture.dwWidth = pInMipLevel->m_nWidth;
                        srcTexture.dwHeight = pInMipLevel->m_nHeight;
                        srcTexture.dwPitch = 0;
                        srcTexture.nBlockWidth = g_MipSetIn.m_nBlockWidth;
                        srcTexture.nBlockHeight = g_MipSetIn.m_nBlockHeight;
                        srcTexture.nBlockDepth = g_MipSetIn.m_nBlockDepth;
                        srcTexture.format = srcFormat;
                        srcTexture.pData = NULL;

                        //========================
                        // Compressed Destination
                        //========================
                        CMP_Texture destTexture;
                        destTexture.dwSize = sizeof(destTexture);
                        destTexture.dwWidth = pInMipLevel->m_nWidth;
                        destTexture.dwHeight = pInMipLevel->m_nHeight;
                        destTexture.dwPitch = 0;
                        destTexture.nBlockWidth = g_CmdPrams.BlockWidth;
                        destTexture.nBlockHeight = g_CmdPrams.BlockHeight;
                        destTexture.format = destFormat;
                        destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);

                        // Check Both Source and Destination formats are compatible
                        if (!MakeFormatCompatible(srcTexture, pInMipLevel, g_CmdPrams.DestFormat))
                        {
                            PrintInfo("Source and Destination formats are not Compatible\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                            return -1;
                        }


                        PrintInfo("Source Texture size = %d Bytes, width = %d px  height = %d px\n",
                            srcTexture.dwDataSize,
                            srcTexture.dwWidth,
                            srcTexture.dwHeight);

                        if (destTexture.dwDataSize > 0)
                            PrintInfo("Destination Texture size = %d Bytes   Resulting compression ratio = %2.2f:1\n",
                                destTexture.dwDataSize,
                                srcTexture.dwDataSize / (float)destTexture.dwDataSize);

                        MipLevel* pOutMipLevel = g_CMIPS->GetMipLevel(&g_MipSetCmp, nMipLevel, nFaceOrSlice);
                        if (!g_CMIPS->AllocateCompressedMipLevelData(pOutMipLevel, destTexture.dwWidth, destTexture.dwHeight, destTexture.dwDataSize))
                        {
                            PrintInfo("Memory Error(1): allocating MIPSet compression level data buffer\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
#ifdef MAKE_FORMAT_COMPATIBLE 
                            if (srcTexture.pData)
                            {
                                free(srcTexture.pData);
                                srcTexture.pData = NULL;
                            }
#endif
                            return -1;
                        }

                        destTexture.pData = pOutMipLevel->m_pbData;
                        g_fProgress = -1;

                        //========================
                        // Process ConvertTexture
                        //========================
#ifdef USE_COMPUTE
                        if (g_CmdPrams.CompressOptions.bUseGPUCompress)
                        {
                            bool format_support_gpu = true;
                            KernalOptions   kernel_options;
                            memset(&kernel_options, 0, sizeof(KernalOptions));
                            PluginInterface_Compute *plugin_compute = NULL;

                            kernel_options.data_type = (unsigned int)(g_CmdPrams.DestFormat);
                            kernel_options.Compute_type = (unsigned int)(g_CmdPrams.CompressOptions.nComputeWith);

                            plugin_compute = reinterpret_cast<PluginInterface_Compute *>(g_pluginManager.GetPlugin("COMPUTE", GetFormatDesc(g_CmdPrams.DestFormat)));

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
                                    printf("Using %s Compute plugin V%d.%d\n", GetFormatDesc(g_CmdPrams.DestFormat), PluginVersion.dwPluginVersionMajor, PluginVersion.dwPluginVersionMinor);

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
                        #ifdef MAKE_FORMAT_COMPATIBLE 
                        if (srcTexture.pData)
                        {
                            free(srcTexture.pData);
                            srcTexture.pData = NULL;
                        }
                        #endif

                        if (g_CmdPrams.showperformance)
                                    compress_nIterations++;

                    }
                    g_MipSetCmp.m_nMipLevels++;
                }

                if (g_CmdPrams.showperformance)
                    QueryPerformanceCounter(&compress_loopEndTime);

                srcFormat    = destFormat;

                // for printing info about what compression was used
                cmpformat   = destFormat;

                if (pFeedbackProc)
                    pFeedbackProc(100, NULL, NULL);
        }
    
        //==============================================
        // Save to file destination buffer if 
        // Uncomprssed file to Compressed File format
        //==============================================

        if ((!SourceFormatIsCompressed) && (DestinationFormatIsCompressed)
            &&
            !IsDestinationUnCompressed((const char *)g_CmdPrams.DestFile.c_str())
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
                PrintInfo("Error: saving image or format is unsupported\n");
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
                MidwayDecompress = true;
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
        if (    
            (!SourceFormatIsCompressed) && (DestinationFormatIsCompressed)
            && 
            IsDestinationUnCompressed((const char *)g_CmdPrams.DestFile.c_str())
            )
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
                                destFormat                        = CMP_FORMAT_ARGB_16F;
                                g_MipSetOut.m_ChannelFormat        = CF_Float16;
                                break;
                        default:
                                destFormat                        = CMP_FORMAT_ARGB_8888;
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
                            destFormat = CMP_FORMAT_ARGB_16F;
                            g_MipSetOut.m_ChannelFormat = CF_Float16;
                            break;
                        default:
                            destFormat = FormatByFileExtension((const char *)g_CmdPrams.DestFile.c_str(), &g_MipSetOut);
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
                else
                if (destFormat == CMP_FORMAT_ARGB_16F)
                    g_MipSetOut.m_ChannelFormat = CF_Float16;

                g_MipSetOut.m_format         = destFormat;
                g_MipSetOut.m_isDeCompressed = srcFormat!= CMP_FORMAT_Unknown ? srcFormat : CMP_FORMAT_MAX;

               // Allocate output MipSet
                if (!g_CMIPS->AllocateMipSet(&g_MipSetOut,
                                            g_MipSetOut.m_ChannelFormat, 
                                            g_MipSetOut.m_TextureDataType, 
                                            p_MipSetIn->m_TextureType,
                                            p_MipSetIn->m_nWidth, 
                                            p_MipSetIn->m_nHeight,
                                            p_MipSetIn->m_nDepth))
                {
                    PrintInfo("Memory Error(2): allocating MIPSet Output buffer\n");
                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                    return -1;
                }

                g_MipSetOut.m_CubeFaceMask = p_MipSetIn->m_CubeFaceMask;

                MipLevel* pCmpMipLevel = g_CMIPS->GetMipLevel(p_MipSetIn, 0);
                int nMaxFaceOrSlice        = MaxFacesOrSlices(p_MipSetIn, 0);
                int nWidth                = pCmpMipLevel->m_nWidth;
                int nHeight                = pCmpMipLevel->m_nHeight;
                //
                BYTE* pMipData = g_CMIPS->GetMipLevel(p_MipSetIn, 0, 0)->m_pbData;
                
                bool use_GPUDecode = g_CmdPrams.CompressOptions.bUseGPUDecompress;
                CMP_GPUDecode DecodeWith = g_CmdPrams.CompressOptions.nGPUDecode;

                if (g_CmdPrams.showperformance)
                    QueryPerformanceCounter(&decompress_loopStartTime);

                for(int nFaceOrSlice=0; nFaceOrSlice<nMaxFaceOrSlice; nFaceOrSlice++)
                {
                    int nMipWidth = nWidth;
                    int nMipHeight = nHeight;

                    for(int nMipLevel=0; nMipLevel<p_MipSetIn->m_nMipLevels; nMipLevel++)
                    {
                        MipLevel* pInMipLevel = g_CMIPS->GetMipLevel(p_MipSetIn, nMipLevel, nFaceOrSlice);
                        if(!pInMipLevel)
                        {
                            PrintInfo("Memory Error(2): allocating MIPSet Output Cmp level buffer\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                            return -1;
                        }

                        // Valid Mip Level ?
                        if(pInMipLevel->m_pbData)
                            pMipData = pInMipLevel->m_pbData;
                
                        if (!g_CMIPS->AllocateMipLevelData(g_CMIPS->GetMipLevel(&g_MipSetOut, nMipLevel, nFaceOrSlice), nMipWidth,
                            nMipHeight, g_MipSetOut.m_ChannelFormat, g_MipSetOut.m_TextureDataType))
                        {
                            PrintInfo("Memory Error(2): allocating MIPSet Output level buffer\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                            return -1;
                        }

                        //----------------------------
                        // Compressed source 
                        //-----------------------------
                        CMP_Texture srcTexture;
                        srcTexture.dwSize            = sizeof(srcTexture);
                        srcTexture.dwWidth           = nMipWidth;
                        srcTexture.dwHeight          = nMipHeight;
                        srcTexture.dwPitch           = 0;
                        srcTexture.nBlockHeight      = p_MipSetIn->m_nBlockHeight;
                        srcTexture.nBlockWidth       = p_MipSetIn->m_nBlockWidth;
                        srcTexture.nBlockDepth       = p_MipSetIn->m_nBlockDepth;
                        srcTexture.format            = srcFormat;
                        srcTexture.dwDataSize        = CMP_CalculateBufferSize(&srcTexture);
                        srcTexture.pData             = pMipData;
                
                        //-----------------------------
                        // Uncompressed Destination
                        //-----------------------------
                        CMP_Texture destTexture;
                        destTexture.dwSize           = sizeof(destTexture);
                        destTexture.dwWidth          = nMipWidth;
                        destTexture.dwHeight         = nMipHeight;
                        destTexture.dwPitch          = 0;
                        destTexture.format           = destFormat;
                        destTexture.dwDataSize       = CMP_CalculateBufferSize(&destTexture);
                        destTexture.pData = g_CMIPS->GetMipLevel(&g_MipSetOut, nMipLevel, nFaceOrSlice)->m_pbData;
                        if (!g_CmdPrams.silent)
                        {
                            if ((nMipLevel > 1) || (nFaceOrSlice > 1))
                                PrintInfo("\rProcessing destination     MipLevel %2d FaceOrSlice %2d",nMipLevel+1,nFaceOrSlice);
                            else
                                PrintInfo("\rProcessing destination    ");
                        }

                        if (/*(srcTexture.dwDataSize > destTexture.dwDataSize) || */(IsBadWritePtr(destTexture.pData, destTexture.dwDataSize)))
                        {
                               PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                            return -1;
                        }

                        g_fProgress = -1;

                        if (use_GPUDecode)
                        {
                            if (srcTexture.format == CMP_FORMAT_ASTC)
                            {
                                PrintInfo("Decompress Error: ASTC decompressed with GPU is not supported yet. Please view ASTC compressed images using CPU.\n");
                                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                return -1;
                            }

                            if (CMP_DecompressTexture(&srcTexture, &destTexture, DecodeWith) != CMP_OK)
                            {
                                PrintInfo("Error in decompressing source texture\n");
                                cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                return -1;
                            }

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
                        nMipWidth = (nMipWidth>1) ? (nMipWidth>>1) : 1;
                        nMipHeight = (nMipHeight>1) ? (nMipHeight>>1) : 1;
                    }
                }

                if (g_CmdPrams.showperformance)
                    QueryPerformanceCounter(&decompress_loopEndTime);

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
                    PrintInfo(" Error: saving image or destination format is unsupported by Qt\n");
                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                    return -1;
                }
                
    }

    if (g_CmdPrams.showperformance)
        QueryPerformanceCounter(&conversion_loopEndTime);

    if ((!g_CmdPrams.silent) && (g_CmdPrams.showperformance))
    {

       LONGLONG compress_Duration = compress_loopEndTime.QuadPart - compress_loopStartTime.QuadPart;
       double compress_fDuration = (((double) compress_Duration) / ((double) frequency.QuadPart));

       LONGLONG decompress_Duration = decompress_loopEndTime.QuadPart - decompress_loopStartTime.QuadPart;
       double decompress_fDuration = (((double) decompress_Duration) / ((double) frequency.QuadPart));

       LONGLONG conversion_Duration = conversion_loopEndTime.QuadPart - conversion_loopStartTime.QuadPart;
       g_CmdPrams.conversion_fDuration = (((double) conversion_Duration) / ((double) frequency.QuadPart));

       //DWORD dwCoreCount = GetProcessorCount();

#ifdef USE_WITH_COMMANDLINE_TOOL
       PrintInfo("\r");
#endif

       if (compress_nIterations)
       PrintInfo("Compressed to %s with %i iteration(s) in %.3f seconds\n",
                 GetFormatDesc(cmpformat),
                 compress_nIterations, 
                 compress_fDuration);
       
       if (decompress_nIterations)
       PrintInfo("DeCompressed to %s with %i iteration(s) in %.3f seconds\n",
                 GetFormatDesc(destFormat),
                 decompress_nIterations, 
                 decompress_fDuration);

       PrintInfo("Total time taken (includes file I/O): %.3f seconds\n", g_CmdPrams.conversion_fDuration);
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


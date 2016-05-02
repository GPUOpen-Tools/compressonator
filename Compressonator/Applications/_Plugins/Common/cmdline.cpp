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
#include "cmdline.h"
#include "Compressonator.h"
#include "Texture.h"
#include "TextureIO.h"
#include "MIPS.h"
#include "PluginManager.h"
#include "PluginInterface.h"
#include "TC_PluginInternal.h"
#include "Version.h"

// #define SHOW_PROCESS_MEMORY

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
        printf("CompressonatorCLI V%d.%d.%d%s Copyright AMD 2016\n", VERSION_MAJOR_MAJOR, VERSION_MAJOR_MINOR, VERSION_MINOR_MAJOR);
    else
    {
        // Keep track of Customer patches from last release to current
        // This is what is shown when you build the exe outside of the automated Build System (such as Jenkins)
        printf("CompressonatorCLI V2.1.2589  SP3 Copyright AMD 2016\n");
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
    printf("Example decompression from compressed image:\n\n");
    printf("CompressonatorCLI.exe  result.dds image.bmp\n\n");
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
            }
        }

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
                            case TDT_ARGB:    swap_Bytes(pInMipLevel->m_pbData,dwWidth,dwHeight,4); break;
                            case TDT_XRGB:    swap_Bytes(pInMipLevel->m_pbData,dwWidth,dwHeight,3); break;
                            default: break;
                        }
            }
        }

}

// Determin if RGB channel to BGA can be done or skipped
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
            return true;    
            break;
            default:
                break;
    }

    return false;
}


bool FormatIsFloat(CMP_Texture InTexture)
{
    switch (InTexture.format)
    {
    case CMP_FORMAT_ARGB_16F :
    case CMP_FORMAT_RG_16F   :
    case CMP_FORMAT_R_16F    :
    case CMP_FORMAT_ARGB_32F :
    case CMP_FORMAT_RGB_32F  :
    case CMP_FORMAT_RG_32F   :
    case CMP_FORMAT_R_32F    :
    case CMP_FORMAT_BC6H     :
                                {
                                        return true;
                                }
                                break;
    default:
                                break;
    }

    return false;
}


bool FormatAreCompatible(CMP_Texture srcTexture, CMP_Texture destTexture)
{
    bool src  = FormatIsFloat(srcTexture);
    bool dest = FormatIsFloat(destTexture);

    return (src == dest);
}


bool GenerateAnalysis(std::string SourceFile, std::string DestFile)
{
    PluginInterface_Analysis *Plugin_Analysis;
    int testpassed = 0;
    Plugin_Analysis = reinterpret_cast<PluginInterface_Analysis *>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        string src_ext = boost::filesystem::extension((SourceFile.c_str()));
        g_CmdPrams.DiffFile = DestFile;
        int lastindex = g_CmdPrams.DiffFile.find_last_of(".");
        g_CmdPrams.DiffFile = g_CmdPrams.DiffFile.substr(0, lastindex);
        g_CmdPrams.DiffFile.append("_diff");
        g_CmdPrams.DiffFile.append(src_ext);

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

//
// Used exclusively by the GUI app 
// ToDo : Remove this code and try to use ProcessCMDLine
MipSet* DecompressMIPSet(MipSet *MipSetIn, bool swizzle = false)
{
    // validate MipSet is Compressed
    if (!MipSetIn->m_compressed) return NULL;

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
    // EXR is saved as CMP_FORMAT_ARGB_32F
    switch (MipSetIn->m_format)
    {
        case CMP_FORMAT_BC6H:
            MipSetOut->m_format            = CMP_FORMAT_ARGB_32F;
            MipSetOut->m_ChannelFormat    = CF_Float32;
            break;
        default:
            MipSetOut->m_format            = CMP_FORMAT_ARGB_8888;
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
                srcTexture.dwWidth        = nMipWidth;
                srcTexture.dwHeight        = nMipHeight;
                srcTexture.dwPitch        = 0;
                srcTexture.format        = MipSetIn->m_format;
                srcTexture.dwDataSize    = CMP_CalculateBufferSize(&srcTexture);
                srcTexture.pData        = pMipData;

                //-----------------------------
                // Uncompressed Destination
                //-----------------------------
                CMP_Texture destTexture;
                destTexture.dwSize        = sizeof(destTexture);
                destTexture.dwWidth        = nMipWidth;
                destTexture.dwHeight    = nMipHeight;
                destTexture.dwPitch        = 0;
                destTexture.format        = MipSetOut->m_format;
                destTexture.dwDataSize    = CMP_CalculateBufferSize(&destTexture);
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
                        PrintInfo("Memory Error(2): Source image\n");
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }

                    if (/*(srcTexture.dwDataSize > destTexture.dwDataSize) ||*/ (IsBadWritePtr(destTexture.pData, destTexture.dwDataSize)))
                    {
                        PrintInfo("Memory Error(2): Destination image must be compatible with source\n");
                        m_CMIPS.FreeMipSet(MipSetOut);
                        delete MipSetOut;
                        MipSetOut = NULL;
                        return NULL;
                    }

                    CMP_ConvertTexture(&srcTexture, &destTexture, &CompressOptions, NULL, NULL, NULL);

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
				PrintInfo("Error: Image Analysis Failed\n");

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
        if (p_userMipSetIn)
        {
            memcpy(&g_MipSetIn, p_userMipSetIn, sizeof(MipSet));
            g_MipSetIn.m_swizzle = KeepSwizzle(destFormat);
            g_MipSetIn.m_pMipLevelTable = p_userMipSetIn->m_pMipLevelTable;
        }

        //----------------
        // Read Input file
        //----------------
        if (g_CmdPrams.showperformance)
            QueryPerformanceCounter(&conversion_loopStartTime);

        if (p_userMipSetIn)
        {
            if (g_MipSetIn.m_swizzle)
            {
                SwizzleMipMap(&g_MipSetIn);
                SwizzledMipSetIn = true;
            }
            Delete_gMipSetIn = false;
        }
        else
        {
            Delete_gMipSetIn = true;

            // ===============================================
            // INPUT IMAGE Swizzling options for DXT formats
            // ===============================================
            g_MipSetIn.m_swizzle = KeepSwizzle(destFormat);

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
                PrintInfo("Error: loading image\n");
                return -1;
            }
        }


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

        //=====================================================
        // Unsupported conversion !
        // ====================================================
        if (SourceFormatIsCompressed && DestinationFormatIsCompressed)
        {
            cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
            PrintInfo("Error: Processing compressed source and compressed destination is not supported\n");
            return -1;
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

                    for(int nFaceOrSlice=0; nFaceOrSlice<MaxFacesOrSlices(&g_MipSetIn, nMipLevel); nFaceOrSlice++)
                    {

                                //=====================
                                // Uncompressed source
                                //======================
                                MipLevel* pInMipLevel   = g_CMIPS->GetMipLevel(&g_MipSetIn, nMipLevel, nFaceOrSlice);
                                srcTexture.dwWidth      = pInMipLevel->m_nWidth;
                                srcTexture.dwHeight     = pInMipLevel->m_nHeight;
                                srcTexture.dwPitch      = 0;
                                srcTexture.format       = srcFormat;
                                srcTexture.dwDataSize   = CMP_CalculateBufferSize(&srcTexture);
                                srcTexture.pData        = pInMipLevel->m_pbData;
                                PrintInfo("Source Texture size = %d Bytes, width = %d px  height = %d px\n", 
                                            srcTexture.dwDataSize,
                                            srcTexture.dwWidth,
                                            srcTexture.dwHeight);

                                //========================
                                // Compressed Destination
                                //========================
                                CMP_Texture destTexture;
                                destTexture.dwSize      = sizeof(destTexture);
                                destTexture.dwWidth     = pInMipLevel->m_nWidth;
                                destTexture.dwHeight    = pInMipLevel->m_nHeight;
                                destTexture.dwPitch     = 0;
                                destTexture.format      = destFormat;
                                destTexture.dwDataSize  = CMP_CalculateBufferSize(&destTexture);
                                
                                if (destTexture.dwDataSize > 0)
                                PrintInfo("Destination Texture size = %d Bytes   Resulting compression ratio = %2.2f:1\n",
                                    destTexture.dwDataSize,
                                    srcTexture.dwDataSize / (float)destTexture.dwDataSize);

                                // Check Both Source and Destination formats are compatible
                                if (!FormatAreCompatible(srcTexture, destTexture))
                                {
                                    PrintInfo("Source and Destination formats are not Compatible\n");
                                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                    return -1;
                                }

                                MipLevel* pOutMipLevel  = g_CMIPS->GetMipLevel(&g_MipSetCmp, nMipLevel, nFaceOrSlice);
                                if (!g_CMIPS->AllocateCompressedMipLevelData(pOutMipLevel, destTexture.dwWidth, destTexture.dwHeight, destTexture.dwDataSize))
                                {
                                    PrintInfo("Memory Error(1): allocating MIPSet compression level data buffer\n");
                                    cleanup(Delete_gMipSetIn, SwizzledMipSetIn);
                                    return -1;
                                }

                                destTexture.pData = pOutMipLevel->m_pbData;
                                g_fProgress = -1;

                                //========================
                                // Process ConvertTexture
                                //========================
                                CMP_ConvertTexture(&srcTexture, &destTexture, &g_CmdPrams.CompressOptions, pFeedbackProc, NULL, NULL);
                        
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
        //                BMP - BMP  with -fd flag(s) 
        //
        //=====================================================
        if (((SourceFormatIsCompressed) && (!DestinationFormatIsCompressed)) || (TranscodeBits) || (MidwayDecompress))
        {
                g_MipSetOut.m_TextureDataType    = TDT_ARGB;

                if (SourceFormatIsCompressed)
                {
                    // BMP is saved as CMP_FORMAT_ARGB_8888
                    // EXR is saved as CMP_FORMAT_ARGB_32F
                    switch (srcFormat)
                    {
                        case CMP_FORMAT_BC6H:
                                destFormat                        = CMP_FORMAT_ARGB_32F;
                                g_MipSetOut.m_ChannelFormat        = CF_Float32;
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
                            destFormat = CMP_FORMAT_ARGB_32F;
                            g_MipSetOut.m_ChannelFormat = CF_Float32;
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

                g_MipSetOut.m_format = destFormat;
                g_MipSetOut.m_isDeCompressed = true;

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
                                PrintInfo("\rProcessing destination MipLevel %2d FaceOrSlice %2d",nMipLevel+1,nFaceOrSlice);
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

                        CMP_ConvertTexture(&srcTexture, &destTexture, &g_CmdPrams.CompressOptions, pFeedbackProc, NULL, NULL);

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
                    PrintInfo("Error: saving image or format is unsupported\n");
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
        PrintInfo(" Done\n");
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

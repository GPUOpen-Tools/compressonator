//
// AMDCompressCLI.cpp
//
//
// Contact Info: Navin Patel @ AMD.com
//
// Initial version - Nov 2014
// Updated code to share with GUI - Sep 19 2015
//
// Code is based on the following Libs
// Standard Library             Ref:http://www.cplusplus.com/reference/
// Standard Template Library Ref:http://en.cppreference.com/w/cpp
//

#ifdef _DEBUG
//#include <vld.h>   Enable to check for code leaks
#endif

// #define USE_AMD_BMP
// #define USE_AMD_PNG

#ifdef _WIN32
#include <windows.h>
#endif

#include "cmdline.h"
#include "PluginManager.h"
#include "TextureIO.h"
#include "Version.h"

#ifdef USE_QT_IMAGELOAD
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>
#endif
// Our Static Plugin Interfaces
#pragma comment(lib, "ASTC.lib")
#pragma comment(lib, "BoxFilter.lib")
#pragma comment(lib, "DDS.lib")
#pragma comment(lib, "EXR.lib")
#pragma comment(lib, "KTX.lib")
#pragma comment(lib, "TGA.lib")
#pragma comment(lib, "IMGAnalysis.lib")

extern void* make_Plugin_ASTC();
extern void* make_Plugin_BoxFilter();
extern void* make_Plugin_DDS();
extern void* make_Plugin_EXR();
extern void* make_Plugin_TGA();
extern void* make_Plugin_KTX();
#ifndef __APPLE__
extern void* make_Plugin_CAnalysis();
#endif

#ifdef USE_AMD_PNG
#pragma comment(lib, "PNG.lib")
extern void* make_Plugin_PNG();
#endif

#ifdef USE_AMD_BMP
#pragma comment(lib, "BMP.lib")
extern void* make_Plugin_BMP();
#endif

extern bool   CompressionCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2);
extern void   LocalPrintF(char* buff);
extern string DefaultDestination(string SourceFile, CMP_FORMAT DestFormat, string DestFileExt);

PluginManager g_pluginManager;
bool          g_bAbortCompression = false;
CMIPS*        g_CMIPS;  // Global MIPS functions shared between app and all IMAGE plugins

void AboutCompressonator()
{
    printf("------------------------------------------------\n");
    // current build release
    if ((VERSION_MINOR_MAJOR > 0))
        printf("CompressonatorCLI V%d.%d.%d Copyright AMD 2018\n", VERSION_MAJOR_MAJOR, VERSION_MAJOR_MINOR, VERSION_MINOR_MAJOR);
    else
    {
        // Keep track of Customer patches from last release to current
        // This is what is shown when you build the exe outside of the automated Build System (such as Jenkins)
        printf("CompressonatorCLI V3.1.0 Copyright AMD 2018\n");
    }
    printf("------------------------------------------------\n");
    printf("\n");
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
#ifdef _WIN32
#ifdef USE_COMPUTE
    printf("-EncodeWith          Compression with HPC or OpenCL\n");
#endif
    printf("-DecodeWith          GPU based decompression using OpenGL, DirectX or Vulkan\n");
#endif
#ifdef USE_MESH_DRACO_EXTENSION
    printf("-draco           Enable draco compression. (only support glTF files)\n");
#ifdef USE_MESH_DRACO_SETTING 
    printf("-dracolvl        Draco compression level (0-10), default=5 , -draco has to be enabled.\n");
    printf("-qpos            Draco quantization bits for position attribute (0-30), default=14. -draco has to be enabled.\n");
    printf("-qtexc           Draco quantization bits for texture coordinates attribute (0-30), default=10. -draco has to be enabled.\n");
    printf("-qnorm           Draco quantization bits for normal attribute (0-30), default=10. -draco has to be enabled.\n");
    printf("-qgen            Draco quantization bits for generic attribute (0-30), default=8. -draco has to be enabled.\n");
#endif
#endif
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
    printf("ETC2_RGBA      Ericsson Texture Compression - Compressed RGB with 8 bit Alpha.\n");
    printf("ETC2_RGBA1     Ericsson Texture Compression - Compressed RGB with 1 bit Alpha.\n");
#ifdef USE_GTC
    printf("GTC            Compressed RGB 8:8:8 format \n");
    printf("               This is a preview version for evaluation: subject to changes\n");
#endif
#ifdef USE_GTC_HDR
    printf("GTCH           High-Dynamic Range  RGB compression format\n");
    printf("               This is a preview version for evaluation: subject to changes\n");
#endif
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
    //printf("-OutputExposure <value>      BC6 only: Sets the resulting exposure of compressed Images\n");
    //printf("                             default is 0.95, lower values produce darker images,\n");
    //printf("                             higher values produce brighter images\n");
    printf("-CompressionSpeed <value>    The trade-off between compression speed & quality\n");
    printf("                             This setting is not used in BC6H and BC7\n");
    printf("-NumThreads <value>          Number of threads to initialize for ASTC,BC6H,BC7\n");
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
#ifdef USE_3DMESH_OPTIMIZE
    printf("-optVCacheSize <value>        Enable vertices optimization with hardware cache size in the value specified. \n");
    printf(
        "                              (Value should be in range 1- no limit as it allows users to simulate hardware cache size to find the most "
        "optimum size).\n");
    printf("                              By default, mesh vertices optimization is enabled with cache size = 16. \n");
    printf("-optVCacheFIFOSize <value>    Enable vertices optimization with hardware cache size(FIFO replacement policy) in the value specified. \n");
    printf(
        "                              (Value should be in range 1- no limit as it allows users to simulate hardware cache size to find the most "
        "optimum size).\n");
    printf("                              By default, mesh vertices optimization with FIFO cache is disabled. \n");
    printf("-optOverdrawACMRThres <value> Enable overdraw optimization with ACMR (average cache miss ratio) threshold value specified. \n");
    printf("                              (Value range 1.00 - 3.00) - default is enabled with ACMR value = 1.05 (i.e. 5 percent worse).\n");
    printf("-optVFetch  <value>           Enable vertices fetch optimization. (Valus is either 0 to disable or 1 to enable.) \n");
    printf("-simplifyMeshLOD <value>      Enable mesh simplification with the LOD (level of details) in the value specified. \n");
    printf(
        "                              (Value should be in range 1- no limit as it allows users to simplify the mesh until the level they desired "
        "for experiment purpose. Higher level means less triangles drawn, less details.)\n");
#endif
    printf("-Analysis <image1> <image2>  Generate analysis metric like SSIM, PSNR values \n");
    printf("                             between 2 images with same size. Analysis_Result.xml file will be generated.\n");
    printf("\n\n");
    printf("-diff_image <image1> <image2> Generate difference between 2 images with same size \n");
    printf(
        "                              A .bmp file will be generated. Please use compressonator GUI to increase the contrast to view the diff "
        "pixels.\n");
    printf("-log                         Logs process information to a process_results.txt file containing\n");
    printf("                             file info, performance data, SSIM, PSNR and MSE. \n");
    printf("-logfile <filename>          Logs process information to a user defined text file\n");
    printf("\n\n");
    printf("-imageprops <image>           Print image properties of image files specifies. \n");
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
    printf("CompressonatorCLI.exe -fd BC7 -ff PNG -fx KTX ./source_dir/ ./dist_dir/\n");
    printf("CompressonatorCLI.exe -fd BC6H image.exr result.dds\n\n");
    printf("Example decompression from compressed image using CPU:\n\n");
    printf("CompressonatorCLI.exe  result.dds image.bmp\n\n");
    printf("Example decompression from compressed image using GPU:\n\n");
    printf("CompressonatorCLI.exe  -UseGPUDecompress result.dds image.bmp\n\n");
    printf("Example compression with decompressed result (Useful for qualitative analysis):\n\n");
    printf("CompressonatorCLI.exe -fd BC7  image.bmp result.bmp\n");
    printf("CompressonatorCLI.exe -fd BC6H image.exr result.exr\n\n");
#ifdef USE_MESH_DRACO_EXTENSION
    printf("Example draco compression usage (support glTF and OBJ file only):\n\n");
    printf("CompressonatorCLI.exe -draco source.gltf dest.gltf\n");
    printf("CompressonatorCLI.exe -draco source.obj dest.drc\n");
    printf("Note: only .obj file produces compressed .drc file. glTF does not produce .drc compressed file.\n");
    printf("Note: You can specify .obj as compressed file format as well, but a new .drc file will be created for this case.\n\n");
#ifdef USE_MESH_DRACO_SETTING 
    printf("Specifies quantization bits settings:\n");
    printf("CompressonatorCLI.exe -draco -dracolvl 7 -qpos 12 -qtexc 8 -qnorm 8 source.gltf dest.gltf\n\n");
#endif
    printf("Example draco decompression usage (support glTF and drc file only):\n\n");
    printf("CompressonatorCLI.exe source.gltf dest.gltf\n");
    printf("CompressonatorCLI.exe source.drc dest.obj\n");
#endif
#ifdef USE_3DMESH_OPTIMIZE
    printf("\n\n");
    printf("Example mesh optimization usage(support glTF and OBJ file only):\n");
    printf(
        "Using default settings : Optimize vertices with cache size = 16; Optimize overdraw with ACMR Threshold = 1.05; Optimize vertices fetch. "
        "\n\n");
    printf("CompressonatorCLI.exe -meshopt source.gltf dest.gltf\n");
    printf("CompressonatorCLI.exe -meshopt source.obj dest.obj\n\n");
    printf("Specifies settings :\n\n");
    printf("CompressonatorCLI.exe -meshopt -optVCacheSize  32 -optOverdrawACMRThres  1.03 -optVFetch 0 source.gltf dest.gltf\n");
#endif
}

bool ProgressCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    return CompressionCallback(fProgress, pUser1, pUser2);
}



//----------------------------------------------------------
// Compressonator Lib compute codec override
//----------------------------------------------------------
#ifdef USE_GTC
PluginInterface_Compute *g_plugin_ComputeGTC = NULL;
HPC_Compress  *g_CompressGTCCodec = NULL;
extern void(*GTC_DecompressBlock)(void *out, void *in);
extern void(*GTC_CompressBlock)(void * srcblock, void *dest, void *blockoptions);

void g_GTC_DecompressBlock(void *in, void *out)
{
    if (g_CompressGTCCodec)
        g_CompressGTCCodec->Decompress(in, out);
}

void g_GTC_CompressBlock(void *in, void *out, void *blockoptions)
{
    if (g_CompressGTCCodec)
        g_CompressGTCCodec->Compress(in, out, blockoptions);
}
#endif


int main(int argc, char* argv[])
{
#ifdef USE_QT_IMAGELOAD
    QCoreApplication app(argc, argv);
#endif

    g_CMIPS = new CMIPS;

    g_pluginManager.registerStaticPlugin("IMAGE", "ASTC", (void*)make_Plugin_ASTC);
    g_pluginManager.registerStaticPlugin("IMAGE", "DDS", (void*)make_Plugin_DDS);
    g_pluginManager.registerStaticPlugin("IMAGE", "EXR", (void*)make_Plugin_EXR);
    g_pluginManager.registerStaticPlugin("IMAGE", "TGA", (void*)make_Plugin_TGA);  // Use for load only, Qt will be used for Save
    g_pluginManager.registerStaticPlugin("IMAGE", "KTX", (void*)make_Plugin_KTX);
#ifndef __APPLE__
    g_pluginManager.registerStaticPlugin("IMAGE", "ANALYSIS", (void*)make_Plugin_CAnalysis);
#endif

#ifdef USE_AMD_PNG
    g_pluginManager.registerStaticPlugin("IMAGE", "PNG", (void*)make_Plugin_PNG);
#endif

#ifdef USE_AMD_BMP
    g_pluginManager.registerStaticPlugin("IMAGE", "BMP", (void*)make_Plugin_BMP);
#endif

    g_pluginManager.registerStaticPlugin("FILTERS", "BOXFILTER", (void*)make_Plugin_BoxFilter);
    g_pluginManager.getPluginList("\\Plugins");

#ifdef USE_QT_IMAGELOAD
    QString dirPath = QCoreApplication::applicationDirPath();
    QCoreApplication::addLibraryPath(dirPath + "./plugins/imageformats");
#endif

    // Check if print status line has been assigned
    // if not get it a default to printf
    if (PrintStatusLine == NULL)
        PrintStatusLine = &LocalPrintF;

    //----------------------------------
    // Process user command line parameters
    //----------------------------------
    if (argc > 1)
    {
        bool ParseOk = ParseParams(argc, argv);
        if (!ParseOk)
        {
            delete g_CMIPS;
            return -1;
        }

        if (g_CmdPrams.SourceFile.length() == 0)
        {
            delete g_CMIPS;
            return -2;
        }

        if (!g_CmdPrams.imageprops && (g_CmdPrams.DestFile.length() == 0))
        {
            // Try to patch the detination file
            if ((g_CmdPrams.DestFile.length() == 0) && (g_CmdPrams.SourceFile.length() > 0))
            {
                g_CmdPrams.DestFile = DefaultDestination(g_CmdPrams.SourceFile, g_CmdPrams.DestFormat, g_CmdPrams.FileOutExt);
                printf("Destination Texture file was not supplied: Defaulting to %s\n", g_CmdPrams.DestFile.c_str());
            }
            else
            {
                delete g_CMIPS;
                return (-3);
            }
        }

#ifdef USE_GTC
        //---------------------------------------
        // attempt to load GTC Codec
        //---------------------------------------
        g_plugin_ComputeGTC = reinterpret_cast<PluginInterface_Compute *>(g_pluginManager.GetPlugin("COMPUTE", "GTC"));
        // Found GTC Codec
        if (g_plugin_ComputeGTC)
        {
            //-------------------------------
            // create the compression  Codec
            //-------------------------------
            g_CompressGTCCodec = (HPC_Compress*)g_plugin_ComputeGTC->TC_Create();

            //------------------------------------------------------------
            // Assign compressonator lib GTC codec to Compute GTC Codec
            //------------------------------------------------------------
            if (g_CompressGTCCodec)
            {
                GTC_CompressBlock = g_GTC_CompressBlock;
                GTC_DecompressBlock = g_GTC_DecompressBlock;
            }
        }
#endif

        int ret = ProcessCMDLine(&CompressionCallback, NULL);

        delete g_CMIPS;

#ifdef USE_GTC
        //------------------------------------------
        // Cleanup the compute GTC compression Codec
        //------------------------------------------
        if (g_plugin_ComputeGTC)
        {
            if (g_CompressGTCCodec)
                g_plugin_ComputeGTC->TC_Destroy(g_CompressGTCCodec);
            delete  g_plugin_ComputeGTC;
        }
#endif

        return ret;
    }
    else
    {
        PrintUsage();
        delete g_CMIPS;
        return 0;
    }
}

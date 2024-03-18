
# Compressonator
[![CMake](https://github.com/GPUOpen-Tools/compressonator/actions/workflows/cmake.yml/badge.svg)](https://github.com/GPUOpen-Tools/compressonator/actions/workflows/cmake.yml)
![download](https://img.shields.io/github/downloads/GPUOpen-Tools/Compressonator/total.svg)
![download](https://img.shields.io/github/downloads/GPUOpen-Tools/Compressonator/V4.5.52/total.svg)

*Download the latest revision for changes that have been made since the last major release by clicking the CMake button above or the link [here](https://github.com/GPUOpen-Tools/compressonator/actions/workflows/cmake.yml). Currently, only Compressonator Framework and Compressonator CLI are built every revision.*

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture assets or model mesh optimizations and easily visualize the quality impact of various compression and rendering technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator supports Microsoft Windows® and Linux builds.

For more details goto the online Compressonator Documents: http://compressonator.readthedocs.io/en/latest/ 

## Build System Updates ##
The code is undergoing a build setup update. This notice will be removed when the changes are completed!
Currently: To use the sln builds run build\fetch_dependencies.py to fetch required external lib dependencies into a Common folder
above this repository.


Get Prebuilt Binaries and Installer here:
---------------------------------------------------
<div>
  <a href="https://github.com/GPUOpen-Tools/Compressonator/releases/latest/"><img src="http://gpuopen-librariesandsdks.github.io/media/latest-release-button.svg" alt="Latest release" title="Latest release"></a>
</div>

To build the source files follow the instructions in http://compressonator.readthedocs.io/en/latest/build_from_source/build_instructions.html

## CMake Build Configuration ##
As of v4.2, The cmake command line options have settings to build specific libs and applications

Examples: Generating Visual Studio Solution File

```c++
Enable building all
    cmake -G "Visual Studio 15 2017 Win64"  
    
Disable all builds except external libs, minimal cmake base setup     
    cmake -DOPTION_ENABLE_ALL_APPS=OFF -G "Visual Studio 15 2017 Win64"
    
Enable only CLI app build    
    cmake -DOPTION_ENABLE_ALL_APPS=OFF -DOPTION_BUILD_APPS_CMP_CLI=ON -G "Visual Studio 15 2017 Win64"
```

For more details reference the CMakeList file on the root folder.

## Style and Format Change ##

The source code of this product is being reformatted to follow the Google C++ Style Guide https://google.github.io/styleguide/cppguide.html

In the interim you may encounter a mix of both an older C++ coding style, as well as the newer Google C++ Style.

Please refer to the _clang-format file in the root directory of the product for additional style information.


## Compressonator Core
Provides block level API access to updated performance and quality driven BCn codecs. The library is designed to be a small self-contained, cross-platform, and linkable library for user applications.

Example usage is shown as below to compress and decompress a single 4x4 image block using BC1 encoder

```c++

// To use Compressonator Core "C" interfaces, just include
// a single header file and CMP_Core lib into  your projects

#include "CMP_Core.h"

// Compress a sample image shape0 which is a 4x4 RGBA_8888 block.
// Users can use a pointer to any sized image buffers to reference
// a 4x4 block by supplying a stride offset for the next row.
// Optional BC1 settings is set to null in this example

unsigned char shape0_RGBA[64] = { filled with image source data as RGBA ...};

// cmpBuffer is a byte array of 8 byte to hold the compressed results.
unsigned char cmpBuffer[8]   = { 0 };

// Compress the source into cmpBuffer
CompressBlockBC1(shape0_RGBA, 16, cmpBuffer,null);

// Example to decompress comBuffer back to a RGBA_8888 4x4 image block
unsigned char imgBuffer[64] = { 0 };
DecompressBlockBC1(cmpBuffer,imgBuffer,null)

```

## Compressonator Framework

Includes Compressonator core with interfaces for multi-threading, mipmap generation, file access of images and HPC pipeline interfaces.

**Example Mip Level Processing using CPU**

```c++

// To use Compressonator Framework "C" interfaces, just include
// a single header file and CMP_Framework lib into  your projects

#include "compressonator.h"

 //--------------------------
 // Init frameworks
 // plugin and IO interfaces
 //--------------------------
 CMP_InitFramework();

//---------------
// Load the image
//---------------
CMP_MipSet MipSetIn;
memset(&MipSetIn, 0, sizeof(CMP_MipSet));
cmp_status = CMP_LoadTexture(pszSourceFile, &MipSetIn);
if (cmp_status != CMP_OK) {
    std::printf("Error %d: Loading source file!\n",cmp_status);
    return -1;
}

//----------------------------------------------------------------------
// generate mipmap level for the source image, if not already generated
//----------------------------------------------------------------------

if (MipSetIn.m_nMipLevels <= 1)
{
    CMP_INT requestLevel = 10; // Request 10 miplevels for the source image

    //------------------------------------------------------------------------
    // Checks what the minimum image size will be for the requested mip levels
    // if the request is too large, a adjusted minimum size will be returned
    //------------------------------------------------------------------------
    CMP_INT nMinSize = CMP_CalcMinMipSize(MipSetIn.m_nHeight, MipSetIn.m_nWidth, 10);

    //--------------------------------------------------------------
    // now that the minimum size is known, generate the miplevels
    // users can set any requested minumum size to use. The correct
    // miplevels will be set acordingly.
    //--------------------------------------------------------------
    CMP_GenerateMIPLevels(&MipSetIn, nMinSize);
}

//==========================
// Set Compression Options
//==========================
KernelOptions   kernel_options;
memset(&kernel_options, 0, sizeof(KernelOptions));

kernel_options.format   = destFormat;   // Set the format to process
kernel_options.fquality = fQuality;     // Set the quality of the result
kernel_options.threads  = 0;            // Auto setting

//=====================================================
// example of using BC1 encoder options 
// kernel_options.bc15 is valid for BC1 to BC5 formats
//=====================================================
if (destFormat == CMP_FORMAT_BC1)
{
    // Enable punch through alpha setting
    kernel_options.bc15.useAlphaThreshold = true;
    kernel_options.bc15.alphaThreshold    = 128;

    // Enable setting channel weights
    kernel_options.bc15.useChannelWeights = true;
    kernel_options.bc15.channelWeights[0] = 0.3086f;
    kernel_options.bc15.channelWeights[1] = 0.6094f;
    kernel_options.bc15.channelWeights[2] = 0.0820f;
}

//--------------------------------------------------------------
// Setup a results buffer for the processed file,
// the content will be set after the source texture is processed
// in the call to CMP_ProcessTexture()
//--------------------------------------------------------------
CMP_MipSet MipSetCmp;
memset(&MipSetCmp, 0, sizeof(CMP_MipSet));

//===============================================
// Compress the texture using Framework Lib
//===============================================
cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, CompressionCallback);
if (cmp_status != CMP_OK) {
  ...
}

//----------------------------------------------------------------
// Save the result into a DDS file
//----------------------------------------------------------------
cmp_status = CMP_SaveTexture(pszDestFile, &MipSetCmp);

CMP_FreeMipSet(&MipSetIn);
CMP_FreeMipSet(&MipSetCmp);

```

**Example GPU based processing using OpenCL**

```c++

// Note: Only MD x64 build is used for GPU processing
// SDK files required for application:
//     compressonator.h
//     CMP_Framework_xx.lib  For static libs xx is either MD or MDd, 
//                      When using DLL's make sure the  CMP_Framework_xx_DLL.dll is in exe path
//
// File(s) required to run with the built application
//
// Using OpenCL (OCL) 
//     CMP_GPU_OCL_MD_DLL.dll    or CMP_GPU_OCL_MDd_DLL.dll
//     Encode Kernel files in plugins/compute folder
//     BC1_Encode_Kernel.cpp
//     BC1_Encode_Kernel.h
//     BCn_Common_kernel.h
//     Common_Def.h
//
// Using DirectX (DXC) 
//     CMP_GPU_DXC_MD_DLL.dll    or CMP_GPU_DXC_MDd_DLL.dll
//     Encode Kernel files in plugins/compute folder
//     BC1_Encode_Kernel.hlsl
//     BCn_Common_kernel.h
//     Common_Def.h

#include "compressonator.h"

CMP_FORMAT      destFormat = CMP_FORMAT_BC1;

//---------------
// Load the image
//---------------
CMP_MipSet MipSetIn;
memset(&MipSetIn, 0, sizeof(CMP_MipSet));
if (CMP_LoadTexture(pszSourceFile, &MipSetIn) != CMP_OK) {
    std::printf("Error: Loading source file!\n");
    return -1;
  } 

 //-----------------------------------------------------
 // when using GPU: The texture must have width and height as a multiple of 4
 // Check texture for width and height
 //-----------------------------------------------------
 if ((MipSetIn.m_nWidth % 4) > 0 || (MipSetIn.m_nHeight % 4) > 0) {
    std::printf("Error: Texture width and height must be multiple of 4\n");
    return -1;
 }
    
//----------------------------------------------------------------------------------------------------------
// Set the target compression format and the host framework to use
// For this example OpenCL is been used
//-----------------------------------------------------------------------------------------------------------
KernelOptions   kernel_options;
memset(&kernel_options, 0, sizeof(KernelOptions));

kernel_options.encodeWith = CMP_GPU_OCL;         // Using OpenCL GPU Encoder, can replace with DXC for DirectX
kernel_options.format     = destFormat;          // Set the format to process
kernel_options.fquality   = fQuality;            // Set the quality of the result

//--------------------------------------------------------------
// Setup a results buffer for the processed file,
// the content will be set after the source texture is processed
// in the call to CMP_ProcessTexture()
//--------------------------------------------------------------
CMP_MipSet MipSetCmp;
memset(&MipSetCmp, 0, sizeof(CMP_MipSet));

//===============================================
// Compress the texture using Framework Lib
//===============================================
cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, CompressionCallback);
if (cmp_status != CMP_OK) {
  ...
}

//----------------------------------------------------------------
// Save the result into a DDS file
//----------------------------------------------------------------
cmp_status = CMP_SaveTexture(pszDestFile, &MipSetCmp);

CMP_FreeMipSet(&MipSetIn);
CMP_FreeMipSet(&MipSetCmp);

```

## Compressonator SDK

Compressonator SDK supported codecs includes BC1-BC7/DXTC, ETC1, ETC2, ASTC, ATC, ATI1N, ATI2N, all available in a single library.

With the new SDK installation, several example applications with source code are provided that demonstrate how easy it is to add texture compression to your own applications using either "High Level" or "Block Level" APIs.

A simple thread safe interface can compress, decompress and transcode any image as required

`CMP_ConvertTexture(CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture,...);`

**For Example:**

```c++

// To use Compressonator's portable "C" interfaces, just include
// a single header file and Compresonator.lib into  your projects

#include "Compressonator.h"
...

//==========================
// Load Source Texture
//==========================
CMP_Texture srcTexture;
// note that LoadDDSFile function is a utils function to initialize the source CMP_Texture
// you can also initialize the source CMP_Texture the same way as initialize destination CMP_Texture
if (!LoadDDSFile(pszSourceFile, srcTexture))
{
  ...
}

//===================================
// Initialize Compressed Destination
//===================================
CMP_Texture destTexture;
destTexture.dwSize     = sizeof(destTexture);
destTexture.dwWidth    = srcTexture.dwWidth;
destTexture.dwHeight   = srcTexture.dwHeight;
destTexture.dwPitch    = 0;
destTexture.format     = CMP_FORMAT_BC6H;
destTexture.dwDataSize = CMP_CalculateBufferSize(&destTexture);
destTexture.pData      = (CMP_BYTE*)malloc(destTexture.dwDataSize);

//==========================
// Set Compression Options
//==========================
CMP_CompressOptions options = {0};
options.dwSize       = sizeof(options);
options.fquality     = 0.05f;
options.dwnumThreads = 8;

//==========================
// Compress Texture
//==========================
CMP_ERROR   cmp_status;
cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, &options, &CompressionCallback, NULL, NULL);
if (cmp_status != CMP_OK)
{
  ...
}

//==========================
// Save Compressed Testure
//==========================
SaveDDSFile(pszDestFile, destTexture))

free(srcTexture.pData);
free(destTexture.pData);

```


## Compressonator CLI
Command line application that can be batch processed and supports:

- Texture Compression, Decompression, Format Transcoding.
- 3D Model Optimization and Mesh Compression.
- Performance and Analysis Logs such as SSIM, MSE, PSNR.
- MIP Maps, Image Differences, etc. ...

```
C:\>CompressonatorCLI -fd BC7 .\images .results
```
```
C:\>CompressonatorCLI -log -fd BC7 .\images\ruby.png ruby_bc7.dds
```
```
CompressonatorCLI Performance Log v1.0

Source        : .\images\ruby.png, Height 416, Wideth 576, Size 0.936 MBytes
Destination   : ruby_bc7.dds
Using         : CPU
Quality       : 0.05
Processed to  : BC7        with  1 iteration(s) in 1.422 seconds
MSE           : 0.78
PSNR          : 49.2
SSIM          : 0.9978
Total time    : 1.432 seconds

--------------
```


## Compressonator GUI
Comprehensive graphical application that can be used to visualize Images and 3D Models, with support for:

- Texture Compression, Decompression, Format Transcoding.
- 3D Model Optimization and Mesh Compression.
- Multiple Image and 3D Model Views.
- MIP Maps, Differences, Analysis, etc. ...

![screenshot 1](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/gui_tool/user_guide/media/image51.png)

**glTF 2.0 Model Render View**

![screenshot 2](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/gui_tool/user_guide/media/image96.png)


## Contributors

Compressonator's GitHub repository (http://github.com/GPUOpen-Tools/Compressonator) is moderated by Advanced Micro Devices, Inc. as part of the GPUOpen initiative.

AMD encourages any and all contributors to submit changes, features, and bug fixes via Git pull requests to this repository.

Users are also encouraged to submit issues and feature requests via the repository's issue tracker.

 


# Compressonator

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture assets or model mesh optimizations and easily visualize the quality impact of various compression and rendering technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator supports Microsoft Windows®, Linux and Mac builds.

For more details goto the online Compressonator Documents: http://compressonator.readthedocs.io/en/latest/ 

Get Prebuilt Binaries and Installer here:
---------------------------------------------------
https://github.com/GPUOpen-Tools/Compressonator/releases

To build the source files follow the instructions in http://compressonator.readthedocs.io/en/latest/build_from_source/build_instructions.html

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

**Example Mip Level Processing**

```c++

// To use Compressonator Framework "C" interfaces, just include
// a single header file and CMP_Framework lib into  your projects

#include "CMP_Framework.h"

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
    // if the request is too large, a adjusted minimum size will be returns
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
KernalOptions   kernel_options;
memset(&kernel_options, 0, sizeof(KernalOptions));

kernel_options.format   = destFormat;   // Set the format to process
kernel_options.fquality = fQuality;     // Set the quality of the result
kernel_options.threads  = 0;            // Auto setting

//--------------------------------------------------------------
// Setup a results buffer for the processed file,
// the content will be set after the source texture is processed
// in the call to CMP_ConvertMipTexture()
//--------------------------------------------------------------
CMP_MipSet MipSetCmp;
memset(&MipSetCmp, 0, sizeof(CMP_MipSet));

//===============================================
// Compress the texture using Compressonator Lib
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

![screenshot cli1](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/gui_tool/user_guide/media/image129.png)

![screenshot cli2](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/gui_tool/user_guide/media/image127.png)

![screenshot cli3](https://github.com/GPUOpen-Tools/Compressonator/blob/master/docs/source/gui_tool/user_guide/media/image128.png)


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



# Compressonator

Compressonator is a set of tools to allow artists and developers to more easily create compressed texture assets or model mesh optimizations and easily visualize the quality impact of various compression and rendering technologies.  It consists of a GUI application, a command line application and an SDK for easy integration into a developer tool chain.

Compressonator supports Microsoft Windows®, Linux and Mac builds.

For more details goto the online Compressonator Documents: http://compressonator.readthedocs.io/en/latest/ 

Get Prebuilt Binaries and Installer here:
---------------------------------------------------
https://github.com/GPUOpen-Tools/Compressonator/releases

To build the source files follow the instructions in http://compressonator.readthedocs.io/en/latest/build_from_source/build_instructions.html


## Compressonator SDK

** NOTE: The master SDK source has a DXTC swizzle fix, make sure any applications using source (v3.1 and older) are adjusted accordingly, BC6H and BC7 are not affected by this change, for details ref: Closed Issue #59** 

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


Processing of BC7 and BC6H 4x4 pixel blocks is also provided in the SDK:

`CMP_EncodeBC7Block(BC7BlockEncoder* encoder  , double in[BC_BLOCK_PIXELS][BC_COMPONENT_COUNT], CMP_BYTE* out);`
`CMP_EncodeBC6HBlock(BC6HBlockEncoder* encoder, CMP_FLOAT  in[BC_BLOCK_PIXELS][BC_COMPONENT_COUNT], CMP_BYTE* out);`

*Reference example3 for details on using BC7*


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



//=====================================================================
// Copyright (c) 2007-2016    Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006    ATI Technologies Inc.
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
//=====================================================================

#ifndef CMP_FRAMEWORK_H
#define CMP_FRAMEWORK_H

#include <stdint.h>
#include <vector>

typedef int                     CMP_INT;
typedef unsigned int            CMP_UINT;
typedef bool                    CMP_BOOL;
typedef void                    CMP_VOID;
typedef float                   CMP_FLOAT;
typedef unsigned char           CMP_BYTE;
typedef std::uint16_t           CMP_WORD;
typedef std::uint32_t           CMP_DWORD;
typedef short                   CMP_HALFSHORT;
typedef std::vector<uint8_t>    CMP_VEC8;
typedef size_t                  CMP_DWORD_PTR;

#if defined(WIN32) || defined(_WIN64)
#define CMP_API __cdecl
#else
#define CMP_API
#endif

//===================================
// Common with Compressonator SDK
//===================================
#ifndef CMP_FORMAT
typedef enum {
    CMP_FORMAT_Unknown = 0,         // Undefined texture format.

    // Channel Component formats --------------------------------------------------------------------------------
    CMP_FORMAT_ARGB_8888,         // ARGB format with 8-bit fixed channels.
    CMP_FORMAT_ABGR_8888,         // ABGR format with 8-bit fixed channels.
    CMP_FORMAT_RGBA_8888,         // RGBA format with 8-bit fixed channels.
    CMP_FORMAT_BGRA_8888,         // BGRA format with 8-bit fixed channels.
    CMP_FORMAT_RGB_888,         // RGB format with 8-bit fixed channels.
    CMP_FORMAT_BGR_888,         // BGR format with 8-bit fixed channels.
    CMP_FORMAT_RG_8,         // Two component format with 8-bit fixed channels.
    CMP_FORMAT_R_8,         // Single component format with 8-bit fixed channels.
    CMP_FORMAT_ARGB_2101010,         // ARGB format with 10-bit fixed channels for color & a 2-bit fixed channel for alpha.
    CMP_FORMAT_ARGB_16,         // ARGB format with 16-bit fixed channels.
    CMP_FORMAT_ABGR_16,         // ABGR format with 16-bit fixed channels.
    CMP_FORMAT_RGBA_16,         // RGBA format with 16-bit fixed channels.
    CMP_FORMAT_BGRA_16,         // BGRA format with 16-bit fixed channels.
    CMP_FORMAT_RG_16,         // Two component format with 16-bit fixed channels.
    CMP_FORMAT_R_16,         // Single component format with 16-bit fixed channels.
    CMP_FORMAT_RGBE_32F,         // RGB format with 9-bit floating point each channel and shared 5 bit exponent
    CMP_FORMAT_ARGB_16F,         // ARGB format with 16-bit floating-point channels.
    CMP_FORMAT_ABGR_16F,         // ABGR format with 16-bit floating-point channels.
    CMP_FORMAT_RGBA_16F,         // RGBA format with 16-bit floating-point channels.
    CMP_FORMAT_BGRA_16F,         // BGRA format with 16-bit floating-point channels.
    CMP_FORMAT_RG_16F,         // Two component format with 16-bit floating-point channels.
    CMP_FORMAT_R_16F,         // Single component with 16-bit floating-point channels.
    CMP_FORMAT_ARGB_32F,         // ARGB format with 32-bit floating-point channels.
    CMP_FORMAT_ABGR_32F,         // ABGR format with 32-bit floating-point channels.
    CMP_FORMAT_RGBA_32F,         // RGBA format with 32-bit floating-point channels.
    CMP_FORMAT_BGRA_32F,         // BGRA format with 32-bit floating-point channels.
    CMP_FORMAT_RGB_32F,         // RGB format with 32-bit floating-point channels.
    CMP_FORMAT_BGR_32F,         // BGR format with 32-bit floating-point channels.
    CMP_FORMAT_RG_32F,         // Two component format with 32-bit floating-point channels.
    CMP_FORMAT_R_32F,         // Single component with 32-bit floating-point channels.

    // Compression formats -----------------------------------------------------------------------------------
    CMP_FORMAT_ASTC, // ASTC (Adaptive Scalable Texture Compression) open texture compression standard
    CMP_FORMAT_ATI1N, // Single component compression format using the same technique as DXT5 alpha. Four bits per pixel.
    CMP_FORMAT_ATI2N, //     Two component compression format using the same technique as DXT5 alpha. Designed for compression of tangent space normal maps. Eight bits per pixel.
    CMP_FORMAT_ATI2N_XY, //    Two component compression format using the same technique as DXT5 alpha. The same as ATI2N but with the channels swizzled. Eight bits per pixel.
    CMP_FORMAT_ATI2N_DXT5, //    ATI2N like format using DXT5. Intended for use on GPUs that do not natively support ATI2N. Eight bits per pixel.
    CMP_FORMAT_ATC_RGB, // CMP - a compressed RGB format.
    CMP_FORMAT_ATC_RGBA_Explicit, // CMP - a compressed ARGB format with explicit alpha.
    CMP_FORMAT_ATC_RGBA_Interpolated, // CMP - a compressed ARGB format with interpolated alpha.
    CMP_FORMAT_BC1, // A four component opaque (or 1-bit alpha) compressed texture format for Microsoft DirectX10. Identical to DXT1.  Four bits per pixel.
    CMP_FORMAT_BC2, // A four component compressed texture format with explicit alpha for Microsoft DirectX10. Identical to DXT3. Eight bits per pixel.
    CMP_FORMAT_BC3, // A four component compressed texture format with interpolated alpha for Microsoft DirectX10. Identical to DXT5. Eight bits per pixel.
    CMP_FORMAT_BC4, // A single component compressed texture format for Microsoft DirectX10. Identical to ATI1N. Four bits per pixel.
    CMP_FORMAT_BC5, // A two component compressed texture format for Microsoft DirectX10. Identical to ATI2N_XY. Eight bits per pixel.
    CMP_FORMAT_BC6H, // BC6H compressed texture format (UF)
    CMP_FORMAT_BC6H_SF, // BC6H compressed texture format (SF)
    CMP_FORMAT_BC7, // BC7  compressed texture format
    CMP_FORMAT_DXT1, // An DXTC compressed texture matopaque (or 1-bit alpha). Four bits per pixel.
    CMP_FORMAT_DXT3, //   DXTC compressed texture format with explicit alpha. Eight bits per pixel.
    CMP_FORMAT_DXT5, //   DXTC compressed texture format with interpolated alpha. Eight bits per pixel.
    CMP_FORMAT_DXT5_xGBR, // DXT5 with the red component swizzled into the alpha channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_RxBG, // swizzled DXT5 format with the green component swizzled into the alpha channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_RBxG, // swizzled DXT5 format with the green component swizzled into the alpha channel & the blue component swizzled into the green channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_xRBG, // swizzled DXT5 format with the green component swizzled into the alpha channel & the red component swizzled into the green channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_RGxB, // swizzled DXT5 format with the blue component swizzled into the alpha channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_xGxR, // two-component swizzled DXT5 format with the red component swizzled into the alpha channel & the green component in the green channel. Eight bits per pixel.
    CMP_FORMAT_ETC_RGB, // ETC   GL_COMPRESSED_RGB8_ETC2  backward compatible
    CMP_FORMAT_ETC2_RGB, // ETC2  GL_COMPRESSED_RGB8_ETC2
    CMP_FORMAT_ETC2_SRGB, // ETC2  GL_COMPRESSED_SRGB8_ETC2
    CMP_FORMAT_ETC2_RGBA, // ETC2  GL_COMPRESSED_RGBA8_ETC2_EAC
    CMP_FORMAT_ETC2_RGBA1, // ETC2  GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    CMP_FORMAT_ETC2_SRGBA, // ETC2  GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    CMP_FORMAT_ETC2_SRGBA1, // ETC2  GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    CMP_FORMAT_PVRTC,

    // Transcoder formats - ------------------------------------------------------------
    CMP_FORMAT_GTC,        ///< GTC   Fast Gradient Texture Compressor
    CMP_FORMAT_BASIS,      ///< BASIS compression

    // End of list
    CMP_FORMAT_MAX = CMP_FORMAT_BASIS
} CMP_FORMAT;
#endif

/// Compress error codes
#ifndef CMP_ERROR
typedef enum {
    CMP_OK = 0,                            // Ok.
    CMP_ABORTED,                           // The conversion was aborted.
    CMP_ERR_INVALID_SOURCE_TEXTURE,        // The source texture is invalid.
    CMP_ERR_INVALID_DEST_TEXTURE,          // The destination texture is invalid.
    CMP_ERR_UNSUPPORTED_SOURCE_FORMAT,     // The source format is not a supported format.
    CMP_ERR_UNSUPPORTED_DEST_FORMAT,       // The destination format is not a supported format.
    CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE,   // The gpu hardware is not supported.
    CMP_ERR_UNSUPPORTED_GPU_BASIS_DECODE,  // The gpu hardware is not supported.
    CMP_ERR_SIZE_MISMATCH,                 // The source and destination texture sizes do not match.
    CMP_ERR_UNABLE_TO_INIT_CODEC,          // Compressonator was unable to initialize the codec needed for conversion.
    CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB,  // GPU_Decode Lib was unable to initialize the codec needed for decompression .
    CMP_ERR_UNABLE_TO_INIT_COMPUTELIB,     // Compute Lib was unable to initialize the codec needed for compression.
    CMP_ERR_CMP_DESTINATION,               // Error in compressing destination texture
    CMP_ERR_MEM_ALLOC_FOR_MIPSET,          // Memory Error: allocating MIPSet compression level data buffer
    CMP_ERR_UNKNOWN_DESTINATION_FORMAT,    // The destination Codec Type is unknown! In SDK refer to GetCodecType()
    CMP_ERR_FAILED_HOST_SETUP,             // Failed to setup Host for processing
    CMP_ERR_PLUGIN_FILE_NOT_FOUND,         // The required plugin library was not found
    CMP_ERR_UNABLE_TO_LOAD_FILE,           // The requested file was not loaded
    CMP_ERR_UNABLE_TO_CREATE_ENCODER,      // Request to create an encoder failed
    CMP_ERR_UNABLE_TO_LOAD_ENCODER,        // Unable to load an encode library
    CMP_ERR_NOSHADER_CODE_DEFINED,         // No shader code is available for the requested framework
    CMP_ERR_GPU_DOESNOT_SUPPORT_COMPUTE,   // The GPU device selected does not support compute
    CMP_ERR_GENERIC                        // An unknown error occurred.
} CMP_ERROR;
#endif

//===================================
// Definitions for CMP MipSet 
//===================================

/// The format of data in the channels of texture.
typedef enum {
    CF_8bit = 0,  // 8-bit integer data.
    CF_Float16 = 1,  // 16-bit float data.
    CF_Float32 = 2,  // 32-bit float data.
    CF_Compressed = 3,  // Compressed data.
    CF_16bit = 4,  // 16-bit integer data.
    CF_2101010 = 5,  // 10-bit integer data in the color channels & 2-bit integer data in the alpha channel.
    CF_32bit = 6,  // 32-bit integer data.
    CF_Float9995E = 7,  // 32-bit partial precision float.
    CF_YUV_420 = 8,  // YUV Chroma formats
    CF_YUV_422 = 9,  // YUV Chroma formats
    CF_YUV_444 = 10, // YUV Chroma formats
    CF_YUV_4444 = 11, // YUV Chroma formats
} CMP_ChannelFormat;

typedef CMP_ChannelFormat   ChannelFormat;

/// The type of data the texture represents.
typedef enum {
    TDT_XRGB = 0,  // An RGB texture padded to DWORD width.
    TDT_ARGB = 1,  // An ARGB texture.
    TDT_NORMAL_MAP = 2,  // A normal map.
    TDT_R = 3,  // A single component texture.
    TDT_RG = 4,  // A two component texture.
    TDT_YUV_SD = 5,  // An YUB Standard Definition texture.
    TDT_YUV_HD = 6,  // An YUB High Definition texture.
} CMP_TextureDataType;

typedef CMP_TextureDataType TextureDataType;

/// The type of the texture.
typedef enum {
    TT_2D = 0,  // A regular 2D texture. data stored linearly (rgba,rgba,...rgba)
    TT_CubeMap = 1,  // A cubemap texture.
    TT_VolumeTexture = 2,  // A volume texture.
    TT__2D_Block = 3,  // 2D texture data stored as [Height][Width] blocks as individual channels using cmp_rgb_t or cmp_yuv_t
    TT_Unknown = 4,  // Unknown type of texture : No data is stored for this type
} CMP_TextureType;

typedef CMP_TextureType TextureType;

typedef struct {
    union {
        CMP_BYTE  rgba[4];  ///< The color as an array of components.
        CMP_DWORD asDword;  ///< The color as a DWORD.
    };
} CMP_COLOR;

/// A MipLevel is the fundamental unit for containing texture data.
/// \remarks
/// One logical mip level can be composed of many MipLevels, see the documentation of MipSet for explanation.
/// \sa \link TC_AppAllocateMipLevelData() TC_AppAllocateMipLevelData \endlink,
/// \link TC_AppAllocateCompressedMipLevelData() TC_AppAllocateCompressedMipLevelData \endlink,
/// \link MipSet \endlink
typedef struct {
    CMP_INT         m_nWidth;         ///< Width of the data in pixels.
    CMP_INT         m_nHeight;        ///< Height of the data in pixels.
    CMP_DWORD       m_dwLinearSize;   ///< Size of the data in bytes.
    union {
        CMP_BYTE*       m_pbData;         ///< pointer unsigned 8  bit.data blocks
        CMP_WORD*       m_pwData;         ///< pointer unsigned 16 bit.data blocks
        CMP_COLOR*      m_pcData;         ///< pointer to a union (array of 4 unsigned 8 bits or one 32 bit) data blocks
        CMP_FLOAT*      m_pfData;         ///< pointer to 32-bit signed float data blocks
        CMP_HALFSHORT*  m_phfsData;       ///< pointer to 16 bit short  data blocks
        CMP_DWORD*      m_pdwData;        ///< pointer to 32 bit data blocks
        CMP_VEC8*       m_pvec8Data;      ///< std::vector unsigned 8 bits data blocks
    };
} CMP_MipLevel;

typedef CMP_MipLevel  MipLevel;

typedef CMP_MipLevel* CMP_MipLevelTable; ///< A pointer to a set of MipLevels.

// Each texture and all its mip-map levels are encapsulated in a MipSet.
// Do not depend on m_pMipLevelTable being there, it is an implementation detail that you see only because there is no easy cross-complier 
// way of passing data around in internal classes.
//
// For 2D textures there are m_nMipLevels MipLevels.
// Cube maps have multiple faces or sides for each mip-map level . Instead of making a totally new data type, we just made each one of these faces be represented by a MipLevel, even though the terminology can be a bit confusing at first. So if your cube map consists of 6 faces for each mip-map level, then your first mip-map level will consist of 6 MipLevels, each having the same m_nWidth, m_nHeight. The next mip-map level will have half the m_nWidth & m_nHeight as the previous, but will be composed of 6 MipLevels still.
// A volume texture is a 3D texture. Again, instead of creating a new data type, we chose to make use of multiple MipLevels to create a single mip-map level of a volume texture. So a single mip-map level of a volume texture will consist of many MipLevels, all having the same m_nWidth and m_nHeight. The next mip-map level will have m_nWidth and m_nHeight half of the previous mip-map level's (to a minimum of 1) and will be composed of half as many MipLevels as the previous mip-map level (the first mip-map level takes this number from the MipSet it's part of), to a minimum of one.

typedef struct {
    CMP_INT           m_nWidth;            // User Setting: Width in pixels of the topmost mip-map level of the mip-map set. Initialized by TC_AppAllocateMipSet.
    CMP_INT           m_nHeight;           // User Setting: Height in pixels of the topmost mip-map level of the mip-map set. Initialized by TC_AppAllocateMipSet.
    CMP_INT           m_nDepth;            // User Setting: Depth in MipLevels of the topmost mip-map level of the mip-map set. Initialized by TC_AppAllocateMipSet. See Remarks.
    CMP_FORMAT        m_format;            // User Setting: Format for this MipSet

    // set by various API for internal use and user ref
    ChannelFormat     m_ChannelFormat;     // A texture is usually composed of channels, such as RGB channels for a texture with red green and blue image data. m_ChannelFormat indicates the representation of each of these channels. So a texture where each channel is an 8 bit integer would have CF_8bit for this. A compressed texture would use CF_Compressed.
    TextureDataType   m_TextureDataType;   // An indication of the type of data that the texture contains. A texture with just RGB values would use TDT_XRGB, while a texture that also uses the alpha channel would use TDT_ARGB.
    TextureType       m_TextureType;       // Indicates whether the texture is 2D, a cube map, or a volume texture. Used to determine how to treat MipLevels, among other things.
    CMP_UINT          m_Flags;             // Flags that mip-map set.
    CMP_BYTE          m_CubeFaceMask;      // A mask of MS_CubeFace values indicating which cube-map faces are present.
    CMP_DWORD         m_dwFourCC;          // The FourCC for this mip-map set. 0 if the mip-map set is uncompressed. Generated using MAKEFOURCC (defined in the Platform SDK or DX SDK).
    CMP_DWORD         m_dwFourCC2;         // An extra FourCC used by The Compressonator internally. Our DDS plugin saves/loads m_dwFourCC2 from pDDSD ddpfPixelFormat.dwPrivateFormatBitCount (since it's not really used by anything else) whether or not it is 0. Generated using MAKEFOURCC (defined in the Platform SDK or DX SDK). The FourCC2 field is currently used to allow differentiation between the various swizzled DXT5 formats. These formats must have a FourCC of DXT5 to be supported by the DirectX runtime but The Compressonator needs to know the swizzled FourCC to correctly display the texture.
    CMP_INT           m_nMaxMipLevels;     // Set by The Compressonator when you call TC_AppAllocateMipSet based on the width, height, depth, and textureType values passed in. Is really the maximum number of mip-map levels possible for that texture including the topmost mip-map level if you integer divide width height and depth by 2, rounding down but never falling below 1 until all three of them are 1. So a 5x10 2D texture would have a m_nMaxMipLevels of 4 (5x10  2x5  1x2  1x1).
    CMP_INT           m_nMipLevels;        // The number of mip-map levels in the mip-map set that actually have data. Always less than or equal to m_nMaxMipLevels. Set to 0 after TC_AppAllocateMipSet.
    CMP_FORMAT        m_transcodeFormat;   // For universal format: Sets the target data format for data processing and analysis
    CMP_BOOL          m_compressed;        // New Flags if data is compressed (example Block Compressed data in form of BCxx)
    CMP_FORMAT        m_isDeCompressed;    // The New MipSet is a decompressed result from a prior Compressed MipSet Format specified
    CMP_BOOL          m_swizzle;           // Flag is used by image load and save to indicate channels were swizzled from the origial source
    CMP_BYTE          m_nBlockWidth;       // Width in pixels of the Compression Block that is to be processed default for ASTC is 4
    CMP_BYTE          m_nBlockHeight;      // Height in pixels of the Compression Block that is to be processed default for ASTC is 4
    CMP_BYTE          m_nBlockDepth;       // Depth in pixels of the Compression Block that is to be processed default for ASTC is 1

    // set by various API for internal use. These values change when processing MipLevels
    CMP_DWORD         dwWidth;             // set by various API for ref,Width of the current active miplevel. if toplevel mipmap then value is same as m_nWidth
    CMP_DWORD         dwHeight;            // set by various API for ref,Height of the current active miplevel. if toplevel mipmap then value is same as m_nHeight
    CMP_DWORD         dwDataSize;          // set by various API for ref,Size of the current active miplevel allocated texture data.
    CMP_BYTE*         pData;               // set by various API for ref,Pointer to the current active miplevel texture data: used in MipLevelTable

    // Structure to hold all mip levels buffers
    CMP_MipLevelTable* m_pMipLevelTable;   // set by various API for ref, This is an implementation dependent way of storing the MipLevels that this mip-map set contains. Do not depend on it, use TC_AppGetMipLevel to access a mip-map set's MipLevels.
    void*              m_pReservedData;    // Reserved for ArchitectMF ImageLoader
} CMP_MipSet;



//===================================
// API Definitions for CMP Framework
//===================================
typedef struct {
    CMP_FLOAT   mipProgress; // The percentage progress of the current MIP level texture compression
    CMP_INT     mipLevel;    // returns the current MIP level been processed 0..max available for the image
    CMP_INT     cubeFace;    // returns the current Cube Face been processed 1..6
} CMP_MIPPROGRESSPARAM;

/// An enum selecting the different GPU driver types.
typedef enum {
    CMP_CPU = 0,   //Use CPU Only, encoders defined CMP_CPUEncode or Compressonator lib will be used
    CMP_HPC = 1,   //Use CPU High Performance Compute Encoders with SPMD support defined in CMP_CPUEncode)
    CMP_GPU = 2,   //Use GPU Kernel Encoders to compress textures using Default GPU Framework auto set by the codecs been used
    CMP_GPU_OCL = 3,   //Use GPU Kernel Encoders to compress textures using OpenCL Framework
    CMP_GPU_DXC = 4,   //Use GPU Kernel Encoders to compress textures using DirectX Compute Framework
    CMP_GPU_VLK = 5    //Use GPU Kernel Encoders to compress textures using Vulkan Compute Framework
} CMP_Compute_type;


typedef enum CMPComputeExtensions {
    CMP_COMPUTE_FP16 = 0x0001,       ///< Enable Packed Math Option for GPU
    CMP_COMPUTE_MAX_ENUM = 0x7FFF
} CMP_ComputeExtensions;


struct KernelOptions {
    CMP_ComputeExtensions   Extensions; // Compute extentions to use, set to 0 if you are not using any extensions
    CMP_DWORD  height;                  // Height of the encoded texture.
    CMP_DWORD  width;                   // Width of the encoded texture.
    CMP_FLOAT  fquality;                // Set the quality used for encoders 0.05 is the lowest and 1.0 for highest.
    CMP_FORMAT format;                  // Encoder codec format to use for processing
    CMP_Compute_type encodeWith;        // Host Type : default is HPC, options are [HPC or GPU]
    CMP_INT    threads;                 // requested number of threads to use (1= single) max is 128 for HPC

    //private: data settings: Do not use it will be removed from this interface!
    CMP_UINT   size;                    // Size of *data
    void *data;                         // Data to pass down from CPU to kernel
    void *dataSVM;                      // Data allocated as Shared by CPU and GPU (used only when code is running in 64bit and devices support SVM)
    char *srcfile;                      // Shader source file location
};

struct ComputeOptions {
    //public: data settings
    bool force_rebuild;                     ///<Force the GPU host framework to rebuild shaders
    //private: data settings: Do not use or set these
    void *plugin_compute;                   ///< Ref to Encoder codec plugin: For Internal use (will be removed!)
};

// The structure describing block encoder level settings.
typedef struct {
    unsigned int  width;   // Width of the encoded texture.
    unsigned int  height;  // Height of the encoded texture.
    unsigned int  pitch;   // Distance to start of next line..
    float         quality; // Set the quality used for encoders 0.05 is the lowest and 1.0 for highest.
    unsigned int  format;  // Format of the encoder to use: this is a enum set see compressonator.h CMP_FORMAT
} CMP_EncoderSetting;

/// CMP_Feedback_Proc
/// Feedback function for conversion.
/// \param[in] fProgress The percentage progress of the texture compression.
/// \param[in] mipProgress The current MIP level been processed, value of fProgress = mipProgress
/// \return non-NULL(true) value to abort conversion
typedef bool(CMP_API* CMP_Feedback_Proc)(CMP_FLOAT fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2);

#ifdef __cplusplus
extern "C"
{
#endif

    /// MIP MAP Interfaces
    extern  CMP_INT CMP_MaxFacesOrSlices(const CMP_MipSet* pMipSet, CMP_INT nMipLevel);
    CMP_INT CMP_API CMP_CalcMinMipSize(CMP_INT nHeight, CMP_INT nWidth, CMP_INT MipsLevel);
    CMP_INT CMP_API CMP_GenerateMIPLevels(CMP_MipSet *pMipSet, CMP_INT nMinSize);
    CMP_ERROR CMP_API CMP_CreateCompressMipSet(CMP_MipSet* pMipSetCMP, CMP_MipSet* pMipSetSRC);

    /// CMP_MIPFeedback_Proc
    /// Feedback function for conversion.
    /// \param[in] fProgress The percentage progress of the texture compression.
    /// \param[in] mipProgress The current MIP level been processed, value of fProgress = mipProgress
    /// \return non-NULL(true) value to abort conversion
    typedef bool(CMP_API* CMP_MIPFeedback_Proc)(CMP_MIPPROGRESSPARAM mipProgress);

    //--------------------------------------------
    // CMP_Compute Lib: Texture Encoder Interfaces
    //--------------------------------------------
    CMP_ERROR  CMP_API CMP_LoadTexture(const char *sourceFile, CMP_MipSet *pMipSet);
    CMP_ERROR  CMP_API CMP_SaveTexture(const char *destFile,   CMP_MipSet *pMipSet);
    CMP_ERROR  CMP_API CMP_ProcessTexture(CMP_MipSet* srcMipSet, CMP_MipSet* dstMipSet, KernelOptions kernelOptions,  CMP_Feedback_Proc pFeedbackProc);
    CMP_ERROR  CMP_API CMP_CompressTexture(KernelOptions *options,CMP_MipSet srcMipSet,CMP_MipSet dstMipSet,CMP_Feedback_Proc pFeedback);
    CMP_VOID   CMP_API CMP_Format2FourCC(CMP_FORMAT format,   CMP_MipSet *pMipSet);
    CMP_FORMAT CMP_API CMP_ParseFormat(char* pFormat);
    CMP_INT    CMP_API CMP_NumberOfProcessors();
    CMP_VOID   CMP_API CMP_FreeMipSet(CMP_MipSet *MipSetIn);
    CMP_VOID   CMP_API CMP_GetMipLevel(CMP_MipLevel *data, const CMP_MipSet* pMipSet, CMP_INT nMipLevel, CMP_INT nFaceOrSlice);
    
    //--------------------------------------------
    // CMP_Compute Lib: Host level interface
    //--------------------------------------------
    CMP_ERROR CMP_API CMP_CreateComputeLibrary(CMP_MipSet *srcTexture, KernelOptions  *kernelOptions, void *Reserved);
    CMP_ERROR CMP_API CMP_DestroyComputeLibrary(CMP_BOOL forceClose);
    CMP_ERROR CMP_API CMP_SetComputeOptions(ComputeOptions *options);
    
    //---------------------------------------------------------
    // Generic API to access the core using CMP_EncoderSetting
    //----------------------------------------------------------
    CMP_ERROR CMP_API CMP_CreateBlockEncoder(void **blockEncoder, CMP_EncoderSetting encodeSettings);
    CMP_ERROR CMP_API CMP_CompressBlock(void  **blockEncoder,void *srcBlock, unsigned int sourceStride, void *dstBlock, unsigned int dstStride);
    CMP_ERROR CMP_API CMP_CompressBlockXY(void  **blockEncoder,unsigned int blockx, unsigned int blocky, void *imgSrc, unsigned int sourceStride, void *cmpDst, unsigned int dstStride);
    void      CMP_API CMP_DestroyBlockEncoder(void  **blockEncoder);

#ifdef __cplusplus
};
#endif


#endif  //  CMP_FRAMEWORK_H

//=====================================================================
// Copyright 2016-2018(c), Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

#ifndef COMMONTYPES_H
#define COMMONTYPES_H


namespace CMP
{
    // Basic types.
    typedef std::uint8_t  BYTE;
    typedef std::uint16_t WORD;
    typedef std::uint32_t DWORD;
    typedef std::int32_t  LONG;
    typedef std::int32_t  BOOL;
    typedef size_t        DWORD_PTR;
    typedef unsigned int  UINT;

}  // namespace CMP

typedef std::uint32_t  CMP_DWORD;  ///< A 32-bit unsigned integer format.
typedef std::uint16_t  CMP_WORD;   ///< A 16-bit unsigned integer format.
typedef std::uint8_t   CMP_BYTE;   ///< An 8-bit unsigned integer format.
typedef char           CMP_CHAR;   ///< An 8-bit signed   char    format.
typedef float          CMP_FLOAT;  ///< A 32-bit signed   float   format.
typedef short          CMP_HALF;   ///< A 16-bit Half format.
typedef long           CMP_LONG;
typedef std::int32_t   CMP_BOOL;  ///< A 32-bit integer boolean format.
typedef size_t         CMP_DWORD_PTR;

typedef struct
{
    union
    {
        CMP_BYTE  rgba[4];  ///< The color as an array of components.
        CMP_DWORD asDword;  ///< The color as a DWORD.
    };
} CMP_COLOR;

/// Texture format.
/// These enum are all used internally for each version of the SDK the order may change
/// Typically reordered in alpha betical order per catagory for easy referance
//  Use the enum by name and not by its value
//
typedef enum
{
    CMP_FORMAT_Unknown,  ///< Undefined texture format.
    // Channel Component formats --------------------------------------------------------------------------------
    CMP_FORMAT_ARGB_8888,     ///< ARGB format with 8-bit fixed channels.
    CMP_FORMAT_ABGR_8888,     ///< ABGR format with 8-bit fixed channels.
    CMP_FORMAT_RGBA_8888,     ///< RGBA format with 8-bit fixed channels.
    CMP_FORMAT_BGRA_8888,     ///< BGRA format with 8-bit fixed channels.
    CMP_FORMAT_RGB_888,       ///< RGB format with 8-bit fixed channels.
    CMP_FORMAT_BGR_888,       ///< BGR format with 8-bit fixed channels.
    CMP_FORMAT_RG_8,          ///< Two component format with 8-bit fixed channels.
    CMP_FORMAT_R_8,           ///< Single component format with 8-bit fixed channels.
    CMP_FORMAT_ARGB_2101010,  ///< ARGB format with 10-bit fixed channels for color & a 2-bit fixed channel for alpha.
    CMP_FORMAT_ARGB_16,       ///< ARGB format with 16-bit fixed channels.
    CMP_FORMAT_ABGR_16,       ///< ABGR format with 16-bit fixed channels.
    CMP_FORMAT_RGBA_16,       ///< RGBA format with 16-bit fixed channels.
    CMP_FORMAT_BGRA_16,       ///< BGRA format with 16-bit fixed channels.
    CMP_FORMAT_RG_16,         ///< Two component format with 16-bit fixed channels.
    CMP_FORMAT_R_16,          ///< Single component format with 16-bit fixed channels.
    CMP_FORMAT_RGBE_32F,      ///< RGB format with 9-bit floating point each channel and shared 5 bit exponent
    CMP_FORMAT_ARGB_16F,      ///< ARGB format with 16-bit floating-point channels.
    CMP_FORMAT_ABGR_16F,      ///< ABGR format with 16-bit floating-point channels.
    CMP_FORMAT_RGBA_16F,      ///< RGBA format with 16-bit floating-point channels.
    CMP_FORMAT_BGRA_16F,      ///< BGRA format with 16-bit floating-point channels.
    CMP_FORMAT_RG_16F,        ///< Two component format with 16-bit floating-point channels.
    CMP_FORMAT_R_16F,         ///< Single component with 16-bit floating-point channels.
    CMP_FORMAT_ARGB_32F,      ///< ARGB format with 32-bit floating-point channels.
    CMP_FORMAT_ABGR_32F,      ///< ABGR format with 32-bit floating-point channels.
    CMP_FORMAT_RGBA_32F,      ///< RGBA format with 32-bit floating-point channels.
    CMP_FORMAT_BGRA_32F,      ///< BGRA format with 32-bit floating-point channels.
    CMP_FORMAT_RGB_32F,       ///< RGB format with 32-bit floating-point channels.
    CMP_FORMAT_BGR_32F,       ///< BGR format with 32-bit floating-point channels.
    CMP_FORMAT_RG_32F,        ///< Two component format with 32-bit floating-point channels.
    CMP_FORMAT_R_32F,         ///< Single component with 32-bit floating-point channels.
    // Compression formats -----------------------------------------------------------------------------------
    CMP_FORMAT_ASTC,   ///< ASTC (Adaptive Scalable Texture Compression) open texture compression standard
    CMP_FORMAT_ATI1N,  ///< Single component compression format using the same technique as DXT5 alpha. Four bits per pixel.
    CMP_FORMAT_ATI2N,  ///<     Two component compression format using the same technique as DXT5 alpha. Designed for compression of tangent space normal maps. Eight bits per pixel.
    CMP_FORMAT_ATI2N_XY,  ///<    Two component compression format using the same technique as DXT5 alpha. The same as ATI2N but with the channels swizzled. Eight bits per pixel.
    CMP_FORMAT_ATI2N_DXT5,  ///<    ATI2N like format using DXT5. Intended for use on GPUs that do not natively support ATI2N. Eight bits per pixel.
    CMP_FORMAT_ATC_RGB,     ///< CMP - a compressed RGB format.
    CMP_FORMAT_ATC_RGBA_Explicit,      ///< CMP - a compressed ARGB format with explicit alpha.
    CMP_FORMAT_ATC_RGBA_Interpolated,  ///< CMP - a compressed ARGB format with interpolated alpha.
    CMP_FORMAT_BC1,  ///< A four component opaque (or 1-bit alpha) compressed texture format for Microsoft DirectX10. Identical to DXT1.  Four bits per pixel.
    CMP_FORMAT_BC2,  ///< A four component compressed texture format with explicit alpha for Microsoft DirectX10. Identical to DXT3. Eight bits per pixel.
    CMP_FORMAT_BC3,  ///< A four component compressed texture format with interpolated alpha for Microsoft DirectX10. Identical to DXT5. Eight bits per pixel.
    CMP_FORMAT_BC4,        ///< A single component compressed texture format for Microsoft DirectX10. Identical to ATI1N. Four bits per pixel.
    CMP_FORMAT_BC5,        ///< A two component compressed texture format for Microsoft DirectX10. Identical to ATI2N_XY.(Red&Green) Eight bits per pixel.
    CMP_FORMAT_BC6H,       ///< BC6H compressed texture format (UF)
    CMP_FORMAT_BC6H_SF,    ///< BC6H compressed texture format (SF)
    CMP_FORMAT_BC7,        ///< BC7  compressed texture format
    CMP_FORMAT_DXT1,       ///< An DXTC compressed texture matopaque (or 1-bit alpha). Four bits per pixel.
    CMP_FORMAT_DXT3,       ///<    DXTC compressed texture format with explicit alpha. Eight bits per pixel.
    CMP_FORMAT_DXT5,       ///<    DXTC compressed texture format with interpolated alpha. Eight bits per pixel.
    CMP_FORMAT_DXT5_xGBR,  ///<    DXT5 with the red component swizzled into the alpha channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_RxBG,  ///<    swizzled DXT5 format with the green component swizzled into the alpha channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_RBxG,  ///<    swizzled DXT5 format with the green component swizzled into the alpha channel & the blue component swizzled into the green channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_xRBG,  ///<    swizzled DXT5 format with the green component swizzled into the alpha channel & the red component swizzled into the green channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_RGxB,  ///<    swizzled DXT5 format with the blue component swizzled into the alpha channel. Eight bits per pixel.
    CMP_FORMAT_DXT5_xGxR,  ///<    two-component swizzled DXT5 format with the red component swizzled into the alpha channel & the green component in the green channel. Eight bits per pixel.
    CMP_FORMAT_ETC_RGB,    ///< ETC   GL_COMPRESSED_RGB8_ETC2  backward compatible
    CMP_FORMAT_ETC2_RGB,   ///< ETC2  GL_COMPRESSED_RGB8_ETC2
    CMP_FORMAT_ETC2_SRGB,  ///< ETC2  GL_COMPRESSED_SRGB8_ETC2
    CMP_FORMAT_ETC2_RGBA,  ///< ETC2  GL_COMPRESSED_RGBA8_ETC2_EAC
    CMP_FORMAT_ETC2_RGBA1, ///< ETC2  GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    CMP_FORMAT_ETC2_SRGBA, ///< ETC2  GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    CMP_FORMAT_ETC2_SRGBA1,///< ETC2  GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    CMP_FORMAT_GT,         ///< GT   (Reserved for a future implementation)
                           //--------------------------------------------------------------------------------------------------------
    CMP_FORMAT_MAX = CMP_FORMAT_GT
} CMP_FORMAT;


/// Compress error codes
typedef enum
{
    CMP_OK = 0,                            ///< Ok.
    CMP_ABORTED,                           ///< The conversion was aborted.
    CMP_ERR_INVALID_SOURCE_TEXTURE,        ///< The source texture is invalid.
    CMP_ERR_INVALID_DEST_TEXTURE,          ///< The destination texture is invalid.
    CMP_ERR_UNSUPPORTED_SOURCE_FORMAT,     ///< The source format is not a supported format.
    CMP_ERR_UNSUPPORTED_DEST_FORMAT,       ///< The destination format is not a supported format.
    CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE,   ///< The gpu hardware is not supported.
    CMP_ERR_SIZE_MISMATCH,                 ///< The source and destination texture sizes do not match.
    CMP_ERR_UNABLE_TO_INIT_CODEC,          ///< Compressonator was unable to initialize the codec needed for conversion.
    CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB,  ///< GPU_Decode Lib was unable to initialize the codec needed for decompression .
    CMP_ERR_UNABLE_TO_INIT_COMPUTELIB,     ///< Compute Lib was unable to initialize the codec needed for compression.
    CMP_ERR_GENERIC                        ///< An unknown error occurred.
} CMP_ERROR;



/// The structure describing a texture.
typedef struct
{
    CMP_DWORD  dwSize;        ///< Size of this structure.
    CMP_DWORD  dwWidth;       ///< Width of the texture.
    CMP_DWORD  dwHeight;      ///< Height of the texture.
    CMP_DWORD  dwPitch;       ///< Distance to start of next line - necessary only for uncompressed textures.
    CMP_FORMAT format;        ///< Format of the texture.
    CMP_BYTE   nBlockHeight;  ///< Size Block to use (Default is 4x4x1).
    CMP_BYTE   nBlockWidth;   ///<
    CMP_BYTE   nBlockDepth;   ///<
    CMP_DWORD  dwDataSize;    ///< Size of the allocated texture data.
    CMP_BYTE*  pData;         ///< Pointer to the texture data
} CMP_Texture;


#if defined(WIN32) || defined(_WIN64)
#define CMP_API __cdecl
#else
#define CMP_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /// CMP_Feedback_Proc
    /// Feedback function for conversion.
    /// \param[in] fProgress The percentage progress of the texture compression.
    /// \param[in] pUser1 User data as passed to CMP_ConvertTexture.
    /// \param[in] pUser2 User data as passed to CMP_ConvertTexture.
    /// \return non-NULL(true) value to abort conversion
    typedef bool(CMP_API* CMP_Feedback_Proc)(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2);

#ifdef __cplusplus
};
#endif

#endif  // !COMMONTYPES_H

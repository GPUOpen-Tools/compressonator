//////////////////////////////////////////////////////////////////////////////
//
//  File Name:   Texture.h
//  Description: Definition of Texture structures as used by the ATI Texture 
//  Plugin API
//
//  Copyright (c) 2007,2008,2016    Advanced Micro Devices, Inc.
//  Copyright (c) 2002-2006            ATI Technologies Inc.
//
//  Version:    1.5
//
//////////////////////////////////////////////////////////////////////////////

/// \file
/// Texture.h declares both texture structures used by the Plugin SDK.

#if !defined(_AMD_TEX_TEXTURE_H_INCLUDED_)
#define _AMD_TEX_TEXTURE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
//palette sizes
#define P1_PALETTE_SIZE 2     ///< The maximum number of palette entries for a 1-bit palette.  
#define P2_PALETTE_SIZE 4     ///< The maximum number of palette entries for a 2-bit palette.
#define P4_PALETTE_SIZE 16    ///< The maximum number of palette entries for a 4-bit palette.
#define P8_PALETTE_SIZE 256   ///< The maximum number of palette entries for a 8-bit palette.

//FourCC defines
//Palettized
#define FOURCC_P1                      MAKEFOURCC('P', '1', ' ', ' ') ///< FourCC code for 1-bit palletized textures.
#define FOURCC_P2                      MAKEFOURCC('P', '2', ' ', ' ') ///< FourCC code for 2-bit palletized textures.
#define FOURCC_P4                      MAKEFOURCC('P', '4', ' ', ' ') ///< FourCC code for 4-bit palletized textures.
#define FOURCC_P8                      MAKEFOURCC('P', '8', ' ', ' ') ///< FourCC code for 8-bit palletized textures.
//Grayscale
#define FOURCC_G1                      MAKEFOURCC('G', '1', ' ', ' ') ///< FourCC code for 1-bit gray-scale textures.
#define FOURCC_G2                      MAKEFOURCC('G', '2', ' ', ' ') ///< FourCC code for 2-bit gray-scale textures.
#define FOURCC_G4                      MAKEFOURCC('G', '4', ' ', ' ') ///< FourCC code for 4-bit gray-scale textures.
#define FOURCC_G8                      MAKEFOURCC('G', '8', ' ', ' ') ///< FourCC code for 8-bit gray-scale textures.
#define FOURCC_G16                     MAKEFOURCC('G', '1', '6', ' ') ///< FourCC code for 16-bit gray-scale textures.
//Grayscale and alpha channel
#define FOURCC_AG1                     MAKEFOURCC('A', 'G', '1', ' ') ///< FourCC code for 1-bit gray-scale with alpha textures.
#define FOURCC_AG2                     MAKEFOURCC('A', 'G', '2', ' ') ///< FourCC code for 2-bit gray-scale with alpha textures.
#define FOURCC_AG4                     MAKEFOURCC('A', 'G', '4', ' ') ///< FourCC code for 4-bit gray-scale with alpha textures.
#define FOURCC_AG8                     MAKEFOURCC('A', 'G', '8', ' ') ///< FourCC code for 8-bit gray-scale with alpha textures.
//alpha
#define FOURCC_A1                      MAKEFOURCC('A', '1', ' ', ' ') ///< FourCC code for 1-bit alpha textures.
#define FOURCC_A2                      MAKEFOURCC('A', '2', ' ', ' ') ///< FourCC code for 2-bit alpha textures.
#define FOURCC_A4                      MAKEFOURCC('A', '4', ' ', ' ') ///< FourCC code for 4-bit alpha textures.
#define FOURCC_A8                      MAKEFOURCC('A', '8', ' ', ' ') ///< FourCC code for 8-bit alpha textures.

#define FOURCC_ATI1N                   MAKEFOURCC('A', 'T', 'I', '1')
#define FOURCC_ATI2N                   MAKEFOURCC('A', 'T', 'I', '2')
#define FOURCC_ATI2N_XY                MAKEFOURCC('A', '2', 'X', 'Y')
#define FOURCC_ATI2N_DXT5              MAKEFOURCC('A', '2', 'D', '5')
#define FOURCC_DXT5_xGBR               MAKEFOURCC('x', 'G', 'B', 'R')
#define FOURCC_DXT5_RxBG               MAKEFOURCC('R', 'x', 'B', 'G')
#define FOURCC_DXT5_RBxG               MAKEFOURCC('R', 'B', 'x', 'G')
#define FOURCC_DXT5_xRBG               MAKEFOURCC('x', 'R', 'B', 'G')
#define FOURCC_DXT5_RGxB               MAKEFOURCC('R', 'G', 'x', 'B')
#define FOURCC_DXT5_xGxR               MAKEFOURCC('x', 'G', 'x', 'R')
#define FOURCC_APC1                    MAKEFOURCC('A', 'P', 'C', '1')
#define FOURCC_APC2                    MAKEFOURCC('A', 'P', 'C', '2')
#define FOURCC_APC3                    MAKEFOURCC('A', 'P', 'C', '3')
#define FOURCC_APC4                    MAKEFOURCC('A', 'P', 'C', '4')
#define FOURCC_APC5                    MAKEFOURCC('A', 'P', 'C', '5')
#define FOURCC_APC6                    MAKEFOURCC('A', 'P', 'C', '6')

#define FOURCC_ATC_RGB                MAKEFOURCC('A', 'T', 'C', ' ')
#define FOURCC_ATC_RGBA_EXPLICIT      MAKEFOURCC('A', 'T', 'C', 'A')
#define FOURCC_ATC_RGBA_INTERP        MAKEFOURCC('A', 'T', 'C', 'I')

#define FOURCC_ETC_RGB                MAKEFOURCC('E', 'T', 'C', ' ')
#define FOURCC_ETC_RGBA_EXPLICIT      MAKEFOURCC('E', 'T', 'C', 'A')
#define FOURCC_ETC_RGBA_INTERP        MAKEFOURCC('E', 'T', 'C', 'I')

// Deprecated but still supported for decompression

#define FOURCC_DXT5_GXRB               MAKEFOURCC('G', 'X', 'R', 'B')
#define FOURCC_DXT5_GRXB               MAKEFOURCC('G', 'R', 'X', 'B')
#define FOURCC_DXT5_RXGB               MAKEFOURCC('R', 'X', 'G', 'B')
#define FOURCC_DXT5_BRGX               MAKEFOURCC('B', 'R', 'G', 'X')

#define FOURCC_BC1                     MAKEFOURCC('B', 'C', '1', ' ')
#define FOURCC_BC2                     MAKEFOURCC('B', 'C', '2', ' ')
#define FOURCC_BC3                     MAKEFOURCC('B', 'C', '3', ' ')
#define FOURCC_BC4                     MAKEFOURCC('B', 'C', '4', ' ')
#define FOURCC_BC4S                    MAKEFOURCC('B', 'C', '4', 'S')
#define FOURCC_BC4U                    MAKEFOURCC('B', 'C', '4', 'U')
#define FOURCC_BC5                     MAKEFOURCC('B', 'C', '5', ' ')
#define FOURCC_BC5S                    MAKEFOURCC('B', 'C', '5', 'S')

#define FOURCC_DX10                    MAKEFOURCC('D', 'X', '1', '0')

// Used for custom format or future DDS support (They are not supported by Direct X)

#define FOURCC_BC6H                    MAKEFOURCC('B', 'C', '6', 'H')
#define FOURCC_BC7                     MAKEFOURCC('B', 'C', '7', 'x')
#define FOURCC_ASTC                    MAKEFOURCC('A', 'S', 'T', 'C')
#define FOURCC_GT                      MAKEFOURCC('G', 'T', '1', 'x')
#define FOURCC_ETC2_RGB                MAKEFOURCC('E', 'T', 'C', '2')

/*
 * FOURCC codes for DX compressed-texture pixel formats
 */
#define FOURCC_DXT1                    (MAKEFOURCC('D','X','T','1'))        // aka BC1
#define FOURCC_DXT2                    (MAKEFOURCC('D','X','T','2'))        // aka BC2
#define FOURCC_DXT3                    (MAKEFOURCC('D','X','T','3'))        // aka BC3
#define FOURCC_DXT4                    (MAKEFOURCC('D','X','T','4'))        // aka BC4
#define FOURCC_DXT5                    (MAKEFOURCC('D','X','T','5'))        // aka BC5





/// The format of data in the channels of texture.
typedef enum
{
   CF_8bit        = 0,  ///< 8-bit integer data.
   CF_Float16     = 1,  ///< 16-bit float data.
   CF_Float32     = 2,  ///< 32-bit float data.
   CF_Compressed  = 3,  ///< Compressed data.
   CF_16bit       = 4,  ///< 16-bit integer data.
   CF_2101010     = 5,  ///< 10-bit integer data in the color channels & 2-bit integer data in the alpha channel.
   CF_32bit       = 6,  ///< 32-bit integer data.
   CF_Float9995E  = 7,  ///< 32-bit partial precision float.
   CF_YUV_420     = 8,  ///< YUV Chroma formats 
   CF_YUV_422     = 9,  ///< YUV Chroma formats
   CF_YUV_444     = 10, ///< YUV Chroma formats
   CF_YUV_4444    = 11, ///< YUV Chroma formats
} ChannelFormat;

/// The type of data the texture represents.
typedef enum
{
   TDT_XRGB       = 0,  ///< An RGB texture padded to DWORD width. 
   TDT_ARGB       = 1,  ///< An ARGB texture. 
   TDT_NORMAL_MAP = 2,  ///< A normal map.
   TDT_R          = 3,  ///< A single component texture.
   TDT_RG         = 4,  ///< A two component texture.
   TDT_YUV_SD     = 5,  ///< An YUB Standard Definition texture. 
   TDT_YUV_HD     = 6,  ///< An YUB High Definition texture. 
} TextureDataType;

/// The type of the texture.
typedef enum
{
   TT_2D             = 0,  ///< A regular 2D texture. data stored linearly (rgba,rgba,...rgba)
   TT_CubeMap        = 1,  ///< A cubemap texture.
   TT_VolumeTexture  = 2,  ///< A volume texture.
   TT__2D_Block      = 3,  ///< 2D texture data stored as [Height][Width] blocks as individual channels using cmp_rgb_t or cmp_yuv_t
} TextureType;

#define   MS_FLAG_Default                0x0000
#define   MS_FLAG_AlphaPremult           0x0001
#define   MS_FLAG_DisableMipMapping      0x0002

typedef enum
{
   MS_Default        = 0,
   MS_AlphaPremult   = 1,
   MS_DisableMipMapping = 2,
} MS_Flags;


/// A set of flags indicating cube-map faces.
typedef enum
{
   MS_CF_None        = 0x00, ///< No cube-map faces.
   MS_CF_PositiveX   = 0x01, ///< The positive-X cube-map face.
   MS_CF_NegativeX   = 0x02, ///< The negative-X cube-map face.
   MS_CF_PositiveY   = 0x04, ///< The positive-Y cube-map face.
   MS_CF_NegativeY   = 0x08, ///< The negative-Y cube-map face.
   MS_CF_PositiveZ   = 0x10, ///< The positive-Z cube-map face.
   MS_CF_NegativeZ   = 0x20, ///< The negative-Z cube-map face.
   MS_CF_All         = 0x3f, ///< All the cube-map faces.
} MS_CubeFace;

/// A MipLevel is the fundamental unit for containing texture data. 
/// \remarks
/// One logical mip level can be composed of many MipLevels, see the documentation of MipSet for explanation.
/// \sa \link TC_AppAllocateMipLevelData() TC_AppAllocateMipLevelData \endlink, 
/// \link TC_AppAllocateCompressedMipLevelData() TC_AppAllocateCompressedMipLevelData \endlink, 
/// \link MipSet \endlink
typedef struct
{
   int             m_nWidth;         ///< Width of the data in pixels.
   int             m_nHeight;        ///< Height of the data in pixels.
   CMP_DWORD       m_dwLinearSize;   ///< Size of the data in bytes.
   union
   {    
      CMP_BYTE*    m_pbData;         ///< A pointer to the texture data that this MipLevel contains.
      CMP_WORD*    m_pwData;         ///< A pointer to the texture data that this MipLevel contains.
      CMP_COLOR*   m_pcData;         ///< A pointer to the texture data that this MipLevel contains.
      CMP_FLOAT*   m_pfData;         ///< A pointer to the texture data that this MipLevel contains.
      CMP_HALF*    m_phfData;        ///< A pointer to the texture data that this MipLevel contains.
      CMP_DWORD*   m_pdwData;        ///< A pointer to the texture data that this MipLevel contains.
   };
} MipLevel;

typedef MipLevel* MipLevelTable; ///< A pointer to a set of MipLevels.


typedef struct cmp_yuv_s {
    CMP_BYTE **y;
    CMP_BYTE **u;
    CMP_BYTE **v;
    CMP_BYTE **a;
    CMP_BYTE **m;
} cmp_yuv_t;

typedef struct cmp_rgb_s {
    CMP_BYTE **r;
    CMP_BYTE **g;
    CMP_BYTE **b;
    CMP_BYTE **a;
    CMP_BYTE **m;
} cmp_rgb_t;

// Data struct used for casting m_pReservedData ref
typedef union mapset_data_u
{
    cmp_rgb_t rgb;
    cmp_yuv_t yuv;
} mapset_data_t;

/// Each texture and all its mip-map levels are encapsulated in a MipSet.
/// \remarks
/// Do not depend on m_pMipLevelTable being there, it is an implementation detail that you see only because there is no easy cross-complier way of passing a class around.
/// \remarks
/// For 2D textures there are m_nMipLevels MipLevels.
/// \remarks
/// Cube maps have multiple faces or sides for each mip-map level . Instead of making a totally new data type, we just made each one of these faces be represented by a MipLevel, even though the terminology can be a bit confusing at first. So if your cube map consists of 6 faces for each mip-map level, then your first mip-map level will consist of 6 MipLevels, each having the same m_nWidth, m_nHeight. The next mip-map level will have half the m_nWidth & m_nHeight as the previous, but will be composed of 6 MipLevels still.
/// \remarks
/// A volume texture is a 3D texture. Again, instead of creating a new data type, we chose to make use of multiple MipLevels to create a single mip-map level of a volume texture. So a single mip-map level of a volume texture will consist of many MipLevels, all having the same m_nWidth and m_nHeight. The next mip-map level will have m_nWidth and m_nHeight half of the previous mip-map level's (to a minimum of 1) and will be composed of half as many MipLevels as the previous mip-map level (the first mip-map level takes this number from the MipSet it's part of), to a minimum of one.
/// \sa \link TC_AppAllocateMipSet() TC_AppAllocateMipSet \endlink, 
/// \link MipLevel \endlink
typedef struct
{
   ChannelFormat     m_ChannelFormat;     ///< A texture is usually composed of channels, such as RGB channels for a texture with red green and blue image data. m_ChannelFormat indicates the representation of each of these channels. So a texture where each channel is an 8 bit integer would have CF_8bit for this. A compressed texture would use CF_Compressed.
   TextureDataType   m_TextureDataType;   ///< An indication of the type of data that the texture contains. A texture with just RGB values would use TDT_XRGB, while a texture that also uses the alpha channel would use TDT_ARGB.
   TextureType       m_TextureType;       ///< Indicates whether the texture is 2D, a cube map, or a volume texture. Used to determine how to treat MipLevels, among other things.
   unsigned int      m_Flags;             ///< Flags that for this mip-map set.
   CMP_BYTE          m_CubeFaceMask;      ///< A mask of MS_CubeFace values indicating which cube-map faces are present.
   CMP_DWORD         m_dwFourCC;          ///< The FourCC for this mip-map set. 0 if the mip-map set is uncompressed. Generated using MAKEFOURCC (defined in the Platform SDK or DX SDK).
   CMP_DWORD         m_dwFourCC2;         ///< An extra FourCC used by The Compressonator internally. Our DDS plugin saves/loads m_dwFourCC2 from pDDSD->ddpfPixelFormat.dwPrivateFormatBitCount (since it's not really used by anything else) whether or not it is 0. Generated using MAKEFOURCC (defined in the Platform SDK or DX SDK). The FourCC2 field is currently used to allow differentiation between the various swizzled DXT5 formats. These formats must have a FourCC of DXT5 to be supported by the DirectX runtime but The Compressonator needs to know the swizzled FourCC to correctly display the texture. 
   int               m_nMaxMipLevels;     ///< Set by The Compressonator when you call TC_AppAllocateMipSet based on the width, height, depth, and textureType values passed in. Is really the maximum number of mip-map levels possible for that texture including the topmost mip-map level if you integer divide width height and depth by 2, rounding down but never falling below 1 until all three of them are 1. So a 5x10 2D texture would have a m_nMaxMipLevels of 4 (5x10 -> 2x5 -> 1x2 -> 1x1).
   int               m_nMipLevels;        ///< The number of mip-map levels in the mip-map set that actually have data. Always less than or equal to m_nMaxMipLevels. Set to 0 after TC_AppAllocateMipSet.
   int               m_nWidth;            ///< Width in pixels of the topmost mip-map level of the mip-map set. Initialized by TC_AppAllocateMipSet.
   int               m_nHeight;           ///< Height in pixels of the topmost mip-map level of the mip-map set. Initialized by TC_AppAllocateMipSet.
   int               m_nDepth;            ///< Depth in MipLevels of the topmost mip-map level of the mip-map set. Initialized by TC_AppAllocateMipSet. See Remarks.
   CMP_FORMAT        m_format;            ///< New format support
   bool              m_compressed;        ///< New Flags if data is compressed (example Block Compressed data in form of BCxx)
   CMP_FORMAT        m_isDeCompressed;    ///< The New MipSet is a decompressed result from a prior Compressed MipSet Format specified
   bool              m_swizzle;           ///< Flag is used by image load and save to indicate compression is to be or has occured on the data; Compression data is typically ARGB.
   int               m_nBlockWidth;       ///< Width in pixels of the Compression Block that is to be processed default for ASTC is 4 
   int               m_nBlockHeight;      ///< Height in pixels of the Compression Block that is to be processed default for ASTC is 4
   int               m_nBlockDepth;       ///< Depth in pixels of the Compression Block that is to be processed default for ASTC is 1
   MipLevelTable*    m_pMipLevelTable;    ///< This is an implementation dependent way of storing the MipLevels that this mip-map set contains. Do not depend on it, use TC_AppGetMipLevel to access a mip-map set's MipLevels.
   void*             m_pReservedData;     ///< Pointer to reserved data types
} MipSet;

CMP_DWORD GetChannelSize(ChannelFormat channelFormat);       //< \internal
CMP_DWORD GetChannelCount(TextureDataType textureDataType);  //< \internal
CMP_DWORD GetPixelSize(const MipSet& mipset);                //< \internal

bool GetMipSetPixelColorARGB8888(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, CMP_COLOR& color);        //< \internal
bool GetMipSetPixelColorARGB2101010(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, CMP_WORD color[4]);    //< \internal
bool GetMipSetPixelColorARGB16161616(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, CMP_WORD color[4]);   //< \internal
bool GetMipSetPixelColorARGB32(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, CMP_DWORD color[4]);        //< \internal
bool GetMipSetPixelColorARGB32F(const MipSet& mipset, int nMipLevel, int nFaceOrSlice, int nXpos, int nYpos, float color[4]);       //< \internal

#ifdef __cplusplus
};
#endif

#define UNREFERENCED_PARAMETER(P)          (P)

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_AMD_TA_TEXTURE_H_INCLUDED_)

/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: 7f8f35be3acc819647c85c65682529b18a792866 $ */

/*
Copyright (c) 2010 The Khronos Group Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and/or associated documentation files (the
"Materials"), to deal in the Materials without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Materials, and to
permit persons to whom the Materials are furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
unaltered in all copies or substantial portions of the Materials.
Any additions, deletions, or changes to the original source files
must be clearly indicated in accompanying documentation.

If only executable code is distributed, then the accompanying
documentation must state that "this software is based in part on the
work of the Khronos Group."

THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

/* 
 * Author: Mark Callow from original code by Georg Kolling
 */

#ifndef KTXINT_H
#define KTXINT_H

/* Define this to include the ETC unpack software in the library. */
#ifndef SUPPORT_SOFTWARE_ETC_UNPACK
  /* Include for all GL versions because have seen OpenGL ES 3
   * implementaions that do not support ETC1 (ARM Mali emulator v1.0)!
   */
  #define SUPPORT_SOFTWARE_ETC_UNPACK 0
#endif

#ifndef SUPPORT_LEGACY_FORMAT_CONVERSION
  #if KTX_OPENGL
    #define SUPPORT_LEGACY_FORMAT_CONVERSION 1
  #elif KTX_OPENGL_ES1
    /* ES1, ES2 & ES3 support the legacy formats */
    #define SUPPORT_LEGACY_FORMAT_CONVERSION 0
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define KTX_IDENTIFIER_REF  { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A }
#define KTX_ENDIAN_REF      (0x04030201)
#define KTX_ENDIAN_REF_REV  (0x01020304)
#define KTX_HEADER_SIZE		(64)

/**
 * @internal
 * @brief KTX file header
 *
 * See the KTX specification for descriptions
 * 
 */
typedef struct KTX_header_t {
	khronos_uint8_t  identifier[12];
	khronos_uint32_t endianness;
	khronos_uint32_t glType;
	khronos_uint32_t glTypeSize;
	khronos_uint32_t glFormat;
	khronos_uint32_t glInternalFormat;
	khronos_uint32_t glBaseInternalFormat;
	khronos_uint32_t pixelWidth;
	khronos_uint32_t pixelHeight;
	khronos_uint32_t pixelDepth;
	khronos_uint32_t numberOfArrayElements;
	khronos_uint32_t numberOfFaces;
	khronos_uint32_t numberOfMipmapLevels;
	khronos_uint32_t bytesOfKeyValueData;
} KTX_header;

/* This will cause compilation to fail if the struct size doesn't match */
typedef int KTX_header_SIZE_ASSERT [sizeof(KTX_header) == KTX_HEADER_SIZE];

/**
 * @internal
 * @brief _ktxCheckHeader returns texture information in this structure
 *
 * TO DO: document properly
 */
typedef struct KTX_texinfo_t {
	/* Data filled in by _ktxCheckHeader() */
	khronos_uint32_t textureDimensions;
	khronos_uint32_t glTarget;
	khronos_uint32_t compressed;
	khronos_uint32_t generateMipmaps;
} KTX_texinfo;

/**
 * @internal
 * @brief used to pass GL context capabilites to subroutines.
 */
#define _KTX_NO_R16_FORMATS     0x0
#define _KTX_R16_FORMATS_NORM	0x1
#define _KTX_R16_FORMATS_SNORM	0x2
#define _KTX_ALL_R16_FORMATS (_KTX_R16_FORMATS_NORM | _KTX_R16_FORMATS_SNORM)
extern GLint _ktxR16Formats;
extern GLboolean _ktxSupportsSRGB;


/*
 * These defines are needed to compile the KTX library. When
 * these things are not available in the GL header in use at
 * compile time, the library provides its own support, handles
 * the expected run-time errors or just needs the token value.
 */
#ifndef GL_LUMINANCE
#define GL_ALPHA						0x1906
#define GL_LUMINANCE					0x1909
#define GL_LUMINANCE_ALPHA				0x190A
#endif
#ifndef GL_INTENSITY
#define GL_INTENSITY					0x8049
#endif
#if SUPPORT_LEGACY_FORMAT_CONVERSION
/* For loading legacy KTX files. */
#ifndef GL_LUMINANCE4
#define GL_ALPHA4						0x803B
#define GL_ALPHA8						0x803C
#define GL_ALPHA12						0x803D
#define GL_ALPHA16						0x803E
#define GL_LUMINANCE4					0x803F
#define GL_LUMINANCE8					0x8040
#define GL_LUMINANCE12					0x8041
#define GL_LUMINANCE16					0x8042
#define GL_LUMINANCE4_ALPHA4			0x8043
#define GL_LUMINANCE6_ALPHA2			0x8044
#define GL_LUMINANCE8_ALPHA8			0x8045
#define GL_LUMINANCE12_ALPHA4			0x8046
#define GL_LUMINANCE12_ALPHA12			0x8047
#define GL_LUMINANCE16_ALPHA16			0x8048
#endif
#ifndef GL_INTENSITY4
#define GL_INTENSITY4					0x804A
#define GL_INTENSITY8					0x804B
#define GL_INTENSITY12					0x804C
#define GL_INTENSITY16					0x804D
#endif
#ifndef GL_SLUMINANCE
#define GL_SLUMINANCE_ALPHA				0x8C44
#define GL_SLUMINANCE8_ALPHA8			0x8C45
#define GL_SLUMINANCE					0x8C46
#define GL_SLUMINANCE8					0x8C47
#endif
#endif /* SUPPORT_LEGACY_FORMAT_CONVERSION */
#ifndef GL_TEXTURE_1D
#define GL_TEXTURE_1D                   0x0DE0
#endif
#ifndef GL_TEXTURE_3D
#define GL_TEXTURE_3D                   0x806F
#endif
#ifndef GL_TEXTURE_CUBE_MAP
#define GL_TEXTURE_CUBE_MAP             0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X  0x8515
#endif
#ifndef GL_TEXTURE_CUBE_MAP_ARRAY
#define GL_TEXTURE_CUBE_MAP_ARRAY       0x9009
#endif
/* from GL_EXT_texture_array */
#ifndef GL_TEXTURE_1D_ARRAY_EXT
#define GL_TEXTURE_1D_ARRAY_EXT         0x8C18
#define GL_TEXTURE_2D_ARRAY_EXT         0x8C1A
#endif
#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP              0x8191
#endif

/* For writer.c */
#if !defined(GL_BGR)
#define GL_BGR							0x80E0
#define GL_BGRA							0x80E1
#endif
#if !defined(GL_RED_INTEGER)
#define GL_RED_INTEGER					0x8D94
#define GL_RGB_INTEGER					0x8D98
#define GL_RGBA_INTEGER					0x8D99
#endif
#if !defined(GL_GREEN_INTEGER)
#define GL_GREEN_INTEGER				0x8D95
#define GL_BLUE_INTEGER					0x8D96
#endif
#if !defined(GL_ALPHA_INTEGER)
#define GL_ALPHA_INTEGER				0x8D97
#endif
#if !defined (GL_BGR_INTEGER)
#define GL_BGR_INTEGER					0x8D9A
#define GL_BGRA_INTEGER					0x8D9B
#endif
#if !defined(GL_INT)
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#endif
#if !defined(GL_HALF_FLOAT)
typedef unsigned short GLhalf;
#define GL_HALF_FLOAT					0x140B
#endif
#if !defined(GL_UNSIGNED_BYTE_3_3_2)
#define GL_UNSIGNED_BYTE_3_3_2			0x8032
#define GL_UNSIGNED_INT_8_8_8_8			0x8035
#define GL_UNSIGNED_INT_10_10_10_2		0x8036
#endif
#if !defined(GL_UNSIGNED_BYTE_2_3_3_REV)
#define GL_UNSIGNED_BYTE_2_3_3_REV		0x8362
#define GL_UNSIGNED_SHORT_5_6_5			0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV		0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV	0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV	0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV		0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV	0x8368
#endif
#if !defined(GL_UNSIGNED_INT_24_8)
#define GL_DEPTH_STENCIL				0x84F9
#define GL_UNSIGNED_INT_24_8			0x84FA
#endif
#if !defined(GL_UNSIGNED_INT_5_9_9_9_REV)
#define GL_UNSIGNED_INT_5_9_9_9_REV		0x8C3E
#endif
#if !defined(GL_UNSIGNED_INT_10F_11F_11F_REV)
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#endif
#if !defined (GL_FLOAT_32_UNSIGNED_INT_24_8_REV)
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV	0x8DAD
#endif

#ifndef GL_ETC1_RGB8_OES
#define GL_ETC1_RGB8_OES				0x8D64
#endif

#if SUPPORT_SOFTWARE_ETC_UNPACK
#ifndef GL_COMPRESSED_R11_EAC
#define GL_COMPRESSED_R11_EAC                            0x9270
#define GL_COMPRESSED_SIGNED_R11_EAC                     0x9271
#define GL_COMPRESSED_RG11_EAC                           0x9272
#define GL_COMPRESSED_SIGNED_RG11_EAC                    0x9273
#define GL_COMPRESSED_RGB8_ETC2                          0x9274
#define GL_COMPRESSED_SRGB8_ETC2                         0x9275
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2      0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2     0x9277
#define GL_COMPRESSED_RGBA8_ETC2_EAC                     0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC				 0x9279
#endif
#ifndef GL_R16_SNORM
#define GL_R16_SNORM					0x8F98
#define GL_RG16_SNORM					0x8F99
#endif
#ifndef GL_RED
#define GL_RED							0x1903
#define GL_GREEN						0x1904
#define GL_BLUE							0x1905
#define GL_RG							0x8227
#define GL_RG_INTEGER					0x8228
#endif
#ifndef GL_R16
#define GL_R16							0x822A
#define GL_RG16							0x822C
#endif
#ifndef GL_RGB8
#define GL_RGB8                         0x8051
#define GL_RGBA8                        0x8058
#endif
#ifndef GL_SRGB8
#define GL_SRGB8						0x8C41
#define GL_SRGB8_ALPHA8					0x8C43
#endif
#endif

#ifndef GL_MAJOR_VERSION
#define GL_MAJOR_VERSION                0x821B
#define GL_MINOR_VERSION                0x821C
#endif

#ifndef GL_CONTEXT_PROFILE_MASK
#define GL_CONTEXT_PROFILE_MASK				 0x9126
#define GL_CONTEXT_CORE_PROFILE_BIT			 0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#endif

#ifndef GL_NUM_EXTENSIONS
#define GL_NUM_EXTENSIONS 0x821D
#endif

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

/* CheckHeader
 * 
 * Reads the KTX file header and performs some sanity checking on the values
 */
KTX_error_code _ktxCheckHeader(KTX_header* header, KTX_texinfo* texinfo);

/*
 * SwapEndian16: Swaps endianness in an array of 16-bit values
 */
void _ktxSwapEndian16(khronos_uint16_t* pData16, int count);

/*
 * SwapEndian32: Swaps endianness in an array of 32-bit values
 */
void _ktxSwapEndian32(khronos_uint32_t* pData32, int count);

/*
 * UncompressETC: uncompresses an ETC compressed texture image
 */
KTX_error_code _ktxUnpackETC(const GLubyte* srcETC, const GLenum srcFormat,
							 khronos_uint32_t active_width, khronos_uint32_t active_height,
							 GLubyte** dstImage,
							 GLenum* format, GLenum* internalFormat, GLenum* type,
							 GLint R16Formats, GLboolean supportsSRGB);

#ifdef __cplusplus
}
#endif

#endif /* KTXINT_H */

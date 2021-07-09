/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

#ifndef KTX_H_A55A6F00956F42F3A137C11929827FE1
#define KTX_H_A55A6F00956F42F3A137C11929827FE1

/**
 * @file
 * @~English
 *
 * @brief Declares the public functions and structures of the
 *        KTX API.
 *
 * @author Georg Kolling, Imagination Technology
 * @author with modifications by Mark Callow, HI Corporation
 *
 * $Id: bcd735e6de9f8497c2cf50ced3800ac401c6d049 $
 * $Date$
 *
 * @todo Find a way so that applications do not have to define KTX_OPENGL{,_ES*}
 *       when using the library.
 */

/*
 * This file copyright (c) 2010 The Khronos Group, Inc.
 */

/*
@~English

LibKTX contains code

@li (c) 2010 The Khronos Group Inc.
@li (c) 2008 and (c) 2010 HI Corporation
@li (c) 2005 Ericsson AB
@li (c) 2003-2010, Troy D. Hanson
@li (c) 2015 Mark Callow

The KTX load tests contain code

@li (c) 2013 The Khronos Group Inc.
@li (c) 2008 and (c) 2010 HI Corporation
@li (c) 1997-2014 Sam Lantinga
@li (c) 2015 Mark Callow

@section default Default License

With the exception of the files listed explicitly below, the source
files are made available under the following BSD-like license. Most
files contain this license explicitly. Some files refer to LICENSE.md
which contains this same text. Such files are licensed under this
Default License.

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

@section hi_mark hi_mark{,_sq}.ktx

The HI logo textures are &copy; & &trade; HI Corporation and are
provided for use only in testing the KTX loader. Any other use requires
specific prior written permission from HI. Furthermore the name HI may
not be used to endorse or promote products derived from this software
without specific prior written permission.

@section uthash uthash.h

uthash.h is made available under the following revised BSD license.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@section SDL2 include/SDL2/

These files are part of the SDL2 source distributed by the [SDL project]
(http://libsdl.org) under the terms of the [zlib license]
(http://www.zlib.net/zlib_license.html).
*/

/**
 * @~English
 * @mainpage The KTX Library
 *
 * @section intro_sec Introduction
 *
 * libktx is a small library of functions for creating KTX (Khronos
 * TeXture) files and instantiating OpenGL&reg; and OpenGL&reg; ES
 * textures from them.
 *
 * For information about the KTX format see the
 * <a href="http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/">
 * formal specification.</a>
 *
 * The library is open source software. Most of the code is licensed under a
 * modified BSD license. The code for unpacking ETC1, ETC2 and EAC compressed
 * textures has a separate license that restricts it to uses associated with
 * Khronos Group APIs. See @ref license for more details.
 *
 * See @ref history for the list of changes.
 *
 * @author Mark Callow, <a href="http://www.hicorp.co.jp">HI Corporation</a>
 * @author Georg Kolling, <a href="http://www.imgtec.com">Imagination Technology</a>
 * @author Jacob Str&ouml;m, <a href="http://www.ericsson.com">Ericsson AB</a>
 *
 * @version 2.0.X
 *
 * $Date$
 */

#include <stdio.h>

#include "KHR/khrplatform.h"
#define KTX_OPENGL 1
#if KTX_OPENGL

    #ifdef _WIN32
      #include <windows.h>
      #undef KTX_USE_GETPROC  /* Must use GETPROC on Windows */
      #define KTX_USE_GETPROC 1
    #else
      #if !defined(KTX_USE_GETPROC)
        #define KTX_USE_GETPROC 0
      #endif
    #endif
    #if KTX_USE_GETPROC
      #include <GL/glew.h>
    #else
      #define GL_GLEXT_PROTOTYPES
      #include <GL/glcorearb.h>
    #endif

    #define GL_APIENTRY APIENTRY
    #define KTX_GLFUNCPTRS "gl_funcptrs.h"

#elif KTX_OPENGL_ES1

    #include <GLES/gl.h>
    #include <GLES/glext.h>

    #define KTX_GLFUNCPTRS "gles1_funcptrs.h"

#elif KTX_OPENGL_ES2

    #define GL_GLEXT_PROTOTYPES
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>

    #define KTX_GLFUNCPTRS "gles2_funcptrs.h"

#elif KTX_OPENGL_ES3

    #define GL_GLEXT_PROTOTYPES
    #include <GLES3/gl3.h>
    #include <GLES2/gl2ext.h>

    #define KTX_GLFUNCPTRS "gles3_funcptrs.h"

#else
#error Please #define one of KTX_OPENGL, KTX_OPENGL_ES1, KTX_OPENGL_ES2 or KTX_OPENGL_ES3 as 1
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Key String for standard orientation value.
 */
#define KTX_ORIENTATION_KEY    "KTXorientation"
/**
 * @brief Standard format for 2D orientation value.
 */
#define KTX_ORIENTATION2_FMT "S=%c,T=%c"
/**
 * @brief Standard format for 3D orientation value.
 */
#define KTX_ORIENTATION3_FMT "S=%c,T=%c,R=%c"
/**
 * @brief Required unpack alignment
 */
#define KTX_GL_UNPACK_ALIGNMENT 4

/**
 * @brief Error codes returned by library functions.
 */
typedef enum KTX_error_code_t {
    KTX_SUCCESS = 0,         /*!< Operation was successful. */
    KTX_FILE_OPEN_FAILED,     /*!< The target file could not be opened. */
    KTX_FILE_WRITE_ERROR,    /*!< An error occurred while writing to the file. */
    KTX_GL_ERROR,            /*!< GL operations resulted in an error. */
    KTX_INVALID_OPERATION,   /*!< The operation is not allowed in the current state. */
    KTX_INVALID_VALUE,         /*!< A parameter value was not valid */
    KTX_NOT_FOUND,             /*!< Requested key was not found */
    KTX_OUT_OF_MEMORY,       /*!< Not enough memory to complete the operation. */
    KTX_UNEXPECTED_END_OF_FILE, /*!< The file did not contain enough data */
    KTX_UNKNOWN_FILE_FORMAT, /*!< The file not a KTX file */
    KTX_UNSUPPORTED_TEXTURE_TYPE, /*!< The KTX file specifies an unsupported texture type. */
} KTX_error_code;

/**
 * @brief structure used to pass information about the texture to ktxWriteKTX
 */
typedef struct KTX_texture_info_t
{
    /**
     * @brief The type of the image data.
     *
     * Values are the same as in the @p type parameter of
     * glTexImage*D. Must be 0 for compressed images.
     */
    khronos_uint32_t glType;
    /**
     * @brief The data type size to be used in case of endianness
     *        conversion.
     *
     * This value is used in the event conversion is required when the
     * KTX file is loaded. It should be the size in bytes corresponding
     * to glType. Must be 1 for compressed images.
     */
    khronos_uint32_t glTypeSize;
    /**
     * @brief The format of the image(s).
     *
     * Values are the same as in the format parameter
     * of glTexImage*D. Must be 0 for compressed images.
     */
    khronos_uint32_t glFormat;
    /** @brief The internalformat of the image(s).
     *
     * Values are the same as for the internalformat parameter of
     * glTexImage*2D. Note: it will not be used when a KTX file
     * containing an uncompressed texture is loaded into OpenGL ES.
     */
    khronos_uint32_t glInternalFormat;
    /** @brief The base internalformat of the image(s)
     *
     * For non-compressed textures, should be the same as glFormat.
     * For compressed textures specifies the base internal, e.g.
     * GL_RGB, GL_RGBA.
     */
    khronos_uint32_t glBaseInternalFormat;
    /** @brief Width of the image for texture level 0, in pixels. */
    khronos_uint32_t pixelWidth;
    /** @brief Height of the texture image for level 0, in pixels.
     *
     * Must be 0 for 1D textures.
     */
     khronos_uint32_t pixelHeight;
    /** @brief Depth of the texture image for level 0, in pixels.
     *
     * Must be 0 for 1D, 2D and cube textures.
     */
    khronos_uint32_t pixelDepth;
    /** @brief The number of array elements.
     *
     * Must be 0 if not an array texture.
     */
    khronos_uint32_t numberOfArrayElements;
    /** @brief The number of cubemap faces.
     *
     * Must be 6 for cubemaps and cubemap arrays, 1 otherwise. Cubemap
     * faces must be provided in the order: +X, -X, +Y, -Y, +Z, -Z.
     */
    khronos_uint32_t numberOfFaces;
    /** @brief The number of mipmap levels.
     *
     * 1 for non-mipmapped texture. 0 indicates that a full mipmap pyramid should
     * be generated from level 0 at load time (this is usually not allowed for
     * compressed formats). Mipmaps must be provided in order from largest size to
     * smallest size. The first mipmap level is always level 0.
     */
    khronos_uint32_t numberOfMipmapLevels;
} KTX_texture_info;


/**
 * @brief Structure used to pass image data to ktxWriteKTX.
 */
typedef struct KTX_image_info {
    GLsizei size;    /*!< Size of the image data in bytes. */
    GLubyte* data;  /*!< Pointer to the image data. */
} KTX_image_info;


/**
 * @brief Structure used to return texture dimensions
 */
typedef struct KTX_dimensions {
    GLsizei width;  /*!< */
    GLsizei height; /*!< */
    GLsizei depth;  /*!< */
} KTX_dimensions;

/**
 * @brief Opaque handle to a KTX_hash_table.
 */
typedef void* KTX_hash_table;

/* ktxLoadTextureF
 *
 * Loads a texture from a stdio FILE.
 */
KTX_error_code
ktxLoadTextureF(FILE*, GLuint* pTexture, GLenum* pTarget,
                KTX_dimensions* pDimensions, GLboolean* pIsMipmapped,
                GLenum* pGlerror,
                unsigned int* pKvdLen, unsigned char** ppKvd);

/* ktxLoadTextureN
 *
 * Loads a texture from a KTX file on disk.
 */
KTX_error_code
ktxLoadTextureN(const char* const filename, GLuint* pTexture, GLenum* pTarget,
                KTX_dimensions* pDimensions, GLboolean* pIsMipmapped,
                GLenum* pGlerror,
                unsigned int* pKvdLen, unsigned char** ppKvd);

/* ktxLoadTextureM
 *
 * Loads a texture from a KTX file in memory.
 */
KTX_error_code
ktxLoadTextureM(const void* bytes, GLsizei size, GLuint* pTexture, GLenum* pTarget,
                KTX_dimensions* pDimensions, GLboolean* pIsMipmapped,
                GLenum* pGlerror,
                unsigned int* pKvdLen, unsigned char** ppKvd);

/* ktxWriteKTXF
 *
 * Writes a KTX file using supplied data.
 */
KTX_error_code
ktxWriteKTXF(FILE*, const KTX_texture_info* imageInfo,
             GLsizei bytesOfKeyValueData, const void* keyValueData,
             GLuint numImages, KTX_image_info images[]);

/* ktxWriteKTXN
 *
 * Writes a KTX file using supplied data.
 */
KTX_error_code
ktxWriteKTXN(const char* dstname, const KTX_texture_info* imageInfo,
             GLsizei bytesOfKeyValueData, const void* keyValueData,
             GLuint numImages, KTX_image_info images[]);

/* ktxWriteKTXM
 *
 * Writes a KTX file into memory using supplied data.
 */
KTX_error_code
ktxWriteKTXM(unsigned char** bytes, GLsizei* size, const KTX_texture_info* textureInfo,
             GLsizei bytesOfKeyValueData, const void* keyValueData,
             GLuint numImages, KTX_image_info images[]);

/* ktxErrorString()
 *
 * Returns a string corresponding to a KTX error code.
 */
const char* const ktxErrorString(KTX_error_code error);

/* ktxHashTable_Create()
 *
 * Creates a key-value hash table
 */
KTX_hash_table ktxHashTable_Create();

/* ktxHashTable_Destroy()
 *
 * Destroys a key-value hash table
 */
void ktxHashTable_Destroy(KTX_hash_table This);

/* ktxHashTable_AddKVPair()
 *
 * Adds a key-value pair to a hash table.
 */
KTX_error_code
ktxHashTable_AddKVPair(KTX_hash_table This, const char* key,
                       unsigned int valueLen, const void* value);


/* ktxHashTable_FindValue()
 *
 * Looks up a key and returns the value.
 */
KTX_error_code
ktxHashTable_FindValue(KTX_hash_table This, const char* key,
                       unsigned int* pValueLen, void** pValue);


/* ktxHashTable_Serialize()
 *
 * Serializes the hash table to a block of memory suitable for
 * writing to a KTX file.
 */
KTX_error_code
ktxHashTable_Serialize(KTX_hash_table This, unsigned int* kvdLen, unsigned char** kvd);


/* ktxHashTable_Deserialize()
 *
 * Creates a hash table from the serialized data read from a
 * a KTX file.
 */
KTX_error_code
ktxHashTable_Deserialize(unsigned int kvdLen, void* kvd, KTX_hash_table* pKvt);


#ifdef __cplusplus
}
#endif

/**
@page history KTX Library Revision History

@section v5 Version 2.0.X
Changed:
@li New build system

@section v4 Version 2.0.1
Added:
@li CMake build files. Thanks to Pavel Rotjberg for the initial version.

Changed:
@li ktxWriteKTXF to check the validity of the type & format combinations
    passed to it.

Fixed:
@li Public Bugzilla <a href="http://www.khronos.org/bugzilla/show_bug.cgi?id=999">999</a>: 16-bit luminance texture cannot be written.
@li compile warnings from compilers stricter than MS Visual C++. Thanks to
    Pavel Rotjberg.

@section v3 Version 2.0
Added:
@li support for decoding ETC2 and EAC formats in the absence of a hardware
    decoder.
@li support for converting textures with legacy LUMINANCE, LUMINANCE_ALPHA,
    etc. formats to the equivalent R, RG, etc. format with an
    appropriate swizzle, when loading in OpenGL Core Profile contexts.
@li ktxErrorString function to return a string corresponding to an error code.
@li    tests for ktxLoadTexture[FN] that run under OpenGL ES 3.0 and OpenGL 3.3.
    The latter includes an EGL on WGL wrapper that makes porting apps between
    OpenGL ES and OpenGL easier on Windows.
@li more texture formats to ktxLoadTexture[FN] and toktx tests.

Changed:
@li ktxLoadTexture[FMN] to discover the capabilities of the GL context at
    run time and load textures, or not, according to those capabilities.

Fixed:
@li failure of ktxWriteKTXF to pad image rows to 4 bytes as required by the KTX
    format.
@li ktxWriteKTXF exiting with KTX_FILE_WRITE_ERROR when attempting to write
    more than 1 byte of face-LOD padding.

Although there is only a very minor API change, the addition of ktxErrorString, the functional changes
are large enough to justify bumping the major revision number.

@section v2 Version 1.0.1
Implemented ktxLoadTextureM.
Fixed the following:
@li Public Bugzilla <a href="http://www.khronos.org/bugzilla/show_bug.cgi?id=571">571</a>: crash when null passed for pIsMipmapped.
@li Public Bugzilla <a href="http://www.khronos.org/bugzilla/show_bug.cgi?id=572">572</a>: memory leak when unpacking ETC textures.
@li Public Bugzilla <a href="http://www.khronos.org/bugzilla/show_bug.cgi?id=573">573</a>: potential crash when unpacking ETC textures with unused padding pixels.
@li Public Bugzilla <a href="http://www.khronos.org/bugzilla/show_bug.cgi?id=576">576</a>: various small fixes.

Thanks to Krystian Bigaj for the ktxLoadTextureM implementation and these fixes.

@section v1 Version 1.0
Initial release.

*/

#endif /* KTX_H_A55A6F00956F42F3A137C11929827FE1 */

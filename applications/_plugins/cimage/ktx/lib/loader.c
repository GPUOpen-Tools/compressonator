/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: a1d73255a63f3e703c641f61bafba89453f05efd $ */

/**
 * @file
 * @~English
 *
 * @brief Functions for instantiating GL or GLES textures from KTX files.
 *
 * @author Georg Kolling, Imagination Technology
 * @author Mark Callow, HI Corporation
 */

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
work of the Khronos Group".

THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "KHR/khrplatform.h"
#include "ktx.h"
#include "ktxint.h"
#include "ktxstream.h"
#include "ktxfilestream.h"
#include "ktxmemstream.h"

#include KTX_GLFUNCPTRS

DECLARE_GL_FUNCPTRS

/**
 * @internal
 * @~English
 * @brief Additional contextProfile bit indicating an OpenGL ES context.
 *
 * This is the same value NVIDIA returns when using an OpenGL ES profile
 * of their desktop drivers. However it is not specified in any official
 * specification as OpenGL ES does not support the GL_CONTEXT_PROFILE_MASK
 * query.
 */
#define _CONTEXT_ES_PROFILE_BIT 0x4

/**
 * @internal
 * @~English
 * @name Supported Sized Format Macros
 *
 * These macros describe values that may be used with the sizedFormats
 * variable.
 */
/**@{*/
#define _NON_LEGACY_FORMATS 0x1 /*< @internal Non-legacy sized formats are supported. */
#define _LEGACY_FORMATS 0x2  /*< @internal Legacy sized formats are supported. */
/**
 * @internal
 * @~English
 * @brief All sized formats are supported
 */
#define _ALL_SIZED_FORMATS (_NON_LEGACY_FORMATS | _LEGACY_FORMATS)
#define _NO_SIZED_FORMATS 0 /*< @internal No sized formats are supported. */
/**@}*/

/**
 * @internal
 * @~English
 * @brief indicates the profile of the current context.
 */
static GLint contextProfile = 0;
/**
 * @internal
 * @~English
 * @brief Indicates what sized texture formats are supported
 *        by the current context.
 */
static GLint sizedFormats = _ALL_SIZED_FORMATS;
static GLboolean supportsSwizzle = GL_TRUE;
/**
 * @internal
 * @~English
 * @brief Indicates which R16 & RG16 formats are supported by the current context.
 */
static GLint R16Formats = _KTX_ALL_R16_FORMATS;
/**
 * @internal
 * @~English
 * @brief Indicates if the current context supports sRGB textures.
 */
static GLboolean supportsSRGB = GL_TRUE;
/**
 * @internal
 * @~English
 * @brief Indicates if the current context supports cube map arrays.
 */
static GLboolean supportsCubeMapArrays = GL_FALSE;

/**
 * @internal
 * @~English
 * @brief Workaround mismatch of glGetString declaration and standard string
 *        function parameters.
 */
#define glGetString(x) (const char*)glGetString(x)

/**
* @internal
* @~English
* @brief Workaround mismatch of glGetStringi declaration and standard string
*        function parameters.
*/
#define pfGlGetStringi(x,y) (const char*)pfGlGetStringi(x,y)

/**
 * @internal
 * @~English
 * @brief Check for existence of OpenGL extension
 */
static GLboolean
hasExtension(const char* extension)
{
	if (pfGlGetStringi == NULL) {
		if (strstr(glGetString(GL_EXTENSIONS), extension) != NULL)
			return GL_TRUE;
		else
			return GL_FALSE;
	}
	else {
		int i, n;

		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		for (i = 0; i < n; i++) {
			if (strcmp(pfGlGetStringi(GL_EXTENSIONS, i), extension) == 0)
				return GL_TRUE;
		}
		return GL_FALSE;
	}
}

/**
 * @internal
 * @~English
 * @brief Discover the capabilities of the current GL context.
 *
 * Queries the context and sets several the following internal variables indicating
 * the capabilities of the context:
 *
 * @li sizedFormats
 * @li supportsSwizzle
 * @li supportsSRGB
 * @li b16Formats
 *
 */
static void discoverContextCapabilities(void)
{
	GLint majorVersion = 1;
	GLint minorVersion = 0;

	// Done here so things will work when GLEW, or equivalent, is being used
	// and GL function names are defined as pointers. Initialization at
	// declaration would happen before these pointers have been initialized.
	INITIALIZE_GL_FUNCPTRS

	if (strstr(glGetString(GL_VERSION), "GL ES") != NULL)
		contextProfile = _CONTEXT_ES_PROFILE_BIT;
	// MAJOR & MINOR only introduced in GL {,ES} 3.0
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	if (glGetError() != GL_NO_ERROR) {
		// < v3.0; resort to the old-fashioned way.
		if (contextProfile & _CONTEXT_ES_PROFILE_BIT)
			sscanf(glGetString(GL_VERSION), "OpenGL ES %d.%d ",
                   &majorVersion, &minorVersion);
		else
			sscanf(glGetString(GL_VERSION), "OpenGL %d.%d ",
                   &majorVersion, &minorVersion);
	}
	if (contextProfile & _CONTEXT_ES_PROFILE_BIT) {
		if (majorVersion < 3) {
			supportsSwizzle = GL_FALSE;
			sizedFormats = _NO_SIZED_FORMATS;
			R16Formats = _KTX_NO_R16_FORMATS;
			supportsSRGB = GL_FALSE;
		} else {
			sizedFormats = _NON_LEGACY_FORMATS;
            if (hasExtension("GL_EXT_texture_cube_map_array")) {
                supportsCubeMapArrays = GL_TRUE;
            }
		}
		if (hasExtension("GL_OES_required_internalformat")) {
			sizedFormats |= _ALL_SIZED_FORMATS;
		}
		// There are no OES extensions for sRGB textures or R16 formats.
	} else {
		// PROFILE_MASK was introduced in OpenGL 3.2.
		// Profiles: CONTEXT_CORE_PROFILE_BIT 0x1, CONTEXT_COMPATIBILITY_PROFILE_BIT 0x2.
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &contextProfile);
		if (glGetError() == GL_NO_ERROR) {
			// >= 3.2
			if (majorVersion == 3 && minorVersion < 3)
				supportsSwizzle = GL_FALSE;
			if ((contextProfile & GL_CONTEXT_CORE_PROFILE_BIT))
				sizedFormats &= ~_LEGACY_FORMATS;
            if (majorVersion >= 4)
                supportsCubeMapArrays = GL_TRUE;
        } else {
			// < 3.2
			contextProfile = GL_CONTEXT_COMPATIBILITY_PROFILE_BIT;
			supportsSwizzle = GL_FALSE;
			// sRGB textures introduced in 2.0
			if (majorVersion < 2 && hasExtension("GL_EXT_texture_sRGB")) {
				supportsSRGB = GL_FALSE;
			}
			// R{,G]16 introduced in 3.0; R{,G}16_SNORM introduced in 3.1.
			if (majorVersion == 3) {
				if (minorVersion == 0)
					R16Formats &= ~_KTX_R16_FORMATS_SNORM;
			} else if (strstr(glGetString(GL_EXTENSIONS), "GL_ARB_texture_rg") != NULL) {
				R16Formats &= ~_KTX_R16_FORMATS_SNORM;
			} else {
				R16Formats = _KTX_NO_R16_FORMATS;
			}
		}
        if (!supportsCubeMapArrays) {
            if (hasExtension("GL_ARB_texture_cube_map_array")) {
                supportsCubeMapArrays = GL_TRUE;
            }
        }
	}
}

#if SUPPORT_LEGACY_FORMAT_CONVERSION
/**
 * @internal
 * @~English
 * @brief Convert deprecated legacy-format texture to modern format.
 *
 * The function sets the GL_TEXTURE_SWIZZLEs necessary to get the same
 * behavior as the legacy format.
 *
 * @param [in] target       texture target on which the swizzle will
 *                          be set.
 * @param [in, out] pFormat pointer to variable holding the base format of the
 *                          texture. The new base format is written here.
 * @param [in, out] pInternalFormat  pointer to variable holding the internalformat
 *                                   of the texture. The new internalformat is
 *                                   written here.
 * @return void unrecognized formats will be passed on to OpenGL. Any loading error
 *              that arises will be handled in the usual way.
 */
static void convertFormat(GLenum target, GLenum* pFormat, GLenum* pInternalFormat) {
	switch (*pFormat) {
	  case GL_ALPHA:
		{
		  GLint swizzle[] = {GL_ZERO, GL_ZERO, GL_ZERO, GL_RED};
		  *pFormat = GL_RED;
		  glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
		  switch (*pInternalFormat) {
	        case GL_ALPHA:
		    case GL_ALPHA4:
		    case GL_ALPHA8:
			  *pInternalFormat = GL_R8;
			  break;
		    case GL_ALPHA12:
		    case GL_ALPHA16:
			  *pInternalFormat = GL_R16;
			  break;
		  }
		}
	  case GL_LUMINANCE:
		{
		  GLint swizzle[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
		  *pFormat = GL_RED;
		  glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
		  switch (*pInternalFormat) {
			case GL_LUMINANCE:
		    case GL_LUMINANCE4:
		    case GL_LUMINANCE8:
			  *pInternalFormat = GL_R8;
			  break;
		    case GL_LUMINANCE12:
		    case GL_LUMINANCE16:
			  *pInternalFormat = GL_R16;
			  break;
#if 0
		    // XXX Must avoid setting TEXTURE_SWIZZLE in these cases
            // XXX Must manually swizzle.
			case GL_SLUMINANCE:
			case GL_SLUMINANCE8:
			  *pInternalFormat = GL_SRGB8;
			  break;
#endif
		  }
		  break;
		}
	  case GL_LUMINANCE_ALPHA:
		{
		  GLint swizzle[] = {GL_RED, GL_RED, GL_RED, GL_GREEN};
		  *pFormat = GL_RG;
		  glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
		  switch (*pInternalFormat) {
			case GL_LUMINANCE_ALPHA:
		    case GL_LUMINANCE4_ALPHA4:
			case GL_LUMINANCE6_ALPHA2:
		    case GL_LUMINANCE8_ALPHA8:
			  *pInternalFormat = GL_RG8;
			  break;
		    case GL_LUMINANCE12_ALPHA4:
			case GL_LUMINANCE12_ALPHA12:
		    case GL_LUMINANCE16_ALPHA16:
			  *pInternalFormat = GL_RG16;
			  break;
#if 0
		    // XXX Must avoid setting TEXTURE_SWIZZLE in these cases
            // XXX Must manually swizzle.
			case GL_SLUMINANCE_ALPHA:
			case GL_SLUMINANCE8_ALPHA8:
			  *pInternalFormat = GL_SRGB8_ALPHA8;
			  break;
#endif
		  }
		  break;
		}
	  case GL_INTENSITY:
		{
		  GLint swizzle[] = {GL_RED, GL_RED, GL_RED, GL_RED};
		  *pFormat = GL_RED;
		  glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
		  switch (*pInternalFormat) {
			case GL_INTENSITY:
		    case GL_INTENSITY4:
		    case GL_INTENSITY8:
			  *pInternalFormat = GL_R8;
			  break;
		    case GL_INTENSITY12:
		    case GL_INTENSITY16:
			  *pInternalFormat = GL_R16;
			  break;
		  }
		  break;
		}
	  default:
	    break;
	}
}
#endif /* SUPPORT_LEGACY_FORMAT_CONVERSION */


/*
 * @~English
 * @brief Load a GL texture object from a ktxStream.
 *
 * The function sets the texture object's GL_TEXTURE_MAX_LEVEL parameter
 * according to the number of levels in the ktxStream, provided the library
 * has been compiled with a version of gl.h where GL_TEXTURE_MAX_LEVEL is
 * defined.
 *
 * It will unpack compressed GL_ETC1_RGB8_OES and GL_ETC2_* format
 * textures in software when the format is not supported by the GL context,
 * provided the library has been compiled with SUPPORT_SOFTWARE_ETC_UNPACK
 * defined as 1.
 *
 * It will also convert textures with legacy formats to their modern equivalents
 * when the format is not supported by the GL context, provided the library
 * has been compiled with SUPPORT_LEGACY_FORMAT_CONVERSION defined as 1.
 *
 * @param [in] stream		pointer to the ktxStream from which to load.
 * @param [in,out] pTexture	name of the GL texture to load. If NULL or if
 *                          <tt>*pTexture == 0</tt> the function will generate
 *                          a texture name. The function binds either the
 *                          generated name or the name given in @p *pTexture
 * 						    to the texture target returned in @p *pTarget,
 * 						    before loading the texture data. If @p pTexture
 *                          is not NULL and a name was generated, the generated
 *                          name will be returned in *pTexture.
 * @param [out] pTarget 	@p *pTarget is set to the texture target used. The
 * 						    target is chosen based on the file contents.
 * @param [out] pDimensions	If @p pDimensions is not NULL, the width, height and
 *							depth of the texture's base level are returned in the
 *                          fields of the KTX_dimensions structure to which it points.
 * @param [out] pIsMipmapped
 *	                        If @p pIsMipmapped is not NULL, @p *pIsMipmapped is set
 *                          to GL_TRUE if the KTX texture is mipmapped, GL_FALSE
 *                          otherwise.
 * @param [out] pGlerror    @p *pGlerror is set to the value returned by
 *                          glGetError when this function returns the error
 *                          KTX_GL_ERROR. glerror can be NULL.
 * @param [in,out] pKvdLen	If not NULL, @p *pKvdLen is set to the number of bytes
 *                          of key-value data pointed at by @p *ppKvd. Must not be
 *                          NULL, if @p ppKvd is not NULL.
 * @param [in,out] ppKvd	If not NULL, @p *ppKvd is set to the point to a block of
 *                          memory containing key-value data read from the file.
 *                          The application is responsible for freeing the memory.
 *
 *
 * @return	KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE @p target is @c NULL or the size of a mip
 * 							    level is greater than the size of the
 * 							    preceding level.
 * @exception KTX_INVALID_OPERATION @p ppKvd is not NULL but pKvdLen is NULL.
 * @exception KTX_UNEXPECTED_END_OF_FILE the file does not contain the
 * 										 expected amount of data.
 * @exception KTX_OUT_OF_MEMORY Sufficient memory could not be allocated to store
 *                              the requested key-value data.
 * @exception KTX_GL_ERROR      A GL error was raised by glBindTexture,
 * 								glGenTextures or gl*TexImage*. The GL error
 *                              will be returned in @p *glerror, if glerror
 *                              is not @c NULL.
 */
static
KTX_error_code
ktxLoadTextureS(struct ktxStream* stream, GLuint* pTexture, GLenum* pTarget,
				KTX_dimensions* pDimensions, GLboolean* pIsMipmapped,
				GLenum* pGlerror,
				unsigned int* pKvdLen, unsigned char** ppKvd)
{
	GLint				previousUnpackAlignment;
	KTX_header			header;
	KTX_texinfo			texinfo;
	void*				data = NULL;
	khronos_uint32_t	dataSize = 0;
	GLuint				texname;
	int					texnameUser;
	khronos_uint32_t    faceLodSize;
	khronos_uint32_t    faceLodSizeRounded;
	khronos_uint32_t	level;
	khronos_uint32_t	face;
	GLenum				glFormat, glInternalFormat;
	KTX_error_code		errorCode = KTX_SUCCESS;
	GLenum				errorTmp;

	if (pGlerror)
		*pGlerror = GL_NO_ERROR;

	if (ppKvd) {
		*ppKvd = NULL;
	}

	if (!stream || !stream->read || !stream->skip) {
		return KTX_INVALID_VALUE;
	}

	if (!pTarget) {
		return KTX_INVALID_VALUE;
	}

	errorCode = stream->read(&header, KTX_HEADER_SIZE, stream->src);
	if (errorCode != KTX_SUCCESS)
		return errorCode;

	errorCode = _ktxCheckHeader(&header, &texinfo);
	if (errorCode != KTX_SUCCESS)
		return errorCode;

	//if (ppKvd) {
	//	if (pKvdLen == NULL)
	//		return KTX_INVALID_OPERATION;
	//	*pKvdLen = header.bytesOfKeyValueData;
	//	if (*pKvdLen) {
	//		*ppKvd = (unsigned char*)malloc(*pKvdLen);
	//		if (*ppKvd == NULL)
	//			return KTX_OUT_OF_MEMORY;
	//		errorCode = stream->read(*ppKvd, *pKvdLen, stream->src);
	//		if (errorCode != KTX_SUCCESS)
	//		{
	//			free(*ppKvd);
	//			*ppKvd = NULL;
    //
	//			return errorCode;
	//		}
	//	}
	//} else {
		/* skip key/value metadata */
		errorCode = stream->skip(header.bytesOfKeyValueData, stream->src);
		if (errorCode != KTX_SUCCESS) {
			return errorCode;
		}
	//}

	if (contextProfile == 0)
		discoverContextCapabilities();

	/* KTX files require an unpack alignment of 4 */
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
	if (previousUnpackAlignment != KTX_GL_UNPACK_ALIGNMENT) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, KTX_GL_UNPACK_ALIGNMENT);
	}

	texnameUser = pTexture && *pTexture;
	if (texnameUser) {
		texname = *pTexture;
	} else {
		glGenTextures(1, &texname);
	}
	glBindTexture(texinfo.glTarget, texname);
    
    /* load as 2D texture if 1D textures are not supported */
    if (texinfo.textureDimensions == 1 &&
        ((texinfo.compressed && (pfGlCompressedTexImage1D == NULL)) ||
         (!texinfo.compressed && (pfGlTexImage1D == NULL))))
    {
        texinfo.textureDimensions = 2;
        texinfo.glTarget = GL_TEXTURE_2D;
        header.pixelHeight = 1;
    }
    
    if (header.numberOfArrayElements > 0)
    {
        if (texinfo.glTarget == GL_TEXTURE_1D)
        {
            texinfo.glTarget = GL_TEXTURE_1D_ARRAY_EXT;
        }
        else if (texinfo.glTarget == GL_TEXTURE_2D)
        {
            texinfo.glTarget = GL_TEXTURE_2D_ARRAY_EXT;
        }
        else if (texinfo.glTarget == GL_TEXTURE_CUBE_MAP)
        {
            texinfo.glTarget = GL_TEXTURE_CUBE_MAP_ARRAY;
        }
        else
        {
            /* No API for 3D arrays yet */
            return KTX_UNSUPPORTED_TEXTURE_TYPE;
        }
        texinfo.textureDimensions++;
    }
    
    /* Reject cube map arrays if unsupported. */
    if (texinfo.glTarget == GL_TEXTURE_CUBE_MAP_ARRAY && !supportsCubeMapArrays)
    {
        return KTX_UNSUPPORTED_TEXTURE_TYPE;        
    }
    
    /* Reject 3D texture if unsupported. */
    if (texinfo.textureDimensions == 3 &&
        ((texinfo.compressed && (pfGlCompressedTexImage3D == NULL)) ||
         (!texinfo.compressed && (pfGlTexImage3D == NULL))))
    {
        return KTX_UNSUPPORTED_TEXTURE_TYPE;
    }

	// Prefer glGenerateMipmaps over GL_GENERATE_MIPMAP
	if (texinfo.generateMipmaps && (pfGlGenerateMipmap == NULL)) {
		glTexParameteri(texinfo.glTarget, GL_GENERATE_MIPMAP, GL_TRUE);
	}
#ifdef GL_TEXTURE_MAX_LEVEL
	if (!texinfo.generateMipmaps)
		glTexParameteri(texinfo.glTarget, GL_TEXTURE_MAX_LEVEL, header.numberOfMipmapLevels - 1);
#endif

	if (texinfo.glTarget == GL_TEXTURE_CUBE_MAP) {
		texinfo.glTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	}

	glInternalFormat = header.glInternalFormat;
	glFormat = header.glFormat;
	if (!texinfo.compressed) {
#if SUPPORT_LEGACY_FORMAT_CONVERSION
		// If sized legacy formats are supported there is no need to convert.
		// If only unsized formats are supported, there is no point in converting
		// as the modern formats aren't supported either.
		if (sizedFormats == _NON_LEGACY_FORMATS && supportsSwizzle) {
			convertFormat(texinfo.glTarget, &glFormat, &glInternalFormat);
			errorTmp = glGetError();
		} else if (sizedFormats == _NO_SIZED_FORMATS)
			glInternalFormat = header.glBaseInternalFormat;
#else
		// When no sized formats are supported, or legacy sized formats are not
		// supported, must change internal format.
		if (sizedFormats == _NO_SIZED_FORMATS
			|| (!(sizedFormats & _LEGACY_FORMATS) &&
				(header.glBaseInternalFormat == GL_ALPHA
				|| header.glBaseInternalFormat == GL_LUMINANCE
				|| header.glBaseInternalFormat == GL_LUMINANCE_ALPHA
				|| header.glBaseInternalFormat == GL_INTENSITY))) {
			glInternalFormat = header.glBaseInternalFormat;
		}
#endif
	}

	for (level = 0; level < header.numberOfMipmapLevels; ++level)
	{
		GLsizei pixelWidth  = MAX(1, header.pixelWidth  >> level);
		GLsizei pixelHeight = MAX(1, header.pixelHeight >> level);
		GLsizei pixelDepth  = MAX(1, header.pixelDepth  >> level);

		errorCode = stream->read(&faceLodSize, sizeof(khronos_uint32_t), stream->src);
		if (errorCode != KTX_SUCCESS) {
			goto cleanup;
		}
		if (header.endianness == KTX_ENDIAN_REF_REV) {
			_ktxSwapEndian32(&faceLodSize, 1);
		}
		faceLodSizeRounded = (faceLodSize + 3) & ~(khronos_uint32_t)3;
		if (!data) {
			/* allocate memory sufficient for the first level */
			data = malloc(faceLodSizeRounded);
			if (!data) {
				errorCode = KTX_OUT_OF_MEMORY;
				goto cleanup;
			}
			dataSize = faceLodSizeRounded;
		}
		else if (dataSize < faceLodSizeRounded) {
			/* subsequent levels cannot be larger than the first level */
			errorCode = KTX_INVALID_VALUE;
			goto cleanup;
		}

		for (face = 0; face < header.numberOfFaces; ++face)
		{
			errorCode = stream->read(data, faceLodSizeRounded, stream->src);
			if (errorCode != KTX_SUCCESS) {
				goto cleanup;
			}

			/* Perform endianness conversion on texture data */
			if (header.endianness == KTX_ENDIAN_REF_REV && header.glTypeSize == 2) {
				_ktxSwapEndian16((khronos_uint16_t*)data, faceLodSize / 2);
			}
			else if (header.endianness == KTX_ENDIAN_REF_REV && header.glTypeSize == 4) {
				_ktxSwapEndian32((khronos_uint32_t*)data, faceLodSize / 4);
			}

			if (texinfo.textureDimensions == 1) {
                assert(pfGlCompressedTexImage1D != NULL && pfGlTexImage1D != NULL);
				if (texinfo.compressed) {
					pfGlCompressedTexImage1D(texinfo.glTarget + face, level,
						glInternalFormat, pixelWidth, 0,
						faceLodSize, data);
				} else {
					pfGlTexImage1D(texinfo.glTarget + face, level,
						glInternalFormat, pixelWidth, 0,
						glFormat, header.glType, data);
				}
			} else if (texinfo.textureDimensions == 2) {
				if (header.numberOfArrayElements) {
					pixelHeight = header.numberOfArrayElements;
				}
				if (texinfo.compressed) {
				    // It is simpler to just attempt to load the format, rather than divine which
					// formats are supported by the implementation. In the event of an error,
					// software unpacking can be attempted.
					glCompressedTexImage2D(texinfo.glTarget + face, level,
						glInternalFormat, pixelWidth, pixelHeight, 0,
						faceLodSize, data);
				} else {
					errorTmp = glGetError();
					glTexImage2D(texinfo.glTarget + face, level,
						glInternalFormat, pixelWidth, pixelHeight, 0,
						glFormat, header.glType, data);
				}
			} else if (texinfo.textureDimensions == 3) {
                assert(pfGlCompressedTexImage3D != NULL && pfGlTexImage3D != NULL);
				if (header.numberOfArrayElements) {
					pixelDepth = header.numberOfArrayElements;
				}
				if (texinfo.compressed) {
					pfGlCompressedTexImage3D(texinfo.glTarget + face, level,
						glInternalFormat, pixelWidth, pixelHeight, pixelDepth, 0,
						faceLodSize, data);
				} else {
					pfGlTexImage3D(texinfo.glTarget + face, level,
						glInternalFormat, pixelWidth, pixelHeight, pixelDepth, 0,
						glFormat, header.glType, data);
				}
			}

			errorTmp = glGetError();
#if SUPPORT_SOFTWARE_ETC_UNPACK
			// Renderion is returning INVALID_VALUE. Oops!!
			if ((errorTmp == GL_INVALID_ENUM || errorTmp == GL_INVALID_VALUE)
				&& texinfo.compressed
				&& texinfo.textureDimensions == 2
				&& (glInternalFormat == GL_ETC1_RGB8_OES || (glInternalFormat >= GL_COMPRESSED_R11_EAC && glInternalFormat <= GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC)))
			{
				GLubyte* unpacked;
				GLenum format, internalFormat, type;

				errorCode = _ktxUnpackETC((GLubyte*)data, glInternalFormat, pixelWidth, pixelHeight,
					                      &unpacked, &format, &internalFormat, &type,
										  R16Formats, supportsSRGB);
				if (errorCode != KTX_SUCCESS) {
					goto cleanup;
				}
				if (!sizedFormats & _NON_LEGACY_FORMATS) {
					if (internalFormat == GL_RGB8)
						internalFormat = GL_RGB;
					else if (internalFormat == GL_RGBA8)
						internalFormat = GL_RGBA;
				}
				glTexImage2D(texinfo.glTarget + face, level,
							 internalFormat, pixelWidth, pixelHeight, 0,
							 format, type, unpacked);

				free(unpacked);
				errorTmp = glGetError();
			}
#endif
            if (errorTmp != GL_NO_ERROR) {
				if (pGlerror)
					*pGlerror = errorTmp;
				errorCode = KTX_GL_ERROR;
				goto cleanup;
			}
		}
	}

cleanup:
	free(data);

	/* restore previous GL state */
	if (previousUnpackAlignment != KTX_GL_UNPACK_ALIGNMENT) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);
	}

	if (errorCode == KTX_SUCCESS)
	{
		if (texinfo.generateMipmaps && pfGlGenerateMipmap) {
			pfGlGenerateMipmap(texinfo.glTarget);
		}
		*pTarget = texinfo.glTarget;
		if (pTexture) {
			*pTexture = texname;
		}
		if (pDimensions) {
			pDimensions->width = header.pixelWidth;
			pDimensions->height = header.pixelHeight;
			pDimensions->depth = header.pixelDepth;
		}
		if (pIsMipmapped) {
			if (texinfo.generateMipmaps || header.numberOfMipmapLevels > 1)
				*pIsMipmapped = GL_TRUE;
			else
				*pIsMipmapped = GL_FALSE;
		}
	} else {
		if (ppKvd && *ppKvd)
		{
			free(*ppKvd);
			*ppKvd = NULL;
		}

		if (!texnameUser) {
			glDeleteTextures(1, &texname);
		}
	}
	return errorCode;
}

/**
 * @~English
 * @brief Load a GL texture object from a stdio FILE stream.
 *
 * The function sets the texture object's GL_TEXTURE_MAX_LEVEL parameter
 * according to the number of levels in the ktxStream, provided the library
 * has been compiled with a version of gl.h where GL_TEXTURE_MAX_LEVEL is
 * defined.
 *
 * It will unpack compressed GL_ETC1_RGB8_OES and GL_ETC2_* format
 * textures in software when the format is not supported by the GL context,
 * provided the library has been compiled with SUPPORT_SOFTWARE_ETC_UNPACK
 * defined as 1.
 *
 * It will also convert texture with legacy formats to their modern equivalents
 * when the format is not supported by the GL context, provided the library
 * has been compiled with SUPPORT_LEGACY_FORMAT_CONVERSION defined as 1.
 *
 * @param [in] file			pointer to the stdio FILE stream from which to
 * 							load.
 * @param [in,out] pTexture	name of the GL texture to load. If NULL or if
 *                          <tt>*pTexture == 0</tt> the function will generate
 *                          a texture name. The function binds either the
 *                          generated name or the name given in @p *pTexture
 * 						    to the texture target returned in @p *pTarget,
 * 						    before loading the texture data. If @p pTexture
 *                          is not NULL and a name was generated, the generated
 *                          name will be returned in *pTexture.
 * @param [out] pTarget 	@p *pTarget is set to the texture target used. The
 * 						    target is chosen based on the file contents.
 * @param [out] pDimensions	If @p pDimensions is not NULL, the width, height and
 *							depth of the texture's base level are returned in the
 *                          fields of the KTX_dimensions structure to which it points.
 * @param [out] pIsMipmapped
 *	                        If @p pIsMipmapped is not NULL, @p *pIsMipmapped is set
 *                          to GL_TRUE if the KTX texture is mipmapped, GL_FALSE
 *                          otherwise.
 * @param [out] pGlerror    @p *pGlerror is set to the value returned by
 *                          glGetError when this function returns the error
 *                          KTX_GL_ERROR. glerror can be NULL.
 * @param [in,out] pKvdLen	If not NULL, @p *pKvdLen is set to the number of bytes
 *                          of key-value data pointed at by @p *ppKvd. Must not be
 *                          NULL, if @p ppKvd is not NULL.
 * @param [in,out] ppKvd	If not NULL, @p *ppKvd is set to the point to a block of
 *                          memory containing key-value data read from the file.
 *                          The application is responsible for freeing the memory.
 *
 *
 * @return	KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE @p target is @c NULL or the size of a mip
 * 							    level is greater than the size of the
 * 							    preceding level.
 * @exception KTX_INVALID_OPERATION @p ppKvd is not NULL but pKvdLen is NULL.
 * @exception KTX_UNEXPECTED_END_OF_FILE the file does not contain the
 * 										 expected amount of data.
 * @exception KTX_OUT_OF_MEMORY Sufficient memory could not be allocated to store
 *                              the requested key-value data.
 * @exception KTX_GL_ERROR      A GL error was raised by glBindTexture,
 * 								glGenTextures or gl*TexImage*. The GL error
 *                              will be returned in @p *glerror, if glerror
 *                              is not @c NULL.
 */
KTX_error_code
ktxLoadTextureF(FILE* file, GLuint* pTexture, GLenum* pTarget,
				KTX_dimensions* pDimensions, GLboolean* pIsMipmapped,
				GLenum* pGlerror,
				unsigned int* pKvdLen, unsigned char** ppKvd)
{
	struct ktxStream stream;
	KTX_error_code errorCode = KTX_SUCCESS;

	errorCode = ktxFileInit(&stream, file);
	if (errorCode != KTX_SUCCESS)
		return errorCode;

	return ktxLoadTextureS(&stream, pTexture, pTarget, pDimensions, pIsMipmapped, pGlerror, pKvdLen, ppKvd);
}

/**
 * @~English
 * @brief Load a GL texture object from a named file on disk.
 *
 * @param [in] filename		pointer to a C string that contains the path of
 * 							the file to load.
 * @param [in,out] pTexture	name of the GL texture to load. See
 *                          ktxLoadTextureF() for details.
 * @param [out] pTarget 	@p *pTarget is set to the texture target used. See
 *                          ktxLoadTextureF() for details.
 * @param [out] pDimensions @p the texture's base level width depth and height
 *                          are returned in structure to which this points.
 *                          See ktxLoadTextureF() for details.
 * @param [out] pIsMipmapped @p pIsMipMapped is set to indicate if the loaded
 *                          texture is mipmapped. See ktxLoadTextureF() for
 *                          details.
 * @param [out] pGlerror    @p *pGlerror is set to the value returned by
 *                          glGetError when this function returns the error
 *                          KTX_GL_ERROR. glerror can be NULL.
 * @param [in,out] pKvdLen	If not NULL, @p *pKvdLen is set to the number of bytes
 *                          of key-value data pointed at by @p *ppKvd. Must not be
 *                          NULL, if @p ppKvd is not NULL.
 * @param [in,out] ppKvd	If not NULL, @p *ppKvd is set to the point to a block of
 *                          memory containing key-value data read from the file.
 *                          The application is responsible for freeing the memory.*
 * @return	KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_FILE_OPEN_FAILED	The specified file could not be opened.
 * @exception KTX_INVALID_VALUE		See ktxLoadTextureF() for causes.
 * @exception KTX_INVALID_OPERATION	See ktxLoadTextureF() for causes.
 * @exception KTX_UNEXPECTED_END_OF_FILE See ktxLoadTextureF() for causes.
 *
 * @exception KTX_GL_ERROR			See ktxLoadTextureF() for causes.
 */
KTX_error_code
ktxLoadTextureN(const char* const filename, GLuint* pTexture, GLenum* pTarget,
				KTX_dimensions* pDimensions, GLboolean* pIsMipmapped,
				GLenum* pGlerror,
				unsigned int* pKvdLen, unsigned char** ppKvd)
{
	KTX_error_code errorCode;
	FILE* file = fopen(filename, "rb");

	if (file) {
		errorCode = ktxLoadTextureF(file, pTexture, pTarget, pDimensions,
								    pIsMipmapped, pGlerror, pKvdLen, ppKvd);
		fclose(file);
	} else
		errorCode = KTX_FILE_OPEN_FAILED;

	return errorCode;
}

/**
 * @~English
 * @brief Load a GL texture object from KTX formatted data in memory.
 *
 * @param [in] bytes		pointer to the array of bytes containing
 * 							the KTX format data to load.
 * @param [in] size			size of the memory array containing the
 *                          KTX format data.
 * @param [in,out] pTexture	name of the GL texture to load. See
 *                          ktxLoadTextureF() for details.
 * @param [out] pTarget 	@p *pTarget is set to the texture target used. See
 *                          ktxLoadTextureF() for details.
 * @param [out] pDimensions @p the texture's base level width depth and height
 *                          are returned in structure to which this points.
 *                          See ktxLoadTextureF() for details.
 * @param [out] pIsMipmapped @p *pIsMipMapped is set to indicate if the loaded
 *                          texture is mipmapped. See ktxLoadTextureF() for
 *                          details.
 * @param [out] pGlerror    @p *pGlerror is set to the value returned by
 *                          glGetError when this function returns the error
 *                          KTX_GL_ERROR. glerror can be NULL.
 * @param [in,out] pKvdLen	If not NULL, @p *pKvdLen is set to the number of bytes
 *                          of key-value data pointed at by @p *ppKvd. Must not be
 *                          NULL, if @p ppKvd is not NULL.
 * @param [in,out] ppKvd	If not NULL, @p *ppKvd is set to the point to a block of
 *                          memory containing key-value data read from the file.
 *                          The application is responsible for freeing the memory.*
 *
 * @return	KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_FILE_OPEN_FAILED	The specified memory could not be opened as a file.
 * @exception KTX_INVALID_VALUE		See ktxLoadTextureF() for causes.
 * @exception KTX_INVALID_OPERATION	See ktxLoadTextureF() for causes.
 * @exception KTX_UNEXPECTED_END_OF_FILE See ktxLoadTextureF() for causes.
 *
 * @exception KTX_GL_ERROR			See ktxLoadTextureF() for causes.
 */
KTX_error_code
ktxLoadTextureM(const void* bytes, GLsizei size, GLuint* pTexture, GLenum* pTarget,
				KTX_dimensions* pDimensions, GLboolean* pIsMipmapped,
				GLenum* pGlerror,
				unsigned int* pKvdLen, unsigned char** ppKvd)
{
	struct ktxMem mem;
	struct ktxStream stream;
	KTX_error_code errorCode = KTX_SUCCESS;

	errorCode = ktxMemInit(&stream, &mem, bytes, size);
	if (errorCode != KTX_SUCCESS)
		return errorCode;

	return ktxLoadTextureS(&stream, pTexture, pTarget, pDimensions, pIsMipmapped, pGlerror, pKvdLen, ppKvd);
}



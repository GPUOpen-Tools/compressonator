/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: 5c28a496fa7703a75b91eb8dd7b1722c2e299741 $ */

/**
 * @internal
 * @file checkheader.c
 * @~English
 *
 * @brief Function to verify a KTX file header
 *
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
 * Author: Georg Kolling, Imagination Technology with modifications
 * by Mark Callow, HI Corporation.
 */
#include <string.h>

#include "ktx.h"
#include "ktxint.h"

#include KTX_GLFUNCPTRS

DECLARE_GL_FUNCPTRS
DECLARE_GL_EXTGLOBALS

/**
 * @internal
 * @~English
 * @brief Check a KTX file header.
 *
 * As well as checking that the header identifies a KTX file, the function
 * sanity checks the values and returns information about the texture in a
 * KTX_texinfo structure.
 *
 * @param header	pointer to the KTX header to check
 * @param texinfo	pointer to a KTX_texinfo structure in which to return
 *                  information about the texture.
 * 
 * @author Georg Kolling, Imagination Technology
 * @author Mark Callow, HI Corporation
 */
KTX_error_code _ktxCheckHeader(KTX_header* header, KTX_texinfo* texinfo)
{
	khronos_uint8_t identifier_reference[12] = KTX_IDENTIFIER_REF;
	khronos_uint32_t max_dim;

	/* Compare identifier, is this a KTX file? */
	if (memcmp(header->identifier, identifier_reference, 12) != 0)
	{
		return KTX_UNKNOWN_FILE_FORMAT;
	}

	if (header->endianness == KTX_ENDIAN_REF_REV)
	{
		/* Convert endianness of header fields if necessary */
		_ktxSwapEndian32(&header->glType, 12);

		if (header->glTypeSize != 1 ||
			header->glTypeSize != 2 ||
			header->glTypeSize != 4)
		{
			/* Only 8, 16, and 32-bit types supported so far */
			return KTX_INVALID_VALUE;
		}
	}
	else if (header->endianness != KTX_ENDIAN_REF)
	{
		return KTX_INVALID_VALUE;
	}

	/* Check glType and glFormat */
	texinfo->compressed = 0;
	if (header->glType == 0 || header->glFormat == 0)
	{
		if (header->glType + header->glFormat != 0)
		{
			/* either both or none of glType, glFormat must be zero */
			return KTX_INVALID_VALUE;
		}
		texinfo->compressed = 1;
	}

	/* Check texture dimensions. KTX files can store 8 types of textures:
	   1D, 2D, 3D, cube, and array variants of these. There is currently
	   no GL extension that would accept 3D array or cube array textures. */
	if ((header->pixelWidth == 0) ||
		(header->pixelDepth > 0 && header->pixelHeight == 0))
	{
		/* texture must have width */
		/* texture must have height if it has depth */
		return KTX_INVALID_VALUE; 
	}

	texinfo->textureDimensions = 1;
	texinfo->glTarget = GL_TEXTURE_1D;
	texinfo->generateMipmaps = 0;
	if (header->pixelHeight > 0)
	{
		texinfo->textureDimensions = 2;
		texinfo->glTarget = GL_TEXTURE_2D;
	}
	if (header->pixelDepth > 0)
	{
		texinfo->textureDimensions = 3;
		texinfo->glTarget = GL_TEXTURE_3D;
	}

	if (header->numberOfFaces == 6)
	{
		if (texinfo->textureDimensions == 2)
		{
			texinfo->glTarget = GL_TEXTURE_CUBE_MAP;
		}
		else
		{
			/* cube map needs 2D faces */
			return KTX_INVALID_VALUE;
		}
	}
	else if (header->numberOfFaces != 1)
	{
		/* numberOfFaces must be either 1 or 6 */
		return KTX_INVALID_VALUE;
	}

	/* load as 2D texture if 1D textures are not supported */
	if (texinfo->textureDimensions == 1 &&
		((texinfo->compressed && (glCompressedTexImage1D == NULL)) ||
		 (!texinfo->compressed && (glTexImage1D == NULL))))
	{
		texinfo->textureDimensions = 2;
		texinfo->glTarget = GL_TEXTURE_2D;
		header->pixelHeight = 1;
	}

	if (header->numberOfArrayElements > 0)
	{
		if (texinfo->glTarget == GL_TEXTURE_1D)
		{
			texinfo->glTarget = GL_TEXTURE_1D_ARRAY_EXT;
		}
		else if (texinfo->glTarget == GL_TEXTURE_2D)
		{
			texinfo->glTarget = GL_TEXTURE_2D_ARRAY_EXT;
		}
		else
		{
			/* No API for 3D and cube arrays yet */
			return KTX_UNSUPPORTED_TEXTURE_TYPE;
		}
		texinfo->textureDimensions++;
	}

	/* reject 3D texture if unsupported */
	if (texinfo->textureDimensions == 3 &&
		((texinfo->compressed && (glCompressedTexImage3D == NULL)) ||
		 (!texinfo->compressed && (glTexImage3D == NULL))))
	{
		return KTX_UNSUPPORTED_TEXTURE_TYPE;
	}

	/* Check number of mipmap levels */
	if (header->numberOfMipmapLevels == 0)
	{
		texinfo->generateMipmaps = 1;
		header->numberOfMipmapLevels = 1;
	}
	max_dim = MAX(MAX(header->pixelWidth, header->pixelHeight), header->pixelDepth);
	if (max_dim < ((khronos_uint32_t)1 << (header->numberOfMipmapLevels - 1)))
	{
		/* Can't have more mip levels than 1 + log2(max(width, height, depth)) */
		return KTX_INVALID_VALUE;
	}

	return KTX_SUCCESS;
}

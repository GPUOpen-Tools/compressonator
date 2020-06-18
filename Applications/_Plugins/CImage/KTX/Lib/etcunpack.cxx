/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: d555f6fdc46c28e4db334b44fdb8ac5b4bcef91a $ */

/* @internal
 * @~English
 * @file
 *
 * Unpack a texture compressed with ETC1
 *
 * @author Mark Callow, HI Corporation.
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

#include <assert.h>
#include <stdlib.h>
#include "ktx.h"
#include "ktxint.h"

#if SUPPORT_SOFTWARE_ETC_UNPACK
typedef unsigned int uint;
typedef unsigned char uint8;

extern void decompressBlockETC2c(uint block_part1, uint block_part2, uint8* img,
								 int width, int height, int startx, int starty, int channels);
extern void decompressBlockETC21BitAlphaC(uint block_part1, uint block_part2, uint8* img, uint8* alphaimg,
										  int width, int height, int startx, int starty, int channels);
extern void decompressBlockAlphaC(uint8* data, uint8* img,
								  int width, int height, int startx, int starty, int channels);
extern void decompressBlockAlpha16bitC(uint8* data, uint8* img,
									   int width, int height, int startx, int starty, int channels);

extern void setupAlphaTable();

// This global variable affects the behaviour of decompressBlockAlpha16bitC.
extern int formatSigned;

static void
readBigEndian4byteWord(khronos_uint32_t* pBlock, const GLubyte *s)
{
	*pBlock = (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
}


/* Unpack an ETC1_RGB8_OES format compressed texture */
extern "C" KTX_error_code
_ktxUnpackETC(const GLubyte* srcETC, const GLenum srcFormat,
			  khronos_uint32_t activeWidth, khronos_uint32_t activeHeight,
			  GLubyte** dstImage,
			  GLenum* format, GLenum* internalFormat, GLenum* type,
			  GLint R16Formats, GLboolean supportsSRGB)
{
	unsigned int width, height;
	unsigned int block_part1, block_part2;
	unsigned int x, y;
	/*const*/ GLubyte* src = (GLubyte*)srcETC;
	// AF_11BIT is used to compress R11 & RG11 though its not alpha data.
	enum {AF_NONE, AF_1BIT, AF_8BIT, AF_11BIT} alphaFormat = AF_NONE;
	int dstChannels, dstChannelBytes;

	switch (srcFormat) {
	  case GL_COMPRESSED_SIGNED_R11_EAC:
		if (R16Formats & _KTX_R16_FORMATS_SNORM) {
			dstChannelBytes = sizeof(GLshort);
			dstChannels = 1;
			formatSigned = GL_TRUE;
			*internalFormat = GL_R16_SNORM;
			*format = GL_RED;
			*type = GL_SHORT;
			alphaFormat = AF_11BIT;
		} else
			return KTX_UNSUPPORTED_TEXTURE_TYPE; 
		break;

	  case GL_COMPRESSED_R11_EAC:
		if (R16Formats & _KTX_R16_FORMATS_NORM) {
			dstChannelBytes = sizeof(GLshort);
			dstChannels = 1;
			formatSigned = GL_FALSE;
			*internalFormat = GL_R16;
			*format = GL_RED;
			*type = GL_UNSIGNED_SHORT;
			alphaFormat = AF_11BIT;
		} else
			return KTX_UNSUPPORTED_TEXTURE_TYPE; 
        break;

	  case GL_COMPRESSED_SIGNED_RG11_EAC:
		if (R16Formats & _KTX_R16_FORMATS_SNORM) {
			dstChannelBytes = sizeof(GLshort);
			dstChannels = 2;
			formatSigned = GL_TRUE;
			*internalFormat = GL_RG16_SNORM;
			*format = GL_RG;
			*type = GL_SHORT;
			alphaFormat = AF_11BIT;
		} else
			return KTX_UNSUPPORTED_TEXTURE_TYPE; 
        break;

	  case GL_COMPRESSED_RG11_EAC:
		if (R16Formats & _KTX_R16_FORMATS_NORM) {
			dstChannelBytes = sizeof(GLshort);
			dstChannels = 2;
			formatSigned = GL_FALSE;
			*internalFormat = GL_RG16;
			*format = GL_RG;
			*type = GL_UNSIGNED_SHORT;
			alphaFormat = AF_11BIT;
		} else
			return KTX_UNSUPPORTED_TEXTURE_TYPE; 
        break;

	  case GL_ETC1_RGB8_OES:
	  case GL_COMPRESSED_RGB8_ETC2:
	    dstChannelBytes = sizeof(GLubyte);
		dstChannels = 3;
		*internalFormat = GL_RGB8;
		*format = GL_RGB;
		*type = GL_UNSIGNED_BYTE;
        break;

	  case GL_COMPRESSED_RGBA8_ETC2_EAC:
	    dstChannelBytes = sizeof(GLubyte);
		dstChannels = 4;
		*internalFormat = GL_RGBA8;
		*format = GL_RGBA;
		*type = GL_UNSIGNED_BYTE;
	    alphaFormat = AF_8BIT;
		break;

	  case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
	    dstChannelBytes = sizeof(GLubyte);
		dstChannels = 4;
		*internalFormat = GL_RGBA8;
		*format = GL_RGBA;
		*type = GL_UNSIGNED_BYTE;
		alphaFormat = AF_1BIT;
        break;

	  case GL_COMPRESSED_SRGB8_ETC2:
		if (supportsSRGB) {
			dstChannelBytes = sizeof(GLubyte);
			dstChannels = 3;
			*internalFormat = GL_SRGB8;
			*format = GL_RGB;
			*type = GL_UNSIGNED_BYTE;
		} else
			return KTX_UNSUPPORTED_TEXTURE_TYPE; 
        break;

	  case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
		if (supportsSRGB) {
			dstChannelBytes = sizeof(GLubyte);
			dstChannels = 4;
			*internalFormat = GL_SRGB8_ALPHA8;
 			*format = GL_RGBA;
			*type = GL_UNSIGNED_BYTE;
			alphaFormat = AF_8BIT;
		} else
			return KTX_UNSUPPORTED_TEXTURE_TYPE; 
		break;

	  case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		if (supportsSRGB) {
			dstChannelBytes = sizeof(GLubyte);
			dstChannels = 4;
			*internalFormat = GL_SRGB8_ALPHA8;
 			*format = GL_RGBA;
			*type = GL_UNSIGNED_BYTE;
			alphaFormat = AF_1BIT;
		} else
			return KTX_UNSUPPORTED_TEXTURE_TYPE; 
        break;

	  default:
	    assert(0); // Upper levels should be passing only one of the above srcFormats.
	}

    /* active_{width,height} show how many pixels contain active data,
	 * (the rest are just for making sure we have a 2*a x 4*b size).
	 */

	/* Compute the full width & height. */
	width = ((activeWidth+3)/4)*4;
	height = ((activeHeight+3)/4)*4;

	/* printf("Width = %d, Height = %d\n", width, height); */
	/* printf("active pixel area: top left %d x %d area.\n", activeWidth, activeHeight); */

	*dstImage = (GLubyte*)malloc(dstChannels*dstChannelBytes*width*height);
	if (!*dstImage) {
		return KTX_OUT_OF_MEMORY;
	}
	
	if (alphaFormat != AF_NONE)
		setupAlphaTable();

	// NOTE: none of the decompress functions actually use the <height> parameter
	if (alphaFormat == AF_11BIT) {
		// One or two 11-bit alpha channels for R or RG.
		for (y=0; y < height/4; y++) {
			for (x=0; x < width/4; x++) {
				decompressBlockAlpha16bitC(src, *dstImage, width, height, 4*x, 4*y, dstChannels);
				src += 8;
				if (srcFormat == GL_COMPRESSED_RG11_EAC || srcFormat == GL_COMPRESSED_SIGNED_RG11_EAC) {
					decompressBlockAlpha16bitC(src, *dstImage + dstChannelBytes, width, height, 4*x, 4*y, dstChannels);
					src += 8;
				}
			}
		}
	} else {
		for (y=0; y < height/4; y++) {
			for (x=0; x < width/4; x++) {
				// Decode alpha channel for RGBA
				if (alphaFormat == AF_8BIT) {
					decompressBlockAlphaC(src, *dstImage + 3, width, height, 4*x, 4*y, dstChannels);
					src += 8;
				}
				// Decode color dstChannels
				readBigEndian4byteWord(&block_part1, src);
				src += 4;
				readBigEndian4byteWord(&block_part2, src);
				src += 4;
				if (alphaFormat == AF_1BIT)
				    decompressBlockETC21BitAlphaC(block_part1, block_part2, *dstImage, 0, width, height, 4*x, 4*y, dstChannels);
				else
				    decompressBlockETC2c(block_part1, block_part2, *dstImage, width, height, 4*x, 4*y, dstChannels);
			}
		}
	}

	/* Ok, now write out the active pixels to the destination image.
	 * (But only if the active pixels differ from the total pixels)
	 */

	if( !(height == activeHeight && width == activeWidth) ) {
		int dstPixelBytes = dstChannels * dstChannelBytes;
		int dstRowBytes = dstPixelBytes * width;
		int activeRowBytes = activeWidth * dstPixelBytes;
		GLubyte *newimg = (GLubyte*)malloc(dstPixelBytes * activeWidth * activeHeight);
		unsigned int xx, yy;
		int zz;

		if (!newimg) {
			free(*dstImage);
			return KTX_OUT_OF_MEMORY;
		}
		
		/* Convert from total area to active area: */

		for (yy = 0; yy < activeHeight; yy++) {
			for (xx = 0; xx < activeWidth; xx++) {
				for (zz = 0; zz < dstPixelBytes; zz++) {
					newimg[ yy*activeRowBytes + xx*dstPixelBytes + zz ] = (*dstImage)[ yy*dstRowBytes + xx*dstPixelBytes + zz];
				}
			}
		}

		free(*dstImage);
		*dstImage = newimg;
	}

	return KTX_SUCCESS;
}

#endif /* SUPPORT_SOFTWARE_ETC_UNPACK */

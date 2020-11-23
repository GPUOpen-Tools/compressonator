/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: b7b03494cb4c3d30b64882e8d4b3e44c3221890b $ */

/**
 * @file writer.c
 * @~English
 *
 * @brief Functions for creating KTX-format files from a set of images.
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ktx.h"
#include "ktxint.h"
#include "ktxstream.h"
#include "ktxfilestream.h"
#include "ktxmemstream.h"

static KTX_error_code validateTypeAndFormat(GLenum format, GLenum type);
static KTX_error_code sizeofGroupAndElement(GLenum format, GLenum type,
											GLuint* groupBytes, GLuint* elementBytes,
											GLboolean* packed);
static KTX_error_code sizeofGLtype(GLenum type, GLuint* size, GLboolean* packed);


/**
 * @~English
 * @brief Write image(s) in a KTX-format to a ktxStream.
 *
 * @param [in] stream           pointer to the ktxStream from which to load.
 * @param [in] textureInfo  pointer to a KTX_texture_info structure providing
 *                          information about the images to be included in
 *                          the KTX file.
 * @param [in] bytesOfKeyValueData
 *                          specifies the number of bytes of key-value data.
 * @param [in] keyValueData a pointer to the keyValue data.
 * @param [in] numImages    number of images in the following array
 * @param [in] images       array of KTX_image_info providing image size and
 *                          data.
 *
 * @return	KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE @p dst or @p target are @c NULL
 * @exception KTX_INVALID_VALUE @c glTypeSize in @p textureInfo is not 1, 2, or 4 or
 *                              is different from the size of the type specified
 *                              in @c glType.
 * @exception KTX_INVALID_VALUE @c pixelWidth in @p textureInfo is 0 or pixelDepth != 0
 *                              && pixelHeight == 0.
 * @exception KTX_INVALID_VALUE @c numberOfFaces != 1 || numberOfFaces != 6 or
 *                              numberOfArrayElements or numberOfMipmapLevels are < 0.
 * @exception KTX_INVALID_VALUE @c glType in @p textureInfo is an unrecognized type.
 * @exception KTX_INVALID_OPERATION
 *                              numberOfFaces == 6 and images are either not 2D or
 *                              are not square.
 * @exception KTX_INVALID_OPERATION
 *							    number of images is insufficient for the specified
 *                              number of mipmap levels and faces.
 * @exception KTX_INVALID_OPERATION
 *                              the size of a provided image is different than that
 *                              required for the specified width, height or depth
 *                              or for the mipmap level being processed.
 * @exception KTX_INVALID_OPERATION
 *								@c glType and @c glFormat in @p textureInfo are mismatched.
 *								See OpenGL 4.4 specification section 8.4.4 and
 *                              table 8.5.
 * @exception KTX_FILE_WRITE_ERROR a system error occurred while writing the file.
 * @exception KTX_OUT_OF_MEMORY system failed to allocate sufficient memory.
 */
static
KTX_error_code
ktxWriteKTXS(struct ktxStream *stream, const KTX_texture_info* textureInfo,
			 GLsizei bytesOfKeyValueData, const void* keyValueData,
			 GLuint numImages, KTX_image_info images[])
{
	KTX_header header = KTX_IDENTIFIER_REF;
	GLuint i, level, dimension, cubemap = 0;
	GLuint numMipmapLevels, numArrayElements;
	GLbyte pad[4] = { 0, 0, 0, 0 };
	KTX_error_code errorCode = KTX_SUCCESS;
	GLboolean compressed = GL_FALSE;
	GLuint groupBytes=4, elementBytes;

	if (!stream) {
		return KTX_INVALID_VALUE;
	}

	//endianess int.. if this comes out reversed, all of the other ints will too.
	header.endianness = KTX_ENDIAN_REF;
	header.glType = textureInfo->glType;
	header.glTypeSize = textureInfo->glTypeSize;
	header.glFormat = textureInfo->glFormat;
	header.glInternalFormat = textureInfo->glInternalFormat;
	header.glBaseInternalFormat = textureInfo->glBaseInternalFormat;
	header.pixelWidth = textureInfo->pixelWidth;
	header.pixelHeight = textureInfo->pixelHeight;
	header.pixelDepth = textureInfo->pixelDepth;
	header.numberOfArrayElements = textureInfo->numberOfArrayElements;
	header.numberOfFaces = textureInfo->numberOfFaces;
	header.numberOfMipmapLevels = textureInfo->numberOfMipmapLevels;
	header.bytesOfKeyValueData = bytesOfKeyValueData;

	/* Do some sanity checking */
	if (header.glTypeSize != 1 &&
		header.glTypeSize != 2 &&
		header.glTypeSize != 4)
	{
		/* Only 8, 16, and 32-bit types are supported for byte-swapping.
		 * See UNPACK_SWAP_BYTES & table 8.4 in the OpenGL 4.4 spec.
		 */
		return KTX_INVALID_VALUE;
	}

	if (header.glType == 0 || header.glFormat == 0)
	{
		if (header.glType + header.glFormat != 0) {
			/* either both or neither of glType & glFormat must be zero */
			return KTX_INVALID_VALUE;
		} else {
            compressed = GL_TRUE;
            if (header.glBaseInternalFormat == GL_RED)
                groupBytes = 1;
            if (header.glBaseInternalFormat == GL_RG)
                groupBytes = 2;
            if (header.glBaseInternalFormat == GL_RGB)
                groupBytes = 3;
            if (header.glBaseInternalFormat == GL_RGBA)
                groupBytes = 4;
        }
	}
	else
	{
		GLboolean packed;

		/* Get size of group and element */
		if ((errorCode = sizeofGroupAndElement(header.glFormat, header.glType,
											   &groupBytes, &elementBytes, &packed)) != KTX_SUCCESS)
		{
			return errorCode;
		}

		/* Check validity of type/format combination for packed types */
		if (packed && (errorCode = validateTypeAndFormat(header.glFormat, header.glType)) != KTX_SUCCESS)
		{
			return errorCode;
		}

		if (header.glTypeSize != elementBytes)
		{
#if defined(GL_FLOAT_32_UNSIGNED_INT_24_8_REV)
			if (header.glType != GL_FLOAT_32_UNSIGNED_INT_24_8_REV || header.glTypeSize != 1)
#endif
			return KTX_INVALID_VALUE;
		}
	}


	/* Check texture dimensions. KTX files can store 8 types of textures:
	 * 1D, 2D, 3D, cube, and array variants of these. There is currently
	 * no GL extension that would accept 3D array or cube array textures
	 * but we'll let such files be created.
	 */
	if ((header.pixelWidth == 0) ||
		(header.pixelDepth > 0 && header.pixelHeight == 0))
	{
		/* texture must have width */
		/* texture must have height if it has depth */
		return KTX_INVALID_VALUE;
	}
	if (header.pixelHeight > 0 && header.pixelDepth > 0)
		dimension = 3;
	else if (header.pixelHeight > 0)
		dimension = 2;
	else
		dimension = 1;

	if (header.numberOfFaces == 6)
	{
		if (dimension != 2)
		{
			/* cube map needs 2D faces */
			return KTX_INVALID_OPERATION;
		}
		if (header.pixelWidth != header.pixelHeight)
		{
			/* cube maps require square images */
			return KTX_INVALID_OPERATION;
		}
	}
	else if (header.numberOfFaces != 1)
	{
		/* numberOfFaces must be either 1 or 6 */
		return KTX_INVALID_VALUE;
	}

	if (header.numberOfArrayElements == 0)
	{
		numArrayElements = 1;
		if (header.numberOfFaces == 6)
			cubemap = 1;
	}
	else
		numArrayElements = header.numberOfArrayElements;

	/* Check number of mipmap levels */
	if (header.numberOfMipmapLevels == 0)
	{
		numMipmapLevels = 1;
	}
	else
		numMipmapLevels = header.numberOfMipmapLevels;
	if (numMipmapLevels > 1) {
		GLuint max_dim = MAX(MAX(header.pixelWidth, header.pixelHeight), header.pixelDepth);
		if (max_dim < ((GLuint)1 << (header.numberOfMipmapLevels - 1)))
		{
			/* Can't have more mip levels than 1 + log2(max(width, height, depth)) */
			return KTX_INVALID_VALUE;
		}
	}

	if (numImages < numMipmapLevels * header.numberOfFaces)
	{
		/* Not enough images */
		return KTX_INVALID_OPERATION;
	}

	//write header
	errorCode = stream->write(&header, sizeof(KTX_header), 1, stream->src);
	if (errorCode != KTX_SUCCESS)
		return errorCode;

	//write keyValueData
	if (bytesOfKeyValueData != 0) {
		if (keyValueData == NULL)
			return KTX_INVALID_OPERATION;

		errorCode = stream->write(keyValueData, 1, bytesOfKeyValueData, stream->src);
		if (errorCode != KTX_SUCCESS)
			return errorCode;
	}

	/* Write the image data */
	for (level = 0, i = 0; level < numMipmapLevels; ++level)
	{
		GLsizei expectedFaceSize;
		GLuint face, faceLodSize, faceLodRounding;
		GLuint pixelWidth, pixelHeight, pixelDepth;
		GLuint packedRowBytes, rowBytes, rowRounding;

		pixelWidth  = MAX(1, header.pixelWidth  >> level);
		pixelHeight = MAX(1, header.pixelHeight >> level);
		pixelDepth  = MAX(1, header.pixelDepth  >> level);

		/* Calculate face sizes for this LoD based on glType, glFormat, width & height */
		expectedFaceSize = groupBytes
						   * pixelWidth
						   * pixelHeight
						   * pixelDepth
						   * numArrayElements;

		rowRounding = 0;
		packedRowBytes = groupBytes * pixelWidth;
		/* KTX format specifies UNPACK_ALIGNMENT==4 */
		/* GL spec: rows are not to be padded when elementBytes != 1, 2, 4 or 8.
		 * As GL currently has no such elements, no test is necessary.
		 */
		if (!compressed && elementBytes < KTX_GL_UNPACK_ALIGNMENT) {
			// Equivalent to UNPACK_ALIGNMENT * ceil((groupSize * pixelWidth) / UNPACK_ALIGNMENT)
			rowRounding = 3 - ((packedRowBytes + KTX_GL_UNPACK_ALIGNMENT-1) % KTX_GL_UNPACK_ALIGNMENT);
			rowBytes = packedRowBytes + rowRounding;
		}

		if (rowRounding == 0) {
			faceLodSize = images[i].size;
		} else {
			/* Need to pad the rows to meet the required UNPACK_ALIGNMENT */
			faceLodSize = rowBytes * pixelHeight * pixelDepth * numArrayElements;
		}
		faceLodRounding = 3 - ((faceLodSize + 3) % 4);

		errorCode = stream->write(&faceLodSize, sizeof(faceLodSize), 1, stream->src);
		if (errorCode != KTX_SUCCESS)
			goto cleanup;

		for (face = 0; face < header.numberOfFaces; ++face, ++i) {
			if (!compressed) {
				/* Sanity check. */
				if (images[i].size != expectedFaceSize) {
					errorCode = KTX_INVALID_OPERATION;
					goto cleanup;
				}
			}
			if (rowRounding == 0) {
				/* Can write whole face at once */
				errorCode = stream->write(images[i].data, faceLodSize, 1, stream->src);
				if (errorCode != KTX_SUCCESS)
					goto cleanup;
			} else {
				/* Write the rows individually, padding each one */
				GLuint row;
				GLuint numRows = pixelHeight
								* pixelDepth
								* numArrayElements;
				for (row = 0; row < numRows; row++) {
					errorCode = stream->write(&images[i].data[row*packedRowBytes], packedRowBytes, 1, stream->src);
					if (errorCode != KTX_SUCCESS)
						goto cleanup;

					errorCode = stream->write(pad, sizeof(GLbyte), rowRounding, stream->src);
					if (errorCode != KTX_SUCCESS)
						goto cleanup;
				}
			}
			if (faceLodRounding) {
				errorCode = stream->write(pad, sizeof(GLbyte), faceLodRounding, stream->src);
				if (errorCode != KTX_SUCCESS)
					goto cleanup;
			}
		}
	}

cleanup:
	return errorCode;
}

/**
 * @~English
 * @brief Write image(s) in a KTX-formatted stdio FILE stream.
 *
 * @param [in] file         pointer to the FILE stream to write to.
 * @param [in] textureInfo  pointer to a KTX_texture_info structure providing
 *                          information about the images to be included in
 *                          the KTX file.
 * @param [in] bytesOfKeyValueData
 *                          specifies the number of bytes of key-value data.
 * @param [in] keyValueData a pointer to the keyValue data.
 * @param [in] numImages    number of images in the following array
 * @param [in] images       array of KTX_image_info providing image size and
 *                          data.
 *
 * @return      KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE @p dst or @p target are @c NULL
 * @exception KTX_INVALID_VALUE @c glTypeSize in @p textureInfo is not 1, 2, or 4 or
 *                              is different from the size of the type specified
 *                              in @c glType.
 * @exception KTX_INVALID_VALUE @c pixelWidth in @p textureInfo is 0 or pixelDepth != 0
 *                              && pixelHeight == 0.
 * @exception KTX_INVALID_VALUE @c numberOfFaces != 1 || numberOfFaces != 6 or
 *                              numberOfArrayElements or numberOfMipmapLevels are < 0.
 * @exception KTX_INVALID_VALUE @c glType in @p textureInfo is an unrecognized type.
 * @exception KTX_INVALID_OPERATION
 *                              numberOfFaces == 6 and images are either not 2D or
 *                              are not square.
 * @exception KTX_INVALID_OPERATION
 *                                                          number of images is insufficient for the specified
 *                              number of mipmap levels and faces.
 * @exception KTX_INVALID_OPERATION
 *                              the size of a provided image is different than that
 *                              required for the specified width, height or depth
 *                              or for the mipmap level being processed.
 * @exception KTX_INVALID_OPERATION
 *                                                              @c glType and @c glFormat in @p textureInfo are mismatched.
 *                                                              See OpenGL 4.4 specification section 8.4.4 and
 *                              table 8.5.
 * @exception KTX_FILE_WRITE_ERROR a system error occurred while writing the file.
 */
KTX_error_code
ktxWriteKTXF(FILE *file, const KTX_texture_info* textureInfo,
						 GLsizei bytesOfKeyValueData, const void* keyValueData,
						 GLuint numImages, KTX_image_info images[])
{
		struct ktxStream stream;
		KTX_error_code errorCode = KTX_SUCCESS;

		errorCode = ktxFileInit(&stream, file);
		if (errorCode != KTX_SUCCESS)
				return errorCode;

		return ktxWriteKTXS(&stream, textureInfo, bytesOfKeyValueData, keyValueData, numImages, images);
}

/**
 * @~English
 * @brief Write image(s) to a KTX file on disk.
 *
 * @param [in] dstname		pointer to a C string that contains the path of
 * 							the file to load.
 * @param [in] textureInfo  pointer to a KTX_texture_info structure providing
 *                          information about the images to be included in
 *                          the KTX file.
 * @param [in] bytesOfKeyValueData
 *                          specifies the number of bytes of key-value data.
 * @param [in] keyValueData a pointer to the keyValue data.
 * @param [in] numImages    number of images in the following array.
 * @param [in] images       array of KTX_image_info providing image size and
 *                          data.
 *
 * @return	KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_FILE_OPEN_FAILED unable to open the specified file for
 *                                 writing.
 *
 * For other exceptions, see ktxWriteKTXF().
 */
KTX_error_code
ktxWriteKTXN(const char* dstname, const KTX_texture_info* textureInfo,
			 GLsizei bytesOfKeyValueData, const void* keyValueData,
			 GLuint numImages, KTX_image_info images[])
{
	KTX_error_code errorCode;
	FILE* dst = fopen(dstname, "wb");

	if (dst) {
		errorCode = ktxWriteKTXF(dst, textureInfo, bytesOfKeyValueData, keyValueData,
								 numImages, images);
		fclose(dst);
	} else
		errorCode = KTX_FILE_OPEN_FAILED;

	return errorCode;
}

/**
 * @~English
 * @brief Write image(s) in KTX format to memory.
 *
 * @param [out] bytes        pointer to the output with KTX data. Application
							is responsible for freeing that memory.
 * @param [out] size         pointer to store size of the memory written.
 * @param [in] textureInfo  pointer to a KTX_texture_info structure providing
 *                          information about the images to be included in
 *                          the KTX file.
 * @param [in] bytesOfKeyValueData
 *                          specifies the number of bytes of key-value data.
 * @param [in] keyValueData a pointer to the keyValue data.
 * @param [in] numImages    number of images in the following array.
 * @param [in] images       array of KTX_image_info providing image size and
 *                          data.
 *
 * @return      KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 */
KTX_error_code
ktxWriteKTXM(unsigned char** bytes, GLsizei* size, const KTX_texture_info* textureInfo,
			GLsizei bytesOfKeyValueData, const void* keyValueData,
			GLuint numImages, KTX_image_info images[])
{
	struct ktxMem mem;
	struct ktxStream stream;
	KTX_error_code rc;

	*bytes = NULL;

	rc = ktxMemInit(&stream, &mem, NULL, 0);
	if (rc != KTX_SUCCESS)
		return rc;

	rc = ktxWriteKTXS(&stream, textureInfo, bytesOfKeyValueData, keyValueData, numImages, images);
	if(rc != KTX_SUCCESS)
	{
		if(mem.bytes)
		{
			free(mem.bytes);
		}
		return rc;
	}

	*bytes = mem.bytes;
	*size = mem.used_size;
	return KTX_SUCCESS;
}

/*
 * @brief Check format and type matching as required by OpenGL.
 *
 * @param [in] format	the format of the image data
 * @param [in] type		the type of the image data
 *
 * @return	KTX_SUCCESS if matched, KTX_INVALID_OPERATION, if mismatched
 *			or KTX_INVALID_VALUE if @p type is invalid.
 */
static KTX_error_code
validateTypeAndFormat(GLenum format, GLenum type)
{
	KTX_error_code retVal = KTX_SUCCESS;

	if ((format >= GL_RED_INTEGER && format <= GL_BGRA_INTEGER) && (type == GL_FLOAT || type == GL_HALF_FLOAT))
	{
		retVal = KTX_INVALID_OPERATION; // Note: OpenGL 4.4 says GL_INVALID_VALUE but we'll mirror the others.
	}

	switch (type)
	{
		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			if (format != GL_RGB && format != GL_RGB_INTEGER)
				retVal = KTX_INVALID_OPERATION; // Matches OpenGL 4.4
			break;

		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
			if (format != GL_RGB && format != GL_RGB_INTEGER)
				retVal = KTX_INVALID_OPERATION;
			break;

		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			if (format != GL_RGBA && format != GL_BGRA
				&& format != GL_RGBA_INTEGER && format != GL_BGRA_INTEGER)
			{
				retVal = KTX_INVALID_OPERATION;
			}
			break;

		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			if (format != GL_RGBA && format != GL_BGRA
				&& format != GL_RGBA_INTEGER && format != GL_BGRA_INTEGER)
			{
				retVal = KTX_INVALID_OPERATION;
			}
			break;

		case GL_UNSIGNED_INT_24_8:
			if (format != GL_DEPTH_STENCIL)
				retVal = KTX_INVALID_OPERATION;
			break;

		case GL_UNSIGNED_INT_10F_11F_11F_REV:
		case GL_UNSIGNED_INT_5_9_9_9_REV:
			if (format != GL_RGB && format != GL_BGR)
				retVal = KTX_INVALID_OPERATION;
			break;

		case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
			// Note: OpenGL 4.4 says GL_INVALID_VALUE in one place,
			// GL_INVALID_OPERATION in another. The latter is more logical.
			retVal = KTX_INVALID_OPERATION;

		default:
			retVal = KTX_INVALID_VALUE;
	}

	return retVal;
}


/*
 * @brief Get the size of the group of elements constituting a pixel in
 *        the given @p type and @p format and the size of an element.
 *
 * Sizes are returned in basic machine units (bytes). The function also
 * indicates if @type is a packed pixel format.
 *
 * @param [in]  format		the format of the image data
 * @param [in]  type		the type of the image data
 * @param [out] groupBytes	pointer to location where to write the size of a group
 * @param [out] size		pointer to location where to write the size of an element
 * @param [out] packed		pointer to location where to write flag indicating
 *							if the type is a packed type.
 *
 * @return	KTX_INVALID_VALUE if the @p type or @p format is invalid.
 */
static KTX_error_code
sizeofGroupAndElement(GLenum format, GLenum type, GLuint* groupBytes,
					  GLuint* elementBytes, GLboolean* packed)
{
	KTX_error_code retVal;

	if ((retVal = sizeofGLtype(type, elementBytes, packed)) != KTX_SUCCESS)
	{
		return retVal;
	}

	if (*packed)
	{
		*groupBytes = *elementBytes;
		return retVal;
	}

	switch (format) {
	case GL_ALPHA:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_LUMINANCE: /* deprecated but needed for ES 1 & 2 */
	case GL_ALPHA_INTEGER:
	case GL_RED_INTEGER:
	case GL_GREEN_INTEGER:
	case GL_BLUE_INTEGER:
	/* case GL_LUMINANCE_INTEGER: deprecated */
		*groupBytes = *elementBytes;
		break;
	case GL_LUMINANCE_ALPHA:
	case GL_RG:
	/* case GL_LUMINANCE_ALPHA_INTEGER: deprecated */
	case GL_RG_INTEGER:
		*groupBytes = *elementBytes * 2;
		break;
	case GL_RGB:
	case GL_BGR:
	case GL_RGB_INTEGER:
	case GL_BGR_INTEGER:
		*groupBytes = *elementBytes * 3;
		break;
	case GL_RGBA:
	case GL_BGRA:
	case GL_RGBA_INTEGER:
	case GL_BGRA_INTEGER:
		*groupBytes = *elementBytes * 4;
		break;
	default:
		retVal = KTX_INVALID_VALUE;
	}

	return retVal;
}

/*
 * @brief Get the size of a GL type in basic machine units
 *        and indicate whether or not it is a packed type.
 *
 * @param [in]  type	the type whose size is to be returned.
 * @param [out] size	pointer to location where to write the size
 * @param [out] packed	pointer to location where to write flag indicating
 *                      if the type is a packed type.
 *
 * @return KTX_INVALID_VALUE if the @p type is unrecognized.
 */
static KTX_error_code
sizeofGLtype(GLenum type, GLuint* size, GLboolean* packed)
{
	assert(packed && size);
	*packed = GL_FALSE;

	switch (type) {
		case GL_BYTE:
			*size = sizeof(GLbyte);
			break;

		case GL_UNSIGNED_BYTE:
			*size = sizeof(GLubyte);
			break;

		case GL_UNSIGNED_BYTE_3_3_2:
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			*packed = GL_TRUE;
			*size = sizeof(GLubyte);
			break;

		case GL_SHORT:
			*size = sizeof(GLshort);
			break;

		case GL_UNSIGNED_SHORT:
			*size = sizeof(GLushort);
			break;

		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_5_6_5_REV:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			*packed = GL_TRUE;
			*size = sizeof(GLushort);
			break;

		case GL_INT:
			*size = sizeof(GLint);
			break;

		case GL_UNSIGNED_INT:
			*size = sizeof(GLuint);
			break;

		case GL_UNSIGNED_INT_8_8_8_8:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
		case GL_UNSIGNED_INT_10_10_10_2:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_24_8:
		case GL_UNSIGNED_INT_10F_11F_11F_REV:
		case GL_UNSIGNED_INT_5_9_9_9_REV:
			*packed = GL_TRUE;
			*size = sizeof(GLuint);
			break;

		case GL_HALF_FLOAT:
			*size = sizeof(GLhalf);
			break;

		case GL_FLOAT:
			*size = sizeof(GLfloat);
			break;

		case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
			*packed = GL_TRUE;
			*size = sizeof(GLfloat) + sizeof(GLint);
			break;

		default:
			return KTX_INVALID_VALUE;
	}
	return KTX_SUCCESS;
}

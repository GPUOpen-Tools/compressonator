/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/**
 * @file
 * @~English
 *
 * @brief Implementation of ktxStream for FILE.
 *
 * @author Maksim Kolesin, Under Development
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

#include <string.h>
#include <stdlib.h>

#include "KHR/khrplatform.h"
#include "ktx.h"
#include "ktxint.h"
#include "ktxfilestream.h"

/**
 * @internal
 * @~English
 * @brief Read bytes from a ktxFileStream.
 *
 * @param [out] dst           pointer to a block of memory with a size
			   of at least @p size bytes, converted to a void*.
 * @param [in] size          total size of bytes to be read.
 * @param [in] src           pointer to a FILE object, converted to a void*, that specifies an input stream.
 *
 * @return      KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE @p dst is @c NULL or @p src is @c NULL.
 * @exception KTX_UNEXPECTED_END_OF_FILE the file does not contain the expected amount of data.
 */
static
KTX_error_code ktxFileStream_read(void* dst, const GLsizei size, void* src)
{
	if (!dst || !src)
		return KTX_INVALID_VALUE;

	if (fread(dst, size, 1, (FILE*)src) != 1)
		return KTX_UNEXPECTED_END_OF_FILE;

	return KTX_SUCCESS;
}

/**
 * @internal
 * @~English
 * @brief Skip bytes in a ktxFileStream.
 *
 * @param [in] count         number of bytes to be skipped.
 * @param [in] src           pointer to a FILE object, converted to a void*, that specifies an input stream.
 *
 * @return      KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE @p dst is @c NULL or @p count is less than zero.
 * @exception KTX_UNEXPECTED_END_OF_FILE the file does not contain the expected amount of data.
 */
static
KTX_error_code ktxFileStream_skip(const GLsizei count, void* src)
{
	if (!src || (count < 0))
		return KTX_INVALID_VALUE;

	if (fseek((FILE*)src, count, SEEK_CUR) != 0)
		return KTX_UNEXPECTED_END_OF_FILE;

	return KTX_SUCCESS;
}

/**
 * @internal
 * @~English
 * @brief Write bytes to a ktxFileStream.
 *
 * @param [in] src           pointer to the array of elements to be written, converted to a const void*.
 * @param [in] size          size in bytes of each element to be written.
 * @param [in] count         number of elements, each one with a @p size of size bytes.
 * @param [out] dst          pointer to a FILE object, converted to a void*, that specifies an output stream.
 *
 * @return      KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE @p dst is @c NULL or @p src is @c NULL.
 * @exception KTX_FILE_WRITE_ERROR a system error occurred while writing the file.
 */
static
KTX_error_code ktxFileStream_write(const void *src, const GLsizei size, const GLsizei count, void* dst)
{
	if (!dst || !src)
		return KTX_INVALID_VALUE;

	if (fwrite(src, size, count, (FILE*)dst) != count)
		return KTX_FILE_WRITE_ERROR;

	return KTX_SUCCESS;
}

/**
 * @internal
 * @~English
 * @brief Initializes a ktxFileStream.
 *
 * @param [in] stream
 * @param [in] file
 *
 * @return      KTX_SUCCESS on success, KTX_INVALID_VALUE on error.
 *
 * @exception KTX_INVALID_VALUE @p stream is @c NULL or @p file is @c NULL.
 */
KTX_error_code ktxFileInit(struct ktxStream* stream, FILE* file)
{
	if (!stream || !file)
		return KTX_INVALID_VALUE;

	stream->src = (void*)file;
	stream->read = ktxFileStream_read;
	stream->skip = ktxFileStream_skip;
	stream->write = ktxFileStream_write;

	return KTX_SUCCESS;
}

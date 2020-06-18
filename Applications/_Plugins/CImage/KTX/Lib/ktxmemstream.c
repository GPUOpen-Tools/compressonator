/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/**
 * @file
 * @~English
 *
 * @brief Implementation of ktxStream for memory.
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
#include "ktxmemstream.h"

/**
 * @internal
 * @~English
 * @brief Expand a ktxMem to fit to a newsize.
 *
 * @param [in] mem           pointer to ktxMem struct to expand.
 * @param [in] newsize       minimum new size required.
 *
 * @return      KTX_SUCCESS on success, KTX_OUT_OF_MEMORY on error.
 *
 * @exception KTX_OUT_OF_MEMORY        System failed to allocate sufficient memory.
 */
static
KTX_error_code ktxMem_expand(struct ktxMem *mem, const GLsizei newsize)
{
	GLsizei new_alloc_size = mem->alloc_size;
	while (new_alloc_size < newsize)
		new_alloc_size <<= 1;

	if (new_alloc_size == mem->alloc_size)
		return KTX_SUCCESS;

	mem->bytes = (unsigned char*)realloc(mem->bytes, new_alloc_size);
	if(!mem->bytes)
	{
		mem->alloc_size = 0;
		mem->used_size = 0;
		return KTX_OUT_OF_MEMORY;
	}

	mem->alloc_size = new_alloc_size;
	return KTX_SUCCESS;
}

/**
 * @internal
 * @~English
 * @brief Read bytes from a ktxMemStream.
 *
 * @param [out] dst          pointer to memory where to copy read bytes.
 * @param [in] count         number of bytes to read.
 * @param [in] src           pointer to ktxMem struct, converted to a void*, that specifies an input stream.
 *
 * @return      KTX_SUCCESS on success, KTX_INVALID_VALUE on error.
 *
 * @exception KTX_INVALID_VALUE        @p dst is @c NULL or @p mem is @c NULL or not sufficient data is available in ktxMem.
 */
static
KTX_error_code ktxMemStream_read(void* dst, const GLsizei count, void* src)
{
	struct ktxMem* mem = (struct ktxMem*)src;

	if(!dst || !mem || (mem->pos + count > mem->used_size) || (mem->pos + count < mem->pos))
		return KTX_INVALID_VALUE;

	memcpy(dst, mem->bytes + mem->pos, count);
	mem->pos += count;

	return KTX_SUCCESS;
}

/**
 * @internal
 * @~English
 * @brief Skip bytes in a ktxFileStream.
 *
 * @param [in] count         number of bytes to skip.
 * @param [in] src           pointer to a ktxMem struct, converted to a void*, that specifies an input stream.
 *
 * @return      KTX_SUCCESS on success, KTX_INVALID_VALUE on error.
 *
 * @exception KTX_INVALID_VALUE        @p mem is @c NULL or not sufficient data is available in ktxMem.
 */
static
KTX_error_code ktxMemStream_skip(const GLsizei count, void* src)
{
	struct ktxMem* mem = (struct ktxMem*)src;

	if(!mem || (mem->pos + count > mem->used_size) || (mem->pos + count < mem->pos))
		return KTX_INVALID_VALUE;

	mem->pos += count;

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
 * @param [out] dst          pointer to a ktxMem struct, converted to a void*, that specifies an output stream.
 *
 * @return      KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE        @p dst is @c NULL or @p mem is @c NULL.
 * @exception KTX_OUT_OF_MEMORY        See ktxMem_expand() for causes.
 */
static
KTX_error_code ktxMemStream_write(const void* src, const GLsizei size, const GLsizei count, void* dst)
{
	struct ktxMem* mem = (struct ktxMem*)dst;
	KTX_error_code rc = KTX_SUCCESS;

	if(!dst || !mem)
		return KTX_INVALID_VALUE;

	if(mem->alloc_size < mem->used_size + size*count)
	{
		rc = ktxMem_expand(mem, mem->used_size + size*count);
		if(rc != KTX_SUCCESS)
			return rc;
	}

	memcpy(mem->bytes + mem->used_size, src, size*count);
	mem->used_size += size*count;

	return KTX_SUCCESS;
}

/**
 * @brief Default allocation size for a ktxMemStream.
 */
#define KTX_MEM_DEFAULT_ALLOCATED_SIZE 256

/**
 * @internal
 * @~English
 * @brief Initialize a ktxMemStream.
 *
 * @param [in] stream        pointer to a ktxStream struct to initialize.
 * @param [in] mem           pointer to a ktxMem struct to use in ktxMemStream.
 * @param [in] bytes         pointer to an array of bytes to use as initial data.
 * @param [in] size          size of array of initial data for ktxMemStream.
 *
 * @return      KTX_SUCCESS on success, other KTX_* enum values on error.
 *
 * @exception KTX_INVALID_VALUE        @p stream is @c NULL or @p mem is @c NULL or @p size is less than 0.
 * @exception KTX_OUT_OF_MEMORY        system failed to allocate sufficient memory.
 */
KTX_error_code ktxMemInit(struct ktxStream* stream, struct ktxMem* mem, const void* bytes, GLsizei size)
{
	if (!stream || !mem || size < 0)
		return KTX_INVALID_VALUE;

	if(!bytes)
	{
		if (size == 0)
			size = KTX_MEM_DEFAULT_ALLOCATED_SIZE;
		mem->bytes = (unsigned char*)malloc(size);
		if (!mem->bytes)
			return KTX_OUT_OF_MEMORY;
		mem->alloc_size = size;
		mem->used_size = 0;
		mem->pos = 0;
	}
	else
	{
		mem->bytes = (unsigned char*)bytes;
		mem->used_size = size;
		mem->alloc_size = size;
		mem->pos = 0;
	}

	stream->src = mem;
	stream->read = ktxMemStream_read;
	stream->skip = ktxMemStream_skip;
	stream->write = ktxMemStream_write;

	return KTX_SUCCESS;
}

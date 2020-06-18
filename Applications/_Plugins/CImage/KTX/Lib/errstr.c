/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: 66fc783ff8cf9a7836f10cccf03607bad0860246 $ */

/**
 * @file errstr.c
 * @~English
 *
 * @brief Function to return a string corresponding to a KTX error code.
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

#include "ktx.h"

static const char* const errorStrings[] = {
	"Operation succeeded",							  /* KTX_SUCCESS */
	"File open failed",								  /* KTX_FILE_OPEN_FAILED */
	"File write failed",							  /* KTX_FILE_WRITE_ERROR */
	"GL error occurred",							  /* KTX_GL_ERROR */
	"Operation not allowed in the current state",     /* KTX_INVALID_OPERATION */
	"Invalid parameter value",						  /* KTX_INVALID_VALUE */
	"Key not found",								  /* KTX_NOT_FOUND */
	"Out of memory",								  /* KTX_OUT_OF_MEMORY */
	"Unexpected end of file",						  /* KTX_UNEXPECTED_END_OF_FILE */
	"Not a KTX file",								  /* KTX_UNKNOWN_FILE_FORMAT */
	"Texture type not supported by GL context"		  /* KTX_UNSUPPORTED_TEXTURE_TYPE */
};
static const int lastErrorCode = (sizeof(errorStrings) / sizeof(char*)) - 1;


/**
 * @~English
 * @brief Return a string corresponding to a KTX error code.
 *
 * @param error		the error code for which to return a string
 *
 * @return pointer to the message string.
 *
 * @internal Use UTF-8 for translated message strings.
 * 
 * @author Mark Callow, HI Corporation
 */
const char* const ktxErrorString(KTX_error_code error)
{
	if (error > lastErrorCode)
		return "Unrecognized error code";
	return errorStrings[error];
}

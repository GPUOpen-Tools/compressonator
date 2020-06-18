/* -*- tab-width: 4; -*- */
/* vi: set sw=2 ts=4: */

/* $Id: e7d280e42d27a4bbebf0bc43c2b48fd5d77af25a $ */

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
 * Author: Mark Callow based on code from Georg Kolling
 */

#ifndef GLES3_FUNCPTRS_H
#define GLES3_FUNCPTRS_H

#ifdef __cplusplus
extern "C" {
#endif

/* remove these where already defined as typedefs */
typedef void (GL_APIENTRY* PFNGLTEXIMAGE1DPROC) (
                        GLenum target, GLint level, GLint internalformat,
                        GLsizei width, GLint border, GLenum format,
                        GLenum type, const GLvoid *pixels
                                                );
typedef void (GL_APIENTRY* PFNGLCOMPRESSEDTEXIMAGE1DPROC) (
                        GLenum target, GLint level, GLenum internalformat,
                        GLsizei width, GLint border, GLsizei imageSize,
                        const GLvoid *data
                                                          );

extern PFNGLTEXIMAGE1DPROC pfGlTexImage1D;
extern PFNGLTEXIMAGE3DPROC pfGlTexImage3D;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC pfGlCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC pfGlCompressedTexImage3D;
extern PFNGLGENERATEMIPMAPPROC pfGlGenerateMipmap;
extern PFNGLGETSTRINGIPROC pfGlGetStringi;

#define DECLARE_GL_FUNCPTRS \
    PFNGLTEXIMAGE1DPROC pfGlTexImage1D; \
    PFNGLTEXIMAGE3DPROC pfGlTexImage3D; \
    PFNGLCOMPRESSEDTEXIMAGE1DPROC pfGlCompressedTexImage1D; \
    PFNGLCOMPRESSEDTEXIMAGE3DPROC pfGlCompressedTexImage3D; \
    PFNGLGENERATEMIPMAPPROC pfGlGenerateMipmap; \
    PFNGLGETSTRINGIPROC pfGlGetStringi;

#define INITIALIZE_GL_FUNCPTRS \
    pfGlTexImage1D = 0; \
    pfGlTexImage3D = glTexImage3D; \
    pfGlCompressedTexImage1D = 0; \
    pfGlCompressedTexImage3D = glCompressedTexImage3D; \
    pfGlGenerateMipmap = glGenerateMipmap; \
    pfGlGetStringi = glGetStringi;

#ifdef __cplusplus
}
#endif

#endif /* GLES3_FUNCPTRS_H */

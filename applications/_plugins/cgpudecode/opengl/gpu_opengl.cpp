//=====================================================================
// Copyright (c) 2016-2018    Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// \file GPU_OpenGL.cpp
//
//=====================================================================

#include "gpu_opengl.h"
#include "common.h"
#include "compressonator.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include "gpu_decode.h"

#include <assert.h>

#if defined(_WIN32) //&& !defined(NO_LEGACY_BEHAVIOR)
#pragma comment(lib, "opengl32.lib")     // Open GL
#pragma comment(lib, "Glu32.lib")        // Glu 
#pragma comment(lib, "glew32.lib")       // glew 
#else
#ifdef _WIN32
#pragma comment(lib, "opengl32.lib")     // Open GL
#pragma comment(lib, "Glu32.lib")        // Glu 
//#ifdef _DEBUG
//    #pragma comment(lib, "glew32d.lib")   // glew
//#else
#pragma comment(lib, "glew32.lib")   // glew
//#endif
#else
#pragma comment(lib, "libglew32.lib")   // glew
#endif
#endif

static_assert(sizeof(unsigned int) == sizeof(GLuint), "Inconsistent size for GLuint");
static_assert(sizeof(unsigned int) == sizeof(GLenum), "Inconsistent size for GLenum");

using namespace GPU_Decode;

GPU_OpenGL::GPU_OpenGL(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback):RenderWindow("OpenGL") {
    //set default width and height if is 0
    if (Width <= 0)
        Width = 640;
    if (Height <= 0)
        Height = 480;

    if (FAILED(InitWindow(Width, Height, callback))) {
        fprintf(stderr, "Failed to initialize Window. Please make sure GLEW is downloaded.\n");
        assert(0);
    }

    EnableWindowContext(m_hWnd, &m_hDC, &m_hRC);
}

GPU_OpenGL::~GPU_OpenGL() {
}

//====================================================================================
// #define SHOW_WINDOW

void GPU_OpenGL::GLRender() {
    // OpenGL animation code goes here
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // setup texture mapping
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPushMatrix();
    glRotatef(theta, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);

#ifdef SHOW_WINDOW
    //for certain image format like bmp, the image texture is upside down, need to use coordinate as below
    //use when showwindow and swapbuffer enabled
    glTexCoord2d(0.0, 1.0);
    glVertex2d(-1.0, -1.0);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(+1.0, -1.0);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(+1.0, +1.0);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(-1.0, +1.0);
#else
    //for dds use coordinate below
    glTexCoord2d(0.0, 0.0);
    glVertex2d(-1.0, -1.0);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(+1.0, -1.0);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(+1.0, +1.0);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(-1.0, +1.0);
#endif
    glEnd();
    glPopMatrix();

#ifdef SHOW_WINDOW
    //for debug when showwindow is enable
    SwapBuffers(m_hDC);
#endif

}

unsigned int GPU_OpenGL::MIP2OLG_Format(const CMP_Texture* pSourceTexture) {
    GLenum m_GLnum;
    switch (pSourceTexture->format) {
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
        m_GLnum = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:
        m_GLnum = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:
        m_GLnum = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:
        m_GLnum = GL_COMPRESSED_RED_RGTC1;
        break;
    case CMP_FORMAT_BC4_S:
        m_GLnum = GL_COMPRESSED_SIGNED_RED_RGTC1;
        break;
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N:
    case CMP_FORMAT_ATI2N_XY:
    case CMP_FORMAT_ATI2N_DXT5:
        m_GLnum = GL_COMPRESSED_RG_RGTC2;
        break;
    case CMP_FORMAT_BC5_S:
        m_GLnum = GL_COMPRESSED_SIGNED_RG_RGTC2;
        break;
    case CMP_FORMAT_BC6H:
        m_GLnum = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
        break;
    case CMP_FORMAT_BC6H_SF:
        m_GLnum = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
        break;
    case CMP_FORMAT_BC7:
        m_GLnum = GL_COMPRESSED_RGBA_BPTC_UNORM;
        break;
    case CMP_FORMAT_ETC_RGB:
    case CMP_FORMAT_ETC2_RGB:
        m_GLnum = GL_COMPRESSED_RGB8_ETC2;
        break;
    case CMP_FORMAT_ETC2_SRGB:
        m_GLnum = GL_COMPRESSED_SRGB8_ETC2;
        break;
    case CMP_FORMAT_ETC2_RGBA:
        m_GLnum = GL_COMPRESSED_RGBA8_ETC2_EAC;
        break;
    case CMP_FORMAT_ETC2_RGBA1:
        m_GLnum = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
        break;
    case CMP_FORMAT_ETC2_SRGBA:
        m_GLnum = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
        break;
    case CMP_FORMAT_ETC2_SRGBA1:
        m_GLnum = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
        break;
    case CMP_FORMAT_ASTC:
        if ((pSourceTexture->nBlockWidth == 4) && (pSourceTexture->nBlockHeight == 4))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        else if ((pSourceTexture->nBlockWidth == 5) && (pSourceTexture->nBlockHeight == 4))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
        else if ((pSourceTexture->nBlockWidth == 5) && (pSourceTexture->nBlockHeight == 5))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
        else if ((pSourceTexture->nBlockWidth == 6) && (pSourceTexture->nBlockHeight == 5))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
        else if ((pSourceTexture->nBlockWidth == 6) && (pSourceTexture->nBlockHeight == 6))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
        else if ((pSourceTexture->nBlockWidth == 8) && (pSourceTexture->nBlockHeight == 5))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
        else if ((pSourceTexture->nBlockWidth == 8) && (pSourceTexture->nBlockHeight == 6))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
        else if ((pSourceTexture->nBlockWidth == 8) && (pSourceTexture->nBlockHeight == 8))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
        else if ((pSourceTexture->nBlockWidth == 10) && (pSourceTexture->nBlockHeight == 5))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
        else if ((pSourceTexture->nBlockWidth == 10) && (pSourceTexture->nBlockHeight == 6))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
        else if ((pSourceTexture->nBlockWidth == 10) && (pSourceTexture->nBlockHeight == 8))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
        else if ((pSourceTexture->nBlockWidth == 10) && (pSourceTexture->nBlockHeight == 10))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
        else if ((pSourceTexture->nBlockWidth == 12) && (pSourceTexture->nBlockHeight == 10))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
        else if ((pSourceTexture->nBlockWidth == 12) && (pSourceTexture->nBlockHeight == 12))
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
        else
            m_GLnum = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        break;
    default:
        m_GLnum = GL_INVALID_ENUM;
        break;
    }
    return static_cast<unsigned int>(m_GLnum);
}

// load pre-compressed texture
unsigned int GPU_OpenGL::LoadTexture(const CMP_Texture* pSourceTexture, bool wrap) {
    GLenum m_GLnum = MIP2OLG_Format(pSourceTexture);
    if (m_GLnum == GL_INVALID_ENUM) {
        fprintf(stderr, "Unsupported format.\n");
        return static_cast<unsigned int>(GLuint(-1));
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW. Please make sure GLEW is downloaded.\n");
        return GLuint(-1);
    }

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    //modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // bilinear filter the closest MIP map for small texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // bilinear filter the first MIP map for large texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // if wrap is true, the texture wraps over at the edges (repeat)
    // false, the texture ends at the edges (clamp)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,wrap ? GL_REPEAT : GL_CLAMP);

    // build texture MIP maps
    glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    //for uncompressed image
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    //for compressed image (only for mip level 1)
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, m_GLnum, pSourceTexture->dwWidth, pSourceTexture->dwHeight, 0, pSourceTexture->dwDataSize, pSourceTexture->pData);

    return texture;
}

// Free Texture
void GPU_OpenGL::FreeTexture(unsigned int texture) {
    glDeleteTextures(1, static_cast<GLuint*>(&texture));
}

//=========================================================================================

CMP_ERROR WINAPI GPU_OpenGL::Decompress(
    const CMP_Texture* pSourceTexture,
    CMP_Texture* pDestTexture
) {

    GLint majVer = 0;
    GLint minVer = 0;

    glGetIntegerv(GL_MAJOR_VERSION, &majVer);
    glGetIntegerv(GL_MINOR_VERSION, &minVer);

    if (majVer < 3 || (majVer < 3 && minVer < 2)) {
        PrintInfo("Error: OpenGL 3.2 and up cannot be detected.\n");
        fprintf(stderr, "Error: OpenGL 3.2 and up cannot be detected.\n" );
        return CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB;
    }

    texture = LoadTexture(pSourceTexture, false);
    if (texture == -1) {
        return CMP_ERR_UNSUPPORTED_SOURCE_FORMAT;
    }

#ifdef SHOW_WINDOW
    //for debug, show window to view image
    ShowWindow(m_hWnd, SW_SHOW);
#endif
    //  Wait in Main message loop, until render is complete
    //  then exit
    MSG msg = { 0 };
    int loopcount = 0;
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) && (loopcount < 100)) {
            loopcount++;
            TranslateMessage(&msg);
        } else {
            GLRender();
            break;
        }
    }

    if (pDestTexture) {
        if (pSourceTexture->format == CMP_FORMAT_ETC_RGB ||
                pSourceTexture->format == CMP_FORMAT_ETC2_RGB ||
                pSourceTexture->format == CMP_FORMAT_ETC2_RGBA ||
                pSourceTexture->format == CMP_FORMAT_ETC2_RGBA1
           )
           glReadPixels(0, 0, pDestTexture->dwWidth, pDestTexture->dwHeight, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pDestTexture->pData);
        else if (pSourceTexture->format == CMP_FORMAT_ETC2_SRGB ||
                 pSourceTexture->format == CMP_FORMAT_ETC2_SRGBA ||
                 pSourceTexture->format == CMP_FORMAT_ETC2_SRGBA1
                )
           glReadPixels(0, 0, pDestTexture->dwWidth, pDestTexture->dwHeight, GL_BGRA_EXT, GL_BYTE, pDestTexture->pData);
        else {
            if (pDestTexture->format == CMP_FORMAT_ARGB_16F)
            {
                glReadPixels(0, 0, pDestTexture->dwWidth, pDestTexture->dwHeight, GL_RGBA, GL_HALF_FLOAT, pDestTexture->pData);
            }
            else if (pDestTexture->format == CMP_FORMAT_RGBA_8888_S)
            {
                glReadPixels(0, 0, pDestTexture->dwWidth, pDestTexture->dwHeight, GL_RGBA_SNORM, GL_BYTE, pDestTexture->pData);
            }
            else
                glReadPixels(0, 0, pDestTexture->dwWidth, pDestTexture->dwHeight, GL_RGBA, GL_UNSIGNED_BYTE, pDestTexture->pData);
        }
    }

    // free the texture
    FreeTexture(texture);

    return CMP_OK; // msg.wParam;
}

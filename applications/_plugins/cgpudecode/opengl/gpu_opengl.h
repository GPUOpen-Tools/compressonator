//=====================================================================
// Copyright (c) 2016    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file GPU_OpenGL.h
//
//=====================================================================

#ifndef H_GPU_OPENGL
#define H_GPU_OPENGL

#include "gpu_decodebase.h"
#include "gpu_decode.h"
#include "compressonator.h"

namespace GPU_Decode {
class GPU_OpenGL : public RenderWindow {
  public:
    GPU_OpenGL(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback);
    ~GPU_OpenGL();

    virtual CMP_ERROR WINAPI Decompress(
        const CMP_Texture* pSourceTexture,
        CMP_Texture* pDestTexture
    );

    unsigned int MIP2OLG_Format(const CMP_Texture* pSourceTexture);
    void GLRender();

  private:
    unsigned int texture;
    float theta = 0.0f;
    bool b_glewInit = false;

    unsigned int  LoadTexture(const CMP_Texture* pSourceTexture, bool wrap);
    void    FreeTexture(unsigned int texture);
};
}

//---------------------------------------------------------------------------------


#endif

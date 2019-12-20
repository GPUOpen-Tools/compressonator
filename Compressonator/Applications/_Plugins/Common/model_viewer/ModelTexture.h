// AMD AMDUtils code
// 
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef MODELTEXTURE_E467CD76_2A95_402D_B989_A39D10E969E1_H
#define MODELTEXTURE_E467CD76_2A95_402D_B989_A39D10E969E1_H

#include "Compressonator.h"
#include "Common.h"
#include "Texture.h"
#include "PluginInterface.h"
#include "TextureIO.h"

// This class provides functionality to create a 2D-texture
// Helper functions to translate Compressonator Formats types to GLTF 2.0 specification standard is also provided

class ModelTexture
{
public:
     ModelTexture();
    ~ModelTexture();

    void CleanMipSet();
    void setCMIPS(CMIPS *cmips);
    INT32  LoadImageMipSetFromFile(const WCHAR *szFilename, void *pluginManager);

    MipSet                *pMipSet;
    CMIPS                 *m_CMIPS;
    CMIPS                  l_cmips;
};

#endif

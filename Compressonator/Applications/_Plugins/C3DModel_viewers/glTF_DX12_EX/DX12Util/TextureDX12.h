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

#pragma once

#include "ResourceViewHeapsDX12.h"
#include "UploadHeapDX12.h"

#define USE_CMP_LOAD

#include "Compressonator.h"
#include "GPU_Decode\GPU_Decode.h"
#include "..\..\..\Common\Texture.h"
#include "..\..\..\Common\PluginInterface.h"
#include "TextureIO.h"

#include "ModelTexture.h"

extern CMIPS *DX12_CMips;

// This class provides functionality to create a 2D-texture from a .DDS file.
// Note that this sample can only load DDS files!

class TextureDX12: public ModelTexture
{
public:
    TextureDX12();
    virtual                 ~TextureDX12();
    virtual void            OnDestroy();

    // load file into heap
    INT32 InitFromFile(ID3D12Device* pDevice, UploadHeapDX12* pUploadHeap, const WCHAR *szFilename, void *pluginManager, void *msghandler);
    void  InitDebugTexture(ID3D12Device* pDevice, UploadHeapDX12* pUploadHeap);
    INT32 InitRendertarget(ID3D12Device* pDevice, CD3DX12_RESOURCE_DESC *pDesc, UINT node = 0, UINT nodemask = 0);
    INT32 InitDepthStencil(ID3D12Device* pDevice, CD3DX12_RESOURCE_DESC *pDesc, UINT node = 0, UINT nodemask = 0);

    ID3D12Resource* Resource()
    {
        return m_pTexture2D;
    }
    
    void CreateUAV(DWORD index, CBV_SRV_UAV *pRV);
    void CreateRTV(DWORD index, RTV *pRV);
    void CreateDSV(DWORD index, DSV *pRV);
    void CreateSRV(DWORD index, CBV_SRV_UAV *pRV);
    void CreateCubeSRV(DWORD index, CBV_SRV_UAV *pRV);

    DWORD GetWidth()  { return m_header.width;  }
    DWORD GetHeight() { return m_header.height; }

private:

    struct DDS_PIXELFORMAT
    {
        UINT32 size;
        UINT32 flags;
        UINT32 fourCC;
        UINT32 bitCount;
        UINT32 bitMaskR;
        UINT32 bitMaskG;
        UINT32 bitMaskB;
        UINT32 bitMaskA;
    } ;

    struct DDS_HEADER
    {
        UINT32       dwSize;
        UINT32       dwHeaderFlags;
        UINT32       dwHeight;
        UINT32       dwWidth;
        UINT32       dwPitchOrLinearSize;
        UINT32       dwDepth;
        UINT32       dwMipMapCount;
        UINT32       dwReserved1[11];
        DDS_PIXELFORMAT ddspf;
        UINT32       dwSurfaceFlags;
        UINT32       dwCubemapFlags;
        UINT32       dwCaps3;
        UINT32       dwCaps4;
        UINT32       dwReserved2;
    };

    struct DDS_HEADER_INFO
    {
        UINT32           width;
        UINT32           height;
        UINT32           depth;
        UINT32           arraySize;
        UINT32           mipMapCount;
        UINT32           format;
    };

    ID3D12Resource         *m_pTexture2D;

    DDS_HEADER_INFO         m_header;

    void                    PatchFmt24To32Bit(unsigned char *pDst, unsigned char *pSrc, UINT32 pixelCount);
    UINT32                  GetDxGiFormat(DDS_PIXELFORMAT pixelFmt) const;
    bool                    isDXT(DXGI_FORMAT format)const;
    UINT32                  GetPixelSize(DXGI_FORMAT fmt) const;
    bool                    isCubemap()const;
};


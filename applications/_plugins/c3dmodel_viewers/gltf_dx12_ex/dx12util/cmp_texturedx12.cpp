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
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
// http://go.microsoft.com/fwlink/?LinkId=248929


#include "cmp_texturedx12.h"
#include "dxgi.h"
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "misc.h"
#include "utilfuncs.h"

//--------------------------------------------------------------------------------------
// Constructor of the Texture class
// initializes all members
//--------------------------------------------------------------------------------------
TextureDX12::TextureDX12() {
    m_pTexture2D = NULL;
}
//--------------------------------------------------------------------------------------
// Destructor of the Texture class
//--------------------------------------------------------------------------------------
TextureDX12::~TextureDX12() {
    OnDestroy();
}

void TextureDX12::OnDestroy() {
    if (m_pTexture2D != NULL) {
        m_pTexture2D->Release();
        m_pTexture2D = NULL;
    }
}

UINT32 TextureDX12::GetDxGiFormat(TextureDX12::DDS_PIXELFORMAT pixelFmt) const {
    if(pixelFmt.flags & 0x00000004) { //DDPF_FOURCC
        if (MAKEFOURCC( 'D', 'X', 'T', '1' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC1_UNORM;
        }
        if (MAKEFOURCC( 'D', 'X', 'T', '3' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC( 'D', 'X', 'T', '5' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC3_UNORM;
        }

        // While pre-multiplied alpha isn't directly supported by the DXGI formats,
        // they are basically the same as these BC formats so they can be mapped
        if (MAKEFOURCC( 'D', 'X', 'T', '2' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC( 'D', 'X', 'T', '4' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC3_UNORM;
        }

        if (MAKEFOURCC( 'A', 'T', 'I', '1' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC( 'B', 'C', '4', 'U' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC( 'B', 'C', '4', 'S' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC4_SNORM;
        }

        if (MAKEFOURCC( 'A', 'T', 'I', '2' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC( 'B', 'C', '5', 'U' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC( 'B', 'C', '5', 'S' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_BC5_SNORM;
        }

        // BC6H and BC7 are written using the "DX10" extended header

        if (MAKEFOURCC( 'R', 'G', 'B', 'G' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_R8G8_B8G8_UNORM;
        }
        if (MAKEFOURCC( 'G', 'R', 'G', 'B' ) == pixelFmt.fourCC) {
            return DXGI_FORMAT_G8R8_G8B8_UNORM;
        }

        if (MAKEFOURCC('Y','U','Y','2') == pixelFmt.fourCC) {
            return DXGI_FORMAT_YUY2;
        }

        // Check for D3DFORMAT enums being set here
        switch(pixelFmt.fourCC) {
        case 36: // D3DFMT_A16B16G16R16
            return DXGI_FORMAT_R16G16B16A16_UNORM;

        case 110: // D3DFMT_Q16W16V16U16
            return DXGI_FORMAT_R16G16B16A16_SNORM;

        case 111: // D3DFMT_R16F
            return DXGI_FORMAT_R16_FLOAT;

        case 112: // D3DFMT_G16R16F
            return DXGI_FORMAT_R16G16_FLOAT;

        case 113: // D3DFMT_A16B16G16R16F
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case 114: // D3DFMT_R32F
            return DXGI_FORMAT_R32_FLOAT;

        case 115: // D3DFMT_G32R32F
            return DXGI_FORMAT_R32G32_FLOAT;

        case 116: // D3DFMT_A32B32G32R32F
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        default:
            return 0;
        }
    } else {
        {
            switch(pixelFmt.bitMaskR) {
            case 0xff:
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case 0x00ff0000:
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            case 0xffff:
                return DXGI_FORMAT_R16G16_UNORM;
            case 0x3ff:
                return DXGI_FORMAT_R10G10B10A2_UNORM;
            case 0x7c00:
                return DXGI_FORMAT_B5G5R5A1_UNORM;
            case 0xf800:
                return DXGI_FORMAT_B5G6R5_UNORM;
            case 0:
                return DXGI_FORMAT_A8_UNORM;
            default:
                return 0;
            };

            if (pixelFmt.bitCount == 32)  {

                if (ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0xff000000)) {
                    return DXGI_FORMAT_R8G8B8A8_UNORM;
                }

                if (ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000)) {
                    return DXGI_FORMAT_B8G8R8A8_UNORM;
                }

                if (ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000)) {
                    return DXGI_FORMAT_B8G8R8X8_UNORM;
                }

                // No DXGI format maps to ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) aka D3DFMT_X8B8G8R8

                // Note that many common DDS reader/writers (including D3DX) swap the
                // the RED/BLUE masks for 10:10:10:2 formats. We assume
                // below that the 'backwards' header mask is being used since it is most
                // likely written by D3DX. The more robust solution is to use the 'DX10'
                // header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

                // For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
                if (ISBITMASK(0x3ff00000,0x000ffc00,0x000003ff,0xc0000000)) {
                    return DXGI_FORMAT_R10G10B10A2_UNORM;
                }

                // No DXGI format maps to ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

                if (ISBITMASK(0x0000ffff,0xffff0000,0x00000000,0x00000000)) {
                    return DXGI_FORMAT_R16G16_UNORM;
                }

                if (ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000)) {
                    // Only 32-bit color channel format in D3D9 was R32F
                    return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
                }
            }

            if (pixelFmt.bitMaskR == 0xffff)
                return DXGI_FORMAT_R16G16_UNORM;

            if (pixelFmt.bitMaskR == 0x3ff)
                return DXGI_FORMAT_R10G10B10A2_UNORM;

            if (pixelFmt.bitMaskR == 0x7c00)
                return DXGI_FORMAT_B5G5R5A1_UNORM;

            if (pixelFmt.bitMaskR == 0xf800)
                return DXGI_FORMAT_B5G6R5_UNORM;

            if (pixelFmt.bitMaskR == 0)
                return DXGI_FORMAT_A8_UNORM;
        }
    }
    return 0;
}

bool TextureDX12::isDXT(DXGI_FORMAT format) const {
    return (format >= DXGI_FORMAT_BC1_TYPELESS) && (format <= DXGI_FORMAT_BC5_SNORM);
}


//--------------------------------------------------------------------------------------
// return the byte size of a pixel (or block if block compressed)
//--------------------------------------------------------------------------------------
UINT32 TextureDX12::GetPixelSize(DXGI_FORMAT fmt) const {
    switch(fmt) {
    case(DXGI_FORMAT_BC1_TYPELESS) :
    case(DXGI_FORMAT_BC1_UNORM) :
    case(DXGI_FORMAT_BC1_UNORM_SRGB) :
    case(DXGI_FORMAT_BC4_TYPELESS) :
    case(DXGI_FORMAT_BC4_UNORM) :
    case(DXGI_FORMAT_BC4_SNORM) :
        return 8;

    case(DXGI_FORMAT_BC2_TYPELESS) :
    case(DXGI_FORMAT_BC2_UNORM) :
    case(DXGI_FORMAT_BC2_UNORM_SRGB) :
    case(DXGI_FORMAT_BC3_TYPELESS) :
    case(DXGI_FORMAT_BC3_UNORM) :
    case(DXGI_FORMAT_BC3_UNORM_SRGB) :
    case(DXGI_FORMAT_BC5_TYPELESS) :
    case(DXGI_FORMAT_BC5_UNORM) :
    case(DXGI_FORMAT_BC5_SNORM) :
    case(DXGI_FORMAT_BC6H_TYPELESS) :
    case(DXGI_FORMAT_BC6H_UF16) :
    case(DXGI_FORMAT_BC6H_SF16) :
    case(DXGI_FORMAT_BC7_TYPELESS) :
    case(DXGI_FORMAT_BC7_UNORM) :
    case(DXGI_FORMAT_BC7_UNORM_SRGB) :
        return 16;

    default:
        break;
    }
    return 0;
}

void TextureDX12::PatchFmt24To32Bit(unsigned char *pDst, unsigned char *pSrc, UINT32 pixelCount) {
    // copy pixel data, interleave with A
    for(unsigned int i = 0; i < pixelCount; ++i) {
        pDst[0] = pSrc[0];
        pDst[1] = pSrc[1];
        pDst[2] = pSrc[2];
        pDst[3] = 0xFF;
        pDst += 4;
        pSrc += 3;
    }
}

bool TextureDX12::isCubemap() const {
    return m_header.arraySize == 6;
}

void TextureDX12::InitDebugTexture(ID3D12Device* pDevice, UploadHeapDX12* pUploadHeap) {
    CD3DX12_RESOURCE_DESC RDescs;
    RDescs = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 32, 32, 1, 1);
    RDescs.Flags |= D3D12_RESOURCE_FLAG_NONE;

    UINT64 DefHeapOffset = 0;

    auto RAInfo = pDevice->GetResourceAllocationInfo(1, 1, &RDescs);

    DefHeapOffset = Align(DefHeapOffset, RAInfo.Alignment);

    pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, pUploadHeap->GetNode(), pUploadHeap->GetNodeMask()),
        D3D12_HEAP_FLAG_NONE,
        &RDescs,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_pTexture2D)
    );

    UINT64 UplHeapSize;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTex2D = { 0 };
    pDevice->GetCopyableFootprints(&RDescs, 0, 1, 0, &placedTex2D, NULL, NULL, &UplHeapSize);

    DefHeapOffset += RAInfo.SizeInBytes;

    UINT8* pixels = pUploadHeap->Suballocate(SIZE_T(UplHeapSize), D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
    placedTex2D.Offset += UINT64(pixels - pUploadHeap->BasePtr());

    // prepare a pBitmap in memory, with bitmapWidth, bitmapHeight, and pixel format of DXGI_FORMAT_B8G8R8A8_UNORM ...
    for (UINT8 r = 0; r < 32; ++r) {
        for (UINT8 g = 0; g < 32; ++g) {
            pixels[r*placedTex2D.Footprint.RowPitch + g * 4 + 0] = r * 7;
            pixels[r*placedTex2D.Footprint.RowPitch + g * 4 + 1] = g * 7;
            pixels[r*placedTex2D.Footprint.RowPitch + g * 4 + 2] = 0;
            pixels[r*placedTex2D.Footprint.RowPitch + g * 4 + 3] = 0xFF;
        }
    }

    CD3DX12_TEXTURE_COPY_LOCATION Dst(m_pTexture2D, 0);
    CD3DX12_TEXTURE_COPY_LOCATION Src(pUploadHeap->GetResource(), placedTex2D);
    pUploadHeap->GetCommandList()->CopyTextureRegion(
        &Dst,
        0, 0, 0,
        &Src,
        NULL
    );

    D3D12_RESOURCE_BARRIER RBDesc = {};
    RBDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    RBDesc.Transition.pResource = m_pTexture2D;
    RBDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    RBDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    RBDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    pUploadHeap->GetCommandList()->ResourceBarrier(1, &RBDesc);
}

INT32 TextureDX12::InitRendertarget(ID3D12Device* pDevice, CD3DX12_RESOURCE_DESC *pDesc, UINT node, UINT nodemask) {
    // Performance tip: Tell the runtime at resource creation the desired clear value.
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = pDesc->Format;
    clearValue.Color[0] = 0.0f;
    clearValue.Color[1] = 0.0f;
    clearValue.Color[2] = 0.0f;
    clearValue.Color[3] = 1.0f;

    pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, node, nodemask),
        D3D12_HEAP_FLAG_NONE,
        pDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&m_pTexture2D));

    m_header.width = (UINT32)pDesc->Width;
    m_header.height = pDesc->Height;
    m_header.mipMapCount = pDesc->MipLevels;

    return 0;
}

void TextureDX12::CreateRTV(DWORD index, RTV *pRV) {
    if (!m_pTexture2D) return;
    ID3D12Device* pDevice;
    m_pTexture2D->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
    D3D12_RESOURCE_DESC texDesc = m_pTexture2D->GetDesc();

    D3D12_RENDER_TARGET_VIEW_DESC RTViewDesc = {};
    RTViewDesc.Format = texDesc.Format;
    if (texDesc.SampleDesc.Count==1)
        RTViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    else
        RTViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

    pDevice->CreateRenderTargetView(m_pTexture2D, &RTViewDesc, pRV->GetCPU(index));

    pDevice->Release();
}

void TextureDX12::CreateUAV(DWORD index, CBV_SRV_UAV *pRV) {
    if (!m_pTexture2D) return;
    ID3D12Device* pDevice;
    m_pTexture2D->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
    D3D12_RESOURCE_DESC texDesc = m_pTexture2D->GetDesc();

    D3D12_UNORDERED_ACCESS_VIEW_DESC UAViewDesc = {};
    UAViewDesc.Format = texDesc.Format;
    UAViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    pDevice->CreateUnorderedAccessView(m_pTexture2D, NULL, &UAViewDesc, pRV->GetCPU(index));

    pDevice->Release();
}

void TextureDX12::CreateSRV(DWORD index, CBV_SRV_UAV *pRV) {
    if (!m_pTexture2D) return;
    ID3D12Device* pDevice;
    m_pTexture2D->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
    D3D12_RESOURCE_DESC texDesc = m_pTexture2D->GetDesc();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    if (texDesc.Format == DXGI_FORMAT_R32_TYPELESS) {
        //special case for the depth buffer
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        if (texDesc.SampleDesc.Count == 1) {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
        } else {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        }
    } else {
        D3D12_RESOURCE_DESC desc = m_pTexture2D->GetDesc();
        srvDesc.Format = desc.Format;
        if (texDesc.SampleDesc.Count == 1) {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = m_header.mipMapCount;
        } else {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        }

    }

    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    pDevice->CreateShaderResourceView(m_pTexture2D, &srvDesc, pRV->GetCPU(index));

    pDevice->Release();
}

void TextureDX12::CreateCubeSRV(DWORD index, CBV_SRV_UAV *pRV) {
    if (!m_pTexture2D) return;
    ID3D12Device* pDevice;
    m_pTexture2D->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));

    D3D12_RESOURCE_DESC texDesc = m_pTexture2D->GetDesc();
    D3D12_RESOURCE_DESC desc = m_pTexture2D->GetDesc();
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.Texture2DArray.ArraySize = m_header.arraySize;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.MipLevels = m_header.mipMapCount;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    pDevice->CreateShaderResourceView(m_pTexture2D, &srvDesc, pRV->GetCPU(index));

    pDevice->Release();
}

void TextureDX12::CreateDSV(DWORD index, DSV *pRV) {
    if (!m_pTexture2D) return;
    ID3D12Device* pDevice;
    m_pTexture2D->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
    D3D12_RESOURCE_DESC texDesc = m_pTexture2D->GetDesc();

    D3D12_DEPTH_STENCIL_VIEW_DESC DSViewDesc = {};
    DSViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    if (texDesc.SampleDesc.Count == 1) {
        DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    } else {
        DSViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
    }
    pDevice->CreateDepthStencilView(m_pTexture2D, &DSViewDesc, pRV->GetCPU(index));

    pDevice->Release();
}

INT32 TextureDX12::InitDepthStencil(ID3D12Device* pDevice, CD3DX12_RESOURCE_DESC *pDesc, UINT node, UINT nodemask) {
    // Performance tip: Tell the runtime at resource creation the desired clear value.
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, node, nodemask),
        D3D12_HEAP_FLAG_NONE,
        pDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&m_pTexture2D));

    m_header.width = (UINT32)pDesc->Width;
    m_header.height = pDesc->Height;
    m_header.mipMapCount = pDesc->MipLevels;

    return 0;
}

DXGI_FORMAT CMP2DXGIFormat(CMP_FORMAT cmp_format) {
    DXGI_FORMAT dxgi_format;

    switch (cmp_format) {
    // Compression formats ----------
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
        dxgi_format = DXGI_FORMAT_BC1_UNORM;
        break;
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:
        dxgi_format = DXGI_FORMAT_BC2_UNORM;
        break;
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:
        dxgi_format = DXGI_FORMAT_BC3_UNORM;
        break;
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:
        dxgi_format = DXGI_FORMAT_BC4_UNORM;
        break;
    case CMP_FORMAT_BC4_S:
        dxgi_format = DXGI_FORMAT_BC4_SNORM;
        break;
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N:
    case CMP_FORMAT_ATI2N_XY:
    case CMP_FORMAT_ATI2N_DXT5:
        dxgi_format = DXGI_FORMAT_BC5_UNORM;
        break;
    case CMP_FORMAT_BC5_S:
        dxgi_format = DXGI_FORMAT_BC5_SNORM;
        break;
    case CMP_FORMAT_BC6H:
        dxgi_format = DXGI_FORMAT_BC6H_UF16;
        break;
    case CMP_FORMAT_BC6H_SF:
        dxgi_format = DXGI_FORMAT_BC6H_SF16;
        break;
    case CMP_FORMAT_BC7:
        dxgi_format = DXGI_FORMAT_BC7_UNORM;
        break;
    //uncompressed format
    case CMP_FORMAT_ARGB_8888:
    case CMP_FORMAT_RGBA_8888:
        dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    case CMP_FORMAT_ABGR_8888:
    case CMP_FORMAT_BGRA_8888:
        dxgi_format = DXGI_FORMAT_B8G8R8A8_UNORM;
        break;
    case CMP_FORMAT_RGB_888:
        dxgi_format = DXGI_FORMAT_B8G8R8A8_UNORM;
        break;
    case CMP_FORMAT_BGR_888:
        dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        break;
    case CMP_FORMAT_RG_8:
        dxgi_format = DXGI_FORMAT_R8_UNORM;
        break;
    case CMP_FORMAT_R_8:
        dxgi_format = DXGI_FORMAT_R8_UNORM;
        break;
    case CMP_FORMAT_ARGB_2101010:
        dxgi_format = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
        break;
    case CMP_FORMAT_RGBA_1010102:
        dxgi_format = DXGI_FORMAT_R10G10B10A2_UNORM;
        break;
    case CMP_FORMAT_R_16:
        dxgi_format = DXGI_FORMAT_R16_UNORM;
        break;
    case CMP_FORMAT_RGBE_32F:
        dxgi_format = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        break;
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
        dxgi_format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;
    case CMP_FORMAT_R_16F:
        dxgi_format = DXGI_FORMAT_R16_FLOAT;
        break;
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
        dxgi_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_BGR_32F:
        dxgi_format = DXGI_FORMAT_R32G32B32_FLOAT;
        break;
    case CMP_FORMAT_RG_32F:
        dxgi_format = DXGI_FORMAT_R32G32_FLOAT;
        break;
    case CMP_FORMAT_R_32F:
        dxgi_format = DXGI_FORMAT_R32_FLOAT;
        break;
    // Unknown compression mapping to Direct X
#if (OPTION_BUILD_ASTC == 1)
    case CMP_FORMAT_ASTC:
#endif
    case CMP_FORMAT_ATC_RGB:
    case CMP_FORMAT_ATC_RGBA_Explicit:
    case CMP_FORMAT_ATC_RGBA_Interpolated:
    case CMP_FORMAT_DXT5_xGBR:
    case CMP_FORMAT_DXT5_RxBG:
    case CMP_FORMAT_DXT5_RBxG:
    case CMP_FORMAT_DXT5_xRBG:
    case CMP_FORMAT_DXT5_RGxB:
    case CMP_FORMAT_DXT5_xGxR:
    case CMP_FORMAT_ETC_RGB:
    case CMP_FORMAT_ETC2_RGB:
    case CMP_FORMAT_ETC2_SRGB:
    case CMP_FORMAT_ETC2_RGBA:
    case CMP_FORMAT_ETC2_RGBA1:
    case CMP_FORMAT_ETC2_SRGBA:
    case CMP_FORMAT_ETC2_SRGBA1:
#ifdef USE_APC
    case CMP_FORMAT_APC:
#endif
#ifdef USE_GTC
    case CMP_FORMAT_GTC:
#endif
    case CMP_FORMAT_BROTLIG:
#ifdef USE_BASIS
    case CMP_FORMAT_BASIS:
#endif
    // -----------------------------------
    case CMP_FORMAT_Unknown:
    default:
        dxgi_format = DXGI_FORMAT_UNKNOWN;
        break;
    }

    return dxgi_format;
}

//--------------------------------------------------------------------------------------
// entry function to initialize an image from a .DDS texture
//--------------------------------------------------------------------------------------
INT32 TextureDX12::InitFromFile(ID3D12Device* pDevice, UploadHeapDX12* pUploadHeap, const WCHAR *pFilename, void *pluginManager, void *msghandler) {
    OnDestroy();

    // Set any external display IO to user
    if (DX12_CMips)
        setCMIPS(DX12_CMips);

    // load the MipSet and Process it to the Model
    if (LoadImageMipSetFromFile(pFilename, pluginManager) == 0) {

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTex2D[6][D3D12_REQ_MIP_LEVELS] = { 0 };
        UINT num_rows[D3D12_REQ_MIP_LEVELS] = { 0 };
        UINT64 row_sizes_in_bytes[D3D12_REQ_MIP_LEVELS] = { 0 };
        UINT32 arraySize = (pMipSet->m_TextureType == TT_CubeMap) ? 6 : 1;
        DXGI_FORMAT dxgiFormat = CMP2DXGIFormat(pMipSet->m_format);

        DDS_HEADER_INFO dx10header = {
            (UINT32)pMipSet->m_nWidth,//header->dwWidth,
            (UINT32)pMipSet->m_nHeight, //header->dwHeight,
            (UINT32)1,//header->dwDepth ? header->dwDepth : 1,
            (UINT32)arraySize,
            (UINT32)pMipSet->m_nMipLevels,
            (UINT32)dxgiFormat
        };

        m_header = dx10header;

        CD3DX12_RESOURCE_DESC RDescs;
        RDescs = CD3DX12_RESOURCE_DESC::Tex2D((DXGI_FORMAT)m_header.format, m_header.width, m_header.height, m_header.arraySize, m_header.mipMapCount);

        pDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, pUploadHeap->GetNode(), pUploadHeap->GetNodeMask()),
            D3D12_HEAP_FLAG_NONE,
            &RDescs,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_pTexture2D)
        );


        int offset = 0;

        for (unsigned int a = 0; a < m_header.arraySize; a++) {
            D3D12_PLACED_SUBRESOURCE_FOOTPRINT *pPlacedTex2D = placedTex2D[a];

            // allocate memory from upload heap
            UINT64 UplHeapSize;
            pDevice->GetCopyableFootprints(&RDescs, 0, m_header.mipMapCount, 0, pPlacedTex2D, num_rows, row_sizes_in_bytes, &UplHeapSize);
            UINT8* pixels = pUploadHeap->Suballocate(SIZE_T(UplHeapSize), D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

            UINT32 mipMapCount = m_header.mipMapCount;

            for (UINT mip = 0; mip < mipMapCount; ++mip) {
                // skip mipmaps that are smaller than blocksize
                if (mip > 0) {
                    UINT w0 = pPlacedTex2D[mip - 1].Footprint.Width;
                    UINT w1 = pPlacedTex2D[mip].Footprint.Width;
                    UINT h0 = pPlacedTex2D[mip - 1].Footprint.Height;
                    UINT h1 = pPlacedTex2D[mip].Footprint.Height;
                    if ((w0 == w1) && (h0 == h1)) {
                        --mipMapCount;
                        --mip;
                        continue;
                    }
                }
                UINT w = isDXT((DXGI_FORMAT)m_header.format) ? pPlacedTex2D[mip].Footprint.Width / 4 : pPlacedTex2D[mip].Footprint.Width;
                UINT h = isDXT((DXGI_FORMAT)m_header.format) ? pPlacedTex2D[mip].Footprint.Height / 4 : pPlacedTex2D[mip].Footprint.Height;
                UINT bytePP = 4;
                if (pMipSet->m_compressed)
                    bytePP = GetPixelSize(dxgiFormat);

                pPlacedTex2D[mip].Footprint.RowPitch = static_cast<UINT>(Align(pPlacedTex2D[mip].Footprint.RowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));

                MipLevel* mipLevel = l_cmips.GetMipLevel(pMipSet, mip);

                if (mipLevel->m_pbData == NULL) return -1;

                // read compressonator mipset buffer
                unsigned char *pData = &pixels[pPlacedTex2D[mip].Offset];
                for (UINT y = 0; y < h; ++y) {
                    UINT8* dest_row_begin = (pData + pPlacedTex2D[mip].Offset) + (y * pPlacedTex2D[mip].Footprint.RowPitch);
                    memcpy(&pData[pPlacedTex2D[mip].Footprint.RowPitch*y], &(mipLevel->m_pbData[row_sizes_in_bytes[mip]*y]), row_sizes_in_bytes[mip]);
                }
                pPlacedTex2D[mip].Offset += UINT64(pixels - pUploadHeap->BasePtr());
            }
        }

        // copy upload texture to texture heap
        for (unsigned int a = 0; a < m_header.arraySize; a++) {
            for (UINT mip = 0; mip < m_header.mipMapCount; ++mip) {
                D3D12_RESOURCE_DESC texDesc = m_pTexture2D->GetDesc();
                CD3DX12_TEXTURE_COPY_LOCATION Dst(m_pTexture2D, a*m_header.mipMapCount + mip);
                CD3DX12_TEXTURE_COPY_LOCATION Src(pUploadHeap->GetResource(), placedTex2D[a][mip]);
                pUploadHeap->GetCommandList()->CopyTextureRegion(&Dst, 0, 0, 0, &Src, NULL);
            }
        }

        // prepare to shader read
        D3D12_RESOURCE_BARRIER RBDesc;
        ZeroMemory(&RBDesc, sizeof(RBDesc));
        RBDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        RBDesc.Transition.pResource = m_pTexture2D;
        RBDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        RBDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        RBDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

        pUploadHeap->GetCommandList()->ResourceBarrier(1, &RBDesc);
    } else {
        // bad load exit!
        return -1;
    }

    // Done cleanup temp used mipset
    CleanMipSet();

    return 0;
}

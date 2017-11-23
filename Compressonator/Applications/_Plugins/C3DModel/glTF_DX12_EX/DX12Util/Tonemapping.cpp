#include "stdafx.h"
#include "DynamicBufferRingDX12.h"
#include "StaticBufferPoolDX12.h"
#include "UploadHeapDX12.h"
#include "ToneMapping.h"


void ToneMapping::OnCreate(ID3D12Device* pDevice, UINT node, DXGI_FORMAT outFormat, ResourceViewHeapsDX12 *pResourceViewHeaps, StaticBufferPoolDX12  *pStaticBufferPool, DynamicBufferRingDX12 *pDynamicBufferRing)
{
    m_pDynamicBufferRing = pDynamicBufferRing;

    m_toneMapping.OnCreate(pDevice, node, "./plugins/shaders/", "Tonemapping.hlsl", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, pResourceViewHeaps, pStaticBufferPool, 1);
}

void ToneMapping::OnDestroy()
{
    m_toneMapping.OnDestroy();
}

void ToneMapping::Draw(ID3D12GraphicsCommandList* pCommandList, CBV_SRV_UAV *pHDRSRV, float exposure, int toneMapper, int gammaTestPattern)
{
    D3D12_GPU_DESCRIPTOR_HANDLE cbTonemappingHandle;
    struct ToneMapping { float exposure; int toneMapper; int gammaTestPattern; } *cbToneMapping;
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(ToneMapping), (void **)&cbToneMapping, &cbTonemappingHandle);
    cbToneMapping->exposure = exposure;
    cbToneMapping->toneMapper = toneMapper;
    cbToneMapping->gammaTestPattern = gammaTestPattern;

    m_toneMapping.Draw(pCommandList, 1, pHDRSRV, cbTonemappingHandle);
}

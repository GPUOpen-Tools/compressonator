#include "stdafx.h"
#include "DynamicBufferRingDX12.h"
#include "StaticBufferPoolDX12.h"
#include "UploadHeapDX12.h"
#include "TextureDX12.h"
#include "SkyDome.h"


void SkyDome::OnCreate(ID3D12Device* pDevice, UploadHeapDX12* pUploadHeap, DXGI_FORMAT outFormat, ResourceViewHeapsDX12 *pResourceViewHeaps, StaticBufferPoolDX12  *pStaticBufferPool, DynamicBufferRingDX12 *pDynamicBufferRing, UINT sampleDescCount, void *pluginManager, void *msghandler)
{
    m_pDynamicBufferRing = pDynamicBufferRing;

    char buff[512];
    GetCurrentDirectoryA(512, buff);

    if (m_CubeDiffuseTexture.InitFromFile(pDevice, pUploadHeap, L"./plugins/media/envmap/diffuse.dds", pluginManager, msghandler) != 0)
        return;

    if (m_CubeSpecularTexture.InitFromFile(pDevice, pUploadHeap, L"./plugins/media/envmap/specular.dds", pluginManager, msghandler) != 0)
        return;
    
    pResourceViewHeaps->AllocCBV_SRV_UAVDescriptor(1, &m_CubeSpecularTextureSRV);
    m_CubeSpecularTexture.CreateCubeSRV(0, &m_CubeSpecularTextureSRV);

    D3D12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    m_skydome.OnCreate(pDevice, pUploadHeap->GetNode(), "./plugins/shaders/", "SkyDome.hlsl", outFormat, pResourceViewHeaps, pStaticBufferPool, 1, NULL, NULL, sampleDescCount);

    m_initok = true;
}

void SkyDome::OnDestroy()
{
    if (m_initok)
    {
        m_skydome.OnDestroy();
        m_CubeDiffuseTexture.OnDestroy();
        m_CubeSpecularTexture.OnDestroy();
        m_initok = false;
    }
}

void SkyDome::Draw(ID3D12GraphicsCommandList* pCommandList, XMMATRIX invViewProj)
{    
    if (!m_initok) return;

    XMMATRIX *cbPerDraw;
    D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer;
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(XMMATRIX), (void **)&cbPerDraw, &constantBuffer);
    *cbPerDraw = invViewProj;

    m_skydome.Draw(pCommandList, 1, &m_CubeSpecularTextureSRV, constantBuffer);
}


void SkyDome::GenerateDiffuseMapFromEnvironmentMap()
{

}


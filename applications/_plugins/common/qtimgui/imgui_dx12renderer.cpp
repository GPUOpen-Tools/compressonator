//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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
// Copyright(c) 2014 - 2018 Omar Cornut

#include "imgui_dx12renderer.h"

#include <wrl.h>

#include <QtCore/qdatetime.h>
#include <Qtgui/qguiapplication.h>
#include <QtGui/qevent.h>
#include <Qtgui/qclipboard.h>
#include <Qtgui/qcursor.h>
#include <QtCore/qdebug.h>

// DirectX
#include <d3d11.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace qtimgui_dx12 {
QHash<int, ImGuiKey> keyMap = {
    { Qt::Key_Tab, ImGuiKey_Tab },
    { Qt::Key_Left, ImGuiKey_LeftArrow },
    { Qt::Key_Right, ImGuiKey_RightArrow },
    { Qt::Key_Up, ImGuiKey_UpArrow },
    { Qt::Key_Down, ImGuiKey_DownArrow },
    { Qt::Key_PageUp, ImGuiKey_PageUp },
    { Qt::Key_PageDown, ImGuiKey_PageDown },
    { Qt::Key_Home, ImGuiKey_Home },
    { Qt::Key_End, ImGuiKey_End },
    { Qt::Key_Delete, ImGuiKey_Delete },
    { Qt::Key_Backspace, ImGuiKey_Backspace },
    { Qt::Key_Enter, ImGuiKey_Enter },
    { Qt::Key_Escape, ImGuiKey_Escape },
    { Qt::Key_A, ImGuiKey_A },
    { Qt::Key_C, ImGuiKey_C },
    { Qt::Key_V, ImGuiKey_V },
    { Qt::Key_X, ImGuiKey_X },
    { Qt::Key_Y, ImGuiKey_Y },
    { Qt::Key_Z, ImGuiKey_Z },
};

QByteArray g_currentClipboardText;
}

using namespace qtimgui_dx12;



struct VERTEX_CONSTANT_BUFFER {
    float        mvp[4][4];
};

ImGuiRenderer_DX12::ImGuiRenderer_DX12() {
    m_pDevice = NULL;
    m_pPipelineState = NULL;
    m_pRootSignature = NULL;
    m_pTexture2D = NULL;
}

ImGuiRenderer_DX12::~ImGuiRenderer_DX12() {
    if (!m_pDevice)
        return;

    if (m_pPipelineState) m_pPipelineState->Release();
    if (m_pRootSignature) m_pRootSignature->Release();
    if (m_pTexture2D) m_pTexture2D->Release();
}

void ImGuiRenderer_DX12::OnCreate(  ID3D12Device* pDevice,
                                    UploadHeapDX12 *pUploadHeap,
                                    ResourceViewHeapsDX12 *pHeaps,
                                    DynamicBufferRingDX12 *pConstantBufferRing,
                                    UINT node,
                                    UINT nodemask) {
    m_pResourceViewHeaps = pHeaps;
    m_pConstBuf = pConstantBufferRing;
    m_pDevice = pDevice;
    m_node = node;
    m_nodeMask = nodemask;

    // Create the vertex shader
    static const char* vertexShader =
        "cbuffer vertexBuffer : register(b0) \
        {\
        float4x4 ProjectionMatrix; \
        };\
        struct VS_INPUT\
        {\
        float2 pos : POSITION;\
        float2 uv  : TEXCOORD;\
        float4 col : COLOR;\
        };\
        \
        struct PS_INPUT\
        {\
        float4 pos : SV_POSITION;\
        float2 uv  : TEXCOORD;\
        float4 col : COLOR;\
        };\
        \
        PS_INPUT main(VS_INPUT input)\
        {\
        PS_INPUT output;\
        output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
        output.col = input.col;\
        output.uv  = input.uv;\
        return output;\
        }";

    Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBlob;
    D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_5_0", 0, 0, &pVertexShaderBlob, NULL);

    // Create the input layout
    D3D12_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = sizeof(layout) / sizeof(layout[0]);

    // Create the pixel shader
    static const char* pixelShader =
        "struct PS_INPUT\
        {\
        float4 pos : SV_POSITION;\
        float2 uv  : TEXCOORD;\
        float4 col : COLOR;\
        };\
        sampler sampler0;\
        Texture2D texture0;\
        \
        float4 main(PS_INPUT input) : SV_Target\
        {\
        float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
        return out_col; \
        }";

    Microsoft::WRL::ComPtr<ID3DBlob> pPixelShaderBlob;
    D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &pPixelShaderBlob, NULL);

    {
        // create a root signature with buffer slots for constants and sampler and Texture
        Microsoft::WRL::ComPtr<ID3DBlob> pOutBlob, pErrorBlob;

        CD3DX12_DESCRIPTOR_RANGE DescRange[3];
        DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);        // b0 <- per frame
        DescRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);        // t0 <- per material
        DescRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);    // s0 <- static

        // Visibility to all stages allows sharing binding tables
        CD3DX12_ROOT_PARAMETER RTSlot[3];
        RTSlot[0].InitAsDescriptorTable(1, &DescRange[0], D3D12_SHADER_VISIBILITY_ALL);
        RTSlot[1].InitAsDescriptorTable(1, &DescRange[1], D3D12_SHADER_VISIBILITY_ALL);
        RTSlot[2].InitAsDescriptorTable(1, &DescRange[2], D3D12_SHADER_VISIBILITY_PIXEL);

        // the root signature contains 3 slots to be used
        CD3DX12_ROOT_SIGNATURE_DESC descRootSignature = CD3DX12_ROOT_SIGNATURE_DESC();
        descRootSignature.NumParameters = 3;
        descRootSignature.pParameters = RTSlot;
        descRootSignature.NumStaticSamplers = 0;
        descRootSignature.pStaticSamplers = NULL;

        // deny uneccessary access to certain pipeline stages
        descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
                                  | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        HRESULT hr = D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob, &pErrorBlob);
        m_pDevice->CreateRootSignature(
            m_nodeMask,
            pOutBlob->GetBufferPointer(),
            pOutBlob->GetBufferSize(),
            __uuidof(ID3D12RootSignature),
            (void**)&m_pRootSignature);
        m_pRootSignature->SetName(L"UI RootSignature");
    }

    // create a PSO description
    D3D12_GRAPHICS_PIPELINE_STATE_DESC descPso = {};
    descPso.InputLayout = { layout, numElements };
    descPso.pRootSignature = m_pRootSignature;
    descPso.VS = { reinterpret_cast<BYTE*>(pVertexShaderBlob->GetBufferPointer()), pVertexShaderBlob->GetBufferSize() };
    descPso.PS = { reinterpret_cast<BYTE*>(pPixelShaderBlob->GetBufferPointer()), pPixelShaderBlob->GetBufferSize() };
    descPso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    descPso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    descPso.RasterizerState.DepthClipEnable = true;
    descPso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    descPso.BlendState.RenderTarget[0].BlendEnable = true;
    descPso.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    descPso.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    descPso.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    descPso.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    descPso.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    descPso.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    descPso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState.DepthEnable = FALSE;
    descPso.SampleMask = UINT_MAX;
    descPso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    descPso.NumRenderTargets = 1;
    descPso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    descPso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    descPso.SampleDesc.Count = 1;
    descPso.NodeMask = m_nodeMask;
    m_pDevice->CreateGraphicsPipelineState(&descPso, IID_PPV_ARGS(&m_pPipelineState));
    m_pPipelineState->SetName(L"UI PSO");

    // create sampler
    {
        m_pResourceViewHeaps->AllocSamplerDescriptor(1, &m_sampler);

        D3D12_SAMPLER_DESC SamplerDesc = {};
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.MipLODBias = 0;
        SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        SamplerDesc.BorderColor[0] = 0.0f;
        SamplerDesc.BorderColor[1] = 0.0f;
        SamplerDesc.BorderColor[2] = 0.0f;
        SamplerDesc.BorderColor[3] = 0.0f;
        SamplerDesc.MinLOD = 0.0f;
        SamplerDesc.MaxLOD = 0.0f;
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU());
    }

    // Create Texture

    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    CD3DX12_RESOURCE_DESC RDescs = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1);

    m_pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, m_node, m_nodeMask),
        D3D12_HEAP_FLAG_NONE,
        &RDescs,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&m_pTexture2D)
    );

    // allocate memory from upload heap
    UINT64 UplHeapSize;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTex2D[1] = { 0 };
    UINT num_rows[1] = { 0 };
    UINT64 row_sizes_in_bytes[1] = { 0 };
    m_pDevice->GetCopyableFootprints(&RDescs, 0, 1, 0, placedTex2D, num_rows, row_sizes_in_bytes, &UplHeapSize);

    UINT8* ptr = pUploadHeap->Suballocate(SIZE_T(UplHeapSize), D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
    placedTex2D[0].Offset += UINT64(ptr - pUploadHeap->BasePtr());

    memcpy(ptr, pixels, width*height * 4);

    {
        // prepare to shader read
        D3D12_RESOURCE_BARRIER RBDesc = {};
        RBDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        RBDesc.Transition.pResource = m_pTexture2D;
        RBDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        RBDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
        RBDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

        pUploadHeap->GetCommandList()->ResourceBarrier(1, &RBDesc);
    }

    // copy upload texture to texture heap
    for (UINT mip = 0; mip < 1; ++mip) {
        D3D12_RESOURCE_DESC texDesc = m_pTexture2D->GetDesc();
        CD3DX12_TEXTURE_COPY_LOCATION Dst(m_pTexture2D, mip);
        CD3DX12_TEXTURE_COPY_LOCATION Src(pUploadHeap->GetResource(), placedTex2D[mip]);
        pUploadHeap->GetCommandList()->CopyTextureRegion(
            &Dst,
            0, 0, 0,
            &Src,
            NULL
        );
    }

    // prepare to shader read
    D3D12_RESOURCE_BARRIER RBDesc = {};
    RBDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    RBDesc.Transition.pResource = m_pTexture2D;
    RBDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    RBDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    RBDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    pUploadHeap->GetCommandList()->ResourceBarrier(1, &RBDesc);

    // allocate the textures SRV
    m_pResourceViewHeaps->AllocCBV_SRV_UAVDescriptor(1, &m_pTextureSRV);
    pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, m_pTextureSRV.GetCPU());

    // Store our identifier
    io.Fonts->TexID = (void *)&m_pTextureSRV;

}

void ImGuiRenderer_DX12::initialize(
    QImGUI_WindowWrapper_DX12 *window,
    ID3D12Device* pDevice,
    UploadHeapDX12 *pUploadHeap,
    ResourceViewHeapsDX12 *pHeaps,
    DynamicBufferRingDX12 *pConstantBufferRing,
    UINT node,
    UINT nodemask) {
    m_window.reset(window);

    ImGuiIO &io = ImGui::GetIO();
    for (ImGuiKey key : keyMap.values()) {
        io.KeyMap[key] = key;
    }

    io.SetClipboardTextFn = [](void *user_data, const char *text) {
        Q_UNUSED(user_data);
        QGuiApplication::clipboard()->setText(text);
    };

    io.GetClipboardTextFn = [](void *user_data) {
        Q_UNUSED(user_data);
        g_currentClipboardText = QGuiApplication::clipboard()->text().toUtf8();
        return (const char *)g_currentClipboardText.data();
    };

    window->installEventFilter(this);

    OnCreate(pDevice,pUploadHeap,pHeaps, pConstantBufferRing, node,nodemask);
}

void ImGuiRenderer_DX12::newFrame() {
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2(m_window->size().width(), m_window->size().height());
    io.DisplayFramebufferScale = ImVec2(m_window->devicePixelRatio(), m_window->devicePixelRatio());

    // Setup time step
    double current_time =  QDateTime::currentMSecsSinceEpoch() / double(1000);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
    g_Time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    if (m_window->isActive()) {
        auto pos = m_window->mapFromGlobal(QCursor::pos());
        io.MousePos = ImVec2(pos.x(), pos.y());   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
    } else {
        io.MousePos = ImVec2(-1,-1);
    }

    for (int i = 0; i < 3; i++) {
        io.MouseDown[i] = g_MousePressed[i];
    }

    io.MouseWheel = g_MouseWheel;
    g_MouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    // glfwSetInputMode(g_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    // Start the frame
    ImGui::NewFrame();
}

void ImGuiRenderer_DX12::onMousePressedChange(QMouseEvent *event) {
    g_MousePressed[0] = event->buttons() & Qt::LeftButton;
    g_MousePressed[1] = event->buttons() & Qt::RightButton;
    g_MousePressed[2] = event->buttons() & Qt::MiddleButton;
}

void ImGuiRenderer_DX12::onWheel(QWheelEvent *event) {
    // 5 lines per unit
    g_MouseWheel += event->pixelDelta().y() / (5.0 * ImGui::GetTextLineHeight());
}

void ImGuiRenderer_DX12::onKeyPressRelease(QKeyEvent *event) {
    ImGuiIO& io = ImGui::GetIO();
    if (keyMap.contains(event->key())) {
        io.KeysDown[keyMap[event->key()]] = event->type() == QEvent::KeyPress;
    }

    if (event->type() == QEvent::KeyPress) {
        QString text = event->text();
        if (text.size() == 1) {
            io.AddInputCharacter(text.at(0).unicode());
        }
    }

#ifdef Q_OS_MAC
    io.KeyCtrl = event->modifiers() & Qt::MetaModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::ControlModifier; // Comamnd key
#else
    io.KeyCtrl = event->modifiers() & Qt::ControlModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::MetaModifier;
#endif
}

bool ImGuiRenderer_DX12::eventFilter(QObject *watched, QEvent *event) {
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        this->onMousePressedChange(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::Wheel:
        this->onWheel(static_cast<QWheelEvent *>(event));
        break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        this->onKeyPressRelease(static_cast<QKeyEvent *>(event));
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

void ImGuiRenderer_DX12::Draw(ID3D12GraphicsCommandList *pCommandList) {
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;

    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Create and grow vertex/index buffers if needed
    char *pVertices = NULL;
    D3D12_VERTEX_BUFFER_VIEW VerticesView;
    m_pConstBuf->AllocVertexBuffer(draw_data->TotalVtxCount, 5 * 4, (void **)&pVertices, &VerticesView);

    char *pIndices = NULL;
    D3D12_INDEX_BUFFER_VIEW IndicesView;
    m_pConstBuf->AllocIndexBuffer(draw_data->TotalIdxCount, sizeof(ImDrawIdx), (void **)&pIndices, &IndicesView);

    ImDrawVert* vtx_dst = (ImDrawVert*)pVertices;
    ImDrawIdx* idx_dst = (ImDrawIdx*)pIndices;
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }

    // Setup orthographic projection matrix into our constant buffer
    D3D12_GPU_DESCRIPTOR_HANDLE ConstantBufferGpuDescriptor;
    {
        VERTEX_CONSTANT_BUFFER* constant_buffer;
        m_pConstBuf->AllocConstantBuffer(sizeof(VERTEX_CONSTANT_BUFFER), (void **)&constant_buffer, &ConstantBufferGpuDescriptor);

        float L = 0.0f;
        float R = ImGui::GetIO().DisplaySize.x;
        float B = ImGui::GetIO().DisplaySize.y;
        float T = 0.0f;
        float mvp[4][4] = {
            { 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
            { 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
            { 0.0f,         0.0f,           0.5f,       0.0f },
            { (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
        };
        memcpy(constant_buffer->mvp, mvp, sizeof(mvp));
    }

    // Setup viewport
    D3D12_VIEWPORT vp;
    memset(&vp, 0, sizeof(D3D12_VIEWPORT));
    vp.Width = ImGui::GetIO().DisplaySize.x;
    vp.Height = ImGui::GetIO().DisplaySize.y;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = vp.TopLeftY = 0.0f;
    pCommandList->RSSetViewports(1, &vp);

    // Bind shader and vertex buffers
    unsigned int stride = sizeof(ImDrawVert);
    unsigned int offset = 0;

    // set pipeline & render state
    pCommandList->SetPipelineState(m_pPipelineState);
    pCommandList->SetGraphicsRootSignature(m_pRootSignature);

    pCommandList->IASetIndexBuffer(&IndicesView);
    pCommandList->IASetVertexBuffers(0, 1, &VerticesView);
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D12DescriptorHeap* pDH[] = { m_pResourceViewHeaps->GetCBV_SRV_UAVHeap(), m_pResourceViewHeaps->GetSamplerHeap() };
    pCommandList->SetDescriptorHeaps(2, &pDH[0]);
    pCommandList->SetGraphicsRootDescriptorTable(0, ConstantBufferGpuDescriptor); // set CB
    pCommandList->SetGraphicsRootDescriptorTable(2, m_sampler.GetGPU());

    // Render command lists
    int vtx_offset = 0;
    int idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            } else {
                const D3D12_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
                pCommandList->RSSetScissorRects(1, &r);
                pCommandList->SetGraphicsRootDescriptorTable(1, m_pTextureSRV.GetGPU());
                //ctx->PSSetShaderResources(0, 1, (CBV_SRV_UAV*)&pcmd->TextureId);

                pCommandList->DrawIndexedInstanced(pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
            }
            idx_offset += pcmd->ElemCount;
        }
        vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

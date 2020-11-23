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
#pragma once

#include "commandlistringdx12.h"
#include "dynamicbufferringdx12.h"
#include "resourceviewheapsdx12.h"
#include "uploadheapdx12.h"

#include "defines.h"

#include <imgui/imgui.h>

#include <QtGui/qopenglextrafunctions.h>
#include <QtCore/qobject.h>

#include <d3dx12.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <malloc.h>
#include <map>
#include <memory>
#include <string>
#include <tchar.h>
#include <vector>

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;


class QImGUI_WindowWrapper_DX12 {
  public:
    virtual ~QImGUI_WindowWrapper_DX12() {}
    virtual void installEventFilter(QObject *object) = 0;
    virtual QSize size() const = 0;
    virtual qreal devicePixelRatio() const = 0;
    virtual bool isActive() const = 0;
    virtual QPoint mapFromGlobal(const QPoint &p) const = 0;
};

class ImGuiRenderer_DX12 : public QObject, QOpenGLExtraFunctions {
    Q_OBJECT
  public:
    ImGuiRenderer_DX12();
    ~ImGuiRenderer_DX12();

    void Draw(ID3D12GraphicsCommandList *pCmdLst);

    void initialize(
        QImGUI_WindowWrapper_DX12 *window,
        ID3D12Device* pDevice,
        UploadHeapDX12 *pUploadHeap,
        ResourceViewHeapsDX12 *pHeaps,
        DynamicBufferRingDX12 *pConstantBufferRing,
        UINT node,
        UINT nodemask);

    void newFrame();
    bool eventFilter(QObject *watched, QEvent *event);

  private:

    void OnCreate(ID3D12Device* pDevice, UploadHeapDX12 *pUploadHeap, ResourceViewHeapsDX12 *pHeaps, DynamicBufferRingDX12 *pConstantBufferRing, UINT node, UINT nodemask);

    void onMousePressedChange(QMouseEvent *event);
    void onWheel(QWheelEvent *event);
    void onKeyPressRelease(QKeyEvent *event);

    std::unique_ptr<QImGUI_WindowWrapper_DX12> m_window;
    double       g_Time = 0.0f;
    bool         g_MousePressed[3] = { false, false, false };
    float        g_MouseWheel = 0.0f;
    GLuint       g_FontTexture = 0;
    int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
    int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
    int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
    unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;

    // DX12
    ID3D12Device              *m_pDevice;
    ResourceViewHeapsDX12     *m_pResourceViewHeaps;
    DynamicBufferRingDX12     *m_pConstBuf;

    ID3D12Resource            *m_pTexture2D;
    ID3D12PipelineState       *m_pPipelineState;
    ID3D12RootSignature       *m_pRootSignature;

    SAMPLER                    m_sampler;
    CBV_SRV_UAV                m_pTextureSRV;

    UINT m_node;
    UINT m_nodeMask;

};



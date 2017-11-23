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

// typical camera class

class Camera
{
public:
    void LookAt(XMVECTOR eyePos, XMVECTOR lookAt);
    void SetFov(float fov, DWORD width, DWORD height);
    void SetPosition(XMVECTOR eyePos) { m_eyePos = eyePos; }
    void UpdateCamera(float roll, float pitch, float distance);
    void UpdateCameraWASD(float roll, float pitch, const bool keyDown[256], double deltaTime);

    XMMATRIX GetView() { return m_View; }
    XMMATRIX GetViewport() { return m_Viewport; }
    XMVECTOR GetPosition() { return m_eyePos; }
    XMVECTOR GetDirection() { return XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixTranspose(m_View)); }
    XMMATRIX GetProjection() { return m_Proj; }

    float GetFovH() { return m_fovH; }
    float GetFovV() { return m_fovV; }

private:
    XMMATRIX            m_View;
    XMMATRIX            m_Proj;
    XMMATRIX            m_Viewport;
    XMVECTOR            m_eyePos;
    float               m_fovV, m_fovH;
    float               m_aspectRatio;
};


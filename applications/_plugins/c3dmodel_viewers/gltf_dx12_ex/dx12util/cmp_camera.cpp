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

#include "cmp_camera.h"

#include <windows.h>

#include <algorithm>

using namespace DirectX;


//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void CMP_Camera::SetFov(float fovV, uint32_t width, uint32_t height)
{
    // fovV = fovV * 0.025f; avacado

    m_aspectRatio = width *1.f / height;

    m_fovV = fovV;
    m_fovH = min(m_fovV * width / height, 3.1415f / 2);
    m_fovV = m_fovH * height / width;

    float halfWidth = (float)width / 2.0f;
    float halfHeight = (float)height / 2.0f;
    m_Viewport = XMMATRIX(
                     halfWidth, 0.0f, 0.0f, 0.0f,
                     0.0f, -halfHeight, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     halfWidth, halfHeight, 0.0f, 1.0f);

    m_Proj = XMMatrixPerspectiveFovRH(fovV, m_aspectRatio, 0.1f, 10000.f);
    m_View = XMMatrixIdentity();
}

//--------------------------------------------------------------------------------------
//
// LookAt
//
//--------------------------------------------------------------------------------------
void CMP_Camera::LookAt(XMVECTOR eyePos, XMVECTOR lookAt)
{
    m_eyePos = eyePos;
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    m_View = XMMatrixLookAtRH(eyePos, lookAt, up);
}

//--------------------------------------------------------------------------------------
//
// UpdateCamera
//
//--------------------------------------------------------------------------------------
void CMP_Camera::UpdateCameraWASD(float roll, float pitch, const bool keyDown[256], double deltaTime)
{
    float speed = 0.0003f * (keyDown[VK_SHIFT] ? 25.f * (float)deltaTime : 5.f * (float)deltaTime);

    float eyeDir[3] = { 0,0,0 };

    if (keyDown['W']) {
        eyeDir[2] = 1.f * speed;
    }
    if (keyDown['S']) {
        eyeDir[2] = -1.f * speed;
    }
    if (keyDown['A']) {
        eyeDir[0] = +1.f * speed;
    }
    if (keyDown['D']) {
        eyeDir[0] = -1.f * speed;
    }
    if (keyDown['E']) {
        eyeDir[1] = -1.f * speed;
    }
    if (keyDown['Q']) {
        eyeDir[1] = +1.f * speed;
    }

    //----------------------------

    XMMATRIX rotation = XMMatrixRotationX(pitch) * XMMatrixRotationY(roll) ;

    XMVECTOR dir = XMVector4Transform(XMVectorSet(0, 0, 1, 0.0f), rotation);

    m_eyePos += XMVector4Transform(XMVectorSet(eyeDir[0], eyeDir[1], eyeDir[2], 0.0f), rotation);

    XMVECTOR at = m_eyePos + dir;
    LookAt(m_eyePos, at);
}

void CMP_Camera::UpdateCamera(float roll, float pitch, float distance)
{
    m_eyePos = XMVectorSet(distance * sinf(roll) * cosf(pitch), distance * sinf(pitch), distance * cosf(roll) * cosf(pitch), 0);
    XMVECTOR at = XMVectorSet(0, 0, 0, 0);
    LookAt(m_eyePos, at);
}
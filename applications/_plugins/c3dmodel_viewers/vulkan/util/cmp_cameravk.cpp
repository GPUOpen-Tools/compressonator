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

#include "cmp_cameravk.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include <algorithm>

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void CMP_Camera::SetFov(float fovV, unsigned int width, unsigned int height)
{
    m_aspectRatio = width * 1.f / height;

    m_fovV = fovV;
    m_fovH = (std::min)(m_fovV * width / height, 3.1415f / 2);
    m_fovV = m_fovH * height / width;

    float halfWidth = (float)width / 2.0f;
    float halfHeight = (float)height / 2.0f;
    m_Viewport = glm::mat4(
                     halfWidth, 0.0f, 0.0f, 0.0f,
                     0.0f, -halfHeight, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     halfWidth, halfHeight, 0.0f, 1.0f);

    m_Proj = glm::perspectiveFovRH(fovV, static_cast<float>(width), static_cast<float>(height), 0.1f, 10000.0f);
    m_View = glm::mat4(1);
}

//--------------------------------------------------------------------------------------
//
// LookAt
//
//--------------------------------------------------------------------------------------
void CMP_Camera::LookAt(glm::vec4 eyePos, glm::vec4 lookAt)
{
    m_eyePos = eyePos;
    glm::vec4 up = glm::vec4(0, 1, 0, 0);
    m_View = glm::lookAtRH(glm::vec3(eyePos), glm::vec3(lookAt), glm::vec3(up));
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

    glm::mat4 rotation = (glm::rotate(glm::mat4(1), pitch, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1), roll, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec4 dir = glm::vec4(0, 0, 1, 0.0f) * rotation;

    m_eyePos += glm::vec4(eyeDir[0], eyeDir[1], eyeDir[2], 0.0f) * rotation;

    glm::vec4 at = m_eyePos + dir;
    LookAt(m_eyePos, at);
}

void CMP_Camera::UpdateCamera(float roll, float pitch, float distance)
{
    m_eyePos = glm::vec4(distance * sinf(roll) * cosf(pitch), distance * sinf(pitch), distance * cosf(roll) * cosf(pitch), 0);
    glm::vec4 at(0, 0, 0, 0);
    LookAt(m_eyePos, at);
}

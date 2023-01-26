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
#include <glm/glm.hpp>

// typical camera class

class CMP_Camera
{
  public:
    void LookAt(glm::vec4 eyePos, glm::vec4 lookAt);
    void SetFov(float fov, unsigned int width, unsigned int height);
    void SetPosition(glm::vec4 eyePos) {
        m_eyePos = eyePos;
    }
    void UpdateCamera(float roll, float pitch, float distance);
    void UpdateCameraWASD(float roll, float pitch, const bool keyDown[256], double deltaTime);

    glm::mat4 GetView() {
        return m_View;
    }
    glm::mat4 GetViewport() {
        return m_Viewport;
    }
    glm::vec4 GetPosition() {
        return m_eyePos;
    }
    glm::vec4 GetDirection() {
        return glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * glm::transpose(m_View);
    }
    glm::mat4 GetProjection() {
        return m_Proj;
    }

    float GetFovH() {
        return m_fovH;
    }
    float GetFovV() {
        return m_fovV;
    }

  private:
    glm::mat4           m_View;
    glm::mat4           m_Proj;
    glm::mat4           m_Viewport;
    glm::vec4           m_eyePos;
    float               m_fovV, m_fovH;
    float               m_aspectRatio;
};


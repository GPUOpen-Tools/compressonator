// The MIT License (MIT)
//
// Copyright(c) 2018 Microsoft Corp
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files(the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and / or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions :
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Modifications Copyright (C) 2018 Advanced Micro Devices, Inc.
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
#pragma once

//
// This file holds all the structures/classes used to load a glTF model
//

#include "gltffeatures.h"

#include <json/json.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

struct tfNode {
    std::vector<tfNode *> m_children;

    int meshIndex = -1;

    glm::mat4x4 m_rotation;
    glm::vec4 m_translation;
    glm::vec4 m_scale;

    glm::mat4x4 GetWorldMat() {
        const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_scale));
        const glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(m_translation));
        const glm::mat4 worldMat = scale * m_rotation * translation;
        return worldMat;
    }
};

struct tfScene {
    std::vector<tfNode *> m_nodes;
};

class tfAccessor {
  public:
    void *m_data = NULL;
    int m_count = 0;
    int m_stride;
    int m_dimension;
    int m_type;

    glm::vec4 m_min;
    glm::vec4 m_max;

    void *Get(int i) {
        if (i >= m_count)
            i = m_count - 1;

        return (char*)m_data + m_stride*i;
    }

    int FindClosestFloatIndex(float val) {
        int ini = 0;
        int fin = m_count - 1;

        while (ini <= fin) {
            int mid = (ini + fin) / 2;
            float v = *(float*)Get(mid);

            if (v <= val)
                ini = mid + 1;
            else
                fin = mid - 1;
        }

        {
            if (*(float*)Get(fin) > val) {
                // Error !!
            }
            if (fin < m_count) {
                if (*(float*)Get(fin + 1) < val) {
                    // Error !!
                }
            }
        }

        return fin;
    }
};

class tfSampler {
  public:
    tfAccessor m_time;
    tfAccessor m_value;

    void SampleLinear(float time, float *frac, float **pCurr, float **pNext) {
        int curr_index = m_time.FindClosestFloatIndex(time);
        int next_index = (std::min)(curr_index + 1, m_time.m_count - 1);

        float curr_time = *(float*)m_time.Get(curr_index);
        float next_time = *(float*)m_time.Get(next_index);

        *frac = (time - curr_time) / (next_time - curr_time);
        *pCurr = (float*)m_value.Get(curr_index);
        *pNext = (float*)m_value.Get(next_index);
    }
};

class tfChannel {
  public:
    ~tfChannel() {
        delete m_pTranslation;
        delete m_pRotation;
        delete m_pScale;
    }

    tfSampler *m_pTranslation;
    tfSampler *m_pRotation;
    tfSampler *m_pScale;
};

struct tfAnimation {
    float m_duration;
    std::map<int, tfChannel> m_channels;
};


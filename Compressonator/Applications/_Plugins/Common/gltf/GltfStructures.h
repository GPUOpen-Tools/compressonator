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

#include "../json/json.h"
#include "GltfFeatures.h"

#ifdef _WIN32
#include <DirectXMath.h>            // Find a replacemnt for this so that the GUI is not platform specific! look at using CMP_MATH
using namespace DirectX;
#else
#ifdef _LINUX

using namespace std;
struct XMVECTOR
{
    union
    {
        float       vector4_f32[4];
        unsigned int    vector4_u32[4];
    };
    XMVECTOR& operator-(const XMVECTOR& rhs) {
        for (int i = 0; i < 4; i++) {
            vector4_f32[i] -= rhs.vector4_f32[i];
        }

        return *this;
    }

    XMVECTOR& operator+(const XMVECTOR& rhs) {
        for (int i = 0; i < 4; i++) {
            vector4_f32[i] += rhs.vector4_f32[i];
        }

        return *this;
    }

    XMVECTOR& operator*(const double value) {
        for (int i = 0; i < 4; i++) {
            vector4_f32[i] *= value;
        }

        return *this;
    }
};

struct XMVECTORF32
{
    union
    {
        float f[4];
        XMVECTOR v;
    };

    inline operator XMVECTOR() const { return v; }
    inline operator const float*() const { return f; }
};

struct XMMATRIX {
    union
    {
        XMVECTOR r[4];
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
    XMMATRIX& operator*(const XMMATRIX& rhs) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++)
                m[i][j] *= rhs.m[i][j];
        }
        
        return *this;
    }
};

typedef const XMVECTOR& FXMVECTOR;


inline XMMATRIX XMMatrixScalingFromVector
(
    FXMVECTOR Scale
)
{
    XMMATRIX M;
    M.m[0][0] = Scale.vector4_f32[0];
    M.m[0][1] = 0.0f;
    M.m[0][2] = 0.0f;
    M.m[0][3] = 0.0f;

    M.m[1][0] = 0.0f;
    M.m[1][1] = Scale.vector4_f32[1];
    M.m[1][2] = 0.0f;
    M.m[1][3] = 0.0f;

    M.m[2][0] = 0.0f;
    M.m[2][1] = 0.0f;
    M.m[2][2] = Scale.vector4_f32[2];
    M.m[2][3] = 0.0f;

    M.m[3][0] = 0.0f;
    M.m[3][1] = 0.0f;
    M.m[3][2] = 0.0f;
    M.m[3][3] = 1.0f;
    return M;

};

inline XMMATRIX XMMatrixTranslationFromVector
(
    FXMVECTOR Offset
)
{

    XMMATRIX M;
    M.m[0][0] = 1.0f;
    M.m[0][1] = 0.0f;
    M.m[0][2] = 0.0f;
    M.m[0][3] = 0.0f;

    M.m[1][0] = 0.0f;
    M.m[1][1] = 1.0f;
    M.m[1][2] = 0.0f;
    M.m[1][3] = 0.0f;

    M.m[2][0] = 0.0f;
    M.m[2][1] = 0.0f;
    M.m[2][2] = 1.0f;
    M.m[2][3] = 0.0f;

    M.m[3][0] = Offset.vector4_f32[0];
    M.m[3][1] = Offset.vector4_f32[1];
    M.m[3][2] = Offset.vector4_f32[2];
    M.m[3][3] = 1.0f;
    return M;
};

inline float XMVectorGetX(FXMVECTOR V)
{
    return V.vector4_f32[0];
};

inline float XMVectorGetY(FXMVECTOR V)
{
    return V.vector4_f32[1];
};

inline XMVECTOR XMVectorSet
(
    float x,
    float y,
    float z,
    float w
)
{

    XMVECTORF32 vResult = { x,y,z,w };
    return vResult.v;

};

inline XMMATRIX XMMatrixSet
(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33
)
{
    XMMATRIX M;

    M.m[0][0] = m00; M.m[0][1] = m01; M.m[0][2] = m02; M.m[0][3] = m03;
    M.m[1][0] = m10; M.m[1][1] = m11; M.m[1][2] = m12; M.m[1][3] = m13;
    M.m[2][0] = m20; M.m[2][1] = m21; M.m[2][2] = m22; M.m[2][3] = m23;
    M.m[3][0] = m30; M.m[3][1] = m31; M.m[3][2] = m32; M.m[3][3] = m33;

    return M;
};

inline XMVECTOR XMVectorAdd
(
    FXMVECTOR V1,
    FXMVECTOR V2
)
{

    XMVECTOR Result;
    Result.vector4_f32[0] = V1.vector4_f32[0] + V2.vector4_f32[0];
    Result.vector4_f32[1] = V1.vector4_f32[1] + V2.vector4_f32[1];
    Result.vector4_f32[2] = V1.vector4_f32[2] + V2.vector4_f32[2];
    Result.vector4_f32[3] = V1.vector4_f32[3] + V2.vector4_f32[3];
    return Result;
};

inline XMVECTOR XMVectorMultiply
(
    FXMVECTOR V1,
    FXMVECTOR V2
)
{

    XMVECTOR Result;
    Result.vector4_f32[0] = V1.vector4_f32[0] * V2.vector4_f32[0];
    Result.vector4_f32[1] = V1.vector4_f32[1] * V2.vector4_f32[1];
    Result.vector4_f32[2] = V1.vector4_f32[2] * V2.vector4_f32[2];
    Result.vector4_f32[3] = V1.vector4_f32[3] * V2.vector4_f32[3];
    return Result;
};

inline XMVECTOR XMVectorSubtract
(
    FXMVECTOR V1,
    FXMVECTOR V2
)
{

    XMVECTOR Result;
    Result.vector4_f32[0] = V1.vector4_f32[0] - V2.vector4_f32[0];
    Result.vector4_f32[1] = V1.vector4_f32[1] - V2.vector4_f32[1];
    Result.vector4_f32[2] = V1.vector4_f32[2] - V2.vector4_f32[2];
    Result.vector4_f32[3] = V1.vector4_f32[3] - V2.vector4_f32[3];
    return Result;
};

static XMVECTORF32 g_XMIdentityR0 = { 1.0f, 0.0f, 0.0f, 0.0f };
static XMVECTORF32 g_XMIdentityR1 = { 0.0f, 1.0f, 0.0f, 0.0f };
static XMVECTORF32 g_XMIdentityR2 = { 0.0f, 0.0f, 1.0f, 0.0f };
static XMVECTORF32 g_XMIdentityR3 = { 0.0f, 0.0f, 0.0f, 1.0f };

const uint32_t XM_PERMUTE_0X = 0;
const uint32_t XM_PERMUTE_0Y = 1;
const uint32_t XM_PERMUTE_0Z = 2;
const uint32_t XM_PERMUTE_0W = 3;
const uint32_t XM_PERMUTE_1X = 4;
const uint32_t XM_PERMUTE_1Y = 5;
const uint32_t XM_PERMUTE_1Z = 6;
const uint32_t XM_PERMUTE_1W = 7;

const uint32_t XM_SWIZZLE_X = 0;
const uint32_t XM_SWIZZLE_Y = 1;
const uint32_t XM_SWIZZLE_Z = 2;
const uint32_t XM_SWIZZLE_W = 3;

//------------------------------------------------------------------------------
inline XMVECTOR XMVectorPermute
(
    FXMVECTOR V1,
    FXMVECTOR V2,
    uint32_t PermuteX,
    uint32_t PermuteY,
    uint32_t PermuteZ,
    uint32_t PermuteW
)
{

    const uint32_t *aPtr[2];
    aPtr[0] = (const uint32_t*)(&V1);
    aPtr[1] = (const uint32_t*)(&V2);

    XMVECTOR Result;
    uint32_t *pWork = (uint32_t*)(&Result);

    const uint32_t i0 = PermuteX & 3;
    const uint32_t vi0 = PermuteX >> 2;
    pWork[0] = aPtr[vi0][i0];

    const uint32_t i1 = PermuteY & 3;
    const uint32_t vi1 = PermuteY >> 2;
    pWork[1] = aPtr[vi1][i1];

    const uint32_t i2 = PermuteZ & 3;
    const uint32_t vi2 = PermuteZ >> 2;
    pWork[2] = aPtr[vi2][i2];

    const uint32_t i3 = PermuteW & 3;
    const uint32_t vi3 = PermuteW >> 2;
    pWork[3] = aPtr[vi3][i3];

    return Result;

};

// General permute template
template<uint32_t PermuteX, uint32_t PermuteY, uint32_t PermuteZ, uint32_t PermuteW>
inline XMVECTOR XMVectorPermute(FXMVECTOR V1, FXMVECTOR V2)
{
    static_assert(PermuteX <= 7, "PermuteX template parameter out of range");
    static_assert(PermuteY <= 7, "PermuteY template parameter out of range");
    static_assert(PermuteZ <= 7, "PermuteZ template parameter out of range");
    static_assert(PermuteW <= 7, "PermuteW template parameter out of range");



    return XMVectorPermute(V1, V2, PermuteX, PermuteY, PermuteZ, PermuteW);


};

inline XMVECTOR XMVectorSwizzle
(
    FXMVECTOR V,
    uint32_t E0,
    uint32_t E1,
    uint32_t E2,
    uint32_t E3
)
{
    XMVECTOR Result = { V.vector4_f32[E0],
        V.vector4_f32[E1],
        V.vector4_f32[E2],
        V.vector4_f32[E3] };
    return Result;
};
// General swizzle template
template<uint32_t SwizzleX, uint32_t SwizzleY, uint32_t SwizzleZ, uint32_t SwizzleW>
inline XMVECTOR    XMVectorSwizzle(FXMVECTOR V)
{
    static_assert(SwizzleX <= 3, "SwizzleX template parameter out of range");
    static_assert(SwizzleY <= 3, "SwizzleY template parameter out of range");
    static_assert(SwizzleZ <= 3, "SwizzleZ template parameter out of range");
    static_assert(SwizzleW <= 3, "SwizzleW template parameter out of range");

    return XMVectorSwizzle(V, SwizzleX, SwizzleY, SwizzleZ, SwizzleW);

};

//------------------------------------------------------------------------------
// Replicate the w component of the vector
inline XMVECTOR XMVectorSplatW
(
    FXMVECTOR V
)
{

    XMVECTOR vResult;
    vResult.vector4_f32[0] =
        vResult.vector4_f32[1] =
        vResult.vector4_f32[2] =
        vResult.vector4_f32[3] = V.vector4_f32[3];
    return vResult;
};

inline XMMATRIX  XMMatrixRotationQuaternion
(
    FXMVECTOR Quaternion
)
{

    static const XMVECTORF32 Constant1110 = { 1.0f, 1.0f, 1.0f, 0.0f };

    XMVECTOR Q0 = XMVectorAdd(Quaternion, Quaternion);
    XMVECTOR Q1 = XMVectorMultiply(Quaternion, Q0);

    XMVECTOR V0 = XMVectorPermute<XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_1W>(Q1, Constant1110.v);
    XMVECTOR V1 = XMVectorPermute<XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1W>(Q1, Constant1110.v);
    XMVECTOR R0 = (Constant1110, V0);
    R0 = XMVectorSubtract(R0, V1);

    V0 = XMVectorSwizzle<XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_W>(Quaternion);
    V1 = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_W>(Q0);
    V0 = XMVectorMultiply(V0, V1);

    V1 = XMVectorSplatW(Quaternion);
    XMVECTOR V2 = XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_W>(Q0);
    V1 = XMVectorMultiply(V1, V2);

    XMVECTOR R1 = XMVectorAdd(V0, V1);
    XMVECTOR R2 = XMVectorSubtract(V0, V1);

    V0 = XMVectorPermute<XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z>(R1, R2);
    V1 = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1Z, XM_PERMUTE_0X, XM_PERMUTE_1Z>(R1, R2);

    XMMATRIX M;
    M.r[0] = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0W>(R0, V0);
    M.r[1] = XMVectorPermute<XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1W, XM_PERMUTE_0W>(R0, V0);
    M.r[2] = XMVectorPermute<XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W>(R0, V1);
    M.r[3] = g_XMIdentityR3.v;
    return M;
};



inline XMMATRIX XMMatrixIdentity()
{
    XMMATRIX M;
    M.r[0] = g_XMIdentityR0.v;
    M.r[1] = g_XMIdentityR1.v;
    M.r[2] = g_XMIdentityR2.v;
    M.r[3] = g_XMIdentityR3.v;
    return M;
};
#endif
#endif


/* Notes
typedef struct _D3DMATRIX {
union {
         struct {
                float        _11, _12, _13, _14;
                float        _21, _22, _23, _24;
                float        _31, _32, _33, _34;
                float        _41, _42, _43, _44;
                };
         float m[4][4];
    };
}
*/

struct tfNode
{
    std::vector<tfNode *> m_children;

    int meshIndex = -1;

    XMMATRIX m_rotation;
    XMVECTOR m_translation;
    XMVECTOR m_scale;

    XMMATRIX GetWorldMat() { 
        return XMMatrixScalingFromVector(m_scale)  * m_rotation  * XMMatrixTranslationFromVector(m_translation); 
    }
};

struct tfScene
{
    std::vector<tfNode *> m_nodes;
};

class tfAccessor
{
public:
    void *m_data = NULL;
    int m_count = 0;
    int m_stride;
    int m_dimension;
    int m_type;

    XMVECTOR m_min;
    XMVECTOR m_max;

    void *Get(int i)
    {
        if (i >= m_count)
            i = m_count - 1;

        return (char*)m_data + m_stride*i;
    }

    int FindClosestFloatIndex(float val)
    {
        int ini = 0;
        int fin = m_count - 1;

        while (ini <= fin)
        {
            int mid = (ini + fin) / 2;
            float v = *(float*)Get(mid);

            if (v <= val)
                ini = mid + 1;
            else
                fin = mid - 1;
        }

        {
            if (*(float*)Get(fin) > val)
            {
                // Error !!
            }
            if (fin < m_count)
            {
                if (*(float*)Get(fin + 1) < val)
                {
                    // Error !!
                }
            }
        }

        return fin;
    }
};

class tfSampler
{
public:
    tfAccessor m_time;
    tfAccessor m_value;

    void SampleLinear(float time, float *frac, float **pCurr, float **pNext)
    {
        int curr_index = m_time.FindClosestFloatIndex(time);
        int next_index = min(curr_index + 1, m_time.m_count - 1);

        float curr_time = *(float*)m_time.Get(curr_index);
        float next_time = *(float*)m_time.Get(next_index);

        *frac = (time - curr_time) / (next_time - curr_time);
        *pCurr = (float*)m_value.Get(curr_index);
        *pNext = (float*)m_value.Get(next_index);
    }
};

class tfChannel
{
public:
    ~tfChannel()
    {
        delete m_pTranslation;
        delete m_pRotation;
        delete m_pScale;
    }

    tfSampler *m_pTranslation;
    tfSampler *m_pRotation;
    tfSampler *m_pScale;
};

struct tfAnimation
{
    float m_duration;
    std::map<int, tfChannel> m_channels;
};


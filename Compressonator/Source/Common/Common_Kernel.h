#ifndef _COMMON_KERNEL_H
#define _COMMON_KERNEL_H

//===============================================================================
// Copyright (c) 2007-2017  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
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
//
//  File Name:   Common_Kernel.h
//  Description: common options used to set parameter for compress format
//
//////////////////////////////////////////////////////////////////////////////


#include "Common_KernelDef.h"

typedef struct
{
    int  m_src_width;
    int  m_src_height;
    int  m_width_in_blocks;
    int  m_height_in_blocks;
} Source_Info;

struct KernalOptions
{
    unsigned int     size;          // Size of data 
    unsigned int     Compute_type;  // Compute Type : default is OpenCL
    unsigned int     data_type;     // Codec Format
    unsigned int     use_code;      // Determines what compiler definition path is used defulat is USE_OPTIMIZED_CODE_0
    void *data;                     // data to pass down from CPU to kernel
    void *dataSVM;                  // data allocated as Shared by CPU and GPU (used only when code is running in 64bit and devices support SVM)
};

#ifdef __OPENCL_VERSION__
// map to OpenCL definitions
typedef uchar4 auchar4;
typedef float3 afloat3;

#else
// when running kernel as cpu c++
#define __kernel 
#define __global
#define __constant          const
#define __local             const

#include "stdio.h"
#include <math.h> 
#include <algorithm>    // std::max

using namespace std;

struct auchar4
{
    auchar4() {};
    auchar4(unsigned char s) : x(s), y(s), z(s), w(s) {}
    auchar4(unsigned char x, unsigned char y, unsigned char z, unsigned char w) : x(x), y(y), z(z), w(w) {}

    unsigned char x, y, z, w;

    inline auchar4 operator*(unsigned char s) const { return auchar4(x*s, y*s, z*s, w*s); }
    inline auchar4 operator^(const auchar4& a) const { return auchar4(x ^ a.x, y ^ a.y, z ^ a.z, w ^ a.w); }
    inline auchar4 operator&(const auchar4& a) const { return auchar4(x & a.x, y & a.y, z & a.z, w & a.w); }
    inline bool operator==(const auchar4& a) const { return (x == a.x && y == a.y && z == a.z && w == a.w); }
    inline auchar4 operator+(const auchar4& a) const { return auchar4(x + a.x, y + a.y, z + a.z, w + a.w); }
};

struct afloat3
{
    afloat3() { x = 0; y = 0; z = 0; };
    afloat3(float s) : x(s), y(s), z(s) {}
    afloat3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;

    inline afloat3 operator*(float s) const { return afloat3(x*s, y*s, z*s); }
    inline float operator*(const afloat3& a) const { return ((x*a.x) + (y*a.y) + (z*a.z)); }
    inline afloat3 operator+(const afloat3& a) const { return afloat3(x + a.x, y + a.y, z + a.z); }
    inline afloat3 operator-(const afloat3& a) const { return afloat3(x - a.x, y - a.y, z - a.z); }
};

#endif

#endif

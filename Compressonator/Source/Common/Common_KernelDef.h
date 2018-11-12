#ifndef _COMMON_KERNELDEF_H
#define _COMMON_KERNELDEF_H

//===============================================================================
// Copyright (c) 2016-2018  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Common_KernelDef.h
//
//////////////////////////////////////////////////////////////////////////////

#define USE_PROCESS_SEPERATE_ALPHA          // Enable this to use higher quality code using CompressDualIndexBlock
#define COMPRESSED_BLOCK_SIZE           16  // Size of a compressed block in bytes
#define MAX_DIMENSION_BIG               4   // Max number of channels  (RGBA)
#define MAX_SUBSETS                     3   // Maximum number of possible subsets
#define MAX_SUBSET_SIZE                 16  // Largest possible size for an individual subset
#define USE_OPT_QUANTIZER                   // High Quality compression
#define USE_OPT_QUANTIZER2                  // Poor Quality
#define USE_optQUANT                        // Good Quality

#ifndef __OPENCL_VERSION__
#define USE_QSORT                           // Default is to use no qsort in BC6H or BC7 when in compute, (enable qsort function only for cpu builds)
#endif

//#define USE_BLOCK_LINEAR    // Source Data is organized in linear form for each block : Experimental Code not fully developed 
//#define USE_DOUBLE          // Default is to use float, enable to use double data types only for float definitions

typedef unsigned int        CGU_UINT;       //16bits
typedef unsigned int        CGU_DWORD;      //32bits
typedef unsigned char       CGU_BYTE;       //8bits
typedef int                 CGU_BOOL;
typedef unsigned short      CGU_SHORT;

#ifdef USE_DOUBLE 
typedef double              CGU_FLOAT;
#define CMP_DBL_MAX         1.7976931348623158e+308 // max value
#define CMP_DBL_MAX_EXP     1024
#else
typedef float               CGU_FLOAT;
#define CMP_DBL_MAX         FLT_MAX // max value
#define CMP_DBL_MAX_EXP     128
#endif



typedef enum CMPComputeExtensions {
    CMP_COMPUTE_FP16     = 0x0001,
    CMP_COMPUTE_MAX_ENUM = 0x7FFF
} CMP_ComputeExtensions;

struct KernalOptions
{
    CMP_ComputeExtensions   Extensions;
    float            fquality;
    unsigned int     size;          // Size of data 
    unsigned int     Compute_type;  // Compute Type : default is OpenCL
    unsigned int     data_type;     // Codec Format
    unsigned int     use_code;      // Determines what compiler definition path is used defulat is USE_OPTIMIZED_CODE_0
    void *data;                     // data to pass down from CPU to kernel
    void *dataSVM;                  // data allocated as Shared by CPU and GPU (used only when code is running in 64bit and devices support SVM)
};

#ifndef __OPENCL_VERSION__

// defined when running code on cpu 
#define __kernel 
#define __global
#define __constant          const
#define __local             const

namespace CMP_Encoder
{

typedef struct
{
    int  m_src_width;
    int  m_src_height;
    int  m_width_in_blocks;
    int  m_height_in_blocks;
} Source_Info;

//==========================================================
// Definitions used by ArchitectMF and Compressonator SDK
//==========================================================
typedef struct
{
    CGU_BYTE  blockType;                                       // 0 = Single 1 = Dual
    CGU_BYTE  best_blockMode;                                  // Bock is Invalid if value is 99: No suitable mode was found
    CGU_BYTE  best_indices[MAX_SUBSETS][MAX_SUBSET_SIZE];

    // This can be union
    int       best_Endpoints[MAX_SUBSETS][2][MAX_DIMENSION_BIG];
    CGU_DWORD packedEndpoints[MAX_SUBSETS][2];

    // Specific to Dual
    CGU_BYTE  best_indexSelection;                             // either 0 or 1
    CGU_BYTE  best_rotation;                                   // either 0 or 2

                                                               // Specific to Single
    CGU_BYTE   bestPartition;
} QUANTIZEBLOCK;


#define QUANTIZEBLOCK_ID "A896F2E4-0C7F-41F1-B074-22545FDE4808"
typedef struct
{
    int         DataType = 0;  // 0 = 64 Bits, 1 = 128 Bits 2 = 512 bits
    int         w = 0;                          // Origin Image size
    int         h = 0;                          // 
    int         x = 4;                          //Block Sizes: Width
    int         y = 4;                          //Block Sizes: Height
    int         curr_blockx = 0;                //Current block been processed 
    int         curr_blocky = 0;                //Current block been processed 
    int         compressed_size = 0;            //Track the current compressed block size
    int         source_size = 64;               //Usually fixed at 64 bytes max
    QUANTIZEBLOCK buffer;
} Quantizer_Buffer;

} // namespace CMP_Encoder
#endif

#ifdef __OPENCL_VERSION__
// map to OpenCL definitions
typedef uchar4 Vec4uc;
typedef float3 afloat3;

#else
#include "stdio.h"
#include <math.h> 
#include <algorithm>    // std::max

using namespace std;

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


struct Vec4uc
{
    Vec4uc() {};
    Vec4uc(unsigned char s) : x(s), y(s), z(s), w(s) {}
    Vec4uc(unsigned char x, unsigned char y, unsigned char z, unsigned char w) : x(x), y(y), z(z), w(w) {}

    unsigned char x, y, z, w;

    inline Vec4uc operator*(unsigned char s) const { return Vec4uc(x*s, y*s, z*s, w*s); }
    inline Vec4uc operator^(const Vec4uc& a) const { return Vec4uc(x ^ a.x, y ^ a.y, z ^ a.z, w ^ a.w); }
    inline Vec4uc operator&(const Vec4uc& a) const { return Vec4uc(x & a.x, y & a.y, z & a.z, w & a.w); }
    inline bool operator==(const Vec4uc& a) const { return (x == a.x && y == a.y && z == a.z && w == a.w); }
    inline Vec4uc operator+(const Vec4uc& a) const { return Vec4uc(x + a.x, y + a.y, z + a.z, w + a.w); }
    inline Vec4uc operator*(const afloat3& a) const { return Vec4uc(x * a.x, y * a.y, z * a.z, w); }
};

// Used by ArchitectMF kernel transcoders : This needs to be replaced by CMP_MATH
typedef Vec4uc auchar4;
typedef int                 CGU_INT;        //16bits
typedef unsigned int        CGU_UINT;       //16bits
typedef unsigned int        CGU_DWORD;      //32bits
typedef unsigned char       CGU_BYTE;       //8bits
typedef int                 CGU_BOOL;       // ?? change to unsigned char! 
typedef unsigned short      CGU_SHORT;

#endif


#endif  // _COMMON_KERNELDEF_H


//### Automated section set by Compute SDK at run time for shader source CRC checksum, do not edit or remove these 3 lines
//
//###


//=====================================================================
// Copyright (c) 2007-2014    Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006    ATI Technologies Inc.
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
//=====================================================================

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <string>
#include <stdint.h>

namespace CMP
{
    // Basic types.
    typedef std::uint8_t  BYTE;
    typedef std::uint16_t WORD;
    typedef std::uint32_t DWORD;
    typedef std::int32_t  LONG;
    typedef std::int32_t  BOOL;
    typedef size_t        DWORD_PTR;
    typedef unsigned int  UINT;

}  // namespace CMP

#include "Compressonator.h"

using namespace std;

#define UNREFERENCED_PARAMETER(P) (P)

//#define USE_DBGTRACE                                //  Show messages via Win Debug
//#define BC7_DEBUG_TO_RESULTS_TXT                    //  Send debug info to a results text file
//#define DXT5_COMPDEBUGGER                           //  Remote connect data to Comp Debugger views
//#define BC6H_COMPDEBUGGER                           //  Remote connect data to Comp Debugger views
//#define BC7_COMPDEBUGGER                            //  Remote connect data to Comp Debugger views
//#define BC6H_NO_OPTIMIZE_ENDPOINTS                  //  Turn off BC6H optimization of endpoints - useful for debugging quantization and mode checking
//#define BC6H_DEBUG_TO_RESULTS_TXT                   //  Generates a Results.txt file on exe working directory; MultiThreading is turned off for debuging to text file
//#define BC6H_DECODE_DEBUG                           //  Enables debug info on decoder
//#define GT_COMPDEBUGGER                             //  Remote connect data to Comp Debugger views

#define ENABLE_MAKE_COMPATIBLE_API  //  Byte<->Float to make all source and dest compatible
// #define USE_GT                                     //  Gradient Texture Compressor patent pending...

// V2.4 / V2.5 features and changes
#define USE_OLD_SWIZZLE  //  Remove swizzle flag and abide by CMP_Formats

// To Be enabled in future releases
// #define ARGB_32_SUPPORT                            // Enables 32bit Float channel formats
// #define SUPPORT_ETC_ALPHA                          // for ATC_RGB output enable A
// #define SHOW_PROCESS_MEMORY                        // display available CPU process memory
// #define USE_COMPUTE                                // Enable compute code for compression

//#define USE_ASSIMP
//#define USE_3DCONVERT                               // Enable 3D model conversion (glTF<->obj) icon
//#define USE_3DVIEWALLAPI                              // Enable 3D model view selection list in application setting

// #define USE_BCN_IMAGE_DEBUG                           // Enables Combobox in Image View for low level BCn based block compression in debug mode
// #define USE_CRN                                    // Enabled .crn file output using CRUNCH encoder

// #define USE_FILEIO

#define USE_MESH_CLI              // CLI Process Mesh (only support glTF and OBJ files)
#define USE_MESH_DRACO_EXTENSION  // CLI Process gltf draco files include Mesh Compression with Draco support in gltf files
// #define USE_GLTF2_MIPSET                              // Enable Image Transcode & Compression support for GLTF files using TextureIO
#define USE_3DMESH_OPTIMIZE  // CLI Mesh Optimize

#endif  // !COMMON_H

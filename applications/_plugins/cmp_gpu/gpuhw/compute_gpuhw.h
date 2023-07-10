//=====================================================================
// Copyright (c) 2020    Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

#ifndef H_COMPUTE_GPUHW
#define H_COMPUTE_GPUHW

#include "compressonator.h"
#include "common_def.h"  // Updated at run time
#include "compute_base.h"
#include "common_kerneldef.h"
#include "cpu_timing.h"
#include "textureio.h"
#include "crc32.h"
#include "cmp_math_common.h"

#include "glad.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <string>

#define MAX_PLATFORMS 4

using namespace CMP_Compute_Base;

class CGpuHW : public ComputeBase {
  public:
    CGpuHW(void* kerneloptions);
    CGpuHW(ComputeOptions CLOptions);
    ~CGpuHW();

    CMP_ERROR   Compress(KernelOptions* Options, MipSet& SrcTexture, MipSet& destTexture, CMP_Feedback_Proc pFeedback);
    GLuint      processtexture();
    void        SetComputeOptions(ComputeOptions* CLOptions);
    float       GetProcessElapsedTimeMS();
    float       GetMTxPerSec();
    int         GetBlockSize();
    const char* GetDeviceName();
    const char* GetVersion();
    int         GetMaxUCores();
    void        CleanUp();

    // shader ops
    void checkShaderStatus(GLuint shader) const;
    void checkProgramStatus() const;
    void shader_destroy();

  private:
    static void errorCallback(int error, const char* description);

    MipSet* m_source;
    MipSet* m_destination;

    bool m_genGPUMipMaps;

    // Window shaders;
    GLuint w_vertex;
    GLuint w_fragment;
    GLuint w_geometry;
    GLuint w_program;

    // Texture shaders;
    GLuint m_vertex;
    GLuint m_fragment;
    GLuint m_geometry;
    GLuint m_program;

    GLuint m_vao_ref;
    GLuint m_vbo_ref;
    GLuint w_vao_ref;
    GLuint w_vbo_ref;

    GLuint m_glformat;

    GLFWwindow* m_gltfwindow;
    int         tupleIndex;
    bool        shouldGenerate;

    size_t m_destination_size;
    unsigned int m_width_in_blocks;
    unsigned int m_height_in_blocks;

    bool       m_initDeviceOk;
    bool       m_programRun;
    CMP_FORMAT m_codecFormat;

    // Performance Info
    bool  m_getPerfStats;
    int   m_num_blocks;
    float m_CmpMTxPerSec;
    float m_computeShaderElapsedMS;

    // Device Info
    std::string m_deviceName;
    std::string m_version;

    // Internal
    std::string    m_source_file;
    KernelOptions* m_kernel_options;

    // Options that users can override or add
    bool m_force_rebuild;

    Source_Info m_SourceInfo;
    bool        m_svmSupport;
    void*       m_svmData;

    // OpenCL file (Binary or Source)
    char m_compile_options[256];

    union {
        char*          buffer;
        unsigned char* ubuffer;
    } p_program;

    size_t m_program_size;
    bool   m_isBinary;
    float  time_device = 0;

    // Need to fill these
    BYTE*       p_destination[MAX_MIPLEVEL_SUPPORTED];
    CMP_BYTE*   m_psource;
    void        Init();
    bool        GetPlatformID();
    bool        GetDeviceInfo();
    bool        CreateContext();
    bool        RunKernel();

};

#endif

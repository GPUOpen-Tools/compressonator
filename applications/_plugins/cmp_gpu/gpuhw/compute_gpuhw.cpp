//=============================================================================
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
//==============================================================================

#include "compute_gpuhw.h"

#ifdef USE_CPU_PERFORMANCE_COUNTERS
#include "query_timer.h"  // can use CPU timing but pref is to use GPU counters
#endif

#ifdef ENABLE_SVM
extern bool SVMInitCodec(KernelOptions* options);
#endif

extern CMIPS* GPU_HWMips;

#define LOG_BUFFER_SIZE 102400
#define KERNEL_ARG_SOURCE 0
#define KERNEL_ARG_DESTINATION 1
#define KERNEL_ARG_SOURCEINFO 2
#define KERNEL_ARG_ENCODE 3

static bool is64Bit() {
    return (sizeof(int*) == 8);
}

void PrintCGpu(const char* Format, ...) {
    // define a pointer to save argument list
    va_list args;
    char    buff[1024];
    // process the arguments into our debug buffer
    va_start(args, Format);
    vsprintf_s(buff, Format, args);
    va_end(args);

    if (GPU_HWMips) {
        GPU_HWMips->Print(buff);
    } else {
        printf(buff);
    }
}

//================================================================================
// clang-format off
// #include <glad/glad.h> // Needs to be first
// #include <glm/ext/matrix_clip_space.hpp>
// #include <glm/ext/matrix_transform.hpp>
#include <exception>
#include <iostream>
#include <random>
#include <algorithm>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <iostream>
#include <stb_image.h>
#include <stdexcept>
#include <vector>

GLuint get_cmpglformat(CMP_FORMAT cmp_format) {
    switch (cmp_format) {
    case CMP_FORMAT_BC1  :
        return(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
    case CMP_FORMAT_BC2  :
        return(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
    case CMP_FORMAT_BC3  :
        return(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
    case CMP_FORMAT_BC4  :
        return(GL_COMPRESSED_RED_RGTC1_EXT);
    case CMP_FORMAT_BC4_S:
        return(GL_COMPRESSED_SIGNED_RED_RGTC1_EXT);
    case CMP_FORMAT_BC5  :
        return(GL_COMPRESSED_RED_GREEN_RGTC2_EXT);
    case CMP_FORMAT_BC5_S:
        return(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT);
    // case CMP_FORMAT_BC6H : return(GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT); not supported in this release!
    case CMP_FORMAT_BC7  :
        return(GL_COMPRESSED_RGBA_BPTC_UNORM);
    }
    return(0);
}


void CGpuHW::checkShaderStatus(const GLuint shader) const {
    char infoLog[512];
    int  success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        throw "Failed to compile shader error: " + std::string(infoLog);
    };
}

void CGpuHW::checkProgramStatus() const {
    char infoLog[512];
    int  success;
    glGetProgramiv(w_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(w_program, 512, nullptr, infoLog);
        throw "Failed to link shader error: " + std::string(infoLog);
    };
}

void CGpuHW::shader_destroy() {
    if (w_program) {
        glDeleteProgram(w_program);
        w_program = 0;
    }
    if (w_vertex) {
        glDeleteShader(w_vertex);
        w_vertex = 0;
    }
    if (w_geometry) {
        glDeleteShader(w_geometry);
        w_vertex = 0;
    }
    if (w_fragment) {
        glDeleteShader(w_fragment);
        w_fragment = 0;
    }
}

static const std::string CMP_SHADER_VERT =
    R"(#version 330 core
layout(location = 0) in vec2 position;

out vec2 v_texCoords;

void main() {
    vec2 coords = (position + 1.0) * 0.5;
    v_texCoords = vec2(coords.x, 1.0 - coords.y);
    gl_Position = vec4(position, 1.0, 1.0);
}
)";

static const std::string WIN_SHADER_VERT = 
R"(#version 330 core
layout(location = 0) in vec2 position;

out vec2 v_texCoords;

void main() {
    v_texCoords = (position + 1.0) * 0.5;
    gl_Position = vec4(position, 1.0, 1.0);
}
)";


static const std::string SHADER_FRAG = 
R"(#version 330 core
in vec2 v_texCoords;

out vec4 fragmentColor;

uniform sampler2D tex;

void main() {
    fragmentColor = texture(tex, v_texCoords);
}
)";


static const float FULL_SCREEN_QUAD[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

void CGpuHW::errorCallback(const int error, const char* description)
{
    std::cerr << "error: " << error << " description: " << description << std::endl;
}

void CGpuHW::Init()
{
    query_timer::initialize();

    m_initDeviceOk              = false;
    m_programRun                = false;
    m_codecFormat               = CMP_FORMAT_Unknown;
    m_num_blocks                = 0;
    m_CmpMTxPerSec              = 0;
    m_computeShaderElapsedMS    = 0.0f;
    m_svmSupport                = false;
    m_svmData                   = NULL;
    p_program.buffer            = NULL;
    m_deviceName                = "";
    m_version                   = "";

    m_gltfwindow                = NULL;
    shouldGenerate              = true;     // remove this
    tupleIndex                  = 0;        // remove this

    // window shaders
    w_vertex                    = NULL;
    w_fragment                  = NULL;
    w_geometry                  = NULL;
    w_program                   = NULL;

    // texture shaders
    m_vertex                    = NULL;
    m_fragment                  = NULL;
    m_geometry                  = NULL;
    m_program                   = NULL;

    // User override options set after this init call
    m_force_rebuild = false;
}

CGpuHW::CGpuHW(ComputeOptions CLOptions)
{
    Init();
    m_force_rebuild = CLOptions.force_rebuild;
}

CGpuHW::~CGpuHW()
{
    if (m_initDeviceOk)
    {
        glfwTerminate();
        m_initDeviceOk = false;
    }
}

void CGpuHW::SetComputeOptions(ComputeOptions* CLOptions)
{
    m_force_rebuild = CLOptions->force_rebuild;
}

CGpuHW::CGpuHW(void* kerneloptions)
{
    m_kernel_options = (KernelOptions *)kerneloptions;
    Init();
}

bool CGpuHW::GetPlatformID()
{
    return true;
}

bool CGpuHW::GetDeviceInfo()
{
    return true;
}

void CGpuHW::CleanUp()
{
    // shader
    shader_destroy(); 

    if (m_gltfwindow) {
        glfwDestroyWindow(m_gltfwindow);
        m_gltfwindow = NULL;
    }
}

float CGpuHW::GetProcessElapsedTimeMS()
{
    return m_computeShaderElapsedMS;
}

float CGpuHW::GetMTxPerSec()
{
    return m_CmpMTxPerSec;
}

int CGpuHW::GetBlockSize()
{ 
    return m_num_blocks;
}

int CGpuHW::GetMaxUCores()
{ 
    return 0;
}

const char* CGpuHW::GetDeviceName()
{
    return m_deviceName.c_str();
}

const char* CGpuHW::GetVersion()
{
    return m_version.c_str();
}

GLuint CGpuHW::processtexture() {
    CMIPS CMips;
    int levels = 1;

    // requested miplevels
    if (m_genGPUMipMaps) {
       levels = m_kernel_options->miplevels;
    }

    int imgChannels = 4; // add code to supprt switching to only 3 channel data!

    if (imgChannels != 3 && imgChannels != 4) {
       // throw std::runtime_error("Image must be RGB or RGBA");
        return 0;
    }

    // Create the destination texture
    GLuint destination;
    glGenTextures(1, &destination);
    glBindTexture(GL_TEXTURE_2D, destination);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels - 1);

    // Create texture from the source image
    GLuint source;
    glGenTextures(1, &source);
    glBindTexture(GL_TEXTURE_2D, source);

    if (imgChannels == 4) {
        if (m_glformat  == GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT) 
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_SourceInfo.m_src_width, m_SourceInfo.m_src_height, 0, GL_RGB, GL_HALF_FLOAT, m_psource);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_SourceInfo.m_src_width, m_SourceInfo.m_src_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_psource);
    } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_SourceInfo.m_src_width, m_SourceInfo.m_src_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_psource);
    }

    // Set the filters for MipMap
    // bilinear filter the closest MIP map for small texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // bilinear filter the first MIP map for large texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Create framebuffer with renderbuffer
    GLuint fbo, fboColor, fboDepth;

    glGenRenderbuffers(1, &fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_SourceInfo.m_src_width, m_SourceInfo.m_src_height);

    glGenTextures(1, &fboColor);
    glBindTexture(GL_TEXTURE_2D, fboColor);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDER, fboDepth);

    // Render the texture to the framebuffer, this will create mipmaps
    for (auto level = 0; level < levels; level++) {
        const auto w = m_SourceInfo.m_src_width >> level;
        const auto h = m_SourceInfo.m_src_height>> level;

        glBindTexture(GL_TEXTURE_2D, fboColor);
        if (m_glformat  == GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT) 
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGB16F, w, h, 0, GL_RGB, GL_HALF_FLOAT, nullptr);
        else
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColor, level);

        glViewport(0, 0, w, h);
        glBindVertexArray(m_vao_ref);
        glBindTexture(GL_TEXTURE_2D, source);
        glUseProgram(w_program);
        glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
    }

    // Set the mipmap levels for the fbo color texture
    glBindTexture(GL_TEXTURE_2D, fboColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels - 1);

    // Copy pixels as compressed texture
    for (auto level = 0; level < levels; level++) {
        const auto w = m_SourceInfo.m_src_width >> level;
        const auto h = m_SourceInfo.m_src_height>> level;

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColor, level);
        glBindTexture(GL_TEXTURE_2D, destination);
        glCopyTexImage2D(GL_TEXTURE_2D, level, m_glformat, 0, 0, w, h, 0);

        GLint compressedSize = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize);

        // Save the output data 
        int storagelevel = level;
        if (!m_genGPUMipMaps) 
            storagelevel = m_destination->m_atmiplevel;
        CMP_MipLevel* pOutMipLevel = CMips.GetMipLevel(m_destination, storagelevel,m_destination->m_atfaceorslice);
        if (pOutMipLevel)
        {
          if (compressedSize <= pOutMipLevel->m_dwLinearSize)
          {
            glGetCompressedTexImage(GL_TEXTURE_2D, level, pOutMipLevel->m_pbData);
          }
        }
    }


    // Cleanup
    glDeleteTextures(1, &source);
    glDeleteTextures(1, &fboColor);
    glDeleteRenderbuffers(1, &fboDepth);
    glDeleteFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return destination;
}

bool CGpuHW::RunKernel()
{
    try
    {
        // Create texture shaders
        auto vertexSrc = WIN_SHADER_VERT.c_str();
        m_vertex         = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(m_vertex, 1, &vertexSrc, nullptr);
        glCompileShader(m_vertex);
        checkShaderStatus(m_vertex);
    
        auto fragmentSrc = SHADER_FRAG.c_str();
        m_fragment         = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_fragment, 1, &fragmentSrc, nullptr);
        glCompileShader(m_fragment);
        checkShaderStatus(m_fragment);
    
        m_program = glCreateProgram();
        glAttachShader(m_program, m_vertex);
        glAttachShader(m_program, m_fragment);
        glLinkProgram(m_program);
        checkProgramStatus();

        // done creating shaders now run process

        glGenVertexArrays(1, &m_vao_ref);
        glGenBuffers(1, &m_vbo_ref);

        glUniform1i(glGetUniformLocation(m_program, "tex"),0);
        glBindVertexArray(m_vao_ref);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ref);
        glBufferData(GL_ARRAY_BUFFER, sizeof(FULL_SCREEN_QUAD), reinterpret_cast<const uint8_t*>(FULL_SCREEN_QUAD), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        if (m_glformat  == GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT) 
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        else
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        // Result result(0, 0);

        int width, height;
        glfwGetFramebufferSize(m_gltfwindow, &width, &height);

        GLuint processed_texture = processtexture();

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, w_vao_ref);

        // Bind the results
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processed_texture);

        // shader
        glUseProgram(w_program);
        glUniform1i(glGetUniformLocation(w_program, "tex"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

        glfwSwapBuffers(m_gltfwindow);
        glfwPollEvents();

        if (m_program)
        {
            glDeleteProgram(m_program);
            m_program = 0;
        }

        if (m_vbo_ref)
        {
            glDeleteVertexArrays(1, &m_vbo_ref);
            m_vbo_ref = 0;
        }

        if (m_vao_ref)
        {
            glDeleteVertexArrays(1, &m_vao_ref);
            m_vao_ref = 0;
        }

        CleanUp();

    }
    catch (...)
    {
        CleanUp();
        return false;
    }

    return true;
}

bool CGpuHW::CreateContext()
{
    if (!m_initDeviceOk)
    {
        glfwSetErrorCallback(errorCallback);
        if (!glfwInit()) {
            return false;
        }
        m_initDeviceOk = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);

    m_gltfwindow = glfwCreateWindow(512, 512,"Texture Compression", nullptr, nullptr);
    if (!m_gltfwindow)
    {
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(m_gltfwindow, this);
    glfwMakeContextCurrent(m_gltfwindow);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    glfwSwapInterval(1);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_RENDERBUFFER);

    if (m_kernel_options->useSRGBFrames)
       glEnable(GL_FRAMEBUFFER_SRGB);
    else
       glEnable(GL_FRAMEBUFFER);


    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable when cube map compression is supported!
    // glEnable(GL_TEXTURE_CUBE_MAP);

    // Create window shaders
    try
    {
        auto vertexSrc = WIN_SHADER_VERT.c_str();
        w_vertex         = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(w_vertex, 1, &vertexSrc, nullptr);
        glCompileShader(w_vertex);
        checkShaderStatus(w_vertex);

        auto fragmentSrc = SHADER_FRAG.c_str();
        w_fragment         = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(w_fragment, 1, &fragmentSrc, nullptr);
        glCompileShader(w_fragment);
        checkShaderStatus(w_fragment);

        w_program = glCreateProgram();
        glAttachShader(w_program, w_vertex);
        glAttachShader(w_program, w_fragment);
        glLinkProgram(w_program);
        checkProgramStatus();

        // set up shaders
        glGenVertexArrays(1, &w_vao_ref);
        glGenVertexArrays(1, &w_vbo_ref);
        glBindVertexArray(w_vao_ref);
        glBindBuffer(GL_ARRAY_BUFFER, w_vbo_ref);
        glBufferData(GL_ARRAY_BUFFER, sizeof(FULL_SCREEN_QUAD), reinterpret_cast<const uint8_t*>(FULL_SCREEN_QUAD),GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);

        if (m_glformat  == GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT) 
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        else
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    }
    catch (...)
    {
        shader_destroy();
        return false;
    }
    return true;
}

CMP_ERROR CGpuHW::Compress(KernelOptions* KernelOptions, MipSet& srcTexture, MipSet& destTexture, CMP_Feedback_Proc pFeedback = NULL)
{
    bool newFormat = false;
    if (m_codecFormat != destTexture.m_format)
    {
        m_codecFormat = destTexture.m_format;
        newFormat = true;
    }

    if (m_codecFormat == CMP_FORMAT_Unknown)
    {
        // dont know how to progress this destination 
        return (CMP_ERR_GENERIC);
    }

    m_glformat = get_cmpglformat(m_codecFormat);
    if (m_glformat == 0)
        return (CMP_ERR_GENERIC);

    m_getPerfStats              = KernelOptions->getPerfStats && (destTexture.m_nIterations < 1);

    m_genGPUMipMaps             = KernelOptions->genGPUMipMaps;
    m_kernel_options->miplevels = KernelOptions->miplevels;
    m_kernel_options->data      = KernelOptions->data;
    m_kernel_options->size      = KernelOptions->size;
    m_kernel_options->format    = KernelOptions->format;
    m_kernel_options->dataSVM   = KernelOptions->dataSVM;;
    m_destination               = &destTexture;
    m_destination_size          = destTexture.dwDataSize;
    m_kernel_options->useSRGBFrames = KernelOptions->useSRGBFrames;

    if (destTexture.m_nBlockWidth > 0)
        m_width_in_blocks = (unsigned int)srcTexture.dwWidth / destTexture.m_nBlockWidth;
    else
    {
        return CMP_ERR_GENERIC;
    }

    if (destTexture.m_nBlockHeight > 0)
        m_height_in_blocks = (unsigned int)srcTexture.dwHeight / destTexture.m_nBlockHeight;
    else
    {
        return CMP_ERR_GENERIC;
    }

    m_psource                       = (CMP_BYTE *)srcTexture.pData;
    m_SourceInfo.m_src_height       = srcTexture.dwHeight;
    m_SourceInfo.m_src_width        = srcTexture.dwWidth;
    m_SourceInfo.m_width_in_blocks  = m_width_in_blocks;
    m_SourceInfo.m_height_in_blocks = m_height_in_blocks;
    m_SourceInfo.m_fquality         = KernelOptions->fquality;

    if (CreateContext()) {
        if (RunKernel()) return CMP_OK;
    }

    return(CMP_ERR_GENERIC);
}


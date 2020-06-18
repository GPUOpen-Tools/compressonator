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
/// \file Compute_DirectX.h
//
//=====================================================================

#ifndef H_COMPUTE_DirectX
#define H_COMPUTE_DirectX

#define __CL_ENABLE_EXCEPTIONS

#include "Common_Def.h"

#include "Compute_Base.h"
#include "Common_KernelDef.h"
#include "TextureIO.h"
#include "crc32.h"
#include <d3d11.h>
#include <d3dcompiler.h>

#if defined(_DEBUG)
#include <dxgidebug.h>
#include <wrl.h>
#endif

#include "query_timer.h"

using namespace CMP_Compute_Base;

#define SAFE_RELEASE( x ) { if ( x ) { x->Release(); x = nullptr; } }
UINT const MAX_QUERY_FRAME_NUM = 5;

#ifndef V_GOTO
#define V_GOTO( x ) { hr = (x); if ( FAILED(hr) ) {goto quit;} }
#endif

#ifndef V_RETURN
#define V_RETURN( x )    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

struct Buffer128Bits
{
    UINT color[4];
};

struct Buffer64Bits
{
    UINT color[2];
};

#define BLOCK_SIZE_Y    4
#define BLOCK_SIZE_X    4
#define BLOCK_SIZE     (BLOCK_SIZE_Y * BLOCK_SIZE_X)

#define ACTIVE_ENCODER_BC1  0
#define ACTIVE_ENCODER_BC2  1
#define ACTIVE_ENCODER_BC3  2
#define ACTIVE_ENCODER_BC4  3
#define ACTIVE_ENCODER_BC5  4
#define ACTIVE_ENCODER_BC6  5
#define ACTIVE_ENCODER_BC7  6

// #define USE_COMMON_PIPELINE_API     // Reserved for updates on next release

#if defined(_DEBUG)
using Microsoft::WRL::ComPtr;
#endif

class CDirectX :public ComputeBase
{
public:
    CDirectX(void *kerneloptions);
    ~CDirectX();

    CMP_ERROR   Compress(KernelOptions *Options, MipSet  &SrcTexture, MipSet  &destTexture,CMP_Feedback_Proc pFeedback);
    void        SetComputeOptions(ComputeOptions *CLOptions);
    float       GetProcessElapsedTimeMS();
    float       GetMTxPerSec();
    int         GetBlockSize();
    const char* GetDeviceName();
    const char* GetVersion();
    int         GetMaxUCores();


private:
    bool            m_programRun;
    CMP_FORMAT      m_codecFormat;

    void    Init();
    void    GetErrorMessages();

    // run time compile
    ID3DBlob*       m_csBlob;         // For mains
    ID3DBlob*       m_csBlobEx[4];    // For extended entry calls
    HRESULT CompileComputeShader( _In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint,_Outptr_ ID3DBlob** blob );
    void    csBlobCleanUp();

#ifdef USE_COMMON_PIPELINE_API
    long    file_size(FILE* p_file);
    bool    load_file();
    bool    Create_Program_File();
    bool    CreateProgramEncoder();
    void    CleanUpKernelAndIOBuffers();
    void    CleanUpProgramEncoder();
    bool    CreateIOBuffers();
    bool    RunKernel();
    bool    GetResults();

    union
    {
        char            *buffer;
        unsigned char   *ubuffer;
    } p_program;

    size_t          m_program_size;
    bool            m_isBinary;
#endif

    //-----------------------------------------
    HRESULT BuildBCnEncoder();
    HRESULT Create2DTexture();
    HRESULT GPU_Process();
    HRESULT GPU_Encode(ID3D11Buffer** ppDstTextureAsBufOut,int miplevel);
    HRESULT GPU_CompressedBuffer(std::vector<ID3D11Buffer*>& subTextureAsBufs);

    bool CreateDevice();
    bool GetDeviceInfo();

    void Cleanup();
    void CleanupEncoders();

    bool CheckCS4Suppot();

    bool    m_getPerfStats;
    float   m_computeShaderElapsedMS;       // Total Elapsed GPU Compute Time to process all the blocks
    int     m_num_blocks;                   // Number of 4x4 pixel blocks
    float   m_CmpMTxPerSec;                 // Number of Texels per second
    bool    m_initDeviceOk;
    bool    m_initQueryOk;
    float   m_fAlphaWeight;
    float   m_fquality;
    int     m_activeEncoder;

    // GPU Performance Monitoring
    cmp_cputimer        m_cmpTimer;
    ID3D11Query*        m_pQueryDisjoint;        // Checks for valid timestamp query
    ID3D11Query*        m_pQueryBegin;           // Individual timestamp queries for Begin of shader exec
    ID3D11Query*        m_pQueryEnd;             // Individual timestamp queries for End   of shader exec
    bool                m_getGPUPerfStats;
    unsigned int        m_totalnumBlocks;
    float               m_GPUFrequency;
    float               m_GPUFrequencyMin;
    float               m_GPUFrequencyMax;
    D3D11_QUERY_DESC    m_queryDisjointDesc;
    D3D11_QUERY_DESC    m_pQueryBeginCS;
    D3D11_QUERY_DESC    m_pQueryEndCS;

    // Debug reports
#if defined(_DEBUG)
    ComPtr<ID3D11Debug>        m_pDebug;
#endif

    void QueryDispatchBegin();
    void QueryDispatchEnd(unsigned int numBlocks);
    void QueryProcessBegin(int miplevel);
    void QueryProcessEnd(int miplevel);

    // Shader execution
    void RunComputeShader(
                          ID3D11ComputeShader* pComputeShader,
                          ID3D11ShaderResourceView** pShaderResourceViews,
                          UINT uNumSRVs,
                          ID3D11Buffer* pCBCS,
                          ID3D11UnorderedAccessView* pUnorderedAccessView,
                          UINT X, UINT Y, UINT Z,
                          UINT numBlocks,
                          bool fixed);

    // Device Info
    std::string          m_deviceName;
    std::string          m_version;
    int                  m_maxUCores;

    // Internal
    std::string          m_sourceShaderFile;
    KernelOptions*       m_kernelOptions;
    MipSet               m_SrcTexture;
    MipSet               m_DstTexture;

    ID3D11Device*        m_pDevice;
    ID3D11DeviceContext* m_pContext;
    ID3D11Texture2D*     m_pTexture2DSourceTexture;
    DXGI_FORMAT          m_fmtEncode;
    ID3D11ComputeShader* m_BC6H_pTryModeG10CS;
    ID3D11ComputeShader* m_BC6H_pTryModeLE10CS;
    ID3D11ComputeShader* m_BC7_pTryMode456CS;
    ID3D11ComputeShader* m_BC7_pTryMode137CS;
    ID3D11ComputeShader* m_BC7_pTryMode02CS;
    ID3D11ComputeShader* m_BCn_pEncodeBlockCS[7];

};

#endif

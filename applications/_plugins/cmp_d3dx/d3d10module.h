//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2020 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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

#pragma once

#include "d3d10_1.h"
#include "Module.h"

typedef HRESULT(WINAPI FAR* D3D10CompileShaderProc)(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName,
        CONST D3D10_SHADER_MACRO* pDefines, ID3D10Include* pInclude,
        LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags,
        ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs);
typedef HRESULT(WINAPI FAR* D3D10DisassembleShaderProc)(CONST UINT* pShader, SIZE_T Size, BOOL EnableColorCode,
        LPCSTR pComments, ID3D10Blob** ppDisassembly);
typedef HRESULT(WINAPI FAR* D3D10ReflectShaderProc)(CONST void * pShaderBytecode, SIZE_T BytecodeLength, ID3D10ShaderReflection ** ppReflector);
typedef HRESULT(WINAPI FAR* D3D10CreateBlobProc)(SIZE_T NumBytes, LPD3D10BLOB *ppBuffer);
typedef HRESULT(WINAPI FAR* D3D10GetInputSignatureBlobProc)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
typedef HRESULT(WINAPI FAR* D3D10GetOutputSignatureBlobProc)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
typedef HRESULT(WINAPI FAR* D3D10GetInputAndOutputSignatureBlobProc)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
typedef HRESULT(WINAPI FAR* D3D10GetShaderDebugInfoProc)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppDebugInfo);

class CD3D10Module : public CModule {
  public:
    CD3D10Module();
    virtual ~CD3D10Module();

    virtual bool LoadModule();
    virtual bool LoadModule(LPCTSTR lpModuleName);

    HRESULT D3D10CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines,
                               ID3D10Include* pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags,
                               ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs);
    HRESULT D3D10DisassembleShader(CONST UINT* pShader, SIZE_T Size, BOOL EnableColorCode, LPCSTR pComments,
                                   ID3D10Blob** ppDisassembly);
    HRESULT D3D10ReflectShader(CONST void * pShaderBytecode, SIZE_T BytecodeLength, ID3D10ShaderReflection ** ppReflector);
    HRESULT D3D10CreateBlob(SIZE_T NumBytes, LPD3D10BLOB *ppBuffer);
    HRESULT WINAPI D3D10GetInputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
    HRESULT WINAPI D3D10GetOutputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
    HRESULT WINAPI D3D10GetInputAndOutputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
    HRESULT D3D10GetShaderDebugInfo(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppDebugInfo);

  protected:
    virtual void UnloadModule();

    D3D10CompileShaderProc m_pD3D10CompileShader;
    D3D10DisassembleShaderProc m_pD3D10DisassembleShader;
    D3D10ReflectShaderProc m_pD3D10ReflectShader;
    D3D10CreateBlobProc m_pD3D10CreateBlob;
    D3D10GetInputSignatureBlobProc m_pD3D10GetInputSignatureBlob;
    D3D10GetOutputSignatureBlobProc m_pD3D10GetOutputSignatureBlob;
    D3D10GetInputAndOutputSignatureBlobProc m_pD3D10GetInputAndOutputSignatureBlob;
    D3D10GetShaderDebugInfoProc m_pD3D10GetShaderDebugInfo;
};

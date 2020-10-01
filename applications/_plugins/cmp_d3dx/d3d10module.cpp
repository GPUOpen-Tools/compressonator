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

#include "D3D10Module.h"

CD3D10Module::CD3D10Module(void) : CModule() {
    m_pD3D10CompileShader = NULL;
    m_pD3D10DisassembleShader = NULL;
    m_pD3D10ReflectShader = NULL;
    m_pD3D10CreateBlob = NULL;
    m_pD3D10GetInputSignatureBlob = NULL;
    m_pD3D10GetOutputSignatureBlob = NULL;
    m_pD3D10GetInputAndOutputSignatureBlob = NULL;
    m_pD3D10GetShaderDebugInfo = NULL;
}

CD3D10Module::~CD3D10Module(void) {
    UnloadModule();
}

bool CD3D10Module::LoadModule() {
    return LoadModule(_T("D3D10.DLL"));
}

bool CD3D10Module::LoadModule(LPCTSTR lpModuleName) {
    if(__super::LoadModule(lpModuleName)) {
        m_pD3D10CompileShader = (D3D10CompileShaderProc) GetProcAddress("D3D10CompileShader");
        m_pD3D10DisassembleShader = (D3D10DisassembleShaderProc) GetProcAddress("D3D10DisassembleShader");
        m_pD3D10ReflectShader = (D3D10ReflectShaderProc) GetProcAddress("D3D10ReflectShader");
        m_pD3D10CreateBlob = (D3D10CreateBlobProc) GetProcAddress("D3D10CreateBlob");
        m_pD3D10GetInputSignatureBlob = (D3D10GetInputSignatureBlobProc) GetProcAddress("D3D10GetInputSignatureBlob");
        m_pD3D10GetOutputSignatureBlob = (D3D10GetOutputSignatureBlobProc) GetProcAddress("D3D10GetOutputSignatureBlob");
        m_pD3D10GetInputAndOutputSignatureBlob = (D3D10GetInputAndOutputSignatureBlobProc) GetProcAddress("D3D10GetInputAndOutputSignatureBlob");
        m_pD3D10GetShaderDebugInfo = (D3D10GetShaderDebugInfoProc) GetProcAddress("D3D10GetShaderDebugInfo");
        if(!m_pD3D10CompileShader || !m_pD3D10DisassembleShader || !m_pD3D10ReflectShader || !m_pD3D10CreateBlob) {
            UnloadModule();
            return false;
        } else
            return true;
    } else
        return false;
}

void CD3D10Module::UnloadModule() {
    __super::UnloadModule();
    m_pD3D10CompileShader = NULL;
    m_pD3D10DisassembleShader = NULL;
    m_pD3D10ReflectShader = NULL;
    m_pD3D10CreateBlob = NULL;
    m_pD3D10GetInputSignatureBlob = NULL;
    m_pD3D10GetOutputSignatureBlob = NULL;
    m_pD3D10GetInputAndOutputSignatureBlob = NULL;
    m_pD3D10GetShaderDebugInfo = NULL;
}

HRESULT CD3D10Module::D3D10CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines,
        ID3D10Include* pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags,
        ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs) {
    if(m_pD3D10CompileShader)
        return m_pD3D10CompileShader(pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile,
                                     Flags, ppShader, ppErrorMsgs);
    else
        return E_OUTOFMEMORY;
}

HRESULT CD3D10Module::D3D10DisassembleShader(CONST UINT* pShader, SIZE_T Size, BOOL EnableColorCode, LPCSTR pComments,
        ID3D10Blob** ppDisassembly) {
    if(m_pD3D10DisassembleShader)
        return m_pD3D10DisassembleShader(pShader, Size, EnableColorCode, pComments, ppDisassembly);
    else
        return E_OUTOFMEMORY;
}

HRESULT CD3D10Module::D3D10ReflectShader(CONST void * pShaderBytecode, SIZE_T BytecodeLength, ID3D10ShaderReflection ** ppReflector) {
    if(m_pD3D10ReflectShader)
        return m_pD3D10ReflectShader(pShaderBytecode, BytecodeLength, ppReflector);
    else
        return E_OUTOFMEMORY;
}

HRESULT CD3D10Module::D3D10CreateBlob(SIZE_T NumBytes, LPD3D10BLOB *ppBuffer) {
    if(m_pD3D10CreateBlob)
        return m_pD3D10CreateBlob(NumBytes, ppBuffer);
    else
        return E_OUTOFMEMORY;
}

HRESULT CD3D10Module::D3D10GetInputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob) {
    if(m_pD3D10GetShaderDebugInfo)
        return m_pD3D10GetInputSignatureBlob(pShaderBytecode, BytecodeLength, ppSignatureBlob);
    else
        return E_OUTOFMEMORY;
}

HRESULT CD3D10Module::D3D10GetOutputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob) {
    if(m_pD3D10GetShaderDebugInfo)
        return m_pD3D10GetOutputSignatureBlob(pShaderBytecode, BytecodeLength, ppSignatureBlob);
    else
        return E_OUTOFMEMORY;
}

HRESULT CD3D10Module::D3D10GetInputAndOutputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob) {
    if(m_pD3D10GetShaderDebugInfo)
        return m_pD3D10GetInputAndOutputSignatureBlob(pShaderBytecode, BytecodeLength, ppSignatureBlob);
    else
        return E_OUTOFMEMORY;
}

HRESULT CD3D10Module::D3D10GetShaderDebugInfo(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppDebugInfo) {
    if(m_pD3D10GetShaderDebugInfo)
        return m_pD3D10GetShaderDebugInfo(pShaderBytecode, BytecodeLength, ppDebugInfo);
    else
        return E_OUTOFMEMORY;
}
